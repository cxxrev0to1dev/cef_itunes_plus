#include "itunes_plus/https.h"
#include <atlconv.h>
#include <winhttp.h>

namespace HTTP{
    namespace internal{
		namespace https{
			void FreeConfig(WINHTTP_CURRENT_USER_IE_PROXY_CONFIG* config) {
				if (config->lpszAutoConfigUrl)
					GlobalFree(config->lpszAutoConfigUrl);
				if (config->lpszProxy)
					GlobalFree(config->lpszProxy);
				if (config->lpszProxyBypass)
					GlobalFree(config->lpszProxyBypass);
			}

			void FreeInfo(WINHTTP_PROXY_INFO* info) {
				if (info->lpszProxy)
					GlobalFree(info->lpszProxy);
				if (info->lpszProxyBypass)
					GlobalFree(info->lpszProxyBypass);
			}
			bool ConfigureSSL(HINTERNET internet){
				DWORD protocols = 0;
				protocols |= WINHTTP_FLAG_SECURE_PROTOCOL_SSL2;
				protocols |= WINHTTP_FLAG_SECURE_PROTOCOL_SSL3;
				protocols |= WINHTTP_FLAG_SECURE_PROTOCOL_TLS1;
				protocols |= WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_1;
				protocols |= WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_2;
				BOOL rv = WinHttpSetOption(internet, WINHTTP_OPTION_SECURE_PROTOCOLS,&protocols,sizeof(protocols));
				return (rv==TRUE);
			}
			bool ApplyProxy(HINTERNET internet,const wchar_t* proxy_str,bool is_direct){
				WINHTTP_PROXY_INFO pi;
				if(is_direct){
					pi.dwAccessType = WINHTTP_ACCESS_TYPE_NO_PROXY;
					pi.lpszProxy = WINHTTP_NO_PROXY_NAME;
					pi.lpszProxyBypass = WINHTTP_NO_PROXY_BYPASS;
				}
				else{
					pi.dwAccessType = WINHTTP_ACCESS_TYPE_NAMED_PROXY;
					pi.lpszProxy = const_cast<LPWSTR>(proxy_str);
					pi.lpszProxyBypass = WINHTTP_NO_PROXY_BYPASS;
				}
				BOOL rv = WinHttpSetOption(internet,WINHTTP_OPTION_PROXY,&pi,sizeof(pi));
				return (rv==TRUE);
			}

