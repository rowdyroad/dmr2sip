#pragma once
#include <linphone/linphonecore.h>
class SIP;

class SIPHandler {
    public:
        virtual void OnOutCallBegin(SIP* sip) { }
        virtual void OnInCallBegin(SIP* sip) { }
        virtual void OnCallEnd(SIP* sip) { }
        virtual void OnCallError(SIP* sip) { }

        virtual void OnCallStreaming(SIP* sip) { }
};

class SIP {
    private:
        std::string identity_;
        std::string password_;
        volatile bool quit_ = false;
        LinphoneCoreVTable v_table_;
        LinphoneCore* lc_;
        LinphoneCall* call_;
        SIPHandler* handler_;

    public:
        SIP(SIPHandler* handler, const size_t device_index)
            : v_table_({0})
            , call_(nullptr)
            , lc_(nullptr)
            , handler_(handler)
        {
            v_table_.call_state_changed = callStateChanged;
            lc_ = linphone_core_new(&v_table_,NULL,NULL,this);
            const char** devs = linphone_core_get_sound_devices(lc_);
            linphone_core_set_playback_device (lc_, devs[device_index]);
            linphone_core_set_capture_device (lc_, devs[device_index]);
            //linphone_core_enable_logs(NULL); //todo: make debug flag from attributes
        }

        ~SIP()
        {
            linphone_core_destroy(lc_);
        }


        void Connect(const std::string& identity, const std::string& password)
        {
            LinphoneProxyConfig* proxy_cfg = linphone_proxy_config_new();
            LinphoneAddress *from = linphone_address_new(identity.c_str());
            LinphoneAuthInfo *info = linphone_auth_info_new(linphone_address_get_username(from), NULL, password.c_str(),NULL,NULL);
            linphone_core_add_auth_info(lc_, info);
            linphone_proxy_config_set_identity(proxy_cfg, identity.c_str()); /*set identity with user name and domain*/
            const char* server_addr = linphone_address_get_domain(from); /*extract domain address from identity*/
            linphone_proxy_config_set_server_addr(proxy_cfg,server_addr); /* we assume domain = proxy server address*/
            linphone_proxy_config_enable_register(proxy_cfg,TRUE); /*activate registration for this proxy config*/
            linphone_address_destroy(from); /*release resource*/
            linphone_core_add_proxy_config(lc_, proxy_cfg); /*add proxy config to linphone core*/
            linphone_core_set_default_proxy(lc_, proxy_cfg); /*set to default proxy*/
        }

        void Stop()
        {
            quit_ = true;
        }

        void Run()
        {
             while(!quit_) {
                linphone_core_iterate(lc_);
                usleep(50000);
            }

            if (call_) {
                linphone_core_terminate_call(lc_, call_);
            }
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
                call_ = nullptr;
            }
        }

    private:
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
                break;
                case LinphoneCallError:
                    printf("Call failure !");
                    if (sip->handler_) {
                        sip->handler_->OnCallError(sip);
                    }

                break;

                case LinphoneCallIncomingReceived:
                    if (sip->call_) {
                        printf("Busy. Ignoring call");
                        break;
                    }
                    sip->call_ = call;
                    linphone_core_accept_call(sip->lc_, call);
                    if (sip->handler_) {
                        sip->handler_->OnInCallBegin(sip);
                    }
                break;
                default:
                        printf("Unhandled notification %i\n",cstate);
            }
        }
};