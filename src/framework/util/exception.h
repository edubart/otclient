#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <exception>

class Exception : public std::exception
{
public:
    Exception() { }
    Exception(const std::string& what) : m_what(what) { }
    virtual ~Exception() throw() { };

    virtual const char* what() const throw() { return m_what.c_str(); }

protected:
    std::string m_what;
};

#endif
