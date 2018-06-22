#ifndef ITUNES_PLUS_ITUNES_CALC_SERVICES_H_
#define ITUNES_PLUS_ITUNES_CALC_SERVICES_H_

#include <string>

namespace iPlus{
	class iTunesCalcServices
	{
	public:
		iTunesCalcServices();
		~iTunesCalcServices(void);
		void Init();
		const std::string key() const;
		void set_key(const std::string& k);
		std::string XAppleActionSignature_1();
		bool XAppleActionSignature_2(const char* src,size_t len);
		std::string XAppleActionSignature_3(const char* src,size_t len);
	private:
		bool SapInitialize(const unsigned long count,const char* src=0);
		std::string key_;
	};
}

#endif