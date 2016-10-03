#pragma once
#include <vector>
#include <string>
#include <sys/stat.h>
#include <linphone/linphonecore.h>
#include "../Debug.h"

class SIP;

class SIPHandler {
    public:
        virtual void OnOutCallBegin(SIP* sip) { }
        virtual bool OnInCallBegin(SIP* sip, const std::string& from, const std::string& to) { return true; }
        virtual void OnCallEnd(SIP* sip) { }
        virtual void OnCallError(SIP* sip) { }
        virtual void OnCallStreaming(SIP* sip) { }
        virtual void OnRegistered(SIP* sip) { }
};

class SIP {
    private:
        LinphoneCoreVTable v_table_;
        LinphoneCore* lc_;
        LinphoneCall* call_;
        SIPHandler* handler_;
        Debug debugger_;
        std::string identity_;
        std::string current_identity_;
        std::string address_;

    public:

        SIP(SIPHandler* handler, const std::string& config)
            : v_table_({0})
            , call_(nullptr)
            , lc_(nullptr)
            , handler_(handler)
            , debugger_("sip")
        {
            debugger_ << "Config file: " << config << std::endl;
            v_table_.call_state_changed = callStateChanged;
            v_table_.registration_state_changed = registrationStateChanged;
            lc_ = linphone_core_new(&v_table_, nullptr, config.c_str(), this);
        }

        void SelectDevice(const std::string& device)
        {
            debugger_ << "Selecting a device " << device << std::endl;
            std::string playback_device_name = "ALSA: plug:" + device + "playback";
            std::string capture_device_name = "ALSA: plug:" + device + "capture";
            auto devices = linphone_core_get_sound_devices(lc_);
            debugger_ << "Available devices list:" << std::endl;
            for(size_t i = 0; devices[i] != NULL; ++i) {
                debugger_ << "\t" << devices[i] << std::endl;
            }

            linphone_core_set_playback_device (lc_, playback_device_name.c_str());
            linphone_core_set_capture_device (lc_, capture_device_name.c_str());
        }

        ~SIP()
        {
            linphone_core_destroy(lc_);
        }

        void Connect(const std::string& host, uint16_t port, const std::string& username, const std::string& password, const std::string& identity = "")
        {
            identity_ = identity.empty() ? username : identity;
            current_identity_ = username;
            address_ = host + ":" + std::to_string(port);

            LinphoneProxyConfig* proxy_cfg = linphone_proxy_config_new();
            LinphoneAuthInfo *info = linphone_auth_info_new(username.c_str(), NULL, password.c_str(),NULL,NULL,NULL);
            linphone_core_add_auth_info(lc_, info);
            linphone_proxy_config_set_identity(proxy_cfg, ("sip:" + current_identity_ + "@" + address_).c_str());
            linphone_proxy_config_set_server_addr(proxy_cfg, ("sip:" + address_).c_str());
            linphone_proxy_config_enable_register(proxy_cfg, true);
            linphone_core_add_proxy_config(lc_, proxy_cfg);
            linphone_core_set_default_proxy_config(lc_, proxy_cfg);
            linphone_core_set_use_info_for_dtmf(lc_, true);
            linphone_core_set_use_rfc2833_for_dtmf(lc_, false);
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

            changeIdentity(identity_);

            call_ = linphone_core_invite(lc_, address.c_str());
            if (call_) {
                linphone_call_ref(call_);
            }
        }


