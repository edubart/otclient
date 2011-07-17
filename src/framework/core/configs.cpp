#include "configs.h"
#include "resources.h"

#include <otml/otml.h>

Configs g_configs;

bool Configs::load(const std::string& fileName)
{
    m_fileName = fileName;

    if(!g_resources.fileExists(fileName))
        return false;

    std::stringstream fin;
    if(!g_resources.loadFile(fileName, fin))
        return false;

    try {
        OTMLParser parser(fin, fileName);
        parser.getDocument()->read(&m_confsMap);
    } catch(OTMLException e) {
        logError("ERROR: Malformed config file: ", e.what());
        return false;
    }

    return true;
}

void Configs::save()
{
    if(!m_fileName.empty()) {
        OTMLEmitter emitter;
        emitter.createDocument()->write(m_confsMap);
        g_resources.saveFile(m_fileName, emitter.emitDocument());
    }
}
