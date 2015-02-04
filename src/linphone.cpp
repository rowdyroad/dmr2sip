#include <linphone/linphonecore.h>
#include <string>
#include <stdio.h>
#include <signal.h>

static void call_state_changed(LinphoneCore *lc, LinphoneCall *call, LinphoneCallState cstate, const char *msg){
        switch(cstate){
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
                break;
                case LinphoneCallEnd:
                        printf("Call is terminated.\n");
                break;
                case LinphoneCallError:
                        printf("Call failure !");
                break;
                default:
                        printf("Unhandled notification %i\n",cstate);
        }
}

volatile bool quit = false;

void signalHandler( int signum )
{
    quit = true;
}

int main(int argc, char*argv[])
{
    char * dest = argv[1];
    signal(SIGINT, signalHandler);
    printf("%d %s\n", argc, dest);
    std::string identity = "sip:1001@192.168.0.59:5060";
    std::string password = "3be8848500ef9e1b692d7db194d6b4e3";

    LinphoneCoreVTable v_table = {0};
     v_table.call_state_changed=call_state_changed;
    LinphoneCore* lc = linphone_core_new(&v_table,NULL,NULL,NULL);


    const char** devs = linphone_core_get_sound_devices(lc);
    size_t i = 0;

    while (devs[i]) {
	printf("device: %s\n", devs[i]);
	++i;
    }
    //return 0;

    linphone_core_set_playback_device (lc, devs[2]);
    linphone_core_set_capture_device (lc, devs[2]);

    LinphoneProxyConfig* proxy_cfg = linphone_proxy_config_new();

    LinphoneAddress *from = linphone_address_new(identity.c_str());

    LinphoneAuthInfo *info = linphone_auth_info_new(linphone_address_get_username(from),NULL,password.c_str(),NULL,NULL);
    linphone_core_add_auth_info(lc,info);

    linphone_proxy_config_set_identity(proxy_cfg,identity.c_str()); /*set identity with user name and domain*/
    const char* server_addr = linphone_address_get_domain(from); /*extract domain address from identity*/
    linphone_proxy_config_set_server_addr(proxy_cfg,server_addr); /* we assume domain = proxy server address*/
    linphone_proxy_config_enable_register(proxy_cfg,TRUE); /*activate registration for this proxy config*/
    linphone_address_destroy(from); /*release resource*/

    linphone_core_add_proxy_config(lc,proxy_cfg); /*add proxy config to linphone core*/
    linphone_core_set_default_proxy(lc,proxy_cfg); /*set to default proxy*/
    
    LinphoneCall* call = linphone_core_invite(lc,dest);

    while(!quit){
        linphone_core_iterate(lc);
        ms_usleep(50000);
    }

    linphone_core_terminate_call(lc, call);
    return 0;
}