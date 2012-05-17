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
    ${CMAKE_CURRENT_LIST_DIR}/otclient.cpp
    ${CMAKE_CURRENT_LIST_DIR}/luafunctions.cpp

    # otclient luascript
    ${CMAKE_CURRENT_LIST_DIR}/luascript/luavaluecasts.cpp

    # otclient core
    ${CMAKE_CURRENT_LIST_DIR}/core/game.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/map.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/mapview.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/thingtype.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/thingstype.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/spritemanager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/item.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/container.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/tile.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/thing.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/creature.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/effect.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/missile.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/localplayer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/outfit.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/animatedtext.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/statictext.cpp

    # otclient ui
    ${CMAKE_CURRENT_LIST_DIR}/ui/uiitem.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/uicreature.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/uimap.cpp

    # otclient net
    ${CMAKE_CURRENT_LIST_DIR}/net/protocollogin.cpp
    ${CMAKE_CURRENT_LIST_DIR}/net/protocolgame.cpp
    ${CMAKE_CURRENT_LIST_DIR}/net/protocolgamesend.cpp
    ${CMAKE_CURRENT_LIST_DIR}/net/protocolgameparse.cpp
)
