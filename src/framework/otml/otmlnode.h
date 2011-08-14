#ifndef OTMLNODE_H
#define OTMLNODE_H

#include "otmldeclarations.h"
#include "otmlexception.h"

class OTMLNode : public std::enable_shared_from_this<OTMLNode>
{
public:
    OTMLNode();
    virtual ~OTMLNode() { }

    std::string value() const;
    std::string tag() const;
    int size() const;
    OTMLNodePtr parent() const;
    const OTMLNodeList& childNodes() const;
    std::string source() const;

    bool hasTag() const;
    bool hasValue() const;
    bool hasChildNodes() const;
    bool hasChild(const std::string& childTag) const;
    bool hasChild(int index) const;
    bool isUnique() const;

    void setTag(std::string tag);
    void setValue(const std::string& value);
    void setParent(const OTMLNodePtr& parent);
    void setUnique(bool unique = true);
    void setSource(const std::string& source);

    /// Same as get but if the child node doesn't exist throws an OTMLException
    OTMLNodePtr at(const std::string& childTag);
    OTMLNodePtr at(int childIndex);

    /// Get a child node, if doesn't exists returns nullptr
    OTMLNodePtr get(const std::string& childTag) const;
    OTMLNodePtr get(int childIndex) const;

    void addChild(const OTMLNodePtr& newChild);
    bool removeChild(const OTMLNodePtr& oldChild);
    bool replaceChild(const OTMLNodePtr& oldChild, const OTMLNodePtr& newChild);

    /// Remove all children
    void clear();

    /// Recursively copy children from another node to this node
    void merge(const OTMLNodePtr& node);

    /// Recursively clone this node into a new one
    OTMLNodePtr clone() const;

    /// Emits this node to a std::string
    virtual std::string emit();

    template<typename T>
    T read();

    template<typename T>
    T read(const T& def);

    template<typename T, typename U>
    T readAt(const U& childIdentifier);

    template<typename T, typename U>
    T readAt(const U& childIdentifier, const T& def);

    template<typename T>
    void write(const T& v);

    template<typename T>
    void writeAt(const std::string& childTag, const T& v);

    template<typename T>
    void writeIn(const T& v);

private:
    std::string m_tag;
    std::string m_value;
    std::string m_source;
    bool m_unique;
    OTMLNodeList m_childNodes;
    OTMLNodeWeakPtr m_parent;
};

// templates for reading values
template<typename T>
T OTMLNode::read() {
    T v;
    if(!from_otmlnode(shared_from_this(), v))
        throw OTMLException(shared_from_this(),
                            aux::make_string("failed to cast node value to type '", aux::demangle_type<T>(), "'"));
    return v;
}

template<typename T>
T OTMLNode::read(const T& def) {
    if(hasValue())
        return read<T>();
    return def;
}

template<typename T, typename U>
T OTMLNode::readAt(const U& childIdentifier) {
    OTMLNodePtr child = at(childIdentifier);
    return child->read<T>();
}

template<typename T, typename U>
T OTMLNode::readAt(const U& childIdentifier, const T& def) {
    OTMLNodePtr child = get(childIdentifier);
    if(!child)
        return def;
    return child->read<T>(def);
}


// templates for writing values
template<typename T>
void OTMLNode::write(const T& v) {
    to_otmlnode(shared_from_this(), v);
}

template<typename T>
void OTMLNode::writeAt(const std::string& childTag, const T& v) {
    OTMLNodePtr child = get(childTag);
    bool created = false;
    if(!child) {
        child = OTMLNodePtr(new OTMLNode);
        child->setTag(childTag);
        child->setUnique();
        created = true;
    }
    child->write<T>(v);
    if(created)
        addChild(child);
}

template<typename T>
void OTMLNode::writeIn(const T& v) {
    OTMLNodePtr child = OTMLNodePtr(new OTMLNode);
    child->write<T>(v);
    addChild(child);
}

// templates for casting a node to another type
template<typename T>
bool from_otmlnode(OTMLNodePtr node, T& v) {
    return aux::cast(node->value(), v);
}

template<typename T>
bool from_otmlnode(OTMLNodePtr node, std::vector<T>& v) {
    v.resize(node->size());
    for(unsigned i=0;i<node->size();++i)
        v[i] = node->readAt<T>(i);
    return true;
}

template<typename T>
bool from_otmlnode(OTMLNodePtr node, std::list<T>& v) {
    for(unsigned i=0;i<node->size();++i)
        v.push_back(node->readAt<T>(i));
    return true;
}

template <typename K, typename T>
bool from_otmlnode(OTMLNodePtr node, std::map<K, T>& m) {
    for(int i=0;i<node->size();++i) {
        K k;
        if(!aux::cast(node->at(i)->tag(), k))
            return false;
        m[k] = node->at(i)->read<T>();
    }
    return true;
}

// templates for casting a type to a node
template<typename T>
void to_otmlnode(OTMLNodePtr node, const T& v) {
    node->setValue(aux::unsafe_cast<std::string>(v));
}

template<typename T>
void to_otmlnode(OTMLNodePtr node, const std::vector<T>& v) {
    for(unsigned i=0;i<v.size();++i) {
        OTMLNodePtr newNode(new OTMLNode);
        newNode->write(v[i]);
        node->addChild(newNode);
    }
}

template<typename T>
void to_otmlnode(OTMLNodePtr node, const std::list<T>& v) {
    for(unsigned i=0;i<v.size();++i) {
        OTMLNodePtr newNode(new OTMLNode);
        newNode->write(v[i]);
        node->addChild(newNode);
    }
}

template <typename K, typename T>
void to_otmlnode(OTMLNodePtr node, const std::map<K, T>& m) {
    for(auto it = m.begin(); it != m.end(); ++it) {
        std::string k = aux::unsafe_cast<std::string>(it->first);
        OTMLNodePtr newNode(new OTMLNode);
        newNode->setTag(k);
        newNode->setUnique();
        newNode->write(it->second);
        node->addChild(newNode);
    }
}

#endif
