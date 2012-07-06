Particles = { }

-- private variables
local particlesWindow
local particlesButton

-- private functions
local function onExtendedParticles(protocol, opcode, buffer)

end

-- public functions
function Particles.init()
  particlesWindow = g_ui.displayUI('particles.otui')
  particlesButton = TopMenu.addLeftButton('particlesButton', tr('Particles Manager'), 'particles.png', Particles.toggle)

  local particlesList = particlesWindow:getChildById('particlesList')
  g_keyboard.bindKeyPress('Up', function() particlesList:focusPreviousChild(KeyboardFocusReason) end, particlesWindow)
  g_keyboard.bindKeyPress('Down', function() particlesList:focusNextChild(KeyboardFocusReason) end, particlesWindow)

  Extended.register(ExtendedParticles, onExtendedParticles)
end

function Particles.terminate()
  particlesWindow:destroy()
  particlesWindow = nil

  particlesButton:destroy()
  particlesButton = nil

  Extended.unregister(ExtendedParticles)
end

function Particles.show()
  Particles.refreshList()
  particlesWindow:show()
  particlesWindow:raise()
  particlesWindow:focus()
end

function Particles.hide()
  particlesWindow:hide()
end

function Particles.toggle()
  if particlesWindow:isVisible() then
    Particles.hide()
  else
    Particles.show()
  end
end

function Particles.refreshInfo()
  local particlesList = particlesWindow:getChildById('particlesList')
  local widget = particlesList:getFocusedChild()

  local name = particlesWindow:getChildById('name')
  name:setText(widget.effect:getName())

  local location = particlesWindow:getChildById('location')
  location:setText(widget.effect:getFile())

  local description = particlesWindow:getChildById('description')
  description:setText(widget.effect:getDescription())
end

function Particles.refreshList()
  local particlesList = particlesWindow:getChildById('particlesList')
  particlesList.onChildFocusChange = nil
  particlesList:destroyChildren()

  local firstChild = nil
  local effects = g_particles.getEffectsTypes()
  for name,effect in pairs(effects) do
    local label = g_ui.createWidget('ParticlesListLabel', particlesList)
    label:setText(name)
    label.effect = effect

    if not firstChild then
      firstChild = label
    end
  end

  particlesList.onChildFocusChange = Particles.refreshInfo
  if firstChild then
    firstChild:focus()
  end
end

function Particles.start()
  local particlesList = particlesWindow:getChildById('particlesList')
  local focusedEffect = particlesList:getFocusedChild()
  local preview = particlesWindow:getChildById('preview')
  preview:addEffect(focusedEffect:getText())
end
