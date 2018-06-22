#ifndef ITUNES_PLUS_MULTI_PART_FORM_DATA_H_
#define ITUNES_PLUS_MULTI_PART_FORM_DATA_H_

#include <string>

namespace iPlus{
	class MultiPartFormData
	{
	public:
		MultiPartFormData(void);
		~MultiPartFormData(void);
		void AddMultiPart(const char* name,const char* src,const size_t srclen);
		std::string ContentType();
		std::string ContentData();
	private:
		std::string multipart_data_;
		std::string multipart_md5_;
	};
}

#endif // !ITUNES_PLUS_MULTI_PART_FORM_DATA_H_

