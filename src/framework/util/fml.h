#ifndef FML_H
#define FML_H

#include <sstream>
#include <string>
#include <vector>
#include <iterator>
#include <map>

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

    // extracting values operator
    template <typename T>
    friend bool operator >> (const Node& node, T& value);

    // get nodes
    Node* at(const std::string& childTag) const;
    Node* at(int pos) const;

    // get values
    std::string value(const std::string& def = std::string()) const;
    std::string valueAt(const std::string childTag, const std::string& def = std::string()) const;

    // read values
    template <typename T>
    T read(T def = T()) const {
        T v = def;
        *this >> v;
        return v;
    }

    template <typename T>
    bool read(T* v) const {
        return (*this >> *v);
    }

    template <typename T>
    T readAt(const std::string& childTag, T def = T()) const {
        T v = def;
        for(NodeList::const_iterator it = m_children.begin(); it != m_children.end(); ++it) {
            if((*it)->tag() == childTag) {
                *(*it) >> v;
                break;
            }
        }
        return v;
    }

    template <typename T>
    bool readAt(const std::string& childTag, T* v) const {
        for(NodeList::const_iterator it = m_children.begin(); it != m_children.end(); ++it) {
            if((*it)->tag() == childTag)
                return (*(*it) >> *v);
        }
        return false;
    }

    void addNode(Node* node);

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
    bool ret = true;
    v.clear();
    v.resize(node.size());
    for(unsigned i=0;i<node.size();++i) {
        if(!(*node.at(i) >> v[i]))
            ret = false;
    }
    return ret;
}

template <typename K, typename T>
bool operator >> (const Node& node, std::map<K, T>& m)
{
    bool ret = true;
    m.clear();
    for(Node::const_iterator it = node.begin(); it != node.end(); ++it) {
        Node* child = (*it);
        K k;
        T v;
        if(fml_convert<std::string, K>(child->tag(), k)) {
            *child >> v;
            m[k] = v;
            ret = false;
        }
    }
    return ret;
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

    bool load(std::istream& in);

    Node* getDocument() const { return m_rootNode; }
    bool hasError() const { return !m_error.empty(); }
    std::string getErrorMessage() { return m_error; }
    std::string getWhat() { return m_what; }

protected:
    void parseLine(std::string& line);
    Node* parseNode(std::string& line);
    std::string parseTextScalar(std::string value);

    void startMultilining(const std::string& param);
    void stopMultilining();
    bool parseMultiline(std::string line);
    bool isMultilining() { return m_multilineMode != DONT_MULTILINE; }

    void setErrorMessage(const std::string& message, int line = 0);

private:
    int m_currentDepth;
    int m_currentLine;
    Node* m_currentParent;
    Node* m_previousNode;
    Node* m_rootNode;
    MultilineMode m_multilineMode;
    std::string m_multilineData;
    std::string m_error;
    std::string m_what;
};

} // namespace FML {

#endif // FML_H
