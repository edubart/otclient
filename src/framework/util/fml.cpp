#include "fml.h"

namespace FML
{

bool fml_convert(const std::string& input, bool& b) {
    static std::string validNames[2][4] = {{"true","yes","on","1"}, {"false","no","off","0"}};
    bool ret = false;
    for(int i=0;i<4;++i) {
        if(input == validNames[0][i]) {
            b = true;
            ret = true;
            break;
        } else if(input == validNames[1][i]) {
            b = false;
            ret = true;
            break;
        }
    }
    return ret;
}

bool fml_convert(const std::string& input, std::string& output)
{
    output = input;
    return true;
}

Node* Node::at(const std::string& childTag) const  {
    int i=0;
    while(i<size() && at(i)->tag()!=childTag)
        ++i;
    return at(i);
}

Node* Node::createNode(std::string tag)
{
    Node* node = new Node;
    node->setTag(tag);
    addNode(node);
    return node;
}

void Node::addNode(Node* node) {
    if(node->hasTag() && node->hasValue()) {
        if(Node* other = at(node->tag())) {
            if(removeNode(other))
                delete node;
        }
    }
    m_children.push_back(node);
    node->setParent(this);
}

bool Node::removeNode(Node* node) {
    for(NodeList::iterator it = m_children.begin(); it != m_children.end(); ++it) {
        if((*it) == node) {
            m_children.erase(it);
            return true;
        }
    }
    return false;
}

std::string Node::generateErrorMessage(const std::string& message) const {
    std::stringstream ss;
    ss << "FML error";
    if(!what().empty())
        ss << " in '" << what() << "'";
    if(m_line > 0)
        ss << " at line " << m_line;
    if(m_line > 0 && hasTag())
        ss << ", in node '" << tag() << "'";
    ss << ": " << message;
    return ss.str();
}

void Parser::load(std::istream& in)
{
    if(m_rootNode)
        delete m_rootNode;
    m_rootNode = new Node(what());
    m_rootNode->setTag("root");
    m_currentParent = m_rootNode;
    m_currentDepth = m_currentLine = 0;
    m_multilineMode = DONT_MULTILINE;
    m_multilineData.clear();

    while(in.good() && !in.eof()) {
        m_currentLine++;
        std::string line;
        std::getline(in, line);
        parseLine(line);
    }

    if(isMultilining())
        stopMultilining();
}

void Parser::parseLine(std::string& line)
{
    // process multiline data first
    if(isMultilining() && parseMultiline(line))
        return;

    // calculate depth
    std::size_t numSpaces = line.find_first_not_of(' ');

    // trim left whitespaces
    boost::trim_left(line);

    // skip comment or empty lines
    if(line[0] == '#' || line.empty())
        return;

    // calculate depth
    int depth = 0;
    if(numSpaces != std::string::npos)
        depth = numSpaces / 2;

    // check for syntax error
    if(numSpaces != std::string::npos && numSpaces % 2 != 0)
        throwError("file must be idented every 2 whitespaces", m_currentLine);

    // a depth above
    if(depth == m_currentDepth+1) {
        // change current parent to the previous added node
        m_currentParent = m_previousNode;
        // a depth below, change parent to previus parent and add new node inside previuos parent
    } else if(depth < m_currentDepth) {
        // change current parent to the the new depth parent
        for(int i=0;i<m_currentDepth-depth;++i)
            m_currentParent = m_currentParent->parent();
        // else if nots the same depth it's a syntax error
    } else if(depth != m_currentDepth) {
        throwError("invalid indentation level", m_currentLine);
    }

    // update current depth
    m_currentDepth = depth;

    // add node
    Node* node = m_currentParent->createNode();
    parseNode(node, line);
    m_previousNode = node;
}

void Parser::parseNode(Node* node, std::string& line)
{
    // determine node tag and value
    std::string tag;
    std::string value;

    // its a node that has tag and possible a value
    std::size_t dotsPos = line.find_first_of(':');
    if(dotsPos != std::string::npos) {
        tag = line.substr(0, dotsPos);
        value = line.substr(dotsPos+1);
    } else if(line[0] == '-') // its a node that has a value but no tag
        value = line.substr(1);
    else // its a node that has only a tag
        tag = line;

    // set node tag
    boost::trim(tag);
    node->setTag(tag);

    // set node line
    node->setLine(m_currentLine);

    // process node value
    parseNodeValue(node, value);
}

void Parser::parseNodeValue(Node* node, std::string& value)
{
    boost::trim(value);
    if(value.empty())
        return;

    // multiline text scalar
    if(value[0] == '|') {
        // determine multiline mode
        m_multilineMode = MULTILINE_DONT_FOLD;
        if(value.length() == 2) {
            switch(value[1]) {
                case '-':
                    m_multilineMode = MULTILINE_FOLD_BLOCK;
                    break;
                case '+':
                    m_multilineMode = MULTILINE_FOLD_FLOW;
                    break;
                default:
                    throwError("invalid multiline identifier", m_currentLine);
                    break;
            }
        }
        m_currentDepth++;
    }
    // sequence
    else if(value[0] == '[') {
        if(!boost::ends_with(value, "]"))
            throwError("missing ']' in sequence", m_currentLine);

        // erase '[' and ']'
        value.erase(value.length()-1, 1);
        value.erase(0, 1);

        // split commas
        boost::tokenizer<boost::escaped_list_separator<char> > tokens(value);
        for(boost::tokenizer<boost::escaped_list_separator<char> >::iterator it = tokens.begin(); it != tokens.end(); ++it) {
            std::string tmp = (*it);
            boost::trim(tmp);
            if(!tmp.empty()) {
                Node* child = node->createNode();
                child->setLine(m_currentLine);
                child->setValue(tmp);
            }
        }
    }
    // text scalar
    else {
        node->setValue(parseTextValue(value));
    }
}

std::string Parser::parseTextValue(std::string value)
{
    if(value[0] == '"' && value[value.length()-1] == '"') {
        value =  value.substr(1, value.length()-2);
        // escape characters
        boost::replace_all(value, "\\\\", "\\");
        boost::replace_all(value, "\\\"", "\"");
        boost::replace_all(value, "\\n", "\n");
    }
    return value;
}

void Parser::stopMultilining()
{
    // remove all new lines at the end
    if(m_multilineMode == MULTILINE_DONT_FOLD || m_multilineMode == MULTILINE_FOLD_BLOCK) {
        while(*m_multilineData.rbegin() == '\n')
            m_multilineData.erase(m_multilineData.length()-1, 1);
    }

    if(m_multilineMode == MULTILINE_FOLD_BLOCK)
        m_multilineData.append("\n");

    m_previousNode->setValue(m_multilineData);
    m_multilineMode = DONT_MULTILINE;
    m_currentDepth--;
    m_multilineData.clear();
}

bool Parser::parseMultiline(std::string line)
{
    // calculate depth
    std::size_t numSpaces = line.find_first_not_of(' ');

    // depth above or equal current depth, add the text to the multiline
    if(numSpaces != std::string::npos && (int)numSpaces >= m_currentDepth*2) {
        m_multilineData += parseTextValue(line.substr(m_currentDepth*2)) + "\n";
        return true;
        // depth below the current depth, check if it is a node
    } else if(numSpaces == std::string::npos || (int)numSpaces < m_currentDepth*2) {
        // if it has contents, its a node, then we must end multiline
        if(line.find_first_not_of(' ') != std::string::npos) {
            stopMultilining();
            // the line still have a node on it
        }
        // no contents, just an extra line
        else {
            m_multilineData += "\n";
            return true;
        }
    }
    return false;
}

void Parser::throwError(const std::string& message, int line)
{
    std::stringstream ss;
    ss << "FML syntax error";
    if(!m_what.empty())
        ss  << " in '" << m_what << "'";
    if(line > 0)
        ss << " at line " << line;
    ss << ": "  << message;
    throw Exception(ss.str());
}

std::string Emitter::emitNodeValue(Node* node)
{
    std::string value = node->value();
    if(value[0] == '"' || *value.rbegin() == '"' || value.find("\n") != std::string::npos) {
        boost::replace_all(value, "\\", "\\\\");
        boost::replace_all(value, "\"", "\\\"");
        boost::replace_all(value, "\n", "\\n");
        value.append("\"");
        value.insert(0, "\"");
    }
    return value;
}

std::string Emitter::emitNode(Node* node, int currentDepth)
{
    std::stringstream ss;
    for(int i=1;i<currentDepth;++i)
        ss << "  ";
    if(currentDepth > 0) {
        if(node->hasTag())
            ss << node->tag();
        if(node->hasValue())
            ss << (node->hasTag() ? ": " : "- ") << emitNodeValue(node) << std::endl;
        else
            ss << std::endl;
    }
    for(int i=0;i<node->size();++i)
        ss << emitNode(node->at(i), currentDepth+1);
    return ss.str();
}

}
