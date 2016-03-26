#pragma once
#include <vector>
#include <string>
#include <sys/stat.h>
#include <linphone/linphonecore.h>


class SIP;

class SIPHandler {
    public:
        virtual void OnOutCallBegin(SIP* sip) { }
        virtual bool OnInCallBegin(SIP* sip) { return true; }
        virtual void OnCallEnd(SIP* sip) { }
        virtual void OnCallError(SIP* sip) { }
        virtual void OnCallStreaming(SIP* sip) { }
};

class SIP {
    private:
        LinphoneCoreVTable v_table_;
        LinphoneCore* lc_;
        LinphoneCall* call_;
        SIPHandler* handler_;

    public:

        SIP(SIPHandler* handler, const std::string& config)
            : v_table_({0})
            , call_(nullptr)
            , lc_(nullptr)
            , handler_(handler)
        {
            v_table_.call_state_changed = callStateChanged;
            v_table_.registration_state_changed = registrationStateChanged;
            lc_ = linphone_core_new(&v_table_, nullptr, config.c_str(), this);
        }

        void SelectDevice(const std::string& device)
        {
            std::string playback_device_name = "ALSA: plug:" + device + "playback";
            std::string capture_device_name = "ALSA: plug:" + device + "capture";
            linphone_core_set_playback_device (lc_, playback_device_name.c_str());
            linphone_core_set_capture_device (lc_, capture_device_name.c_str());
        }

        ~SIP()
        {
            linphone_core_destroy(lc_);
        }

        void Connect(const std::string& host, uint16_t port, const std::string& username, const std::string& password)
        {
            LinphoneProxyConfig* proxy_cfg = linphone_proxy_config_new();
            std::string address = host + ":" + std::to_string(port);
            LinphoneAuthInfo *info = linphone_auth_info_new(username.c_str(), NULL, password.c_str(),NULL,NULL,NULL);
            linphone_core_add_auth_info(lc_, info);
            linphone_proxy_config_set_identity(proxy_cfg, ("sip:" + username + "@" + address).c_str());
            linphone_proxy_config_set_server_addr(proxy_cfg, ("sip:" + address).c_str());
            linphone_proxy_config_enable_register(proxy_cfg, true);
            linphone_core_add_proxy_config(lc_, proxy_cfg);
            linphone_core_set_default_proxy_config(lc_, proxy_cfg);
            linphone_core_set_use_info_for_dtmf(lc_, true);
        }

        void Iterate()
        {
            linphone_core_iterate(lc_);
        }

        void Call(const std::string& address)
        {
            if (call_) {
                Hangup();
            }

            call_ = linphone_core_invite(lc_, address.c_str());
            if (call_) {
                linphone_call_ref(call_);
            }
        }


        const char * CallAddress() const
        {
            if (!call_) {
                return nullptr;
            }
            auto p = linphone_call_get_remote_address(call_);
            return  linphone_address_get_username(p);
        }

        void Hangup()
        {
            if (call_) {
                linphone_core_terminate_call(lc_, call_);
                flushCall();
            }
        }

        void SendDTMF(uint8_t code)
        {
            if (call_) {
                linphone_call_send_dtmf(call_, code);
            }
        }

    	bool IsCalling()
    	{
    	    return call_ != nullptr;
    	}

    private:

        void flushCall()
        {
            if (call_) {
                linphone_call_unref(call_);
                call_ = nullptr;
            }
        }

        static void globalStateChanged(LinphoneCore *lc, LinphoneGlobalState gstate, const char *message)
        {
            printf("GLOBAL STATE: %d - %s\n", gstate, message);

        }

        static void registrationStateChanged(LinphoneCore *lc, LinphoneProxyConfig *cfg, LinphoneRegistrationState cstate, const char *message)
        {
           printf("REGISTRAtION STATE: %d - %s\n", cstate, message);
        }

        static void callStateChanged(LinphoneCore *lc_, LinphoneCall *call, LinphoneCallState cstate, const char *msg)
        {

            printf("CALL STATE: %d\n", cstate);

            SIP * sip = (SIP*)linphone_core_get_user_data(lc_);
            switch(cstate) {

                case LinphoneCallOutgoingInit:
                    {
                        printf("Outcomming call init\n");
                        sip->call_ = call;
                        if (sip->handler_) {
                            sip->handler_->OnOutCallBegin(sip);
                        }
                    }
                break;
                case LinphoneCallOutgoingRinging:
                        printf("It is now ringing remotely !\n");
                break;
                case LinphoneCallOutgoingEarlyMedia:
                        printf("Receiving some early media\n");
                break;
                case LinphoneCallConnected:
                    printf("We are connected !\n");
                break;
                case LinphoneCallStreamsRunning:
                    printf("Media streams established !\n");
                    if (sip->handler_) {
                        sip->handler_->OnCallStreaming(sip);
                    }
                break;
                case LinphoneCallEnd:
                    printf("Call is terminated.\n");
                    if (sip->handler_) {
                        sip->handler_->OnCallEnd(sip);
                    }
                    sip->flushCall();
                break;
                case LinphoneCallError:
                    printf("Call failure !");
                    if (sip->handler_) {
                        sip->handler_->OnCallError(sip);
                    }
                    sip->flushCall();
                break;

                case LinphoneCallIncomingReceived:
                    printf("Call Icoming Received\n");
                    if (sip->call_) {
                        printf("Busy. Ignoring call");
                        break;
                    }
                    sip->call_ = call;
                    if (sip->handler_) {
                        if (sip->handler_->OnInCallBegin(sip)) {
                            linphone_core_accept_call(sip->lc_, call);
                        } else {
                            linphone_core_decline_call(sip->lc_, call, LinphoneReason::LinphoneReasonDeclined);
                        }
                    } else {
                        linphone_core_decline_call(sip->lc_, call, LinphoneReason::LinphoneReasonNotImplemented);
                    }
                break;
                default:
                    printf("Unhandled notification %i\n",cstate);
            }
        }
};