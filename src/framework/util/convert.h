#ifndef CONVERT_H
#define CONVERT_H

#include <string>
#include <sstream>

bool safe_convert(const std::string& input, bool& b);
bool safe_convert(const std::string& input, std::string& output);
bool safe_convert(const bool& b, std::string& out);
template <typename V, typename T>
bool safe_convert(const V& in, T& out) {std::stringstream ss; ss << in; ss >> out; return !!ss; }

template<typename R, typename T>
R convert(const T& t) { R r; safe_convert(t, r); return r; }

#endif // CONVERT_H