        void Call(const std::string& address, const std::string& from)
        {
            if (call_) {
                Hangup();
            }

            changeIdentity(from);

            call_ = linphone_core_invite(lc_, address.c_str());
            if (call_) {
                linphone_call_ref(call_);
            }
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

        void changeIdentity(const std::string& identity)
        {
            if (current_identity_ == identity) {
                debugger_ << "Identity is same as current. No need to change" << std::endl;
            }
            auto proxy_cfg = linphone_core_get_default_proxy_config(lc_); /* get default proxy config*/

            debugger_ << "Change identity:" << current_identity_ << " -> " << identity << std::endl;

            linphone_proxy_config_edit(proxy_cfg);
            linphone_proxy_config_set_identity(proxy_cfg, ("sip:" + identity + "@" + address_).c_str());
            linphone_proxy_config_done(proxy_cfg);

            current_identity_ = identity;
        }

        void flushCall()
        {
            if (call_) {
                linphone_call_unref(call_);
                call_ = nullptr;
            }
        }

        static void globalStateChanged(LinphoneCore *lc, LinphoneGlobalState gstate, const char *message)
        {
            SIP * sip = (SIP*)linphone_core_get_user_data(lc);
            Debug& debugger_ = sip->debugger_;
            debugger_ << debugger_.debug << "Global state changed - " << gstate << " " << message << std::endl;
        }

        static void registrationStateChanged(LinphoneCore *lc, LinphoneProxyConfig *cfg, LinphoneRegistrationState cstate, const char *message)
        {
            SIP * sip = (SIP*)linphone_core_get_user_data(lc);
            Debug& debugger_ = sip->debugger_;
            debugger_ << debugger_.debug << "Registration state changed - " << cstate << " " << message << std::endl;
            if (cstate == 2) {
                if (sip->handler_) {
                    sip->handler_->OnRegistered(sip);
                }
            }
        }

        static void callStateChanged(LinphoneCore *lc, LinphoneCall *call, LinphoneCallState cstate, const char *message)
        {
            SIP * sip = (SIP*)linphone_core_get_user_data(lc);
            Debug& debugger_ = sip->debugger_;
            debugger_ << debugger_.debug << "Call state changed - " << cstate << " " << message << std::endl;
            switch(cstate) {
                case LinphoneCallOutgoingInit:
                {
                    debugger_ << debugger_.debug << "Outcomming call init" << std::endl;
                    sip->call_ = call;
                    if (sip->handler_) {
                        sip->handler_->OnOutCallBegin(sip);
                    }
                }
                break;
                case LinphoneCallOutgoingRinging:
                    debugger_ << debugger_.debug << "Outgoing call ringing" << std::endl;
                break;
                case LinphoneCallOutgoingEarlyMedia:
                    debugger_ << debugger_.debug << "Outgoing call early media" << std::endl;
                break;
                case LinphoneCallConnected:
                    debugger_ << debugger_.debug << "Call connected" << std::endl;
                break;
                case LinphoneCallStreamsRunning:
                    debugger_ << debugger_.debug << "Call streams running" << std::endl;
                    if (sip->handler_) {
                        sip->handler_->OnCallStreaming(sip);
                    }
                break;
                case LinphoneCallEnd:
                    debugger_ << debugger_.debug << "Call ended" << std::endl;
                    if (sip->handler_) {
                        sip->handler_->OnCallEnd(sip);
                    }
                    sip->flushCall();
                break;
                case LinphoneCallError:
                    debugger_ << debugger_.debug << "Call error" << std::endl;
                    if (sip->handler_) {
                        sip->handler_->OnCallError(sip);
                    }
                    sip->flushCall();
                break;

                case LinphoneCallIncomingReceived:
                {
                    debugger_ << debugger_.debug << "Incomming call received" << std::endl;
                    if (sip->call_) {
                        debugger_ << debugger_.debug << "\tWe are busy. Ignoring call" << std::endl;
                        linphone_core_decline_call(sip->lc_, call, LinphoneReason::LinphoneReasonBusy);
                        break;
                    }
                    auto cl = linphone_call_get_call_log(call);
                    std::string from = linphone_address_get_username(linphone_call_log_get_from_address(cl));
                    std::string to = linphone_address_get_username(linphone_call_log_get_to_address(cl));
                    sip->call_ = call;
                    if (sip->handler_) {
                        if (sip->handler_->OnInCallBegin(sip, from, to)) {
                            linphone_core_accept_call(sip->lc_, call);
                        } else {
                            linphone_core_decline_call(sip->lc_, call, LinphoneReason::LinphoneReasonDeclined);
                        }
                    } else {
                        linphone_core_decline_call(sip->lc_, call, LinphoneReason::LinphoneReasonNotImplemented);
                    }
                }
                break;
                default:
                    debugger_ << debugger_.debug << "Unhandled notification" << std::endl;
            }
        }
};