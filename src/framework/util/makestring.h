#ifndef MAKESTRING_H
#define MAKESTRING_H

#include <sstream>
#include <string>

inline void fill_ostream(std::ostringstream&) { }
template<class T, class... Args>
void fill_ostream(std::ostringstream& stream, const T& first, const Args&... rest) {
   stream << first;
   fill_ostream(stream, rest...);
}

template<class... T>
std::string make_string(const T&... args) {
   std::ostringstream buf;
   fill_ostream(buf, args...);
   return buf.str();
}

#endif // MAKESTRING_H
