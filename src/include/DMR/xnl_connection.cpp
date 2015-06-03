//The MOTOTRBO ADP XCMP Console Demo Application software is considered Motorola Confidential Restricted information and
//contains intellectual property owned by Motorola, Inc., which is protected by U.S. copyright laws and international
//copyright treaties.  Unauthorized reproduction or distribution of this software is strictly prohibited.  2009 Motorola,
//Inc. All Rights Reserved.

#include "xnl_connection.h"
#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#include <stdio.h>
#include <string.h>

#include <assert.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

uint32_t GetTickCount()
{
    struct timespec ts;
    unsigned theTick = 0U;
    #ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
        clock_serv_t cclock;
        mach_timespec_t mts;
        host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
        int status = clock_get_time(cclock, &mts);
        mach_port_deallocate(mach_task_self(), cclock);
        ts.tv_sec = mts.tv_sec;
        ts.tv_nsec = mts.tv_nsec;
    #else
        clock_gettime(CLOCK_REALTIME, &ts);
    #endif
    theTick  = ts.tv_nsec / 1000000;
    theTick += ts.tv_sec * 1000;
    return theTick;
}

enum CALL_STATE {
    XCMP_CALL_DECODED = 0x01,
    XCMP_CALL_IN_PROGRESS,
    XCMP_CALL_ENDED,
    XCMP_CALL_INITIATED
};

CXNLConnection::CXNLConnection(const std::string& host, uint16_t port, const std::string& auth_key, uint32_t delta, CXNLConnectionHandler* handler)
{
    m_handler = handler;
    m_delta = delta;
    m_bConnected = false;
    m_XnlState = XNL_WAIT_MASTER_BRDCST;

    /* get the 16 bytes key from the string, now the auth_key format is similar as
     * 0x 11 22 33 44 0x556677880x99aabbcc0xddeeff00. Start from the 1st byte, each 10
     * bytes can be converted to a uint32_t integer.
     */

    for (size_t i = 0; i < 4; ++i) {
	m_auth_key[i] = strtoul(auth_key.substr(i* 10, 10).c_str(), NULL, 16);
    }

    encrypted_seed[0] = 0;
    encrypted_seed[1] = 0;
    m_trans_id_base = 0;
    m_bWaitForAck = false;
    m_bCloseSocket = false;
    conn_retry = 0;
    encrypted_seed[0] = 0;
    encrypted_seed[1] = 0;
    m_trans_id_base = 0;
    m_TxXCMPCount = 0;
    m_tx_xnl_flag = 0;
    m_rx_xnl_flag = 8; /* Set it to an invalid value */
    m_xnl_src_addr = 0;
    m_xnl_dst_addr = 0;
    m_pSendQueHdr = NULL;
    m_pSendQueTail = NULL;
    m_pLastSendMsg = NULL;
    m_XCMP_ver = 0;


     struct hostent *he = gethostbyname(host.c_str());
    struct sockaddr_in  target;
    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    assert(m_socket != -1);

    target.sin_family = AF_INET;
    target.sin_port = htons(port);
    memcpy(&target.sin_addr, he->h_addr_list[0], he->h_length);
    auto error = connect(m_socket, (struct sockaddr *)&target, sizeof(target));
    assert(!error);
}

CXNLConnection::~CXNLConnection(void)
{
    Stop();
}

void CXNLConnection::Stop()
{
    m_bCloseSocket = true;
}


///////////////////////////////////////////////////////////////////////////////
void CXNLConnection::Run()
{
    MSG_QUEUE_T *p_send_msg_node = NULL;
    uint8_t *p_send_msg = NULL;
    uint8_t *p_rcv_msg = NULL;
    bool bSocketErr = true;
    fd_set fdread;
    struct timeval read_timeval = {0, 10000};/* wait 10ms */
    uint32_t StartTime = GetTickCount();
    uint32_t CurTime = 0;
    size_t retry = 0;
    int ret = 0;

    while (!m_bCloseSocket)
    {
        /* Receive a Message first */

        /* Clear the read set before calling select */
        FD_ZERO(&fdread);
        /* Add m_socket to the read set */
        FD_SET(m_socket, &fdread);

        /* Non-blocking operation */
        ret = select(m_socket + 1, &fdread, NULL, NULL, &read_timeval);
        if (ret == -1) {
            break;
        } else if (ret > 0) {
            if (FD_ISSET(m_socket, &fdread))
            {
                /* receive the xnl message */
                bSocketErr = recv_xnl_message(&p_rcv_msg);

                /* check whether there are errors occurred during the message receiving */
                if (bSocketErr == false) {
                    break;
                }

                /* Process the received xnl message */
                OnXnlMessageProcess(p_rcv_msg);

                /* free the buffer */
                free(p_rcv_msg);
            }
        }

        /* The next is to send an xcmp message. First check whether there is a message need to resend. */
        if (m_bWaitForAck == false) /* there is no message need to resend  */
        {
            p_send_msg_node = dequeue_msg(); /* Dequeue an XCMP message from the sending queue */
            if (p_send_msg_node != NULL)
            {
                p_send_msg = p_send_msg_node->p_msg;

		m_handler->OnXnlMessageSent(this, p_send_msg, ((xnl_msg_hdr_t *)p_send_msg)->msg_len );
                /* Need to set the XNL flag and trans id */
                ((xnl_msg_hdr_t *)p_send_msg)->xnl_flag = m_tx_xnl_flag;
                *((uint8_t *)(&((xnl_msg_hdr_t *)p_send_msg)->trans_id)) = m_trans_id_base;
                *((uint8_t *)(&((xnl_msg_hdr_t *)p_send_msg)->trans_id) + 1) = m_TxXCMPCount;

                bSocketErr = send_xnl_message(p_send_msg);

                /* check whether there are errors occurred during the message sending */
                if (bSocketErr == false)
                {
                    break;
                }

                /* For each XCMP message, an ACK message shall be received. */
                m_bWaitForAck = true;

                /* Save the message for retry */
                m_pLastSendMsg = p_send_msg;
                StartTime = GetTickCount();

                /* free the node */
                free(p_send_msg_node);
            }
        }
        else  /* Re-send the xcmp message */
        {
            CurTime = GetTickCount();
            if ((CurTime - StartTime) >= 500) /* resend the message after 500ms */
            {
                bSocketErr = send_xnl_message(m_pLastSendMsg);
                /* check whether there are errors occurred during the message sending */
                if (bSocketErr == false) {
                    break;
                }

                StartTime = CurTime;
                if (++retry == 4) /* Discard the message after retry 4 times */
                {
                    /* reset m_bWaitForAck */
                    m_bWaitForAck = false;
                    if (m_tx_xnl_flag < 7)
                    {
                        m_tx_xnl_flag ++;
                    }
                    else
                    {
                        m_tx_xnl_flag = 0;
                    }
                    m_TxXCMPCount ++;
                    free(m_pLastSendMsg);
                    m_pLastSendMsg = NULL;
                    retry = 0;
                }
            }

        }

    }
    shutdown(m_socket, SHUT_RDWR);
    close(m_socket);
    MSG_QUEUE_T* p_tmp;
    for (; m_pSendQueHdr; )
    {
        p_tmp = m_pSendQueHdr;
        m_pSendQueHdr = m_pSendQueHdr->next;
        free(p_tmp->p_msg);
        free(p_tmp);
    }

    if (m_pLastSendMsg != NULL)
    {
        free(m_pLastSendMsg);
    }
}

