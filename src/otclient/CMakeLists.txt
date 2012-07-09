# CMAKE_CURRENT_LIST_DIR cmake 2.6 compatiblity
IF(${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 6)
    GET_FILENAME_COMPONENT(CMAKE_CURRENT_LIST_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)
ENDIF(${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 6)

# otclient options
OPTION(BOT_PROTECTION "Enable bot protection" ON)
SET(PROTOCOL 860 CACHE "Protocol version" STRING)
OPTION(CIPSOFT_RSA "Use cipsoft RSA to login into original tibia" OFF)
ADD_DEFINITIONS(-DPROTOCOL=${PROTOCOL})
MESSAGE(STATUS "Protocol: " ${PROTOCOL})

IF(CIPSOFT_RSA)
    ADD_DEFINITIONS(-DCIPSOFT_RSA -DOSTYPE=2)
    MESSAGE(STATUS "RSA: CipSoft")
ELSE()
    MESSAGE(STATUS "RSA: OTServ")
ENDIF()

IF(BOT_PROTECTION)
    ADD_DEFINITIONS(-DBOT_PROTECTION)
    MESSAGE(STATUS "Bot protection: ON")
ELSE(BOT_PROTECTION)
    MESSAGE(STATUS "Bot protection: OFF")
ENDIF(BOT_PROTECTION)

SET(otclient_SOURCES ${otclient_SOURCES}
    # otclient
    ${CMAKE_CURRENT_LIST_DIR}/const.h
    ${CMAKE_CURRENT_LIST_DIR}/global.h
    ${CMAKE_CURRENT_LIST_DIR}/luafunctions.cpp
    ${CMAKE_CURRENT_LIST_DIR}/otclient.cpp
    ${CMAKE_CURRENT_LIST_DIR}/otclient.h

    # core
    ${CMAKE_CURRENT_LIST_DIR}/core/animatedtext.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/animatedtext.h
    ${CMAKE_CURRENT_LIST_DIR}/core/container.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/container.h
    ${CMAKE_CURRENT_LIST_DIR}/core/creature.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/creature.h
    ${CMAKE_CURRENT_LIST_DIR}/core/declarations.h
    ${CMAKE_CURRENT_LIST_DIR}/core/effect.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/effect.h
    ${CMAKE_CURRENT_LIST_DIR}/core/game.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/game.h
    ${CMAKE_CURRENT_LIST_DIR}/core/shadermanager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/shadermanager.h
    ${CMAKE_CURRENT_LIST_DIR}/core/item.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/item.h
    ${CMAKE_CURRENT_LIST_DIR}/core/localplayer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/localplayer.h
    ${CMAKE_CURRENT_LIST_DIR}/core/map.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/map.h
    ${CMAKE_CURRENT_LIST_DIR}/core/mapview.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/mapview.h
    ${CMAKE_CURRENT_LIST_DIR}/core/missile.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/missile.h
    ${CMAKE_CURRENT_LIST_DIR}/core/outfit.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/outfit.h
    ${CMAKE_CURRENT_LIST_DIR}/core/player.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/player.h
    ${CMAKE_CURRENT_LIST_DIR}/core/spritemanager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/spritemanager.h
    ${CMAKE_CURRENT_LIST_DIR}/core/statictext.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/statictext.h
    ${CMAKE_CURRENT_LIST_DIR}/core/thing.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/thing.h
    ${CMAKE_CURRENT_LIST_DIR}/core/thingtypemanager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/thingtypemanager.h
    ${CMAKE_CURRENT_LIST_DIR}/core/thingtypedat.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/thingtypedat.h
    ${CMAKE_CURRENT_LIST_DIR}/core/thingtypeotb.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/thingtypeotb.h
    ${CMAKE_CURRENT_LIST_DIR}/core/tile.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/tile.h
    ${CMAKE_CURRENT_LIST_DIR}/core/houses.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/houses.h
    ${CMAKE_CURRENT_LIST_DIR}/core/towns.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/towns.h

    # lua
    ${CMAKE_CURRENT_LIST_DIR}/luascript/luavaluecasts.cpp
    ${CMAKE_CURRENT_LIST_DIR}/luascript/luavaluecasts.h

    # net
    ${CMAKE_CURRENT_LIST_DIR}/net/declarations.h
    ${CMAKE_CURRENT_LIST_DIR}/net/protocolcodes.h
    ${CMAKE_CURRENT_LIST_DIR}/net/protocolgame.cpp
    ${CMAKE_CURRENT_LIST_DIR}/net/protocolgame.h
    ${CMAKE_CURRENT_LIST_DIR}/net/protocolgameparse.cpp
    ${CMAKE_CURRENT_LIST_DIR}/net/protocolgamesend.cpp

    # ui
    ${CMAKE_CURRENT_LIST_DIR}/ui/declarations.h
    ${CMAKE_CURRENT_LIST_DIR}/ui/uicreature.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/uicreature.h
    ${CMAKE_CURRENT_LIST_DIR}/ui/uiitem.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/uiitem.h
    ${CMAKE_CURRENT_LIST_DIR}/ui/uimap.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/uimap.h
    ${CMAKE_CURRENT_LIST_DIR}/ui/uiprogressrect.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/uiprogressrect.h

    # util
    ${CMAKE_CURRENT_LIST_DIR}/util/position.h
)
