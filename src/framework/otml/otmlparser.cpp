#include "otmlparser.h"
#include "otmlnode.h"

#include <boost/algorithm/string.hpp>

OTMLParser::OTMLParser(std::istream& in, std::string what) :
    m_currentDepth(0), m_currentLine(0),
    m_rootNode(new OTMLNode), m_currentParent(m_rootNode), m_previousNode(0),
    m_what(what), m_in(in)
{
    parse();
}

OTMLParser::~OTMLParser()
{
    delete m_rootNode;
}

void OTMLParser::throwError(const std::string& message, int line)
{
    std::stringstream ss;
    ss << "OTML syntax error";
    if(!m_what.empty())
        ss  << " in '" << m_what << "'";
    if(line > 0)
        ss << " at line " << line;
    ss << ": "  << message;
    throw OTMLException(ss.str());
}

void OTMLParser::parse()
{
    m_rootNode->setTag("document");

    while(m_in.good() && !m_in.eof()) {
        m_currentLine++;
        std::string line;
        std::getline(m_in, line);
        parseLine(line);
    }
}

void OTMLParser::parseLine(std::string line)
{
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

    // a depth above, change current parent to the previous added node
    if(depth == m_currentDepth+1) {
        m_currentParent = m_previousNode;
    // a depth below, change parent to previus parent
    } else if(depth < m_currentDepth) {
        for(int i=0;i<m_currentDepth-depth;++i)
            m_currentParent = m_currentParent->parent();
    // else if it isn't the current depth it's a syntax error
    } else if(depth != m_currentDepth) {
        throwError("invalid indentation level", m_currentLine);
    }

    // update current depth
    m_currentDepth = depth;

    // add node
    OTMLNode* node = m_currentParent->createNode();
    m_previousNode = node;
    parseNode(node, line);
}

void OTMLParser::parseNode(OTMLNode* node, std::string data)
{
    std::string tag;
    std::string value;
    std::size_t dotsPos = data.find_first_of(':');

    // its a node that has a value but no tag
    if(!data.empty() && data[0] == '-') {
        value = data.substr(1);
        boost::trim(value);

        //  check if it's value is shortcut for adding a child node
        if(dotsPos != std::string::npos && !value.empty() && value[0] != '"') {
            OTMLNode* child = node->createNode();
            parseNode(child, value);
            value.clear();
        }
    }
    // its a node that has tag and possible a value
    else if(dotsPos != std::string::npos) {
        tag = data.substr(0, dotsPos);
        value = data.substr(dotsPos+1);
    }
    // its a node that has only a tag
    else {
        tag = data;
    }

    // set node tag
    boost::trim(tag);
    node->setTag(tag);

    // set node line
    node->setLine(m_currentLine);

    // process node value
    parseNodeValue(node, value);
}

void OTMLParser::parseNodeValue(OTMLNode* node, std::string value)
{
    boost::trim(value);
    if(value.empty())
        return;

    // multiline text scalar
    if(value[0] == '|') {
        std::string multiLineData;
        do {
            std::string line;
            size_t lastPos = m_in.tellg();
            std::getline(m_in, line);

            // calculate numspaces
            std::size_t numSpaces = line.find_first_not_of(' ');

            // depth above current depth, add the text to the multiline
            if(numSpaces != std::string::npos && (int)numSpaces >= (m_currentDepth+1)*2) {
                boost::trim(line);
                parseTextValue(line);
                multiLineData += line + "\n";
            }
            // if has contents below the current depth, its a node
            else if(numSpaces != std::string::npos) {
                m_in.seekg(lastPos, std::ios::beg);
                break;
            }
            // else its just a new line
            else {
                multiLineData += "\n";
            }
        } while(!m_in.eof());

        // determine how to treat last new lines
        if(value.length() == 1 || (value.length() == 2 && value[1] == '-')) {
            // remove all new lines at the end
            while(*multiLineData.rbegin() == '\n')
                multiLineData.erase(multiLineData.length()-1, 1);
            // keep just one extra line
            if(value[0] == '-')
                multiLineData.append("\n");
        } else if(value.length() > 2 || value[1] != '+')
            throwError("invalid multiline identifier", m_currentLine);

        node->setValue(multiLineData);
    }
    // sequence
    else if(value[0] == '[') {
        std::vector<std::string> tokens;
        parseTokens(value.substr(1), tokens);
        foreach(std::string& token, tokens) {
            OTMLNode* child = node->createNode();
            child->setLine(m_currentLine);
            parseNodeValue(child, token);
        }
    }
    // inline map
    else if(value[0] == '{') {
        std::vector<std::string> tokens;
        parseTokens(value.substr(1), tokens);
        foreach(std::string& token, tokens) {
            OTMLNode* child = node->createNode();
            parseNode(child, token);
        }
    }
    // text scalar
    else {
        parseTextValue(value);
        node->setValue(value);
    }
}

void OTMLParser::parseTextValue(std::string& value)
{
    if(value[0] == '"' && value[value.length()-1] == '"') {
        value =  value.substr(1, value.length()-2);
        // escape characters
        boost::replace_all(value, "\\\\", "\\");
        boost::replace_all(value, "\\\"", "\"");
        boost::replace_all(value, "\\n", "\n");
    }
}

void OTMLParser::parseTokens(std::string data, std::vector<std::string>& out)
{
    bool inQuote = false;
    int brackets = 1;

    std::string tmp;
    uint i = 0;
    do {
        if(i<data.length()) {
            char c = data[i];
            tmp += c;

            if(c == '"') {
                if(!inQuote)
                    inQuote = true;
                else if(data[i-1] != '\\')
                    inQuote = false;
            } else if(!inQuote) {
                if(c == '{' || c == '[')
                    brackets++;
                else if(c == '}' || c == ']')
                    brackets--;
                else if(c == ',' && brackets == 1) {
                    tmp.erase(tmp.length()-1);
                    boost::trim(tmp);
                    if(!tmp.empty())
                        out.push_back(tmp);
                    tmp.clear();
                }
            }

            if(brackets == 0) {
                tmp.erase(tmp.length()-1);
                boost::trim(tmp);
                if(!tmp.empty())
                    out.push_back(tmp);
                break;
            }
        }
        if(i+1 >= data.length() && !m_in.eof()) {
            std::string line;
            std::getline(m_in, line);
            boost::trim(line);
            data += " ";
            data += line;
        }
        ++i;
    } while(i<data.length());

    if(brackets != 0)
        throwError("no matching bracket while parsing, did you forget to close one?", m_currentLine);
}