/*******************************************************************************
*
* FUNCTION NAME : recv_xnl_message
*
*---------------------------------- PURPOSE ------------------------------------
* This function is to receive an XNL message through the socket.
*---------------------------------- SYNOPSIS -----------------------------------
*
* PARAMETERS:   pp_rcv_msg - pointer to a buffer, it is used to return the
                             received XNL message.
*
* RETURN VALUE: If the function successfully receives an XNL message, then return
*               true, otherwise false.
*
*******************************************************************************/
bool CXNLConnection::recv_xnl_message(uint8_t * * pp_rcv_msg)
{
    int expected_size       = 0;
    int total_size          = 0;
    uint8_t * p_msg_buf        = NULL;
    uint8_t * p_cur_buf        = NULL;
    int len                         = 0;
    unsigned short msg_len = 0;
    bool ret = true;

    /* Receive the first 2 bytes to get the message length */
    p_cur_buf = (uint8_t *)&msg_len;
    expected_size = sizeof(msg_len);
    while (expected_size > 0)
    {
        len = recv(m_socket, p_cur_buf, expected_size, 0);
        if (len == -1)
        {
            ret = false;
            break;
        }
        else if (len == 0) /* the socket is closed */
        {
            ret = false;
            break;
        }
        else
        {
            expected_size   -= len;
            total_size += len;
            p_cur_buf += len;
        }
    }
    if (ret == false)
    {
        return (ret);
    }
    /* continue to receive the payload */
    expected_size = ntohs(msg_len);
    /* malloc a buffer to receive the message */
    p_msg_buf = (uint8_t *) malloc(expected_size + 2);

    if (p_msg_buf == NULL)
    {
        return (false);
    }

    memset(p_msg_buf, 0, expected_size + 2);
    /* Copy the received 2 bytes to the message  */
    memcpy(p_msg_buf, (uint8_t *)&msg_len, sizeof(msg_len));

    p_cur_buf = p_msg_buf + 2;
    /* Continue to receive the whole XNL message */
    while (expected_size > 0)
    {
        len = recv(m_socket, p_cur_buf, expected_size, 0);
        if (len == -1)
        {
            ret = false;
            break;
        }
        else if (len == 0)
        {
            ret = false;
            break;
        }
        else
        {
            expected_size -= len;
            p_cur_buf += len;
            total_size += len;
        }
    }
    *pp_rcv_msg = p_msg_buf;

    return (ret);
}

void CXNLConnection::OnXnlMessageProcess(uint8_t* pBuf)
{
    unsigned short xnl_opcode = 0;
    XNL_NOTIFY_MSG *p_notify_msg = (XNL_NOTIFY_MSG *)malloc(sizeof(XNL_NOTIFY_MSG));
    unsigned short msg_len = 0;
    uint8_t * p_msg_buf = NULL;

    if ((pBuf == NULL) || (p_notify_msg == NULL))
    {
        return;
    }


    msg_len = ntohs(*((unsigned short *)pBuf)) + 2;
    m_handler->OnXnlMessageReceived(this, pBuf, msg_len);
    p_msg_buf = (uint8_t *)malloc(msg_len);
    memcpy(p_msg_buf, pBuf, msg_len);
    /* Forward the received message to main window to display the raw data message. */
    p_notify_msg->event = XNL_RCV_XCMP_MSG;
    p_notify_msg->p_msg = p_msg_buf;

    
    /* Get the xnl_opcode, byte 3 and byte 4 is the xnl opcode */
    xnl_opcode = ntohs(*((unsigned short *)(pBuf + 2)));
    switch (xnl_opcode)
    {
        case XNL_MASTER_STATUS_BROADCAST:
            /* start the xnl connection */
            decode_xnl_master_status_broadcast(pBuf);
            break;

        case XNL_AUTH_KEY_REPLY:
            if (m_XnlState == XNL_WAIT_AUTH_KEY_REPLY)
            {
                decode_xnl_auth_key_reply(pBuf);
            }
            /* ignore this message when the app is in other states */
            break;

        case XNL_CONNECTION_REPLY:
            if (m_XnlState == XNL_WAIT_CONN_REPLY)
            {
                decode_xnl_connection_reply(pBuf);
            }
            /* ignore this message when the app is in other states */
            break;

        case XNL_SYSMAP_BRDCST:
            if (m_XnlState >= XNL_WAIT_SYSMAP_BRDCST)
            {
                decode_xnl_sysmap_brdcst(pBuf);
            }
            break;

        case XNL_DATA_MSG:
            if (m_XnlState >= XNL_WAIT_DEV_INIT_STATUS)
            {
                decode_xnl_data_msg(pBuf);
            }
            break;

        case XNL_DATA_MSG_ACK:
            if (m_XnlState >= XNL_WAIT_DEV_INIT_STATUS)
            {
                decode_xnl_data_msg_ack(pBuf);
            }
            break;

        default: /* Unknown Opcode, just ignore it */
            break;

    }

}

