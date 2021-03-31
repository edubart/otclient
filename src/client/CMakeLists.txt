# client options
add_definitions(-DCLIENT)
option(BOT_PROTECTION "Enable bot protection" ON)
if(BOT_PROTECTION)
    add_definitions(-DBOT_PROTECTION)
    message(STATUS "Bot protection: ON")
else(BOT_PROTECTION)
    message(STATUS "Bot protection: OFF")
endif(BOT_PROTECTION)

set(client_SOURCES ${client_SOURCES}
    # client
    ${CMAKE_CURRENT_LIST_DIR}/const.h
    ${CMAKE_CURRENT_LIST_DIR}/global.h
    ${CMAKE_CURRENT_LIST_DIR}/luafunctions.cpp
    ${CMAKE_CURRENT_LIST_DIR}/client.cpp
    ${CMAKE_CURRENT_LIST_DIR}/client.h

    # core
    ${CMAKE_CURRENT_LIST_DIR}/animatedtext.cpp
    ${CMAKE_CURRENT_LIST_DIR}/animatedtext.h
    ${CMAKE_CURRENT_LIST_DIR}/animator.h
    ${CMAKE_CURRENT_LIST_DIR}/animator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/container.cpp
    ${CMAKE_CURRENT_LIST_DIR}/container.h
    ${CMAKE_CURRENT_LIST_DIR}/creature.cpp
    ${CMAKE_CURRENT_LIST_DIR}/creature.h
    ${CMAKE_CURRENT_LIST_DIR}/declarations.h
    ${CMAKE_CURRENT_LIST_DIR}/effect.cpp
    ${CMAKE_CURRENT_LIST_DIR}/effect.h
    ${CMAKE_CURRENT_LIST_DIR}/game.cpp
    ${CMAKE_CURRENT_LIST_DIR}/game.h
    ${CMAKE_CURRENT_LIST_DIR}/shadermanager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/shadermanager.h
    ${CMAKE_CURRENT_LIST_DIR}/item.cpp
    ${CMAKE_CURRENT_LIST_DIR}/item.h
    ${CMAKE_CURRENT_LIST_DIR}/localplayer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/localplayer.h
    ${CMAKE_CURRENT_LIST_DIR}/map.cpp
    ${CMAKE_CURRENT_LIST_DIR}/map.h
    ${CMAKE_CURRENT_LIST_DIR}/mapio.cpp
    ${CMAKE_CURRENT_LIST_DIR}/mapview.cpp
    ${CMAKE_CURRENT_LIST_DIR}/mapview.h
    ${CMAKE_CURRENT_LIST_DIR}/minimap.cpp
    ${CMAKE_CURRENT_LIST_DIR}/minimap.h
    ${CMAKE_CURRENT_LIST_DIR}/lightview.cpp
    ${CMAKE_CURRENT_LIST_DIR}/lightview.h
    ${CMAKE_CURRENT_LIST_DIR}/missile.cpp
    ${CMAKE_CURRENT_LIST_DIR}/missile.h
    ${CMAKE_CURRENT_LIST_DIR}/outfit.cpp
    ${CMAKE_CURRENT_LIST_DIR}/outfit.h
    ${CMAKE_CURRENT_LIST_DIR}/player.cpp
    ${CMAKE_CURRENT_LIST_DIR}/player.h
    ${CMAKE_CURRENT_LIST_DIR}/spritemanager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/spritemanager.h
    ${CMAKE_CURRENT_LIST_DIR}/statictext.cpp
    ${CMAKE_CURRENT_LIST_DIR}/statictext.h
    ${CMAKE_CURRENT_LIST_DIR}/thing.cpp
    ${CMAKE_CURRENT_LIST_DIR}/thing.h
    ${CMAKE_CURRENT_LIST_DIR}/thingtypemanager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/thingtypemanager.h
    ${CMAKE_CURRENT_LIST_DIR}/thingtype.cpp
    ${CMAKE_CURRENT_LIST_DIR}/thingtype.h
    ${CMAKE_CURRENT_LIST_DIR}/itemtype.cpp
    ${CMAKE_CURRENT_LIST_DIR}/itemtype.h
    ${CMAKE_CURRENT_LIST_DIR}/tile.cpp
    ${CMAKE_CURRENT_LIST_DIR}/tile.h
    ${CMAKE_CURRENT_LIST_DIR}/houses.cpp
    ${CMAKE_CURRENT_LIST_DIR}/houses.h
    ${CMAKE_CURRENT_LIST_DIR}/towns.cpp
    ${CMAKE_CURRENT_LIST_DIR}/towns.h
    ${CMAKE_CURRENT_LIST_DIR}/creatures.cpp
    ${CMAKE_CURRENT_LIST_DIR}/creatures.h

    # lua
    ${CMAKE_CURRENT_LIST_DIR}/luavaluecasts.cpp
    ${CMAKE_CURRENT_LIST_DIR}/luavaluecasts.h

    # net
    ${CMAKE_CURRENT_LIST_DIR}/protocolcodes.cpp
    ${CMAKE_CURRENT_LIST_DIR}/protocolcodes.h
    ${CMAKE_CURRENT_LIST_DIR}/protocolgame.cpp
    ${CMAKE_CURRENT_LIST_DIR}/protocolgame.h
    ${CMAKE_CURRENT_LIST_DIR}/protocolgameparse.cpp
    ${CMAKE_CURRENT_LIST_DIR}/protocolgamesend.cpp

    # ui
    ${CMAKE_CURRENT_LIST_DIR}/uicreature.cpp
    ${CMAKE_CURRENT_LIST_DIR}/uicreature.h
    ${CMAKE_CURRENT_LIST_DIR}/uiitem.cpp
    ${CMAKE_CURRENT_LIST_DIR}/uiitem.h
    ${CMAKE_CURRENT_LIST_DIR}/uimap.cpp
    ${CMAKE_CURRENT_LIST_DIR}/uimap.h
    ${CMAKE_CURRENT_LIST_DIR}/uiminimap.cpp
    ${CMAKE_CURRENT_LIST_DIR}/uiminimap.h
    ${CMAKE_CURRENT_LIST_DIR}/uiprogressrect.cpp
    ${CMAKE_CURRENT_LIST_DIR}/uiprogressrect.h
    ${CMAKE_CURRENT_LIST_DIR}/uimapanchorlayout.cpp
    ${CMAKE_CURRENT_LIST_DIR}/uimapanchorlayout.h
    ${CMAKE_CURRENT_LIST_DIR}/uisprite.cpp
    ${CMAKE_CURRENT_LIST_DIR}/uisprite.h

    # util
    ${CMAKE_CURRENT_LIST_DIR}/position.h
)

if(NOT MSVC)
    set_source_files_properties(${CMAKE_CURRENT_LIST_DIR}/luafunctions.cpp
                                PROPERTIES LANGUAGE CXX COMPILE_FLAGS "-g0 -Os")
endif()
