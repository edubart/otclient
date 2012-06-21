/*
 * Copyright (c) 2010-2012 OTClient <https://github.com/edubart/otclient>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "itemloader.h"
#include <framework/core/resourcemanager.h>
#include <framework/core/filestream.h>
#define TIXML_USE_STL // use STL strings instead.
#include <framework/thirdparty/tinyxml.h>

ItemLoader g_itemLoader;

ItemLoader::~ItemLoader()
{
	m_items.clear();
}

ItemDataPtr ItemLoader::getType(uint16 id) const
{
	for (const ItemDataPtr &it : m_items) {
		if (it->id == id)
			return it;
	}

	return nullptr;
}

void ItemLoader::addType(uint16 id, ItemDataPtr type)
{
	if (getType(id))
		return;

	m_items.push_back(type);
}

bool ItemLoader::loadOtb(const std::string &name)
{
	FileStreamPtr fin = g_resources.openFile(name);
	if (!fin) {
		g_logger.error(stdext::format("failed to open file '%s'", name));
		return false;
	}

	fin->getU32(); // skip version

	uint32 type;
	uint8 node = fin->readNode(node, type);

	fin->getU32(); // skip flags...
	if (fin->getU8() == 0x01) { // version
		fin->getU8(); // skip length.
		dwMajorVersion = fin->getU32();
		dwMinorVersion = fin->getU32();
		dwBuildNumber  = fin->getU32();
	}

	uint16 lastId = 99;
	ItemDataPtr newItem;

	while ((node = fin->readNode(node, type))) {
		if (!(newItem = ItemDataPtr(new ItemData))) {
			g_logger.error("failed to read new item from OTB");
			return false;
		}

		newItem->group = (ItemGroup)type;
		fin->getU32(); // skip flags

		ItemAttrib attr;
		while ((attr = (ItemAttrib)fin->getU8())) {
			uint16 dsize = fin->getU16();
			switch (attr) {
			case ServerId: {
				if (dsize != sizeof(uint16)) {
					g_logger.error("Invalid data size");
					return false;
				}

				uint16 serverId = fin->getU16();
				if (serverId > 20000 && serverId < 20100) {
					serverId -= 20000;
				} else if (lastId > 99 && lastId != serverId - 1) {
					static ItemDataPtr dummyItemType(new ItemData);
					while (lastId != serverId - 1) {
						dummyItemType->id = ++lastId;
						addType(lastId, dummyItemType);
					}
				}

				newItem->id = serverId;
				lastId = serverId;
				break;
			} case ClientId: {
				newItem->clientId = fin->getU16();
				break;
			} case Speed: {
				fin->getU16(); // skip speed
				break;
			} case Light2: {
				if (!fin->seek(dsize)) {
					g_logger.error(stdext::format("fail to skip light block with size %d", dsize));
					return false;
				}
			} default: {
				if (!fin->seek(dsize)) {
					g_logger.error(stdext::format("fail to skip unknown data with size %d", dsize));
					return false;
				}
			}
			}
		}

		addType(newItem->id, newItem);
	}

	return true;
}

bool ItemLoader::loadXML(const std::string &name)
{
	TiXmlDocument doc(name.c_str());
	if (!doc.LoadFile()) {
		g_logger.error(stdext::format("failed to load xml '%s'", name));
		return false;
	}

	TiXmlElement* root = doc.FirstChildElement();
	if (!root) {
		g_logger.error("invalid xml root");
		return false;
	}

	if (root->ValueTStr() != "items") {
		g_logger.error("invalid xml tag name, should be 'items'");
		return false;
	}

	for (TiXmlElement *element = root->FirstChildElement(); element; element = element->NextSiblingElement()) {
		if (element->ValueTStr() != "item")
			continue;

		std::string name = element->Attribute("id");
		if (name.empty())
			continue;

		uint16 id = stdext::unsafe_cast<uint16>(element->Attribute("id"));
		uint16 idEx = 0;
		if (!id) {
			bool found = false;
			// fallback into reading fromid and toid
			uint16 fromid = stdext::unsafe_cast<uint16>(element->Attribute("fromid"));
			uint16 toid = stdext::unsafe_cast<uint16>(element->Attribute("toid"));
			ItemDataPtr iType;
			for (int __id = fromid; __id < toid; ++__id) {
				if (!(iType = getType(__id)))
					continue;

				iType->name = name;
				idEx = iType->id == fromid ? fromid : toid;
				found = true;
			}

			if (!found)
				continue;
		}

		ItemDataPtr iType = getType(id);
		if (!iType) {
			iType = ItemDataPtr(new ItemData);
			iType->id = idEx ? idEx : id;
			iType->name = name;
			addType(iType->id, iType);
		}

		iType->name = name;

		for (TiXmlElement *attr = element->FirstChildElement(); attr; attr = attr->NextSiblingElement()) {
			if (attr->ValueTStr() != "attribute")
				continue;

			std::string key = attr->Attribute("key");
			std::string value = attr->Attribute("value");
			if (key == "type") {
				if (value == "magicfield")
					iType->group = IsMagicField;
				else if (value == "key")
					iType->group = IsKey;
				else if (value == "depot")
					iType->isDepot = true;
				else if (value == "teleport")
					iType->group = IsTeleport;
				else if (value == "bed")
					iType->isBed = true;
				else if (value == "door")
					iType->group = IsDoor;
			} else if (key == "name") {
				iType->name = value;
			} else if (key == "description") {
				iType->description = value;
			} else if (key == "weight") {
				iType->weight = stdext::unsafe_cast<double>(stdext::unsafe_cast<double>(value) / 100.f);
			} else if (key == "containerSize") {
				int containerSize = stdext::unsafe_cast<int>(value);
				if (containerSize)
					iType->containerSize = containerSize;
				iType->group = IsContainer;
			} else if (key == "writeable") {
				if (!value.empty())
					iType->group = IsWritable;
			} else if (key == "maxTextLen") {
				iType->maxTextLength = stdext::unsafe_cast<int>(value);
			} else if (key == "charges") {
				iType->charges = stdext::unsafe_cast<int>(value);
			}
		}
	}

	doc.Clear();
	return true;
}
