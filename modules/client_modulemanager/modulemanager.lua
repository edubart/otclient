ModuleManager = {}

local moduleManagerWindow
local moduleManagerButton
local moduleList

function ModuleManager.init()
  moduleManagerWindow = displayUI('modulemanager.otui')
  moduleManagerWindow:hide()
  moduleList = moduleManagerWindow:getChildById('moduleList')
  connect(moduleList, { onChildFocusChange = function(self, focusedChild)
                          if focusedChild == nil then return end
                          ModuleManager.updateModuleInfo(focusedChild:getText())
                        end })

  moduleManagerButton = TopMenu.addButton('moduleManagerButton', 'Module manager', 'modulemanager.png', ModuleManager.toggle)

  addEvent(ModuleManager.listModules)
end

function ModuleManager.terminate()
  moduleManagerWindow:destroy()
  moduleManagerWindow = nil
  moduleManagerButton:destroy()
  moduleManagerButton = nil
  moduleList = nil
end

function ModuleManager.hide()
  moduleManagerWindow:hide()
end

function ModuleManager.show()
  moduleManagerWindow:show()
  moduleManagerWindow:focus()
  moduleManagerWindow:raise()

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
  moduleList:destroyChildren()

  local modules = g_modules.getModules()
  for i,module in ipairs(modules) do
    local label = createWidget('ModuleListLabel', moduleList)
    label:setText(module:getName())
  end

  moduleList:focusChild(moduleList:getFirstChild(), ActiveFocusReason)
end

function ModuleManager.updateModuleInfo(moduleName)
  local name = ''
  local description = ''
  local autoLoad = ''
  local author = ''
  local website = ''
  local version = ''
  local canLoad = false
  local canUnload = false

  local module = g_modules.getModule(moduleName)
  if module then
    name = module:getName()
    description = module:getDescription()
    author = module:getAuthor()
    website = module:getWebsite()
    version = module:getVersion()
    canUnload = module:isLoaded()
    canLoad = not canUnload
  end

  moduleManagerWindow:recursiveGetChildById('moduleName'):setText(name)
  moduleManagerWindow:recursiveGetChildById('moduleDescription'):setText(description)
  moduleManagerWindow:recursiveGetChildById('moduleDescription'):wrapText()
  moduleManagerWindow:recursiveGetChildById('moduleAuthor'):setText(author)
  moduleManagerWindow:recursiveGetChildById('moduleWebsite'):setText(website)
  moduleManagerWindow:recursiveGetChildById('moduleVersion'):setText(version)

  moduleManagerWindow:recursiveGetChildById('moduleLoadButton'):setEnabled(canLoad)
  moduleManagerWindow:recursiveGetChildById('moduleUnloadButton'):setEnabled(canUnload)
end

function ModuleManager.loadCurrentModule()
  local focusedChild = moduleList:getFocusedChild()
  if focusedChild then
    local module = g_modules.getModule(focusedChild:getText())
    if module then
      module:load()
      ModuleManager.updateModuleInfo(module:getName())
    end
  end
end

function ModuleManager.unloadCurrentModule()
  local focusedChild = moduleList:getFocusedChild()
  if focusedChild then
    local module = g_modules.getModule(focusedChild:getText())
    if module then
      module:unload()
      ModuleManager.updateModuleInfo(module:getName())
    end
  end
end

