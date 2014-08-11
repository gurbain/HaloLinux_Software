#ifndef ELAS_EXCEPTION_H
#define ELAS_EXCEPTION_H

#include <iostream>
#include <sstream>
#include <exception>

using namespace std;
class NotEnoughPoints_Exception : public exception{
public:
	NotEnoughPoints_Exception(const string m="Less than 3 verticies."):msg(m){}
	~NotEnoughPoints_Exception(void) throw() {}
	const char* what(){return msg.c_str();}
private:
           string msg;
};

class OtherElas_exception : public exception{
public:
	OtherElas_exception(const string m="Unknown elas exception."):msg(m){}
	~OtherElas_exception(void) throw() {}
	const char* what(){return msg.c_str();}
private:
           string msg;
};

#endif