void CXNLConnection::decode_xnl_master_status_broadcast(uint8_t * p_msg_buf)
{
    xnl_master_status_broadcast_t * p_msg = (xnl_master_status_broadcast_t *)p_msg_buf;
    if ((p_msg) && (ntohs(p_msg->msg_hdr.xnl_opcode) == XNL_MASTER_STATUS_BROADCAST))
    {
        /* Get the xnl address of the master device */
        m_xnl_dst_addr = ntohs(((xnl_msg_hdr_t *)p_msg)->src_addr);
        send_xnl_auth_key_request();
        m_XnlState = XNL_WAIT_AUTH_KEY_REPLY;
    }
}

void CXNLConnection::send_xnl_auth_key_request()
{
    /* malloc a buffer for the auth key request message */
    xnl_auth_key_request_t * p_msg = (xnl_auth_key_request_t *)malloc(sizeof(xnl_auth_key_request_t));

    if (!p_msg) {
        return;
    }
    /* compose the xnl authentication key request */
    p_msg->msg_hdr.msg_len = htons(XNL_AUTH_KEY_REQ_SIZE - 2); /* exclude the 2 bytes length field */
    p_msg->msg_hdr.xnl_opcode = htons(XNL_AUTH_KEY_REQ);
    p_msg->msg_hdr.xnl_flag = 0;
    p_msg->msg_hdr.protocol_id = 0;
    p_msg->msg_hdr.src_addr = 0;
    p_msg->msg_hdr.dst_addr = htons(m_xnl_dst_addr);
    p_msg->msg_hdr.trans_id = 0;
    p_msg->msg_hdr.payload_len = 0;

    send_xnl_message((uint8_t *)p_msg);
    free(p_msg);

}

void CXNLConnection::decode_xnl_auth_key_reply(uint8_t *p_msg_buf)
{
    xnl_auth_key_reply_t *p_msg = (xnl_auth_key_reply_t *)p_msg_buf;


    if ((p_msg != NULL) && (ntohs(p_msg->msg_hdr.xnl_opcode) == XNL_AUTH_KEY_REPLY))
    {
        /* Get the temporary xnl address from the message */
        m_xnl_src_addr = ntohs(p_msg->tmp_xnl_addr);
        send_xnl_connection_request((uint8_t*)p_msg->auth_seed);
        m_XnlState = XNL_WAIT_CONN_REPLY;
    }
}


void CXNLConnection::send_xnl_connection_request(uint8_t *p_auth_seed)
{
    uint32_t * p_int = (uint32_t *)p_auth_seed;
    xnl_conn_request_t *p_msg = (xnl_conn_request_t *)malloc(sizeof(xnl_conn_request_t));

    if (p_msg == NULL)
    {
        return;
    }
    p_msg->msg_hdr.msg_len = htons(XNL_CONNECTION_REQUEST_SIZE - 2);
    p_msg->msg_hdr.xnl_opcode = htons(XNL_CONNECTION_REQUEST);
    p_msg->msg_hdr.xnl_flag = 0;
    p_msg->msg_hdr.protocol_id = 0;
    p_msg->msg_hdr.src_addr = htons(m_xnl_src_addr);
    p_msg->msg_hdr.dst_addr = htons(m_xnl_dst_addr);
    p_msg->msg_hdr.trans_id = 0;
    p_msg->msg_hdr.payload_len = htons(0x000c);
    p_msg->preferred_xnl_addr = 0;
    p_msg->dev_type = 0x0A;
    p_msg->auth_index = 0x01;

    *p_int = ntohl(*p_int);
    *(p_int + 1) = ntohl(*(p_int+1));

    /* encrypted the authentication seed */
    encipher((uint32_t *)p_auth_seed, encrypted_seed, m_auth_key);
    encrypted_seed[0] = htonl(encrypted_seed[0]);
    encrypted_seed[1] = htonl(encrypted_seed[1]);
    memcpy(p_msg->auth_key, encrypted_seed, sizeof(encrypted_seed));

    send_xnl_message((uint8_t *)p_msg);
    free(p_msg);
}

void CXNLConnection::encipher(uint32_t *const v,
                              uint32_t *const w,
                              const uint32_t *const k)
{
     uint32_t y=v[0], z=v[1], sum=0;
     uint32_t delta= m_delta;
     uint32_t a=k[0], b=k[1], c=k[2], d=k[3];
     uint32_t n=32;

    while(n-->0)
    {
        sum += delta;
        y += (z << 4)+a ^ z+sum ^ (z >> 5)+b;
        z += (y << 4)+c ^ y+sum ^ (y >> 5)+d;
    }

    w[0]=y; w[1]=z;
}

void CXNLConnection::decipher(uint32_t *const v,
                 uint32_t *const w,
                 const uint32_t *const k)
{
     uint32_t y=v[0], z=v[1];
     uint32_t delta = m_delta;
     uint32_t sum = delta * 32;
     uint32_t a=k[0],b=k[1], c=k[2], d=k[3];
     uint32_t n=32;

    /* sum = delta<<5, in general sum = delta * n */
    while(n-->0)
    {
         z -= (y << 4)+c ^ y+sum ^ (y >> 5)+d;
         y -= (z << 4)+a ^ z+sum ^ (z >> 5)+b;
         sum -= delta;
    }

    w[0]=y; w[1]=z;
}

