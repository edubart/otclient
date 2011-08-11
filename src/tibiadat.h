#ifndef TIBIADAT_H
#define TIBIADAT_H

#include <global.h>
#include "item.h"

class TibiaDat
{
public:
    bool load(const std::string& filename);

    ItemAttributes *getItemAttributes(uint16 id);

    uint16 getGroupCount(int i) { return m_groupCount[i]; }

    uint32 getSignature() { return m_signature; }
    uint16 getTotalCount() { return m_totalCount; }

private:
    uint32 m_signature, m_totalCount;
    uint16 m_groupCount[4];

    ItemAttributes **m_itemsAttributes;
};

extern TibiaDat g_tibiaDat;

#endif // TIBIADAT_H
