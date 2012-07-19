ModuleManager = {}

local moduleManagerWindow
local moduleManagerButton
local moduleList

function ModuleManager.init()
  moduleManagerWindow = g_ui.displayUI('modulemanager.otui')
  moduleManagerWindow:hide()
  moduleList = moduleManagerWindow:getChildById('moduleList')
  connect(moduleList, { onChildFocusChange = function(self, focusedChild)
                          if focusedChild == nil then return end
                          ModuleManager.updateModuleInfo(focusedChild:getText())
                        end })

  g_keyboard.bindKeyPress('Up', function() moduleList:focusPreviousChild(KeyboardFocusReason) end, moduleManagerWindow)
  g_keyboard.bindKeyPress('Down', function() moduleList:focusNextChild(KeyboardFocusReason) end, moduleManagerWindow)

  moduleManagerButton = TopMenu.addLeftButton('moduleManagerButton', tr('Module Manager'), 'modulemanager.png', ModuleManager.toggle)

  -- refresh modules only after all modules are loaded
  addEvent(ModuleManager.listModules)
end

function ModuleManager.terminate()
  moduleManagerWindow:destroy()
  moduleManagerWindow = nil
  moduleManagerButton:destroy()
  moduleManagerButton = nil
  moduleList = nil
  ModuleManager = nil
end

function ModuleManager.hide()
  moduleManagerWindow:hide()
end

function ModuleManager.show()
  moduleManagerWindow:show()
  moduleManagerWindow:raise()
  moduleManagerWindow:focus()
end

function ModuleManager.toggle()
  if moduleManagerWindow:isVisible() then
    ModuleManager.hide()
  else
    ModuleManager.show()
  end
end

function ModuleManager.refreshModules()
  g_modules.discoverModules()
  ModuleManager.listModules()
end

function ModuleManager.listModules()
  if not moduleManagerWindow then return end

  moduleList:destroyChildren()

  local modules = g_modules.getModules()
  for i,module in ipairs(modules) do
    local label = g_ui.createWidget('ModuleListLabel', moduleList)
    label:setText(module:getName())
    label:setOn(module:isLoaded())
  end

  moduleList:focusChild(moduleList:getFirstChild(), ActiveFocusReason)
end

function ModuleManager.refreshLoadedModules()
  if not moduleManagerWindow then return end

  for i,child in ipairs(moduleList:getChildren()) do
    local module = g_modules.getModule(child:getText())
    child:setOn(module:isLoaded())
  end
end

function ModuleManager.updateModuleInfo(moduleName)
  if not moduleManagerWindow then return end

  local name = ''
  local description = ''
  local autoLoad = ''
  local author = ''
  local website = ''
  local version = ''
  local loaded = false
  local canReload = false
  local canUnload = false

  local module = g_modules.getModule(moduleName)
  if module then
    name = module:getName()
    description = module:getDescription()
    author = module:getAuthor()
    website = module:getWebsite()
    version = module:getVersion()
    loaded = module:isLoaded()
    canReload = module:canReload()
    canUnload = module:canUnload()
  end

  moduleManagerWindow:recursiveGetChildById('moduleName'):setText(name)
  moduleManagerWindow:recursiveGetChildById('moduleDescription'):setText(description)
  moduleManagerWindow:recursiveGetChildById('moduleAuthor'):setText(author)
  moduleManagerWindow:recursiveGetChildById('moduleWebsite'):setText(website)
  moduleManagerWindow:recursiveGetChildById('moduleVersion'):setText(version)

  local reloadButton = moduleManagerWindow:recursiveGetChildById('moduleReloadButton')
  reloadButton:setEnabled(canReload)
  if loaded then reloadButton:setText('Reload')
  else reloadButton:setText('Load') end

  local unloadButton = moduleManagerWindow:recursiveGetChildById('moduleUnloadButton')
  unloadButton:setEnabled(canUnload)
end

function ModuleManager.reloadCurrentModule()
  local focusedChild = moduleList:getFocusedChild()
  if focusedChild then
    local module = g_modules.getModule(focusedChild:getText())
    if module then
      module:reload()
      if ModuleManager == nil then return end
      ModuleManager.updateModuleInfo(module:getName())
      ModuleManager.refreshLoadedModules()
      ModuleManager.show()
    end
  end
end

function ModuleManager.unloadCurrentModule()
  local focusedChild = moduleList:getFocusedChild()
  if focusedChild then
    local module = g_modules.getModule(focusedChild:getText())
    if module then
      module:unload()
      if ModuleManager == nil then return end
      ModuleManager.updateModuleInfo(module:getName())
      ModuleManager.refreshLoadedModules()
    end
  end
end

function ModuleManager.reloadAllModules()
  g_modules.reloadModules()
  ModuleManager.refreshLoadedModules()
  ModuleManager.show()
end