void CXNLConnection::decode_xnl_connection_reply(uint8_t *p_msg_buf)
{
    xnl_conn_reply_t * p_msg = (xnl_conn_reply_t *)p_msg_buf;
    uint32_t deciphered_seed[2] = {0};
    uint32_t *p_encripted_seed = NULL;
    XNL_NOTIFY_MSG *p_notify_msg = (XNL_NOTIFY_MSG *)malloc(sizeof(XNL_NOTIFY_MSG));

    if ((p_msg != NULL) && (ntohs(p_msg->msg_hdr.xnl_opcode) == XNL_CONNECTION_REPLY))
    {
        if (p_msg->result_code == XNL_SUCESS) /* xnl connection success */
        {
            /* Verify the Master device's authentication key */
            p_encripted_seed = (uint32_t *)p_msg->encrypted_seed;
            /* Convert the byte order to Little Endian */
            *p_encripted_seed = ntohl(*p_encripted_seed);
            *(p_encripted_seed + 1) = ntohl(*(p_encripted_seed + 1));
            decipher(p_encripted_seed, deciphered_seed, m_auth_key);

            /* convert the deciphered_seed to Big endian */
            deciphered_seed[0] = htonl(deciphered_seed[0]);
            deciphered_seed[1] = htonl(deciphered_seed[1]);

            /* Compare the deciphered_seed */
            if ((deciphered_seed[0] == encrypted_seed[0]) && (deciphered_seed[1] == encrypted_seed[1]))
            {
                m_trans_id_base = p_msg->trans_id_base;
                /* Get the new assigned xnl address from the message */
                m_xnl_src_addr = ntohs(p_msg->assigned_xnl_addr);

                m_XnlState = XNL_WAIT_SYSMAP_BRDCST;
                free(p_notify_msg);
            }
            else /* Reject the xnl connection */
            {
                m_XnlState = XNL_WAIT_MASTER_BRDCST;
                p_notify_msg->event = XNL_CONNECTION_FAILURE;
                p_notify_msg->p_msg = NULL;
            }


        }
        else /* xnl connection failure */
        {
            /* Retry the authentication process by sending out a new AUTH_KEY_REQUEST message */
            if (conn_retry < 3)
            {
                send_xnl_auth_key_request();
                m_handler->OnConnectionFailure(this);
            }
        }
    }
}

void CXNLConnection::decode_xnl_sysmap_brdcst(uint8_t *p_msg_buf)
{
    xnl_sysmap_brdcst_t *p_msg = (xnl_sysmap_brdcst_t *)p_msg_buf;
    if ((p_msg != NULL) && (ntohs(p_msg->msg_hdr.xnl_opcode) == XNL_SYSMAP_BRDCST))
    {
        /* Since this PC application just communicates with the radio, it does
         * not care the other xcmp devices. So no need to parse this message. */
        if (m_XnlState == XNL_WAIT_SYSMAP_BRDCST)
        {
            m_XnlState = XNL_WAIT_DEV_INIT_STATUS;
        }
        /* else just keep the current state. It's for the case that the application
         * receives the broadcast when the xnl connection has already created.*/

    }
}

/* Call xcmp handler to process the xcmp message */
void CXNLConnection::decode_xnl_data_msg(uint8_t * p_msg_buf)
{
    xnl_msg_hdr_t * p_msg = (xnl_msg_hdr_t *)p_msg_buf;

    if (p_msg != NULL)
    {
        /* First send back an ACK message for the received xnl data message */
        send_xnl_data_msg_ack(p_msg_buf);

        /* Validate the received xnl data message */
        /* Make sure the destination address is correct (target to the app) */
        if ((ntohs(p_msg->dst_addr) != 0) && (ntohs(p_msg->dst_addr) != m_xnl_src_addr))
        {
            /* Just ignore this message */
        }
        else if (ntohs(p_msg->src_addr) != m_xnl_dst_addr) /* Make sure the source address is correct (only receive messages from the radio) */
        {
            /* Just ignore this message */
        }
        else
        {
            /* Check the xnl_flag to determine whether it's a new xcmp message or resent message */
            if (m_rx_xnl_flag != p_msg->xnl_flag)
            {
                OnXCMPMessageProcess(p_msg_buf);
                /* save the xnl_flag */
                m_rx_xnl_flag = p_msg->xnl_flag;

            }
            /* Ignore the resent message */
        }

    }

}

void CXNLConnection::decode_xnl_data_msg_ack(uint8_t * p_msg_buf)
{
    xnl_data_msg_ack_t *p_msg = (xnl_data_msg_ack_t *)p_msg_buf;

    if ((p_msg_buf != NULL) && (m_bWaitForAck == true))
    {
        /* check the xnl flag */
        if (m_tx_xnl_flag == p_msg->msg_hdr.xnl_flag)
        {
            /* reset m_bWaitForAck */
            m_bWaitForAck = false;
            if (m_tx_xnl_flag < 7)
            {
                m_tx_xnl_flag ++;
            }
            else
            {
                m_tx_xnl_flag = 0;
            }
            m_TxXCMPCount ++;
            /* free the saved msg */
            if (m_pLastSendMsg != NULL)
            {
                free(m_pLastSendMsg);
                m_pLastSendMsg = NULL;
            }
            /* Stop the timer */
        }
    }
}

