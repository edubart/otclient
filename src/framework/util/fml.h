#ifndef __FML_H
#define __FML_H

#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <iterator>
#include <map>
#include <stdexcept>
#include <typeinfo>

#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include <boost/utility.hpp>

namespace FML {

bool fml_convert(const std::string& input, bool& b);
bool fml_convert(const std::string& input, std::string& output);

template <typename V, typename T>
bool fml_convert(const V& in, T& out) { std::stringstream ss; ss << in; ss >> out; return !!ss; }

inline std::string fml_tostr(bool b) { return (b ? "true" : "false"); }

template < typename T>
std::string fml_tostr(T v) { std::stringstream ss; ss << v; return ss.str(); }

class Exception : public std::runtime_error {
public:
    Exception(const std::string& what) : std::runtime_error(what) {}
};

class Parser;

class Node : boost::noncopyable
{
public:
    typedef std::vector<Node*> NodeList;
    typedef NodeList::iterator iterator;
    typedef NodeList::const_iterator const_iterator;

    Node(std::string what = "") : m_parent(0), m_line(0), m_what(what) { }
    ~Node() { for(int i=0;i<size();++i) delete at(i); }

    bool hasTag() const { return !m_tag.empty(); }
    bool hasChildren() const { return size() > 0; }
    bool hasValue() const { return !m_value.empty(); }
    bool hasNode(const std::string ctag) const { return at(ctag) != 0; }

    void setTag(std::string tag) { m_tag = tag; }
    void setLine(int line) { m_line = line; }
    void setValue(const std::string& value) { m_value = value; }
    void setParent(Node* parent) { m_parent = parent; }

    std::string tag() const { return m_tag; }
    int line() const { return m_line; }
    int size() const { return m_children.size(); }
    Node* parent() { return m_parent; }
    std::string what() const { return (m_parent  ? m_parent->what() : m_what); }

    iterator begin() { return m_children.begin(); }
    iterator end() { return m_children.end(); }
    const_iterator begin() const { return m_children.begin(); }
    const_iterator end() const { return m_children.end(); }

    Node* front() const { return at(0); }
    Node* back() const { return at(size()-1); }

    void throwError(const std::string& message) const { throw Exception(generateErrorMessage(message)); }

    Node* at(const std::string& ctag) const;
    Node* at(int pos) const { return ((pos < size() && pos >= 0) ? m_children[pos] : 0); }

    std::string value(const std::string& def = "") const { return (m_value.empty() ? def : m_value); }
    std::string valueAt(const std::string ctag, const std::string& def = "") const { Node* c = at(ctag); return (c ? c->value() : def); }
    std::string valueAt(int pos, const std::string& def = "") const { Node* n = at(pos); return (n ? n->value() : def); }

    Node* createNode(std::string tag = "");
    void addNode(Node* node);
    bool removeNode(Node* node);

    std::string generateErrorMessage(const std::string& message) const;

    // read into memory
    template <typename T>
    void read(T* v) const {
        if(!(*this >> *v))
            throw Exception(generateErrorMessage("failed to cast node value to type " + std::string(typeid(T).name())));
    }

    template <typename T>
    bool readAt(const std::string& ctag, T* v) const {
        if(Node* node = at(ctag)) {
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

    // read returning the result
    template <typename T>
    T read() const { T v; read<T>(&v);  return v;}

    template <typename T>
    T readAt(const std::string& ctag) const {
        T v;
        if(!readAt(ctag, &v))
            throw Exception(generateErrorMessage("child node " + ctag + " not found"));
        return v;
    }

    template <typename T>
    T readAt(int pos) const {
        T v;
        if(!readAt(pos, &v))
            throw Exception(generateErrorMessage("child node at pos " + fml_tostr(pos) + " not found"));
        return v;
    }

    // read with default supplied
    template <typename T>
    T readAt(const std::string& ctag, const T& def) const { Node* c = at(ctag); return (c ? c->read<T>() : def); }

    template <typename T>
    T readAt(int pos, const T& def) const { Node* c = at(pos); return (c ? c->read<T>() : def); }

    // writing
    template <typename T>
    void write(T v) {
        if(!(*this << v))
            throw Exception(generateErrorMessage("failed to cast to string node value of type " + std::string(typeid(T).name())));
    }

    template <typename T>
    void writeIn(int pos, T v) {
        Node* c;
        while(!at(pos))
            c = createNode();
        c->write<T>(v);
    }

    template <typename T>
    void writeIn(const std::string& ctag, T v) {
        Node* c = createNode(ctag);
        c->write<T>(v);
    }

private:
    Node* m_parent;
    int m_line;
    std::string m_what;
    NodeList m_children;
    std::string m_tag;
    std::string m_value;
};

// read operators
template <typename T>
bool operator >> (const Node& node, T& v) { return fml_convert(node.value(), v); }

template <typename T>
bool operator >> (const Node& node, std::vector<T>& v) {
    v.resize(node.size());
    for(unsigned i=0;i<node.size();++i)
        v[i] = node.readAt<T>(i);
    return true;
}

template <typename T>
bool operator >> (const Node& node, std::list<T>& v) {
    for(unsigned i=0;i<node.size();++i)
        v.push_back(node.readAt<T>(i));
    return true;
}

template <typename K, typename T>
bool operator >> (const Node& node, std::map<K, T>& m) {
    for(int i=0;i<node.size();++i)
        m[node.at(i)->read<K>()] = node.at(i)->read<T>();
    return true;
}

// write operators
template <typename T>
bool operator << (Node& node, const T& v) { node.setValue(fml_tostr(v)); return true; }

template <typename T>
bool operator << (Node& node, const std::vector<T>& v) {
    for(unsigned i=0;i<v.size();++i)
        node.createNode()->write(v[i]);
    return true;
}

template <typename T>
bool operator << (Node& node, const std::list<T>& v) {
    for(unsigned i=0;i<v.size();++i)
        node.createNode()->write(v[i]);
    return true;
}

template <typename K, typename T>
bool operator << (Node& node, const std::map<K, T>& m) {
    typename std::map<K, T>::const_iterator it;
    for(it = m.begin(); it != m.end(); ++it)
        node.createNode(fml_tostr(it->first))->write(it->second);
    return true;
}

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
    ~Parser() { if(m_rootNode) delete m_rootNode; }

    Node* getDocument() const { return m_rootNode; }
    std::string what() { return m_what; }

    void load(std::istream& in);

protected:
    void parseLine(std::string& line);
    void parseNode(Node* node, std::string& line);
    void parseNodeValue(Node* node, std::string& value);
    std::string parseTextValue(std::string value);

    void stopMultilining();
    bool parseMultiline(std::string line);
    bool isMultilining() { return m_multilineMode != DONT_MULTILINE; }

    void throwError(const std::string& message, int line);

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

class Emitter
{
public:
    Emitter() : m_rootNode(0) { }
    ~Emitter() { if(m_rootNode) delete m_rootNode; }

    Node* createDocument() { m_rootNode = new Node; return m_rootNode; }
    std::string emitDocument() { if(m_rootNode) return emitNode(m_rootNode, 0); return std::string(); }

    static std::string emitNodeValue(Node* node);
    static std::string emitNode(Node* node, int currentDepth = 0);

private:
    Node* m_rootNode;
};

} // namespace FML {

#endif // __FML_H
