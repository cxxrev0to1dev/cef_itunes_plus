#include "itunes_plus/multi_part_form_data.h"

namespace iPlus{
	MultiPartFormData::MultiPartFormData(void){
		multipart_data_.resize(0);
		multipart_md5_.resize(0);
		multipart_md5_ = "9574984A0908574A6771B90F83687F49";
	}
	MultiPartFormData::~MultiPartFormData(void){
		multipart_data_.resize(0);
		multipart_md5_.resize(0);
	}
	void MultiPartFormData::AddMultiPart(const char* name,const char* src,const size_t srclen){
		if(name!=nullptr&&src!=nullptr){
			const std::string start_part = std::string("--")+std::string(multipart_md5_)+std::string("\r\n");
			const std::string content_disposition = std::string("Content-Disposition: form-data; name=\"")+std::string(name)+std::string("\"\r\n\r\n");
			multipart_data_.append(start_part.c_str(),start_part.length());
			multipart_data_.append(content_disposition.c_str(),content_disposition.length());
			multipart_data_.append(src,srclen);
			multipart_data_.append(std::string("\r\n"));
		}
	}
	std::string MultiPartFormData::ContentData(){
		const std::string end_part = std::string("--")+std::string(multipart_md5_)+std::string("--");
		multipart_data_.append(end_part);
		return multipart_data_;
	}
	std::string MultiPartFormData::ContentType(){
		const std::string name = "multipart/form-data; boundary=";
		return (name+multipart_md5_);
	}
}