void CXNLConnection::OnXCMPMessageProcess(uint8_t * pBuf)
{
    unsigned short xcmp_opcode = 0;

    if (pBuf == NULL)
    {
        return;
    }
    xnl_msg_hdr_t *p_xnl_hdr = (xnl_msg_hdr_t *)pBuf;
    xcmp_opcode = ntohs(*((unsigned short *)(pBuf + sizeof(xnl_msg_hdr_t))));

    switch (xcmp_opcode)
    {
        case XCMP_DEVICE_INIT_STATUS_BRDCST:
            decode_xcmp_dev_init_status(pBuf);
            break;
	case XCMP_CALL_CTRL_BRDCST:
	{
		xcmp_call_ctrl_broadcast_t *msg = (xcmp_call_ctrl_broadcast_t *)pBuf;
		uint32_t* d_addr = (uint32_t*)&msg->rmt_addr.rmt_addr[0];
		std::string addr =  std::to_string(ntohl(*d_addr) >> 8);

		switch (msg->call_state) {
		    case XCMP_CALL_INITIATED:
		    case XCMP_CALL_DECODED:
			m_handler->OnCallInitiated(this, addr);
		    break;
		    case XCMP_CALL_ENDED:
			m_handler->OnCallEnded(this);
		    break;
		}
        } break;
        default: /* just forward the xcmp message to the application */
            /* The XCMP message has already forwarded to the Main process, so do nothing here. */
            break;
    }
}




void CXNLConnection::decode_xcmp_dev_init_status(uint8_t * p_msg_buf)
{
    xcmp_dev_init_status_t *p_msg = (xcmp_dev_init_status_t *)p_msg_buf;
    XNL_NOTIFY_MSG *p_notify_msg = NULL;
    int offset = 0;

    if ((p_msg != NULL) && (ntohs(p_msg->msg_hdr.xnl_opcode) == XNL_DATA_MSG) &&
        (ntohs(p_msg->xcmp_opcode) == XCMP_DEVICE_INIT_STATUS_BRDCST))
    {

        /* parse the xcmp device init status message */
        if (p_msg->dev_init_type == 0x00) /* Device Init Status */
        {

            if (ntohs(p_msg->dev_status) != 0x0000)/* check device's status */
            {
                /* The radio reports that it encounters error during power up */
                /* what should the radio do next? */
            }
            else /* the radio is ok */
            {
               /* Get the XCMP version */
               memcpy((uint8_t *)&m_XCMP_ver, p_msg->xcmp_ver, 4);
               m_XCMP_ver = ntohl(m_XCMP_ver);
               /* this application is only applicable for display radio */
               while (offset < p_msg->array_size) /* parse the device descriptor */
               {
                   switch (*(p_msg_buf + 11 + offset))
                   {
                       case XCMP_DEVICE_FAMILY:
                           /* check whether it's MOTOTRBO radio */

                           break;

                       case XCMP_DEVICE_DISPLAY:
                           /* check whether it's display radio */
                           break;

                       case XCMP_DEVICE_RF_BAND:
                           /* This PC application doesn't care this descriptor */
                           break;

                       case XCMP_DEVICE_GPIO_CTRL:
                           /* This PC application doesn't care this descriptor */
                           break;

                       case XCMP_DEVICE_RADIO_TYPE:
                           /* This PC application doesn't care this descriptor */
                           break;

                       case XCMP_DEVICE_KEYPAD:
                           /* This PC application doesn't care this descriptor */
                           break;

                       case XCMP_DEVICE_CHANNEL_KNOB:
                           /* This PC application doesn't care this descriptor */
                           break;

                       case XCMP_DEVICE_VIRTUAL_PERSONALITY_SUPPORT:
                           /* This PC application doesn't care this descriptor */
                           break;

                       default: /* Just ignore unknown descriptor */
                           break;
                   }
                   offset += 2;
               }
               send_xcmp_dev_init_status();
            }
            m_XnlState = XNL_WAIT_DEV_INIT_COMPLETE;

        }
        else if (p_msg->dev_init_type == 0x01) /* Device Init Complete */
        {
            m_XnlState = XNL_INIT_COMPLETE;
            m_bConnected = true;

            m_handler->OnConnectionSuccess(this);
        }
        else /* wrong message */
        {
            /* it's radio's error. report the error */
        }
    }
}

void CXNLConnection::send_xcmp_dev_init_status()
{
    xcmp_dev_init_status_t *p_xcmp_msg = (xcmp_dev_init_status_t *)malloc(sizeof(xcmp_dev_init_status_t) + 3);
    int payload_len = sizeof(xcmp_dev_init_status_t) + 3 - sizeof(xnl_msg_hdr_t);

    if (p_xcmp_msg == NULL)
    {
        return;
    }
    p_xcmp_msg->xcmp_opcode = htons(XCMP_DEVICE_INIT_STATUS_BRDCST);
    p_xcmp_msg->xcmp_ver[0] = 0x00;
    p_xcmp_msg->xcmp_ver[1] = 0x00;
    p_xcmp_msg->xcmp_ver[2] = 0x00;
    p_xcmp_msg->xcmp_ver[3] = m_XCMP_ver;  /* echo the xcmp version */
    p_xcmp_msg->dev_init_type = 0x00;
    p_xcmp_msg->dev_type = 0x0A;
    p_xcmp_msg->dev_status = 0x0000;
    p_xcmp_msg->array_size = 0x04;
    p_xcmp_msg->dev_descriptor_array[0] = 0x00;  /* XCMP_DEVICE_FAMILY */
    p_xcmp_msg->dev_descriptor_array[1] = 0x00;
    p_xcmp_msg->dev_descriptor_array[2] = 0x02;  /* XCMP_DEVICE_DISPLAY */
    p_xcmp_msg->dev_descriptor_array[3] = 0xFF;

    init_xnl_header_of_xcmp_msg((uint8_t *)p_xcmp_msg, payload_len);
    /* Add the message to the sending queue */
    enqueue_msg((uint8_t *)p_xcmp_msg);

}

