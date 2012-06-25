-- this is the first file executed when the application starts
-- we have to load the first modules form here

-- setup application name and version
g_app.setName('OTClient')
g_app.setCompactName('otclient')
g_app.setVersion('0.4.0_dev')

-- setup logger
g_logger.setLogFile(g_resources.getWorkDir() .. g_app.getCompactName() .. ".log")

-- print first terminal message
g_logger.info(g_app.getName() .. ' ' .. g_app.getVersion() .. ' (rev ' .. g_app.getBuildRevision() .. ') built on ' .. g_app.getBuildDate())

-- add modules directory to the search path
if not g_resources.addToSearchPath(g_resources.getWorkDir() .. "modules", true) then
    g_logger.fatal("Unable to add modules directory to the search path.")
end

-- try to add addons path too
g_resources.addToSearchPath(g_resources.getWorkDir() .. "addons", true)

-- setup directory for saving configurations
g_resources.setupWriteDir(g_app.getCompactName())

-- load configurations
g_configs.load("/config.otml")

g_modules.discoverModules()

-- core modules 0-99
g_modules.autoLoadModules(99);
g_modules.ensureModuleLoaded("corelib")

-- client modules 100-499
g_modules.autoLoadModules(499);
g_modules.ensureModuleLoaded("client")

-- game modules 500-999
g_modules.autoLoadModules(999);
g_modules.ensureModuleLoaded("game")

-- addons 1000-9999
g_modules.autoLoadModules(9999)

if g_resources.fileExists("/otclientrc.lua") then
    dofile("/otclientrc.lua")
end

g_things.loadOtb('/lalal.otb')
