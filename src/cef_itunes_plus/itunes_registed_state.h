#ifndef ITUNES_PLUS_ITUNES_REGISTED_STATE_H_
#define ITUNES_PLUS_ITUNES_REGISTED_STATE_H_

#include <map>
#include <fstream>
#include <string>
#include <atlconv.h>
#include <glib/gutils.h>
#include "include/cef_app.h"

namespace iPlus{
	static const char* xp_cookie = "https://xp.apple.com/register";
	static const char* signup_wizard = "https://buy.itunes.apple.com/WebObjects/MZFinance.woa/wa/signupWizard";
	class ITunesState
	{
	public:
		enum class RegistedState{
			ITUNES_INIT,
			ITUNES_CONTINUE_BUTTON,
			ITUNES_CONNTINUE_COMPLETE,
			ALL_OK,
		};
		enum class SignupWizard{
			INIT,
			SUCCESS,
			S_ERROR
		};
		ITunesState():reg_state_(RegistedState::ITUNES_INIT),wsw_xaa_sig_(SignupWizard::INIT){}
		~ITunesState(){
			set_wsw_xaa_sig(SignupWizard::INIT);
		}
		static ITunesState* Instance(){
			static ITunesState* info;
			if(!info){
				ITunesState* new_info = new ITunesState;
				if(InterlockedCompareExchangePointer(reinterpret_cast<PVOID*>(&info),new_info,NULL)){
					delete new_info;
				}
				assert(info!=NULL);
			}
			return info;
		}
		cef_string_t UserAgent(){
			char url[1024] = "iTunes/12.0.1.26 (Windows; Microsoft Windows 7 x64 Ultimate Edition Service Pack 1 (Build 7601)) AppleWebKit/536.27.1";
			cef_string_t cef_user_agent = {0};
			cef_string_utf8_to_utf16(url,strlen(url),&cef_user_agent);
			return cef_user_agent;
		}
		bool IsFilterKey(const std::string& url){
			const char* cb_before_key[] = {"register","signupWizard",NULL};
			for(int i=0;cb_before_key[i]!=NULL;i++){
				if(IsClickContinueButton()){
					if(url.find(cb_before_key[i])!=std::string::npos){
						return true;
					}
				}
			}
			return false;
		}
		bool IsClickContinueButton() const{
			return (get_reg_state()==RegistedState::ITUNES_CONTINUE_BUTTON);
		}
		void set_reg_state(const RegistedState& logic){
			reg_state_ = logic;
		}
		RegistedState get_reg_state() const{
			return reg_state_;
		}
		void set_wsw_xaa_sig(SignupWizard is_wait){
			wsw_xaa_sig_ = is_wait;
		}
		SignupWizard wsw_xaa_sig() const{ //wait signup wizard http header X-Apple-ActionSignature filed
			return wsw_xaa_sig_;
		}
		void set_request_end(bool state){
			is_request_end_ = state;
		}
		bool is_request_end() const{
			return is_request_end_;
		}
		std::string machineGUID() const{
			return std::string("16DDE994.16532622.1D132928.19BDDA1D.1D161971.198570FE.162208C2");
		}
	private:
		RegistedState reg_state_;
		SignupWizard wsw_xaa_sig_;
		bool is_request_end_;
	};
}

#endif