void CXNLConnection::init_xnl_header_of_xcmp_msg(uint8_t *p_msg, int payload_len)
{
    xnl_msg_hdr_t *p_xnl_hdr = (xnl_msg_hdr_t *)p_msg;

    if (p_msg != NULL)
    {
          p_xnl_hdr->msg_len = htons(sizeof(xnl_msg_hdr_t) + payload_len - 2);
          p_xnl_hdr->xnl_opcode = htons(XNL_DATA_MSG);
          p_xnl_hdr->protocol_id = 0x01; /* xcmp command message */
          p_xnl_hdr->dst_addr = htons(m_xnl_dst_addr);
          p_xnl_hdr->src_addr = htons(m_xnl_src_addr);
          p_xnl_hdr->payload_len = htons(payload_len);
          /* xnl_flag and trans_id will be set when it is sent out, here just set them to 0 */
          p_xnl_hdr->xnl_flag = 0;
          p_xnl_hdr->trans_id = 0;
    }
}

bool CXNLConnection::send_xcmp_brightness_msg(uint8_t function, uint8_t intensity)
{
    xcmp_brightness_request_t *p_msg = (xcmp_brightness_request_t *) malloc(sizeof(xcmp_brightness_request_t));
    int payload_len = sizeof(xcmp_brightness_request_t) - sizeof(xnl_msg_hdr_t);

    if (p_msg == NULL)
    {
        return (false);
    }

    p_msg->xcmp_opcode = htons(XCMP_BRIGHTNESS_REQ);
    p_msg->function = function;
    p_msg->intensity = intensity;

    init_xnl_header_of_xcmp_msg((uint8_t *)p_msg, payload_len);
    /* Add the message to the sending queue */
    enqueue_msg((uint8_t *)p_msg);

    return (true);
}

bool CXNLConnection::send_xcmp_pui_brdcst(uint8_t pui_type,
                                                 unsigned short pui_id,
                                                 uint8_t           pui_state,
                                                 uint8_t           pui_state_min,
                                                 uint8_t           pui_state_max)
{
    xcmp_pui_broadcast_t *p_msg = (xcmp_pui_broadcast_t *)malloc(sizeof(xcmp_pui_broadcast_t));

    if (p_msg == NULL)
    {
        return false;
    }

    p_msg->xcmp_opcode = htons(XCMP_PHYSICAL_USER_INPUT_BRDCST);
    p_msg->pui_source = 0x00; /* Control Head / Control Device Input */
    p_msg->pui_type = pui_type;
    p_msg->pui_id = htons(pui_id);
    p_msg->pui_state = pui_state;
    p_msg->pui_state_min = pui_state_min;
    p_msg->pui_state_max = pui_state_max;

    init_xnl_header_of_xcmp_msg((uint8_t *)p_msg, 9);
    /* Add the message to the sending queue */
    enqueue_msg((uint8_t *)p_msg);

    return (true);
}

void CXNLConnection::Call()
{
    send_xcmp_tx_ctrl_request(1,0,1);
    //this->send_xcmp_call_ctrl_request(1, 6, 1, std::stoi(addr), std::stoi(addr));
}

void CXNLConnection::PTT(PTT_FUNCTION ptt_function)
{
    send_xcmp_tx_ctrl_request(ptt_function,0, 1);

    //this->send_xcmp_call_ctrl_request(1, 6, 1, std::stoi(addr), std::stoi(addr));
}

bool CXNLConnection::send_xcmp_call_ctrl_request(uint8_t function,
                                                 uint8_t call_type,
                                                 uint8_t addr_type,
                                                 uint32_t rmt_addr,
                                                 uint32_t group_id)
{
    xcmp_call_ctrl_request_t *p_msg = NULL;
    unsigned short            msg_size = sizeof(xcmp_call_ctrl_request_t);
    uint8_t *p_addr = (uint8_t *)&rmt_addr;
    uint8_t * p_cur_msg = NULL;
    xcmp_group_id_t tmp_group_id = {0};

    if (call_type == 0x06) /* if the call type is group call, then need to add group id structure in the msg */
    {
        msg_size += sizeof(xcmp_group_id_t);
    }

    if (addr_type == 0x01) /* if the addr type is MOTOTRBO ID, then the addr size must be set to 3 */
    {
        msg_size += 3; /* 1 byte for addr structure and the other 2 bytes for address port */
    }
    else if (addr_type == 0x05) /* It's a MDC ID */
    {
        msg_size += 2; /* add 2 bytes for address port */
    }
    else
    {
        /* Invalid address type */
        return false;
    }

    p_msg = (xcmp_call_ctrl_request_t *)malloc(msg_size);

    if (p_msg == NULL)
    {
        return false;
    }
    memset((uint8_t *)p_msg, 0, msg_size);

    p_msg->xcmp_opcode = htons(XCMP_CALL_CTRL_REQ);
    p_msg->function = function; /* Control Head / Control Device Input */
    p_msg->call_type = call_type;
    p_msg->rmt_addr.addr_type = addr_type;

    rmt_addr = htonl(rmt_addr);

    if (addr_type == 0x01) /* if the addr type is MOTOTRBO ID, then the addr size must be set to 3 */
    {

        p_msg->rmt_addr.addr_size = 3;
        /* since MOTOTRBO ID just has 3 bytes, so we shall copy the addr starting from the second byte */
        memcpy(p_msg->rmt_addr.rmt_addr, p_addr + 1, 3);
        p_cur_msg = (uint8_t *)p_msg + sizeof(xcmp_call_ctrl_request_t) + 3;
    }
    else /* it is a MDC ID, the size must be set to 2 */
    {
        p_msg->rmt_addr.addr_size = 2;
        memcpy(p_msg->rmt_addr.rmt_addr, p_addr + 2, 2);
        p_cur_msg = (uint8_t *)p_msg + sizeof(xcmp_call_ctrl_request_t) + 2;
    }

    if (call_type == 0x06) /* init the group id structure */
    {
        tmp_group_id.id_size = 3;
        /* Convert the byte order to Big Endian */
        group_id = htonl(group_id);
        /* since the group id just has 3 bytes, so we shall copy the id starting from the second byte */
        memcpy(tmp_group_id.group_id, (uint8_t *)&group_id + 1, 3);
        memcpy(p_cur_msg, (uint8_t *)&tmp_group_id, 4);
    }

    init_xnl_header_of_xcmp_msg((uint8_t *)p_msg, msg_size - sizeof(xnl_msg_hdr_t));
    /* Add the message to the sending queue */
    enqueue_msg((uint8_t *)p_msg);

    return (true);
}

