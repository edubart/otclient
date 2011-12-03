#include "otclient.h"

OTClient g_otclient;
Application& g_app = g_otclient;

void OTClient::init(const std::vector<std::string>& args)
{
    logInfo(Otc::AppName, " ", Otc::AppVersion);
    Application::init(Otc::AppName, args);
}
