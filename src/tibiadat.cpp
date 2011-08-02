#include "tibiadat.h"
#include <core/resources.h>

TibiaDat g_tibiaDat;

bool TibiaDat::load(const std::string& filename)
{
    std::stringstream fin;
    if(!g_resources.loadFile(filename, fin))
        return false;

    fin.read((char*)&m_signature, 4);

    fin.read((char*)&m_groupCount[0], 2);
    fin.read((char*)&m_groupCount[1], 2);
    fin.read((char*)&m_groupCount[2], 2);
    fin.read((char*)&m_groupCount[3], 2);

    m_groupCount[0] -= 99;

    m_totalCount = m_groupCount[0] + m_groupCount[1] + m_groupCount[2] + m_groupCount[3];

    m_items = new Item*[m_totalCount+1];
    for(uint16 i = 0; i <= m_totalCount; i++)
        m_items[i] = NULL;

    uint8 read_byte;
    uint16 read_short;
    uint32 read_long;

    for(uint16 id = 0; (id <= m_totalCount) && !fin.eof(); id++) {
        m_items[id] = new Item();
        m_items[id]->id = id;

        uint8 opt;
        bool done = false;
        while(!done) {
            fin.read((char*)&opt, 1);

            if(opt == 0x00) { // Ground tile
                fin.read((char*)&m_items[id]->speed, 2);
                m_items[id]->group = ITEM_GROUP_GROUND;
            }
            else if(opt == 0x01) { // All OnTop
                m_items[id]->alwaysOnTop = true;
                m_items[id]->alwaysOnTopOrder = 1;
            }
            else if(opt == 0x02) { // Can walk trough (open doors, arces, bug pen fence)
                m_items[id]->alwaysOnTop = true;
                m_items[id]->alwaysOnTopOrder = 2;
            }
            else if(opt == 0x03) { // Can walk trough (arces)
                m_items[id]->alwaysOnTop = true;
                m_items[id]->alwaysOnTopOrder = 3;
            }
            else if(opt == 0x04) { // Container
                m_items[id]->group = ITEM_GROUP_CONTAINER;
            }
            else if(opt == 0x05) { // Stackable
                m_items[id]->stackable = true;
            }
            else if(opt == 0x06) { // Unknown

            }
            else if(opt == 0x07) { // Useable
                m_items[id]->useable = true;
            }
            else if(opt == 0x08) { // Writtable
                m_items[id]->group = ITEM_GROUP_WRITEABLE;
                m_items[id]->readable = true;
                fin.read((char*)&m_items[id]->subParam07, 2);
            }
            else if(opt == 0x09) { // Writtable once
                // Writtable objects that can't be edited by players
                m_items[id]->readable = true;
                fin.read((char*)&m_items[id]->subParam08, 2);
            }
            else if(opt == 0x0A) { // Fluid containers
                fin.read((char*)&read_byte, 1);
                m_items[id]->group = ITEM_GROUP_FLUID;
            }
            else if(opt == 0x0B) { // Splashes
                m_items[id]->group = ITEM_GROUP_SPLASH;
            }
            else if(opt == 0x0C) { // Blocks solid objects (creatures, walls etc)
                m_items[id]->blockSolid = true;
            }
            else if(opt == 0x0D) { // Not moveable
                m_items[id]->moveable = false;
            }
            else if(opt == 0x0E) { // Blocks missiles (walls, magic wall etc)
                m_items[id]->blockProjectile = true;
            }
            else if(opt == 0x0F) { // Blocks pathfind algorithms (monsters)
                m_items[id]->blockPathFind = true;
            }
            else if(opt == 0x10) { // Blocks monster movement (flowers, parcels etc)
                m_items[id]->pickupable = true;
            }
            else if(opt == 0x11) { // Hangable objects (wallpaper etc)
                m_items[id]->isHangable = true;
            }
            else if(opt == 0x12) { // Horizontal wall
                m_items[id]->isHorizontal = true;
            }
            else if(opt == 0x13) { // Vertical wall
                m_items[id]->isVertical = true;
            }
            else if(opt == 0x14) { // Rotable
                m_items[id]->rotable = true;
            }
            else if(opt == 0x15) { // Light info
                fin.read((char*)&m_items[id]->lightLevel, 2);
                fin.read((char*)&m_items[id]->lightColor, 2);
            }
            else if(opt == 0x16) {
            }
            else if(opt == 0x17) { // Changes floor
            }
            else if(opt == 0x18) { // Unknown
                fin.read((char*)&read_long, 4);
            }
            else if(opt == 0x19) { // Has height
                m_items[id]->hasHeight = true;
                fin.read((char*)&m_items[id]->uheight, 2);
            }
            else if(opt == 0x1A) {
            }
            else if(opt == 0x1B) {
            }
            else if(opt == 0x1C) { // Minimap color
                fin.read((char*)&m_items[id]->miniMapColor, 2);
                m_items[id]->hasMiniMapColor = true;
            }
            else if(opt == 0x1D) { // Unknown
                fin.read((char*)&read_short, 2);
                if(read_short == 1112)
                    m_items[id]->readable = true;
            }
            else if(opt == 0x1E) {
            }
            else if(opt == 0x1F) {
                m_items[id]->lookThrough = true;
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

        fin.read((char*)&m_items[id]->width, 1);
        fin.read((char*)&m_items[id]->height, 1);
        if((m_items[id]->width > 1) || (m_items[id]->height > 1))
            fin.read((char*)&read_byte, 1);

        fin.read((char*)&m_items[id]->blendframes, 1);
        fin.read((char*)&m_items[id]->xdiv, 1);
        fin.read((char*)&m_items[id]->ydiv, 1);
        fin.read((char*)&m_items[id]->zdiv, 1);
        fin.read((char*)&m_items[id]->animcount, 1);

        // Read sprites id.
        uint16 totalSprites = m_items[id]->width * m_items[id]->height * m_items[id]->blendframes * m_items[id]->xdiv * m_items[id]->ydiv * m_items[id]->zdiv * m_items[id]->animcount;

        m_items[id]->sprites = new uint16[totalSprites];
        for(uint16 i = 0; i < totalSprites; i++) {
            fin.read((char*)&read_short, 2);
            m_items[id]->sprites[i] = read_short-1;
        }
    }

    return true;
}

Item *TibiaDat::getItem(uint16 id)
{
    // items id start at 100.
    return m_items[id - 100];
}
