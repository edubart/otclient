#include "otmlparser.h"
#include "otmldocument.h"

OTMLParser::OTMLParser(OTMLDocumentPtr doc, std::istream& in) :
    currentDepth(0), currentLine(0),
    doc(doc), currentParent(doc), previousNode(0),
    in(in)
{
}

void OTMLParser::parse()
{
    if(!in.good())
        throw OTMLException(doc, "cannot read from input stream");

    while(!in.eof())
        parseLine(getNextLine());
}

std::string OTMLParser::getNextLine()
{
    currentLine++;
    std::string line;
    std::getline(in, line);
    return line;
}

int OTMLParser::getLineDepth(const std::string& line, bool multilining)
{
    // count number of spaces at the line beginning
    std::size_t spaces = 0;
    while(line[spaces] == ' ')
        spaces++;

    // pre calculate depth
    int depth = spaces / 2;

    if(!multilining || depth <= currentDepth) {
        // check the next character is a tab
        if(line[spaces] == '\t')
            throw OTMLException(doc, "indentation with tabs are not allowed", currentLine);

        // must indent every 2 spaces
        if(spaces % 2 != 0)
            throw OTMLException(doc, "must indent every 2 spaces", currentLine);
    }

    return depth;
}

void OTMLParser::parseLine(std::string line)
{
    int depth = getLineDepth(line);

    // remove line sides spaces
    boost::trim(line);

    // skip empty lines
    if(line.empty())
        return;

    // skip comments
    if(boost::starts_with(line, "//"))
        return;

    // a depth above, change current parent to the previous added node
    if(depth == currentDepth+1) {
        currentParent = previousNode;
    // a depth below, change parent to previous parent
    } else if(depth < currentDepth) {
        for(int i=0;i<currentDepth-depth;++i)
            currentParent = currentParent->parent();
    // if it isn't the current depth, it's a syntax error
    } else if(depth != currentDepth)
        throw OTMLException(doc, "invalid indentation depth, are you indenting correctly?", currentLine);

    // sets current depth
    currentDepth = depth;

    // alright, new depth is set, the line is not empty and it isn't a comment
    // then it must be a node, so we parse it
    parseNode(line);
}

void OTMLParser::parseNode(const std::string& data)
{
    std::string tag;
    std::string value;
    std::size_t dotsPos = data.find_first_of(':');

    // node that has no tag and may have a value
    if(!data.empty() && data[0] == '-') {
        value = data.substr(1);
        boost::trim(value);
    // node that has tag and possible a value
    } else if(dotsPos != std::string::npos) {
        tag = data.substr(0, dotsPos);
        if(data.size() > dotsPos+1)
            value = data.substr(dotsPos+1);
    // node that has only a tag
    } else {
        tag = data;
    }

    boost::trim(tag);
    boost::trim(value);

    // process multitine values
    if(value == "|" || value == "|-" || value == "|+") {
        // reads next lines until we can a value below the same depth
        std::string multiLineData;
        do {
            size_t lastPos = in.tellg();
            std::string line = getNextLine();
            int depth = getLineDepth(line, true);

            // depth above current depth, add the text to the multiline
            if(depth > currentDepth) {
                multiLineData += line.substr((currentDepth+1)*2);
            // it has contents below the current depth
            } else {
                // if not empty, its a node
                boost::trim(line);
                if(!line.empty()) {
                    // rewind and break
                    in.seekg(lastPos, std::ios::beg);
                    currentLine--;
                    break;
                }
            }
            multiLineData += "\n";
        } while(!in.eof());

        /* determine how to treat new lines at the end
         * | strip all new lines at the end and add just a new one
         * |- strip all new lines at the end
         * |+ keep all the new lines at the end (the new lines until next node)
         */
        if(value == "|" || value == "|-") {
            // remove all new lines at the end
            int lastPos = multiLineData.length();
            while(multiLineData[--lastPos] == '\n')
                multiLineData.erase(lastPos, 1);

            if(value == "|")
                multiLineData.append("\n");
        } // else it's |+

        value = multiLineData;
    }

    // create the node
    OTMLNodePtr node(new OTMLNode);
    node->setUnique(dotsPos != std::string::npos);
    node->setTag(tag);
    node->setValue(value);
    node->setSource(doc->source() + ":" + fw::safe_cast<std::string>(currentLine));
    currentParent->addChild(node);
    previousNode = node;
}