bool CXNLConnection::send_xcmp_rmt_radio_ctrl_request(uint8_t feature,
                                                 uint8_t operation,
                                                 uint8_t addr_type,
                                                 uint32_t rmt_addr)
{
    xcmp_rmt_radio_ctrl_request_t *p_msg = NULL;
    unsigned short            msg_size = sizeof(xcmp_rmt_radio_ctrl_request_t) + 3; /* the address size is 3, so need to plus 1 */
    uint8_t *p_addr = (uint8_t *)&rmt_addr;
    uint8_t * p_cur_msg = NULL;

    p_msg = (xcmp_rmt_radio_ctrl_request_t *)malloc(msg_size);

    if (p_msg == NULL)
    {
        return false;
    }
    memset((uint8_t *)p_msg, 0, msg_size);

    p_msg->xcmp_opcode = htons(XCMP_RMT_RADIO_CTRL_REQ);
    p_msg->feature = feature;
    p_msg->operation = operation;
    p_msg->rmt_addr.addr_type = addr_type;
    p_msg->rmt_addr.addr_size = 3;

    rmt_addr = htonl(rmt_addr);

    if (addr_type == 0x01) /* if the addr type is MOTOTRBO ID, then the addr size must be set to 3 */
    {

        p_msg->rmt_addr.addr_size = 3;
        /* since MOTOTRBO ID just has 3 bytes, so we shall copy the addr starting from the second byte */
        memcpy(p_msg->rmt_addr.rmt_addr, p_addr + 1, 3);
    }

    init_xnl_header_of_xcmp_msg((uint8_t *)p_msg, msg_size - sizeof(xnl_msg_hdr_t));
    /* Add the message to the sending queue */
    enqueue_msg((uint8_t *)p_msg);

    return (true);
}

bool CXNLConnection::send_xcmp_menu_navigation_request(uint8_t function,
                                                      uint8_t display_mode,
                                                      unsigned short menu_id,
                                                      unsigned short  count)
{
    xcmp_menu_navigation_request_t *p_msg = NULL;
    int payload_len = 0;


    p_msg = (xcmp_menu_navigation_request_t *)malloc(sizeof(xcmp_menu_navigation_request_t));

    if (p_msg == NULL)
    {
        return false;
    }

    p_msg->xcmp_opcde = htons(XCMP_MENU_NAVIGATION_REQ);
    p_msg->function = function;
    p_msg->display_mode = display_mode;
    p_msg->menu_id = htons(menu_id);

    if (m_XCMP_ver == 1)
    {
       p_msg->data[0] = count;
       payload_len = sizeof(xcmp_menu_navigation_request_t) - sizeof(xnl_msg_hdr_t)- 1;
    }
    else
    {
        count = htons(count);
        memcpy(p_msg->data, (uint8_t *)&count, 2);
        payload_len = sizeof(xcmp_menu_navigation_request_t) - sizeof(xnl_msg_hdr_t);
    }

    init_xnl_header_of_xcmp_msg((uint8_t *)p_msg, payload_len);

    /* Add the message to the sending queue */
    enqueue_msg((uint8_t *)p_msg/*, true*/);

    return (true);
}


bool CXNLConnection::send_xcmp_chan_zone_selection_request(uint8_t function,
                                                                        unsigned short zone_num,
                                                                        unsigned short chan_num)
{
    xcmp_chan_zone_selection_request_t *p_msg = (xcmp_chan_zone_selection_request_t *) malloc(sizeof(xcmp_chan_zone_selection_request_t));
    int payload_len = sizeof(xcmp_chan_zone_selection_request_t) - sizeof(xnl_msg_hdr_t) - 1;
    uint8_t * p_cur_ch = NULL;

    if (p_msg == NULL)
    {
        return (false);
    }

    p_msg->xcmp_opcode = htons(XCMP_CHAN_SELECTION_REQ);
    p_msg->function = function;

    p_cur_ch = (uint8_t *)&p_msg->function;
    /* copy the zone number into the message */
    zone_num = htons(zone_num);
    memcpy(p_cur_ch + 1, (uint8_t *)&zone_num, sizeof(zone_num));

    /* copy the chan number into the message */
    chan_num = htons(chan_num);
    memcpy(p_cur_ch + 3, (uint8_t *)&chan_num, sizeof(chan_num));

    init_xnl_header_of_xcmp_msg((uint8_t *)p_msg, payload_len);
    /* Add the message to the sending queue */
    enqueue_msg((uint8_t *)p_msg);

    return (true);

}

void CXNLConnection::select_mic(uint8_t mic)
{

    xcmp_mic_ctrl_request_t *p_msg = (xcmp_mic_ctrl_request_t *) malloc(sizeof(xcmp_mic_ctrl_request_t));
    int payload_len = sizeof(xcmp_mic_ctrl_request_t) - sizeof(xnl_msg_hdr_t);

    if (p_msg == NULL)
    {
        return;
    }

    p_msg->xcmp_opcode = htons(XCMP_MIC_CTRL_REQ);
    p_msg->function = 0x03;
    p_msg->mic = mic;
    p_msg->signaling = 0xff;
    p_msg->gain = 0x00;

    init_xnl_header_of_xcmp_msg((uint8_t *)p_msg, payload_len);
    /* Add the message to the sending queue */
    enqueue_msg((uint8_t *)p_msg);
}

