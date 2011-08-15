#include "tibiadat.h"
#include "tibiaspr.h"
#include <core/resourcemanager.h>

TibiaDat g_tibiaDat;

bool TibiaDat::load(const std::string& filename)
{
    std::stringstream fin;
    g_resources.loadFile(filename, fin);

    fin.read((char*)&m_signature, 4);

    fin.read((char*)&m_groupCount[0], 2);
    fin.read((char*)&m_groupCount[1], 2);
    fin.read((char*)&m_groupCount[2], 2);
    fin.read((char*)&m_groupCount[3], 2);

    m_groupCount[0] -= 99;

    m_totalCount = m_groupCount[0] + m_groupCount[1] + m_groupCount[2] + m_groupCount[3];

    m_thingsAttributes = new ThingAttributes*[m_totalCount+1];
    for(uint16 i = 0; i <= m_totalCount; i++)
        m_thingsAttributes[i] = NULL;

    uint8 read_byte;
    uint16 read_short;
    //uint32 read_long;

    for(uint16 id = 0; (id <= m_totalCount) && !fin.eof(); id++) {
        m_thingsAttributes[id] = new ThingAttributes();
        m_thingsAttributes[id]->id = id;

        uint8 opt;
        bool done = false;
        while(!done) {
            fin.read((char*)&opt, 1);

            if(opt == 0x00) { // Ground tile
                fin.read((char*)&m_thingsAttributes[id]->speed, 2);
                m_thingsAttributes[id]->group = THING_GROUP_GROUND;
            }
            else if(opt == 0x01) { // All OnTop
                m_thingsAttributes[id]->alwaysOnTop = true;
                m_thingsAttributes[id]->alwaysOnTopOrder = 1;
            }
            else if(opt == 0x02) { // Can walk trough (open doors, arces, bug pen fence)
                m_thingsAttributes[id]->alwaysOnTop = true;
                m_thingsAttributes[id]->alwaysOnTopOrder = 2;
            }
            else if(opt == 0x03) { // Can walk trough (arces)
                m_thingsAttributes[id]->alwaysOnTop = true;
                m_thingsAttributes[id]->alwaysOnTopOrder = 3;
            }
            else if(opt == 0x04) { // Container
                m_thingsAttributes[id]->group = THING_GROUP_CONTAINER;
            }
            else if(opt == 0x05) { // Stackable
                m_thingsAttributes[id]->stackable = true;
            }
            else if(opt == 0x06) { // Unknown

            }
            else if(opt == 0x07) { // Useable
                m_thingsAttributes[id]->useable = true;
            }
            else if(opt == 0x08) { // Writtable
                m_thingsAttributes[id]->group = THING_GROUP_WRITEABLE;
                m_thingsAttributes[id]->readable = true;
                fin.read((char*)&m_thingsAttributes[id]->subParam07, 2);
            }
            else if(opt == 0x09) { // Writtable once
                // Writtable objects that can't be edited by players
                m_thingsAttributes[id]->readable = true;
                fin.read((char*)&m_thingsAttributes[id]->subParam08, 2);
            }
            else if(opt == 0x0A) { // Fluid containers
                fin.read((char*)&read_byte, 1);
                m_thingsAttributes[id]->group = THING_GROUP_FLUID;
            }
            else if(opt == 0x0B) { // Splashes
                m_thingsAttributes[id]->group = THING_GROUP_SPLASH;
            }
            else if(opt == 0x0C) { // Blocks solid objects (creatures, walls etc)
                m_thingsAttributes[id]->blockSolid = true;
            }
            else if(opt == 0x0D) { // Not moveable
                m_thingsAttributes[id]->moveable = false;
            }
            else if(opt == 0x0E) { // Blocks missiles (walls, magic wall etc)
                m_thingsAttributes[id]->blockProjectile = true;
            }
            else if(opt == 0x0F) { // Blocks pathfind algorithms (monsters)
                m_thingsAttributes[id]->blockPathFind = true;
            }
            else if(opt == 0x10) { // Blocks monster movement (flowers, parcels etc)
                m_thingsAttributes[id]->pickupable = true;
            }
            else if(opt == 0x11) { // Hangable objects (wallpaper etc)
                m_thingsAttributes[id]->isHangable = true;
            }
            else if(opt == 0x12) { // Horizontal wall
                m_thingsAttributes[id]->isHorizontal = true;
            }
            else if(opt == 0x13) { // Vertical wall
                m_thingsAttributes[id]->isVertical = true;
            }
            else if(opt == 0x14) { // Rotable
                m_thingsAttributes[id]->rotable = true;
            }
            else if(opt == 0x15) { // Light info
                fin.read((char*)&m_thingsAttributes[id]->lightLevel, 2);
                fin.read((char*)&m_thingsAttributes[id]->lightColor, 2);
            }
            else if(opt == 0x16) {
            }
            else if(opt == 0x17) { // Changes floor
            }
            else if(opt == 0x18) { // Thing must be drawed with offset
                m_thingsAttributes[id]->hasHeight = true;
                fin.read((char*)&m_thingsAttributes[id]->xOffset, 1);
                fin.read((char*)&m_thingsAttributes[id]->yOffset, 1);

                fin.read((char*)&read_short, 2);
            }
            else if(opt == 0x19) { // pixels characters height

                fin.read((char*)&m_thingsAttributes[id]->xOffset, 1);
                fin.read((char*)&m_thingsAttributes[id]->yOffset, 1);
                //logDebug((int)m_thingsAttributes[id]->xOffset, " ", (int)m_thingsAttributes[id]->yOffset);
            }
            else if(opt == 0x1A) {
                //m_thingsAttributes[id]->hasHeight = true;
            }
            else if(opt == 0x1B) {
            }
            else if(opt == 0x1C) { // Minimap color
                fin.read((char*)&m_thingsAttributes[id]->miniMapColor, 2);
                m_thingsAttributes[id]->hasMiniMapColor = true;
            }
            else if(opt == 0x1D) { // Unknown
                fin.read((char*)&read_short, 2);
                if(read_short == 1112)
                    m_thingsAttributes[id]->readable = true;
            }
            else if(opt == 0x1E) {
            }
            else if(opt == 0x1F) {
                m_thingsAttributes[id]->lookThrough = true;
            }
            else if(opt == 0x20) {
            }
            else if(opt == 0xFF) {
                done = true;
            }
            else {
                logDebug("Unknown byte code: ", opt);
                return false;
            }
        }

        fin.read((char*)&m_thingsAttributes[id]->width, 1);
        fin.read((char*)&m_thingsAttributes[id]->height, 1);
        if((m_thingsAttributes[id]->width > 1) || (m_thingsAttributes[id]->height > 1))
            fin.read((char*)&read_byte, 1);

        fin.read((char*)&m_thingsAttributes[id]->blendframes, 1);
        fin.read((char*)&m_thingsAttributes[id]->xdiv, 1);
        fin.read((char*)&m_thingsAttributes[id]->ydiv, 1);
        fin.read((char*)&m_thingsAttributes[id]->zdiv, 1);
        fin.read((char*)&m_thingsAttributes[id]->animcount, 1);

        // Read sprites id.
        uint16 totalSprites = m_thingsAttributes[id]->width * m_thingsAttributes[id]->height * m_thingsAttributes[id]->blendframes * m_thingsAttributes[id]->xdiv * m_thingsAttributes[id]->ydiv * m_thingsAttributes[id]->zdiv * m_thingsAttributes[id]->animcount;

        m_thingsAttributes[id]->sprites = new uint16[totalSprites];
        for(uint16 i = 0; i < totalSprites; i++) {
            fin.read((char*)&read_short, 2);
            m_thingsAttributes[id]->sprites[i] = read_short-1;
        }
    }

    return true;
}

ThingAttributes *TibiaDat::getItemAttributes(uint16 id)
{
    return m_thingsAttributes[id - 100];
}

ThingAttributes *TibiaDat::getCreatureAttributes(uint16 id)
{
    return m_thingsAttributes[id - 1 + m_groupCount[0]];
}

ThingAttributes *TibiaDat::getEffectAttributes(uint16 id)
{
    return m_thingsAttributes[id - 1 + m_groupCount[0] + m_groupCount[1]];
}

ThingAttributes *TibiaDat::getShotAttributes(uint16 id)
{
    return m_thingsAttributes[id - 100 + m_groupCount[0] + m_groupCount[1] + m_groupCount[2]];
}
