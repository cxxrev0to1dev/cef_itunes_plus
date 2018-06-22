#ifndef BASE_CPP_SUPPORT_H_
#define BASE_CPP_SUPPORT_H_

#include <cctype>
#include <string>
#include <cassert>
#include <mutex>

#define DECLARE_EMPTY_DESTRUCTOR(class_name)	\
	virtual ~class_name(){}
#define DISALLOW_EVIL_CONSTRUCTORS(TypeName)    \
	TypeName(const TypeName&);                    \
	void operator=(const TypeName&)

class ConvChar
{
public:
	virtual const char* ConvCharOK() = 0;
};
class MakeLower :public ConvChar{
public:
	MakeLower(char *pass_letter){
		letter_ = pass_letter;
	}
	virtual ~MakeLower(){
		letter_.resize(0);
	}
	const char* ConvCharOK(){
		size_t len = letter_.size();
		for(size_t i= 0;i<len;i++){
			letter_[i] = tolower(letter_[i]);
		}
		return letter_.c_str();
	}
private:
	std::string letter_;
};
class MakeUpper : public ConvChar{
public:
	MakeUpper(char *pass_letter){
		letter_ = pass_letter;
	}
	virtual ~MakeUpper(){
		letter_.resize(0);
	}
	const char* ConvCharOK(){
		size_t len = letter_.size();
		for(size_t i= 0;i<len;i++){
			letter_[i] = toupper(letter_[i]);
		}
		return letter_.c_str();
	}
private:
	std::string letter_;
};


#endif // !BASE_CPP_SUPPORT_H_
