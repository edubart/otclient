#ifndef OTMLNODE_H
#define OTMLNODE_H

#include "declarations.h"

class OTMLNode : public std::enable_shared_from_this<OTMLNode>
{
public:
    virtual ~OTMLNode() { }

    static OTMLNodePtr create(std::string tag = fw::empty_string, bool unique = false);
    static OTMLNodePtr create(std::string tag, std::string value);

    std::string tag() const { return m_tag; }
    int size() const { return m_children.size(); }
    OTMLNodePtr parent() const { return m_parent.lock(); }
    std::string source() const { return m_source; }

    bool isUnique() const { return m_unique; }
    bool isNull() const { return m_null; }

    bool hasTag() const { return !m_tag.empty(); }
    bool hasValue() const { return !m_value.empty(); }
    bool hasChildren() const;
    bool hasChildAt(const std::string& childTag) { return !!get(childTag); }
    bool hasChildAtIndex(int childIndex) { return !!getIndex(childIndex); }

    void setTag(std::string tag) { m_tag = tag; }
    void setValue(const std::string& value) { m_value = value; }
    void setNull(bool null) { m_null = null; }
    void setUnique(bool unique) { m_unique = unique; }
    void setParent(const OTMLNodePtr& parent) { m_parent = parent; }
    void setSource(const std::string& source) { m_source = source; }

    OTMLNodePtr get(const std::string& childTag) const;
    OTMLNodePtr getIndex(int childIndex) const;

    OTMLNodePtr at(const std::string& childTag);
    OTMLNodePtr atIndex(int childIndex);

    void addChild(const OTMLNodePtr& newChild);
    bool removeChild(const OTMLNodePtr& oldChild);
    bool replaceChild(const OTMLNodePtr& oldChild, const OTMLNodePtr& newChild);
    void merge(const OTMLNodePtr& node);
    void clear();

    OTMLNodeList children() const;
    OTMLNodePtr clone() const;

    template<typename T = std::string>
    T value();
    template<typename T = std::string>
    T valueAt(const std::string& childTag);
    template<typename T = std::string>
    T valueAtIndex(int childIndex);
    template<typename T = std::string>
    T valueAt(const std::string& childTag, const T& def);
    template<typename T = std::string>
    T valueAtIndex(int childIndex, const T& def);

    template<typename T>
    void write(const T& v);
    template<typename T>
    void writeAt(const std::string& childTag, const T& v);
    template<typename T>
    void writeIn(const T& v);

    virtual std::string emit();

protected:
    OTMLNode() : m_unique(false), m_null(false) { }

    OTMLNodeList m_children;
    OTMLNodeWeakPtr m_parent;
    std::string m_tag;
    std::string m_value;
    std::string m_source;
    bool m_unique;
    bool m_null;
};

#include "otmlexception.h"

template<typename T>
T OTMLNode::value() {
    T ret;
    if(!fw::cast(m_value, ret))
        throw OTMLException(shared_from_this(), fw::mkstr("failed to cast node value to type '", fw::demangle_type<T>(), "'"));
    return ret;
}

template<typename T>
T OTMLNode::valueAt(const std::string& childTag) {
    OTMLNodePtr node = at(childTag);
    return node->value<T>();
}

template<typename T>
T OTMLNode::valueAtIndex(int childIndex) {
    OTMLNodePtr node = atIndex(childIndex);
    return node->value<T>();
}

template<typename T>
T OTMLNode::valueAt(const std::string& childTag, const T& def) {
    if(OTMLNodePtr node = get(childTag))
        if(!node->isNull())
            return node->value<T>();
    return def;
}

template<typename T>
T OTMLNode::valueAtIndex(int childIndex, const T& def) {
    if(OTMLNodePtr node = getIndex(childIndex))
        return node->value<T>();
    return def;
}

template<typename T>
void OTMLNode::write(const T& v) {
    m_value = fw::safe_cast<std::string>(v);
}

template<typename T>
void OTMLNode::writeAt(const std::string& childTag, const T& v) {
    OTMLNodePtr child = OTMLNode::create(childTag);
    child->write<T>(v);
    addChild(child);
}

template<typename T>
void OTMLNode::writeIn(const T& v) {
    OTMLNodePtr child = OTMLNode::create();
    child->write<T>(v);
    addChild(child);
}

#endif

