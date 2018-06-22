#include "itunes_plus/itunes_calc_services.h"
#include <memory>
#include <string>
#include <Windows.h>
#include <atlconv.h>
#pragma comment(lib,"libeay32.lib")
#pragma comment(lib,"ssleay32.lib")
#include <openssl/evp.h>
#include "itunes_plus/https.h"

namespace iPlus{
	const unsigned long kMaxCertLength = 1024*1024*8;
	const char* sssb_msg = 
		"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
		"<!DOCTYPE plist PUBLIC \"-//Apple Computer//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">"
		"<plist version=\"1.0\">"
		"<dict>"
		"<key>sign-sap-setup-buffer</key>"
		"<data>";
	iTunesCalcServices::iTunesCalcServices(){
		set_key("");
	}
	iTunesCalcServices::~iTunesCalcServices(void){
		set_key("");
	}
	void iTunesCalcServices::Init(){
		set_key(HTTP::internal::ReadHTTP(L"127.0.0.1",L"/Initialize"));
		const std::string process_key = key();
		if(process_key.length()&&process_key.find("key=")!=std::string::npos){
			SapInitialize(2);
		}
	}
	const std::string iTunesCalcServices::key() const{
		return key_;
	}
	void iTunesCalcServices::set_key(const std::string& k){
		key_ = k;
	}
	std::string iTunesCalcServices::XAppleActionSignature_1(){
		std::string protocol = "type=1&data=null";
		const std::string process_key = std::string("/XAppleActionSignature?")+key();
		USES_CONVERSION;
		std::string xaa_sig = 
			HTTP::internal::SendHTTP(L"127.0.0.1",
			A2W(process_key.c_str()),
			protocol.c_str(),
			protocol.length());
		if(!xaa_sig.length()){
			return "";
		}
		return xaa_sig;
	}
	bool iTunesCalcServices::XAppleActionSignature_2(const char* src,size_t len){
		if(!src||!len){
			return false;
		}
		std::string protocol = "type=2&data="+HTTP::URL::Encode(std::string(src));
		const std::string process_key = std::string("/XAppleActionSignature?")+key();
		USES_CONVERSION;
		std::string xaa_sig = 
			HTTP::internal::SendHTTP(L"127.0.0.1",
			A2W(process_key.c_str()),
			protocol.c_str(),
			protocol.length());
		if(xaa_sig=="success"){
			return true;
		}
		return false;
	}
	std::string iTunesCalcServices::XAppleActionSignature_3(const char* src,size_t len){
		if(!src||!len){
			return "";
		}
		std::string protocol = "type=3&data="+HTTP::URL::Encode(std::string(src));
		const std::string process_key = std::string("/XAppleActionSignature?")+key();
		USES_CONVERSION;
		std::string xaa_sig = 
			HTTP::internal::SendHTTP(L"127.0.0.1",
			A2W(process_key.c_str()),
			protocol.c_str(),
			protocol.length());
		return xaa_sig;
	}
	bool iTunesCalcServices::SapInitialize(const unsigned long count,const char* src){
		std::string sap_setup;
		if(count==2){
			unsigned long ss_length = 0;
			for(int i=0;i<3;i++){
				std::string signSapSetupCert = HTTP::internal::ReadHTTPS(L"init.itunes.apple.com",L"/WebObjects/MZInit.woa/wa/signSapSetupCert", NULL,HTTP::internal::apple_itunes,NULL,NULL);
				if(signSapSetupCert.length()){
					signSapSetupCert = signSapSetupCert.substr(std::string("<plist>\n<dict>\n<key>sign-sap-setup-cert</key>\n<data>").length());
					sap_setup = signSapSetupCert.substr(0,signSapSetupCert.length()-std::string("</data>\n</dict>\n</plist>\n").length());
					break;
				}
				Sleep(1000);
			}
			//send services protocol
			std::string protocol = "X-Apple-ActionSignature=1&sign-sap-setup-cert=";
			protocol.append(HTTP::URL::Encode(sap_setup));
			const std::string process_key = std::string("/SapSetupInitialize?")+key();
			USES_CONVERSION;
			std::string sap_calc_value = 
				HTTP::internal::SendHTTP(L"127.0.0.1",
				A2W(process_key.c_str()),
				protocol.c_str(),
				protocol.length());
			if(!sap_calc_value.length()){
				return false;
			}
			std::string message = sssb_msg;
			message.append(sap_calc_value);
			message.append("</data></dict></plist>");
			//send ok
			std::string sign_sap_setup_buffer;
			for(int i=0;i<3;i++){
				sign_sap_setup_buffer = HTTP::internal::SendHTTPS(L"buy.itunes.apple.com",L"/WebObjects/MZPlay.woa/wa/signSapSetup",
					message.c_str(),message.length(),HTTP::internal::apple_itunes,NULL, NULL, NULL);
				if(!sign_sap_setup_buffer.length()){
					Sleep(1000);
					continue;
				}
				sign_sap_setup_buffer = sign_sap_setup_buffer.substr(std::string("<plist>\n<dict>\n<key>sign-sap-setup-buffer</key>\n<data>").length());
				sap_setup = sign_sap_setup_buffer.substr(0,sign_sap_setup_buffer.length()-std::string("</data>\n</dict>\n</plist>\n").length());
				break;
			}
			return SapInitialize(count-1,sap_setup.c_str());
		}
		else if(count==1){
			size_t sb_length = kMaxCertLength;
			std::string protocol = "X-Apple-ActionSignature=1&sign-sap-setup-buffer=";
			protocol.append(HTTP::URL::Encode(src));
			const std::string process_key = std::string("/SapSetupInitialize?")+key();
			USES_CONVERSION;
			std::string sap_calc_value = 
				HTTP::internal::SendHTTP(L"127.0.0.1",
				A2W(process_key.c_str()),
				protocol.c_str(),
				protocol.length());
			return (!!sap_calc_value.length());
		}
		else{
			return false;
		}
	}
}