#ifndef ITUNES_PLUS_HTTPS_H_
#define ITUNES_PLUS_HTTPS_H_

#include <cctype>
#include <iomanip>
#include <sstream>
#include <string>

namespace HTTP{
	namespace URL{
		static std::string Encode(const std::string &value){
			std::ostringstream escaped;
			escaped.fill('0');
			escaped << std::hex;
			for(std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
				std::string::value_type c = (*i);
				if (isalnum((int)c) || c == '-' || c == '_' || c == '.' || c == '~') {
					escaped << c;
					continue;
				}
				escaped << '%' << std::setw(2) << int((unsigned char) c);
			}
			return escaped.str();
		}
	}
	namespace internal{
		enum iTunesExtHeader{
			apple_itunes = -1,
			apple_authenticate,
			apple_signSapSetup
		};
		std::string ReadHTTPS(const wchar_t* domain,
			const wchar_t* path,
			const wchar_t* header,
			iTunesExtHeader options = apple_itunes,
			const wchar_t* referer=NULL,
			const char* port=NULL);
		std::string SendHTTPS(const wchar_t* domain,
			const wchar_t* path,
			const void* src,
			const size_t length,
			iTunesExtHeader options,
			const wchar_t* header,
			const wchar_t* referer=NULL,
			const char* post = NULL);
		std::string ReadHTTP(const wchar_t* domain,
			const wchar_t* path);
		std::string SendHTTP(const wchar_t* domain,
			const wchar_t* path,
			const void* src,
			const size_t length,
			const wchar_t* app_header = NULL);
	}
}

#endif