			bool ConfigureProxy(HINTERNET internet){
				WINHTTP_CURRENT_USER_IE_PROXY_CONFIG ie_config = {0};
				if(!WinHttpGetIEProxyConfigForCurrentUser(&ie_config)){
					return false;
				}
				WINHTTP_AUTOPROXY_OPTIONS options = {0};
				options.fAutoLogonIfChallenged = TRUE;
				if(ie_config.fAutoDetect){
					options.lpszAutoConfigUrl = ie_config.lpszAutoConfigUrl;
					options.dwFlags = WINHTTP_AUTOPROXY_CONFIG_URL;
				}
				else{
					options.dwFlags = WINHTTP_AUTOPROXY_AUTO_DETECT;  
					options.dwAutoDetectFlags = WINHTTP_AUTO_DETECT_TYPE_DHCP|WINHTTP_AUTO_DETECT_TYPE_DNS_A;
				}
				std::wstring query_url(L"http://www.baidu.com");
				WINHTTP_PROXY_INFO info = {0};
				BOOL rv = WinHttpGetProxyForUrl(internet,query_url.c_str(),&options,&info);
				WINHTTP_PROXY_INFO pi;
				switch(info.dwAccessType){
				case WINHTTP_ACCESS_TYPE_NO_PROXY:
					ApplyProxy(internet,NULL,TRUE);
					break;
				case WINHTTP_ACCESS_TYPE_NAMED_PROXY:
					ApplyProxy(internet,ie_config.lpszProxy,FALSE);
					break;
				default:
					pi.dwAccessType = WINHTTP_ACCESS_TYPE_NAMED_PROXY;
					pi.lpszProxy = ie_config.lpszProxy;
					pi.lpszProxyBypass = ie_config.lpszProxyBypass;
					rv = WinHttpSetOption(internet,WINHTTP_OPTION_PROXY,&pi,sizeof(pi));
					break;
				}
				FreeConfig(&ie_config);
				FreeInfo(&info);
				return (rv==TRUE);
			}
		}
		const wchar_t* user_agent = L"iTunes/12.0.1 (Windows; Microsoft Windows 7 x64 Ultimate Edition Service Pack 1 (Build 7601)) AppleWebKit/7600.1017.0.24";
		std::string ReadHTTPS(const wchar_t* domain,const wchar_t* path,const wchar_t* header,iTunesExtHeader options,const wchar_t* referer,const char* port) {
			HINTERNET hOpen = 0;
			HINTERNET hConnect = 0;
			HINTERNET hRequest = 0;
			IStream *stream = NULL;
			std::string message = "";
			for(;;){
				unsigned long option_flag = SECURITY_FLAG_IGNORE_CERT_CN_INVALID|SECURITY_FLAG_IGNORE_CERT_DATE_INVALID|SECURITY_FLAG_IGNORE_UNKNOWN_CA;
				hOpen = WinHttpOpen(user_agent,WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,WINHTTP_NO_PROXY_NAME,WINHTTP_NO_PROXY_BYPASS,0);
				if(!hOpen){
					break;
				}
				hConnect = WinHttpConnect(hOpen,domain,port==NULL?INTERNET_DEFAULT_HTTPS_PORT:atoi(port),0);
				if(!hConnect){
					break;
				}
				const wchar_t* types[50] = {{L"text/html, application/xhtml+xml, application/xml;q=0.9, */*;q=0.8"},{0}};
				hRequest = WinHttpOpenRequest(hConnect,L"GET",path,NULL,referer,types,(port==NULL||atoi(port)!=80)?WINHTTP_FLAG_SECURE:WINHTTP_FLAG_BYPASS_PROXY_CACHE);
				if(!hRequest){
					break;
				}
				https::ConfigureProxy(hRequest);
				if(port==NULL||atoi(port)!=80){
					https::ConfigureSSL(hRequest);
					WinHttpSetOption(hRequest,WINHTTP_OPTION_SECURITY_FLAGS,(LPVOID)(&option_flag),sizeof(unsigned long));
				}
				std::wstring some_header(L"Cache-Control: no-cache\r\nConnection: Keep-Alive\r\nAccept-Language: zh-cn,zh;q=0.5\r\n");
				if(options==internal::apple_authenticate){
					some_header.append(L"Content-Type: application/x-apple-plist\r\n");
				}
				if(header){
					some_header.append(header);
				}
				if(!WinHttpSendRequest(hRequest,some_header.c_str(),some_header.length(),WINHTTP_NO_REQUEST_DATA,0,0,WINHTTP_FLAG_SECURE)){
					break;
				}
				if(!WinHttpReceiveResponse(hRequest,0)){
					break;
				}
				DWORD cch = 1;
				if(CreateStreamOnHGlobal(0, TRUE, &stream)){
					break;
				}
				char *p = new char[4096];
				if(!p){
					break;
				}
				DWORD dwReceivedTotal = 0;
				while(WinHttpQueryDataAvailable(hRequest, &cch) && cch){
					if(cch > 4096){
						cch = 4096;
					}
					dwReceivedTotal += cch;
					WinHttpReadData(hRequest,p,cch,&cch);
					stream->Write(p,cch,NULL);
				}
				delete[] p;
				p = NULL;
				stream->Write(&p, 1, NULL);
				HGLOBAL hgl;
				if(GetHGlobalFromStream(stream, &hgl)){
					break;
				}
				p = reinterpret_cast<char*>(GlobalLock(hgl));
				if(!p){
					break;
				}
				message.resize(0);
				message.append(p,dwReceivedTotal);
				GlobalUnlock(hgl);
				break;
			}
			if(stream){
				stream->Release();
			}
			if(hRequest){
				WinHttpCloseHandle(hRequest);
			}
			if(hConnect){
				WinHttpCloseHandle(hConnect);
			}
			if(hOpen){
				WinHttpCloseHandle(hOpen);
			}
			return message;
		}

