#ifndef DATMANAGER_H
#define DATMANAGER_H

#include <framework/global.h>
#include "thing.h"

class DatManager
{
public:
    bool load(const std::string& filename);
    void unload();

    void parseThingAttributes(std::stringstream& fin, ThingAttributes& thingAttributes);
    void parseThingAttributesOpt(std::stringstream& fin, ThingAttributes& thingAttributes, uint8 opt);

    const ThingAttributes& getItemAttributes(uint16 id) { return m_itemsAttributes[id - 100]; }
    const ThingAttributes& getCreatureAttributes(uint16 id)  { return m_creaturesAttributes[id - 1]; }
    const ThingAttributes& getEffectAttributes(uint16 id) { return m_effectsAttributes[id - 100]; }
    const ThingAttributes& getShotAttributes(uint16 id) { return m_shotsAttributes[id - 1]; }

    uint32 getSignature() { return m_signature; }

private:
    uint32 m_signature;

    std::vector<ThingAttributes> m_itemsAttributes;
    std::vector<ThingAttributes> m_creaturesAttributes;
    std::vector<ThingAttributes> m_effectsAttributes;
    std::vector<ThingAttributes> m_shotsAttributes;
};

extern DatManager g_dat;

#endif