bool CXNLConnection::send_xcmp_tx_ctrl_request(uint8_t function, uint8_t mode, uint8_t source)
{
    xcmp_tx_ctrl_request_t *p_msg = (xcmp_tx_ctrl_request_t *)malloc(sizeof(xcmp_tx_ctrl_request_t));
    int payload_len = sizeof(xcmp_tx_ctrl_request_t) - sizeof(xnl_msg_hdr_t);

    if (p_msg == NULL)
    {
        return false;
    }

    p_msg->xcmp_opcode = htons(XCMP_TX_CTRL_REQ);
    p_msg->function = function;
    p_msg->mode = mode;
    p_msg->source = source;

    init_xnl_header_of_xcmp_msg((uint8_t *)p_msg, payload_len);
    /* Add the message to the sending queue */
    enqueue_msg((uint8_t *)p_msg);

    return (true);

}

bool CXNLConnection::send_xcmp_radio_status_request(uint8_t condition)
{
    xcmp_radio_status_request_t *p_msg = (xcmp_radio_status_request_t *)malloc(sizeof(xcmp_radio_status_request_t));
    int payload_len = sizeof(xcmp_radio_status_request_t) - sizeof(xnl_msg_hdr_t) - 1;

    if (p_msg == NULL)
    {
        return false;
    }

    p_msg->xcmp_opcode = htons(XCMP_RADIO_STATUS_REQ);
    p_msg->condition = condition;

    init_xnl_header_of_xcmp_msg((uint8_t *)p_msg, payload_len);
    /* Add the message to the sending queue */
    enqueue_msg((uint8_t *)p_msg);

    return (true);
}

/* p_xcmp_data_msg - pointer to the received xcmp data message */
void CXNLConnection::send_xnl_data_msg_ack(uint8_t * p_xcmp_data_msg)
{
    xnl_data_msg_ack_t *p_msg_ack = (xnl_data_msg_ack_t *)malloc(sizeof(xnl_data_msg_ack_t));

    if (p_msg_ack == NULL)
    {
        return;
    }

    p_msg_ack->msg_hdr.msg_len = htons(XNL_DATA_MSG_ACK_SIZE - 2);
    p_msg_ack->msg_hdr.xnl_opcode = htons(XNL_DATA_MSG_ACK);
    p_msg_ack->msg_hdr.protocol_id = 0x01;  /* xcmp command message */
    p_msg_ack->msg_hdr.xnl_flag = ((xnl_msg_hdr_t *)p_xcmp_data_msg)->xnl_flag;
    p_msg_ack->msg_hdr.dst_addr = ((xnl_msg_hdr_t *)p_xcmp_data_msg)->src_addr;
    p_msg_ack->msg_hdr.src_addr = htons(m_xnl_src_addr);;
    p_msg_ack->msg_hdr.trans_id = ((xnl_msg_hdr_t *)p_xcmp_data_msg)->trans_id;
    p_msg_ack->msg_hdr.payload_len = 0;

    /* For ACK message, don't add it to the sending queue, but directly send out it */
    send_xnl_message((uint8_t *)p_msg_ack);
    free(p_msg_ack);

}

bool CXNLConnection::send_xnl_message(uint8_t *p_msg_buf)
{
    int len = 0;
    int n_left = 0;
    unsigned short msg_len = 0;
    uint8_t *p_cur_buf = p_msg_buf;
    XNL_NOTIFY_MSG *p_notify_msg = (XNL_NOTIFY_MSG *)malloc(sizeof(XNL_NOTIFY_MSG));
    uint8_t *p_raw_data = NULL;

    bool ret = true;

    if (p_msg_buf == NULL)
    {
        return (ret);
    }
    /* The first two bytes is the xnl message's length field */
    msg_len = ntohs(*((unsigned short *)p_msg_buf)) + 2; /* 2 is the length of "length field". */
    n_left = msg_len;

    while (n_left > 0)
    {
        len = send(m_socket, p_msg_buf, msg_len, 0);
        if (len == -1)
        {
            ret = false;
            break;
        }
        else if (len == 0)
        {
            ret = false;
            break;
        }
        n_left -= len;
        p_cur_buf += len;
    }

    /* Send the message to main window for raw data display */
    if (msg_len == 0xef00) {
        msg_len = 0;
    }
    p_notify_msg->event = XNL_SENT_XCMP_MSG;
    /* copy the sending message */
    p_raw_data = (uint8_t *)malloc(msg_len);
    memcpy(p_raw_data, p_msg_buf, msg_len);
    p_notify_msg->p_msg = p_raw_data;
    //::PostMessage(m_hWnd, WM_RX_XCMP_MESSAGE, NULL, (LPARAM)p_notify_msg);

    return (ret);
}

/* Add the message to the list tail. */
void CXNLConnection::enqueue_msg(uint8_t * p_msg)
{
    MSG_QUEUE_T *p_new_node = NULL;

    if (p_msg != NULL)
    {
        /* Create a node for the message */
        p_new_node = (MSG_QUEUE_T *)malloc(sizeof(MSG_QUEUE_T));

        if (p_new_node == NULL)
        {
            return;
        }

        p_new_node->p_msg = p_msg;
        p_new_node->next = NULL;

        if (m_pSendQueHdr == NULL)
        {
            m_pSendQueHdr = p_new_node;
            m_pSendQueTail = p_new_node;
        }
        else /* Append the message to the list tail */
        {
            m_pSendQueTail->next = p_new_node;
            m_pSendQueTail = p_new_node;
        }
    }

}

/* Remove the header from the list */
MSG_QUEUE_T * CXNLConnection::dequeue_msg()
{
    MSG_QUEUE_T *p_cur_node = NULL;

    if (m_pSendQueHdr != NULL)
    {
        /* Remove the first node of the list */
        p_cur_node = m_pSendQueHdr;
        m_pSendQueHdr = m_pSendQueHdr->next;

        if (m_pSendQueHdr == NULL)
        {
            m_pSendQueTail = NULL;
        }
    }

    return (p_cur_node);
}

