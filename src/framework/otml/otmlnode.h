#ifndef OTMLNODE_H
#define OTMLNODE_H

#include <string>
#include <vector>
#include <list>
#include <map>
#include <stdexcept>
#include <typeinfo>

#include <util/foreach.h>
#include <util/makestring.h>
#include <util/convert.h>

class OTMLException : public std::runtime_error {
public:
    OTMLException(const std::string& what) : std::runtime_error(what) {}
};

class OTMLNode
{
public:
    typedef std::vector<OTMLNode*> NodeList;
    typedef NodeList::iterator iterator;
    typedef NodeList::const_iterator const_iterator;

    OTMLNode(std::string what = "");
    ~OTMLNode();

    bool hasTag() const { return !m_tag.empty(); }
    bool hasChildren() const { return size() > 0; }
    bool hasValue() const { return !m_value.empty(); }
    bool hasChild(const std::string ctag) const { return at(ctag) != 0; }

    void setTag(std::string tag) { m_tag = tag; }
    void setLine(int line) { m_line = line; }
    void setValue(const std::string& value) { m_value = value; }
    void setParent(OTMLNode* parent) { m_parent = parent; }

    std::string tag() const { return m_tag; }
    int line() const { return m_line; }
    int size() const { return m_children.size(); }
    OTMLNode* parent() { return m_parent; }
    std::string what() const { return (m_parent  ? m_parent->what() : m_what); }

    iterator begin() { return m_children.begin(); }
    iterator end() { return m_children.end(); }
    const_iterator begin() const { return m_children.begin(); }
    const_iterator end() const { return m_children.end(); }

    OTMLNode* front() const { return at(0); }
    OTMLNode* back() const { return at(size()-1); }

    OTMLNode* at(const std::string& ctag) const;
    OTMLNode* at(int pos) const;
    OTMLNode *atPath(const std::string& path) const;

    OTMLNode* createNode(std::string tag = "");
    void addNode(OTMLNode* node);
    bool removeNode(OTMLNode* node);

    std::string generateErrorMessage(const std::string& message) const;
    void throwError(const std::string& message) const;

    std::string value(const std::string& def = "") const { return (m_value.empty() ? def : m_value); }
    std::string valueAt(const std::string ctag, const std::string& def = "") const {
        OTMLNode* c = at(ctag);
        return (c ? c->value() : def);
    }
    std::string valueAt(int pos, const std::string& def = "") const {
        OTMLNode* n = at(pos);
        return (n ? n->value() : def);
    }
    std::string valueAtPath(const std::string path, const std::string& def = "") const {
        OTMLNode* c = atPath(path);
        return (c ? c->value() : def);
    }

    // read into memory
    template <typename T>
    void read(T* v) const {
        if(!(*this >> *v))
            throwError(make_string("failed to cast node value to type ", std::string(typeid(T).name())));
    }

    template <typename T>
    bool readAt(const std::string& ctag, T* v) const {
        if(OTMLNode* node = at(ctag)) {
            node->read<T>(v);
            return true;
        }
        return false;
    }

    template <typename T>
    bool readAt(int pos, T* v) const {
        if(OTMLNode* node = at(pos)) {
            node->read<T>(v);
            return true;
        }
        return false;
    }

    template <typename T>
    bool readAtPath(const std::string& ctag, T* v) const {
        if(OTMLNode* node = atPath(ctag)) {
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
            throwError(make_string("child node \'", ctag, "\' not found"));
        return v;
    }

    template <typename T>
    T readAt(int pos) const {
        T v;
        if(!readAt(pos, &v))
            throwError(make_string("child node at pos ", pos, " not found"));
        return v;
    }

    template <typename T>
    T readAtPath(const std::string& ctag) const {
        T v;
        if(!readAtPath(ctag, &v))
            throwError(make_string("child node in path \'", ctag, "\' not found"));
        return v;
    }

    // read with default supplied
    template <typename T>
    T readAt(const std::string& ctag, const T& def) const {
        OTMLNode* c = at(ctag);
        return (c ? c->read<T>() : def);
    }

    template <typename T>
    T readAt(int pos, const T& def) const {
        OTMLNode* c = at(pos);
        return (c ? c->read<T>() : def);
    }

    template <typename T>
    T readAtPath(const std::string& path, const T& def) const {
        OTMLNode* c = atPath(path);
        return (c ? c->read<T>() : def);
    }

    // writing
    template <typename T>
    void write(T v) {
        if(!(*this << v))
            throwError(make_string("failed to cast to string node value of type ", typeid(T).name()));
    }

    template <typename T>
    void writeIn(int pos, T v) {
        OTMLNode* c;
        while(!at(pos))
            c = createNode();
        c->write<T>(v);
    }

    template <typename T>
    void writeIn(const std::string& ctag, T v) {
        OTMLNode* c = at(ctag);
        if(!c)
            c = createNode(ctag);
        c->write<T>(v);
    }

private:
    OTMLNode* m_parent;
    int m_line;
    std::string m_what;
    NodeList m_children;
    std::string m_tag;
    std::string m_value;
};

// read operators
template <typename T>
bool operator >> (const OTMLNode& node, T& v) { return safe_convert(node.value(), v); }

template <typename T>
bool operator >> (const OTMLNode& node, std::vector<T>& v) {
    v.resize(node.size());
    for(unsigned i=0;i<node.size();++i)
        v[i] = node.readAt<T>(i);
    return true;
}

template <typename T>
bool operator >> (const OTMLNode& node, std::list<T>& v) {
    for(unsigned i=0;i<node.size();++i)
        v.push_back(node.readAt<T>(i));
    return true;
}

template <typename K, typename T>
bool operator >> (const OTMLNode& node, std::map<K, T>& m) {
    for(int i=0;i<node.size();++i) {
        K k;
        if(!safe_convert(node.at(i)->tag(), k))
            return false;
        m[k] = node.at(i)->read<T>();
    }
    return true;
}

// write operators
template <typename T>
bool operator << (OTMLNode& node, const T& v) {
    std::string out;
    if(!safe_convert(v, out))
        return false;
    node.setValue(out);
    return true;
}

template <typename T>
bool operator << (OTMLNode& node, const std::vector<T>& v) {
    for(unsigned i=0;i<v.size();++i)
        node.createNode()->write(v[i]);
    return true;
}

template <typename T>
bool operator << (OTMLNode& node, const std::list<T>& v) {
    for(unsigned i=0;i<v.size();++i)
        node.createNode()->write(v[i]);
    return true;
}

template <typename K, typename T>
bool operator << (OTMLNode& node, const std::map<K, T>& m) {
    typename std::map<K, T>::const_iterator it;
    for(it = m.begin(); it != m.end(); ++it) {
        std::string k;
        if(!safe_convert(it->first, k))
            return false;
        node.createNode(k)->write(it->second);
    }
    return true;
}

#include "otmlnodeext.h"

#endif // OTMLNODE_H
