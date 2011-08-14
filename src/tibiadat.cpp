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

    m_itemsAttributes = new ItemAttributes*[m_totalCount+1];
    for(uint16 i = 0; i <= m_totalCount; i++)
        m_itemsAttributes[i] = NULL;

    uint8 read_byte;
    uint16 read_short;
    uint32 read_long;

    for(uint16 id = 0; (id <= m_totalCount) && !fin.eof(); id++) {
        m_itemsAttributes[id] = new ItemAttributes();
        m_itemsAttributes[id]->id = id;

        uint8 opt;
        bool done = false;
        while(!done) {
            fin.read((char*)&opt, 1);

            if(opt == 0x00) { // Ground tile
                fin.read((char*)&m_itemsAttributes[id]->speed, 2);
                m_itemsAttributes[id]->group = ITEM_GROUP_GROUND;
            }
            else if(opt == 0x01) { // All OnTop
                m_itemsAttributes[id]->alwaysOnTop = true;
                m_itemsAttributes[id]->alwaysOnTopOrder = 1;
            }
            else if(opt == 0x02) { // Can walk trough (open doors, arces, bug pen fence)
                m_itemsAttributes[id]->alwaysOnTop = true;
                m_itemsAttributes[id]->alwaysOnTopOrder = 2;
            }
            else if(opt == 0x03) { // Can walk trough (arces)
                m_itemsAttributes[id]->alwaysOnTop = true;
                m_itemsAttributes[id]->alwaysOnTopOrder = 3;
            }
            else if(opt == 0x04) { // Container
                m_itemsAttributes[id]->group = ITEM_GROUP_CONTAINER;
            }
            else if(opt == 0x05) { // Stackable
                m_itemsAttributes[id]->stackable = true;
            }
            else if(opt == 0x06) { // Unknown

            }
            else if(opt == 0x07) { // Useable
                m_itemsAttributes[id]->useable = true;
            }
            else if(opt == 0x08) { // Writtable
                m_itemsAttributes[id]->group = ITEM_GROUP_WRITEABLE;
                m_itemsAttributes[id]->readable = true;
                fin.read((char*)&m_itemsAttributes[id]->subParam07, 2);
            }
            else if(opt == 0x09) { // Writtable once
                // Writtable objects that can't be edited by players
                m_itemsAttributes[id]->readable = true;
                fin.read((char*)&m_itemsAttributes[id]->subParam08, 2);
            }
            else if(opt == 0x0A) { // Fluid containers
                fin.read((char*)&read_byte, 1);
                m_itemsAttributes[id]->group = ITEM_GROUP_FLUID;
            }
            else if(opt == 0x0B) { // Splashes
                m_itemsAttributes[id]->group = ITEM_GROUP_SPLASH;
            }
            else if(opt == 0x0C) { // Blocks solid objects (creatures, walls etc)
                m_itemsAttributes[id]->blockSolid = true;
            }
            else if(opt == 0x0D) { // Not moveable
                m_itemsAttributes[id]->moveable = false;
            }
            else if(opt == 0x0E) { // Blocks missiles (walls, magic wall etc)
                m_itemsAttributes[id]->blockProjectile = true;
            }
            else if(opt == 0x0F) { // Blocks pathfind algorithms (monsters)
                m_itemsAttributes[id]->blockPathFind = true;
            }
            else if(opt == 0x10) { // Blocks monster movement (flowers, parcels etc)
                m_itemsAttributes[id]->pickupable = true;
            }
            else if(opt == 0x11) { // Hangable objects (wallpaper etc)
                m_itemsAttributes[id]->isHangable = true;
            }
            else if(opt == 0x12) { // Horizontal wall
                m_itemsAttributes[id]->isHorizontal = true;
            }
            else if(opt == 0x13) { // Vertical wall
                m_itemsAttributes[id]->isVertical = true;
            }
            else if(opt == 0x14) { // Rotable
                m_itemsAttributes[id]->rotable = true;
            }
            else if(opt == 0x15) { // Light info
                fin.read((char*)&m_itemsAttributes[id]->lightLevel, 2);
                fin.read((char*)&m_itemsAttributes[id]->lightColor, 2);
            }
            else if(opt == 0x16) {
            }
            else if(opt == 0x17) { // Changes floor
            }
            else if(opt == 0x18) { // Unknown
                fin.read((char*)&read_long, 4);
            }
            else if(opt == 0x19) { // Has height
                m_itemsAttributes[id]->hasHeight = true;
                fin.read((char*)&m_itemsAttributes[id]->uheight, 2);
            }
            else if(opt == 0x1A) {
            }
            else if(opt == 0x1B) {
            }
            else if(opt == 0x1C) { // Minimap color
                fin.read((char*)&m_itemsAttributes[id]->miniMapColor, 2);
                m_itemsAttributes[id]->hasMiniMapColor = true;
            }
            else if(opt == 0x1D) { // Unknown
                fin.read((char*)&read_short, 2);
                if(read_short == 1112)
                    m_itemsAttributes[id]->readable = true;
            }
            else if(opt == 0x1E) {
            }
            else if(opt == 0x1F) {
                m_itemsAttributes[id]->lookThrough = true;
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

        fin.read((char*)&m_itemsAttributes[id]->width, 1);
        fin.read((char*)&m_itemsAttributes[id]->height, 1);
        if((m_itemsAttributes[id]->width > 1) || (m_itemsAttributes[id]->height > 1))
            fin.read((char*)&read_byte, 1);

        fin.read((char*)&m_itemsAttributes[id]->blendframes, 1);
        fin.read((char*)&m_itemsAttributes[id]->xdiv, 1);
        fin.read((char*)&m_itemsAttributes[id]->ydiv, 1);
        fin.read((char*)&m_itemsAttributes[id]->zdiv, 1);
        fin.read((char*)&m_itemsAttributes[id]->animcount, 1);

        // Read sprites id.
        uint16 totalSprites = m_itemsAttributes[id]->width * m_itemsAttributes[id]->height * m_itemsAttributes[id]->blendframes * m_itemsAttributes[id]->xdiv * m_itemsAttributes[id]->ydiv * m_itemsAttributes[id]->zdiv * m_itemsAttributes[id]->animcount;

        m_itemsAttributes[id]->sprites = new uint16[totalSprites];
        for(uint16 i = 0; i < totalSprites; i++) {
            fin.read((char*)&read_short, 2);
            m_itemsAttributes[id]->sprites[i] = read_short-1;
        }
    }

    return true;
}

ItemAttributes *TibiaDat::getItemAttributes(uint16 id)
{
    // items id start at 100.
    return m_itemsAttributes[id - 100];
}
