#ifndef OTCLIENT_H
#define OTCLIENT_H

#include <framework/application.h>
#include <otclient/global.h>

class OTClient : public Application
{
public:
    void init(const std::vector<std::string>& args);
    void registerLuaFunctions();
};

extern OTClient g_otclient;

#endif
