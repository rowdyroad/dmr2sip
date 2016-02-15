
//The MOTOTRBO ADP XCMP Console Demo Application software is considered Motorola Confidential Restricted information and
//contains intellectual property owned by Motorola, Inc., which is protected by U.S. copyright laws and international
//copyright treaties.  Unauthorized reproduction or distribution of this software is strictly prohibited.  2009 Motorola,
//Inc. All Rights Reserved.

#pragma once
#include "xnl_messages_def.h"
#include "xcmp_msg_defs.h"
#include <string>
#include <time.h>
#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <unistd.h>
#include <exception>
#include <string.h>
#include <errno.h>

typedef enum
{
    XNL_WAIT_MASTER_BRDCST = 1,
    XNL_WAIT_AUTH_KEY_REPLY,
    XNL_WAIT_CONN_REPLY,
    XNL_WAIT_SYSMAP_BRDCST,
    XNL_WAIT_DEV_INIT_STATUS,
    XNL_WAIT_DEV_INIT_COMPLETE,
    XNL_INIT_COMPLETE,
} XNL_CONNECTION_STATE;


typedef enum {
    PTT_PUSH = 1,
    PTT_RELEASE
} PTT_FUNCTION;

typedef struct msg_queue{
    uint8_t * p_msg;
    struct msg_queue *next;
}MSG_QUEUE_T;

typedef enum{
    XNL_CONNECTION_SUCCESS = 0,
    XNL_CONNECTION_FAILURE = 1,
    XNL_RCV_XCMP_MSG = 2,
    XNL_SENT_XCMP_MSG = 3,
}XNL_EVENT;

typedef struct {
    XNL_EVENT event;
    uint8_t *p_msg;
} XNL_NOTIFY_MSG;


class CXNLConnection;

class CXNLConnectionException : public std::exception
{
    private:
        int code_;
        std::string message_;
    public:
        CXNLConnectionException(int code, const std::string& message)  noexcept
            : code_(code)
            , message_(message_)
        {}

        CXNLConnectionException() noexcept
            : code_(errno)
            , message_(strerror(errno))
        {}

        int getCode() const
        {
            return code_;
        }

        const std::string& getMessage() const
        {
            return message_;
        }

        virtual const char* what() const noexcept
        {
            return message_.c_str();
        }
};

class CXNLConnectionHandler
{
    public:
        virtual void OnConnectionSuccess(CXNLConnection* connection) { }
        virtual void OnConnectionFailure(CXNLConnection* connection) { }
        virtual void OnXnlMessageReceived(CXNLConnection* connection, uint8_t* msg, size_t len)  { }
	virtual void OnXnlMessageSent(CXNLConnection* connection, uint8_t* msg, size_t len) { }
        virtual void OnCallInitiated(CXNLConnection* connection, const std::string& address)  { }
	virtual void OnCallEnded(CXNLConnection* connection) { }
};

class CXNLConnection
{
  public:
    CXNLConnection(const std::string& host, uint16_t port, const std::string& auth_key, uint32_t delta, CXNLConnectionHandler* handler);
    ~CXNLConnection();


    void Call();
    void PTT(PTT_FUNCTION ptt_function);
    bool send_xcmp_brightness_msg(uint8_t function, uint8_t intensity);
    bool send_xcmp_pui_brdcst(uint8_t pui_type, uint16_t pui_id, uint8_t pui_state, uint8_t pui_state_min, uint8_t pui_state_max);
    bool send_xcmp_call_ctrl_request(uint8_t function, uint8_t call_type, uint8_t addr_type, uint32_t rmt_addr, uint32_t group_id);
    bool send_xcmp_rmt_radio_ctrl_request(uint8_t feature, uint8_t operation, uint8_t addr_type, uint32_t rmt_addr);
    bool send_xcmp_menu_navigation_request(uint8_t function, uint8_t display_mode, uint16_t menu_id, uint16_t  count);
    bool send_xcmp_chan_zone_selection_request(uint8_t function, uint16_t zone_num, uint16_t chan_num);
    bool send_xcmp_tx_ctrl_request(uint8_t function, uint8_t mode, uint8_t source);
    bool send_xcmp_radio_status_request(uint8_t condition);


    void select_mic(uint8_t mic);
    void Run();
    void Stop();

  private:
    void OnXnlMessageProcess(uint8_t* pBuf);
    void OnXCMPMessageSent(uint8_t* pBuf);
    void OnXCMPMessageProcess(uint8_t *pBuf);

    bool recv_xnl_message(uint8_t **pp_rcv_msg);
    bool send_xnl_message(uint8_t * p_msg_buf);
    void decode_xnl_master_status_broadcast(uint8_t *p_msg_buf);
    void decode_xnl_auth_key_reply(uint8_t *p_msg_buf);
    void decode_xnl_connection_reply(uint8_t *p_msg_buf);
    void decode_xnl_sysmap_brdcst(uint8_t *p_msg_buf);
    void decode_xnl_data_msg(uint8_t *p_msg_buf);
    void decode_xnl_data_msg_ack(uint8_t *p_msg_buf);
    void decode_xcmp_dev_init_status(uint8_t *p_msg_buf);
    void send_xnl_auth_key_request();
    void send_xnl_connection_request(uint8_t *p_auth_key);
    void send_xcmp_dev_init_status();
    void send_xnl_data_msg_ack(uint8_t *p_xcmp_data_msg);
    void init_xnl_header_of_xcmp_msg(uint8_t *p_msg, int payload_len);
    void encipher(uint32_t *const v, uint32_t *const w, const uint32_t *const k);
    void decipher(uint32_t *const v, uint32_t *const w, const uint32_t *const k);
    void enqueue_msg(uint8_t *p_msg);
    MSG_QUEUE_T * dequeue_msg();

  public:
    bool    m_bConnected;
    uint32_t m_XCMP_ver; /* XCMP version */

  private:
    CXNLConnectionHandler* m_handler;
    int  m_socket;
  //    bool    m_bIsDisplayRadio;
    volatile bool    m_bWaitForAck;
    volatile bool    m_bCloseSocket;
    int conn_retry;
    uint32_t m_delta;
    XNL_CONNECTION_STATE m_XnlState;
    uint32_t m_auth_key[4];
    uint32_t encrypted_seed[2]; /* Used to verify the master device */
    uint8_t m_trans_id_base;     /* used as the upper byte of the transaction Id for subsequent requests */
    uint8_t m_TxXCMPCount;
    uint8_t m_tx_xnl_flag;
    uint8_t m_rx_xnl_flag;
    uint16_t m_xnl_src_addr;
    uint16_t m_xnl_dst_addr;
    MSG_QUEUE_T*   m_pSendQueHdr;
    MSG_QUEUE_T*   m_pSendQueTail;
    uint8_t *m_pLastSendMsg;
  };
