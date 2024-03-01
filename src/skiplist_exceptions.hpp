#ifndef SKIPLIST_EXCEPTIONS_H
#define SKIPLIST_EXCEPTIONS_H

#include<string>
#include<stdexcept>

class SLNodeException : public std::exception {

protected:
	std::string info;

public:
	SLNodeException(const std::string& i="") throw() :info(i){}

	const char* what() const throw() { return info.c_str(); }

	~SLNodeException() throw() {}
};


class SkiplistException : public std::exception {

protected:
	std::string info;

public:
	SkiplistException(const std::string& i="") throw() :info(i){}

	const char* what() const throw() { return info.c_str(); }

	~SkiplistException() throw() {}
};

#endif // SKIPLIST_EXCEPTIONS_H