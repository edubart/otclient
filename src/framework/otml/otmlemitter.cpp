#include "otmlemitter.h"
#include "otmldocument.h"

std::string OTMLEmitter::emitNode(const OTMLNodePtr& node, int currentDepth)
{
    std::stringstream ss;

    // emit nodes
    if(currentDepth >= 0) {
        // fill spaces for current depth
        for(int i=0;i<currentDepth;++i)
            ss << "  ";

        // emit node tag
        if(node->hasTag()) {
            ss << node->tag();

            // add ':' to if the node is unique or has value
            if(node->hasValue() || node->isUnique())
                ss << ":";
        } else
            ss << "-";

        // emit node value
        if(node->hasValue()) {
            ss << " ";

            std::string value = node->value();

            // emit multiline values
            if(value.find("\n") != std::string::npos) {
                if(value[value.length()-1] == '\n' && value[value.length()-2] == '\n')
                    ss << "|+";
                else if(value[value.length()-1] == '\n')
                    ss << "|";
                else
                    ss << "|-";

                //  multilines
                for(std::size_t pos = 0; pos < value.length(); ++pos) {
                    ss << "\n";

                    // fill spaces for multiline depth
                    for(int i=0;i<currentDepth+1;++i)
                        ss << "  ";

                    // fill until a new line
                    while(pos < value.length()) {
                        if(value[pos] == '\n')
                            break;
                        ss << value[pos++];
                    }
                }
            // emit inline values
            } else
                ss << value;
        }
    }

    // emit children
    for(int i=0;i<node->size();++i) {
        if(currentDepth >= 0 || i != 0)
            ss << "\n";
        ss << emitNode(node->at(i), currentDepth+1);
    }

    return ss.str();
}