		std::string SendHTTPS(const wchar_t* domain,const wchar_t* path,const void* src,const size_t length,iTunesExtHeader options,const wchar_t* header,const wchar_t* referer,const char* post){
			HINTERNET hOpen = 0;
			HINTERNET hConnect = 0;
			HINTERNET hRequest = 0;
			IStream *stream = NULL;
			std::string message = "";
			for(;;){
				unsigned long option_flag = SECURITY_FLAG_IGNORE_CERT_CN_INVALID|SECURITY_FLAG_IGNORE_CERT_DATE_INVALID|SECURITY_FLAG_IGNORE_UNKNOWN_CA;
				unsigned long write_length = 0;
				hOpen = WinHttpOpen(user_agent, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,WINHTTP_NO_PROXY_NAME,WINHTTP_NO_PROXY_BYPASS,0);
				if(!hOpen){
					break;
				}
				hConnect = WinHttpConnect(hOpen,domain,post==NULL?INTERNET_DEFAULT_HTTPS_PORT:atoi(post),0);
				if(!hConnect){
					break;
				}
				const wchar_t* types[50] = {{L"text/html, application/xhtml+xml, application/xml;q=0.9, */*;q=0.8"},{0}};
				hRequest = WinHttpOpenRequest(hConnect,L"POST",path,NULL,referer,types,WINHTTP_FLAG_SECURE);
				if(!hRequest){
					break;
				}
				if(header){
					if(!WinHttpAddRequestHeaders(hRequest,header,wcslen(header),WINHTTP_ADDREQ_FLAG_ADD)){
						break;
					}
				}
				https::ConfigureProxy(hRequest);
				https::ConfigureSSL(hRequest);
// 				if(!WinHttpSetTimeouts(hRequest,20000,20000,20000,20000)){
// 					break;
// 				}
				WinHttpSetOption(hRequest,WINHTTP_OPTION_SECURITY_FLAGS,(LPVOID)(&option_flag),sizeof(unsigned long));
				std::wstring request_header = L"Accept-Language: zh-cn,zh;q=0.5\r\nConnection: Keep-Alive\r\nCache-Control: no-cache\r\n";
				if(options==internal::apple_authenticate){
					request_header.append(L"Content-Type: application/x-apple-plist\r\n");
				}
				if(!WinHttpSendRequest(hRequest,request_header.c_str(),request_header.length(),(LPVOID)src,length,length,WINHTTP_FLAG_SECURE)){
					break;
				}
				if(!WinHttpReceiveResponse(hRequest,0)){
					break;
				}
				if(CreateStreamOnHGlobal(0,TRUE,&stream)){
					break;
				}
				char *p = new char[4096];
				if(!p){
					break;
				}
				unsigned long dwReceivedTotal = 0;
				for(unsigned long cch = 4096;WinHttpReadData(hRequest,p,cch,&cch) && cch;cch = 4096){
					dwReceivedTotal += cch;
					stream->Write(p,cch,NULL);
				}
				delete[] p;
				p = NULL;
				stream->Write(&p, 1, NULL);
				HGLOBAL hgl;
				if(GetHGlobalFromStream(stream, &hgl)){
					break;
				}
				p = reinterpret_cast<char*>(GlobalLock(hgl));
				if(!p){
					break;
				}
				message.resize(0);
				message.append(p,dwReceivedTotal);
				GlobalUnlock(hgl);
				break;
			}
			if(stream){
				stream->Release();
			}
			if(hRequest){
				WinHttpCloseHandle(hRequest);
				hRequest = NULL;
			}
			if(hConnect){
				WinHttpCloseHandle(hConnect);
				hConnect = NULL;
			}
			if(hOpen){
				WinHttpCloseHandle(hOpen);
				hOpen = NULL;
			}
			return message;
		}
		std::string ReadHTTP(const wchar_t* domain,
			const wchar_t* path){
			HINTERNET hOpen = 0;
			HINTERNET hConnect = 0;
			HINTERNET hRequest = 0;
			IStream *stream = NULL;
			std::string message = "";
			for(;;){
				hOpen = WinHttpOpen(user_agent, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
				if(!hOpen){
					break;
				}
				hConnect = WinHttpConnect(hOpen, domain, INTERNET_DEFAULT_HTTP_PORT, 0);
				if(!hConnect){
					break;
				}
				const wchar_t* types[50] = {{L"text/html, application/xhtml+xml, application/xml"},{0}};
				hRequest = WinHttpOpenRequest(hConnect,L"GET",path,NULL,WINHTTP_NO_REFERER,types,WINHTTP_FLAG_BYPASS_PROXY_CACHE);
				if(!hRequest){
					break;
				}
				if(!WinHttpSendRequest(hRequest,WINHTTP_NO_ADDITIONAL_HEADERS,0,WINHTTP_NO_REQUEST_DATA,0,0,0)){
					break;
				}
				if(!WinHttpReceiveResponse(hRequest,0)){
					break;
				}
				wchar_t szContentLength[32] = {0};
				DWORD cch = 64;
				DWORD dwHeaderIndex = WINHTTP_NO_HEADER_INDEX;
				BOOL haveContentLength = WinHttpQueryHeaders(hRequest,WINHTTP_QUERY_CONTENT_LENGTH,NULL,&szContentLength,&cch,&dwHeaderIndex);
				DWORD dwContentLength;
				if (haveContentLength) {
					dwContentLength = _wtoi(szContentLength);
				}
				if(CreateStreamOnHGlobal(0, true, &stream)){
					break;
				}
				char *p = new char[4096];
				if(!p){
					break;
				}
				DWORD dwReceivedTotal = 0;
				while(WinHttpQueryDataAvailable(hRequest, &cch) && cch){
					if(cch > 4096){
						cch = 4096;
					}
					dwReceivedTotal += cch;
					WinHttpReadData(hRequest,p,cch,&cch);
					stream->Write(p,cch,NULL);
				}
				delete[] p;
				p = NULL;
				stream->Write(&p, 1, NULL);
				HGLOBAL hgl;
				if(GetHGlobalFromStream(stream, &hgl)){
					break;
				}
				p = reinterpret_cast<char*>(GlobalLock(hgl));
				if(!p){
					break;
				}
				message.resize(0);
				message.append(p,dwReceivedTotal);
				GlobalUnlock(hgl);
				break;
			}
			if(stream){
				stream->Release();
			}
			if(hRequest){
				WinHttpCloseHandle(hRequest);
			}
			if(hConnect){
				WinHttpCloseHandle(hConnect);
			}
			if(hOpen){
				WinHttpCloseHandle(hOpen);
			}
			return message;
		}
		std::string SendHTTP(const wchar_t* domain,
			const wchar_t* path,
			const void* src,
			const size_t length,
			const wchar_t* app_header){
			HINTERNET hOpen = 0;
			HINTERNET hConnect = 0;
			HINTERNET hRequest = 0;
			IStream *stream = NULL;
			std::string message = "";
			for(;;){
				unsigned long write_length = 0;
				hOpen = WinHttpOpen(user_agent, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,WINHTTP_NO_PROXY_NAME,WINHTTP_NO_PROXY_BYPASS,0);
				if(!hOpen){
					break;
				}
				hConnect = WinHttpConnect(hOpen,domain,INTERNET_DEFAULT_HTTP_PORT,0);
				if(!hConnect){
					break;
				}
				const wchar_t* types[50] = {{L"*/*"},{0}};
				hRequest = WinHttpOpenRequest(hConnect,L"POST",path,NULL,WINHTTP_NO_REFERER,types,WINHTTP_FLAG_BYPASS_PROXY_CACHE);
				if(!hRequest){
					break;
				}
				std::wstring externsion_header = L"Content-Type: application/x-www-form-urlencoded\r\n";
				if(!WinHttpSendRequest(hRequest,externsion_header.c_str(),externsion_header.length(),(LPVOID)src,length,length,WINHTTP_FLAG_BYPASS_PROXY_CACHE)){
					break;
				}
				if(!WinHttpReceiveResponse(hRequest,0)){
					break;
				}
				if(CreateStreamOnHGlobal(0,TRUE,&stream)){
					break;
				}
				char *p = new char[4096];
				if(!p){
					break;
				}
				unsigned long dwReceivedTotal = 0;
				for(unsigned long cch = 4096;WinHttpReadData(hRequest,p,cch,&cch) && cch;cch = 4096){
					dwReceivedTotal += cch;
					stream->Write(p,cch,NULL);
				}
				delete[] p;
				p = NULL;
				stream->Write(&p, 1, NULL);
				HGLOBAL hgl;
				if(GetHGlobalFromStream(stream, &hgl)){
					break;
				}
				p = reinterpret_cast<char*>(GlobalLock(hgl));
				if(!p){
					break;
				}
				message.resize(0);
				message.append(p,dwReceivedTotal);
				GlobalUnlock(hgl);
				break;
			}
			if(stream){
				stream->Release();
			}
			if(hRequest){
				WinHttpCloseHandle(hRequest);
			}
			if(hConnect){
				WinHttpCloseHandle(hConnect);
			}
			if(hOpen){
				WinHttpCloseHandle(hOpen);
			}
			return message;
		}
    }
}