#ifndef FML_H
#define FML_H

#include <sstream>
#include <string>
#include <vector>
#include <iterator>
#include <map>
#include <iostream>
#include <stdexcept>
#include <typeinfo>

#include <boost/utility.hpp>
#include <boost/foreach.hpp>

namespace FML {

///////////////////////////////////////////////////////////////////////////////
// Utilities

bool fml_convert(const std::string& input, bool& b);
bool fml_convert(const std::string& input, std::string& output);

template <typename V, typename T>
bool fml_convert(const V& in, T& out) {
    std::stringstream ss;
    ss << in;
    ss >> out;
    return !!ss;
}

std::string fml_int2str(int v);

///////////////////////////////////////////////////////////////////////////////
// Exception

class Exception : public std::runtime_error {
public:
    explicit Exception(const std::string& what) : std::runtime_error(what) {}
};


///////////////////////////////////////////////////////////////////////////////
// Node

class Parser;

class Node : boost::noncopyable
{
public:
    typedef std::vector<Node*> NodeList;

    Node(Parser* parser = 0) : m_parser(parser), m_parent(0), m_line(0) { }
    ~Node();

    bool hasTag() const { return !m_tag.empty(); }
    bool hasChildren() const { return size() > 0; }
    bool hasValue() const { return !m_value.empty(); }
    bool hasNode(const std::string childTag) const { return at(childTag) != 0; }

    void setTag(std::string tag) { m_tag = tag; }
    void setLine(int line) { m_line = line; }
    void setValue(const std::string& value) { m_value = value; }
    void setParent(Node* parent) { m_parent = parent; }

    std::string tag() const { return m_tag; }
    int line() const { return m_line; }
    int size() const { return m_children.size(); }
    Node* parent() { return m_parent; }
    std::string what() const;

    // iterators
    typedef NodeList::iterator iterator;
    typedef NodeList::const_iterator const_iterator;

    iterator begin() { return m_children.begin(); }
    iterator end() { return m_children.end(); }
    const_iterator begin() const { return m_children.begin(); }
    const_iterator end() const { return m_children.end(); }

    Node* front() const { return at(0); }
    Node* back() const { return at(size()-1); }

    // util for generating error message
    std::string generateErrorMessage(const std::string& message) const;
    void throwError(const std::string& message) const { throw Exception(generateErrorMessage(message)); }

    // extracting values operator
    template <typename T>
    friend bool operator >> (const Node& node, T& value);

    // get nodes
    Node* at(const std::string& childTag) const;
    Node* at(int pos) const;

    // get values
    std::string value(const std::string& def = std::string()) const  {
        if(!m_value.empty())
            return m_value;
        return def;
    }

    std::string valueAt(const std::string childTag, const std::string& def = std::string()) const {
        if(Node* node = at(childTag))
            return node->value();
        return def;
    }

    std::string valueAt(int pos, const std::string& def = std::string()) const {
        if(Node* node = at(pos))
            return node->value();
        return def;
    }

    // read values into memory
    template <typename T>
    void read(T* v) const {
        if(!(*this >> *v))
            throw Exception(generateErrorMessage("failed to cast node value to type " + std::string(typeid(T).name())));
    }

    template <typename T>
    bool readAt(const std::string& childTag, T* v) const {
        if(Node* node = at(childTag)) {
            node->read<T>(v);
            return true;
        }
        return false;
    }

    template <typename T>
    bool readAt(int pos, T* v) const {
        if(Node* node = at(pos)) {
            node->read<T>(v);
            return true;
        }
        return false;
    }

    // read values
    template <typename T>
    T read() const {
        T v;
        read<T>(&v);
        return v;
    }

    template <typename T>
    T readAt(const std::string& childTag) const {
        if(Node* node = at(childTag))
            return node->read<T>();
        throw Exception(generateErrorMessage("child node " + childTag + " not found"));
        return T();
    }

    template <typename T>
    T readAt(int pos) const {
        if(Node* node = at(pos))
            return node->read<T>();
        throw Exception(generateErrorMessage("child node at pos " + fml_int2str(pos) + " not found"));
        return T();
    }

    // read values with defaults
    template <typename T>
    T readAt(const std::string& childTag, const T& def) const {
        if(Node* node = at(childTag))
            return node->read<T>();
        else
            return def;
    }

    template <typename T>
    T readAt(int pos, const T& def) const {
        if(Node* node = at(pos))
            return node->read<T>();
        else
            return def;
    }

    void addNode(Node* node);

    std::string emitValue();
    std::string emit(int depth = 0);

private:
    Parser* m_parser;
    Node* m_parent;
    int m_line;
    NodeList m_children;
    std::string m_tag;
    std::string m_value;
};


///////////////////////////////////////////////////////////////////////////////
// Node operators

template <typename T>
bool operator >> (const Node& node, T& v)
{
    return fml_convert(node.value(), v);
}

template <typename T>
bool operator >> (const Node& node, std::vector<T>& v)
{
    std::vector<T>& tmp;
    tmp.resize(node.size());
    for(unsigned i=0;i<node.size();++i)
        v[i] = node.readAt<T>(i);
    v = tmp;
    return true;
}

template <typename K, typename T>
bool operator >> (const Node& node, std::map<K, T>& m)
{
    std::map<K, T> tmp;
    for(Node::const_iterator it = node.begin(); it != node.end(); ++it) {
        K k;
        T v;
        if(fml_convert<std::string, K>((*it)->tag(), k)) {
            (*it)->read(&v);
            tmp[k] = v;
        } else
            return false;
    }
    m = tmp;
    return true;
}


///////////////////////////////////////////////////////////////////////////////
// Parser

class Parser
{
    enum MultilineMode {
        DONT_MULTILINE = 0,
        MULTILINE_DONT_FOLD,
        MULTILINE_FOLD_BLOCK,
        MULTILINE_FOLD_FLOW
    };

public:
    Parser(std::string what = std::string()) : m_rootNode(0), m_what(what) { }
    Parser(std::istream& in, std::string what = std::string()) : m_rootNode(0), m_what(what) { load(in); }
    ~Parser();

    void load(std::istream& in);

    Node* getDocument() const { return m_rootNode; }
    std::string what() { return m_what; }

protected:
    void parseLine(std::string& line);
    Node* parseNode(std::string& line);
    std::string parseTextScalar(std::string value);

    void startMultilining(const std::string& param);
    void stopMultilining();
    bool parseMultiline(std::string line);
    bool isMultilining() { return m_multilineMode != DONT_MULTILINE; }

    void throwError(const std::string& message, int line = 0);

private:
    int m_currentDepth;
    int m_currentLine;
    Node* m_currentParent;
    Node* m_previousNode;
    Node* m_rootNode;
    MultilineMode m_multilineMode;
    std::string m_multilineData;
    std::string m_what;
};

} // namespace FML {

#endif // FML_H
