#include <linphone/linphonecore.h>
#include <string>
#include <stdio.h>
#include <signal.h>

class SIP;

class SIPHandler {
    public:
        void OnOutCallBegin(SIP* sip) { }
        void OnInCallBegin(SIP* sip) { }
        void OnCallEnd(SIP* sip) { }
        void OnCallError(SIP* sip) { }
};

class SIP {
    private:
        std::string identity_;
        std::string password_;
        volatile bool quit_ = false;
        LinphoneCoreVTable v_table_;
        LinphoneCore* lc_;
        LinphoneCall* call_;
        SIPHanlder* hanlder_

    public:
        SIP(SIPHanlder* handler, const size_t device_index)
            : v_table_({0})
            , call_(nullptr)
            , lc_(nullptr)
            , hanlder_(hanlder)
        {
            v_table_.call_state_changed = callStateChanged;
            lc_ = linphone_core_new(&v_table_,NULL,NULL,this);
            const char** devs = linphone_core_get_sound_devices(lc_);
            linphone_core_set_playback_device (lc_, devs[2]);
            linphone_core_set_capture_device (lc_, devs[2]);
        }


        void Connect(const std::string& identity, const std::string& password)
        {
            LinphoneProxyConfig* proxy_cfg = linphone_proxy_config_new();
            LinphoneAddress *from = linphone_address_new(identity.c_str());
            LinphoneAuthInfo *info = linphone_auth_info_new(linphone_address_get_username(from),NULL,password.c_str(),NULL,NULL);
            linphone_core_add_auth_info(lc_, info);
            linphone_proxy_config_set_identity(proxy_cfg,identity.c_str()); /*set identity with user name and domain*/
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

            call_ = linphone_core_invite(lc_, address_.c_str());
            if (call_) {
                linphone_call_ref(call_);
            }
        }

        void Hangup()
        {

        }

    private:
        static void callStateChanged(LinphoneCore *lc_, LinphoneCall *call, LinphoneCallState cstate, const char *msg)
        {
            SIP * sip = (SIP*)linphone_core_get_user_data(lc_);
            switch(cstate) {
                case LinphoneCallOutgoingRinging:
                        printf("It is now ringing remotely !\n");
                break;
                case LinphoneCallOutgoingEarlyMedia:
                        printf("Receiving some early media\n");
                break;
                case LinphoneCallConnected:
                    printf("We are connected !\n");
                    if (hanlder_) {
                        header_->OnOutCallBegin(sip);
                    }
                break;
                case LinphoneCallStreamsRunning:
                        printf("Media streams established !\n");
                break;
                case LinphoneCallEnd:
                    if (hanlder_) {
                        header_->OnCallEnd(sip);
                    }
                    printf("Call is terminated.\n");
                break;
                case LinphoneCallError:
                    if (hanlder_) {
                        header_->OnCallError(sip);
                    }
                    printf("Call failure !");
                break;

                case LinphoneCallIncomingReceived:
                    if (hanlder_) {
                        header_->OnInCallBegin(sip);
                    }
                break;
                default:
                        printf("Unhandled notification %i\n",cstate);
            }
        }
};

volatile bool quit = false;


SIP sip(nullptr, 2);

void signalHandler( int signum )
{
    sip.Stop();
}

int main(int argc, char*argv[])
{
    char * dest = argv[1];
    signal(SIGINT, signalHandler);
    printf("%d %s\n", argc, dest);
    std::string identity = "sip:1001@192.168.0.59:5060";
    std::string password = "3be8848500ef9e1b692d7db194d6b4e3";


    sip.Connect(identity, password);
    sip.Run();

    return 0;
}