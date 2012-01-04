-- place any code for testing purposes here

function init()
  local box = createWidget('ComboBox')
  box:moveTo({x=100, y=8})
  box:addOption('Option 1')
  box:addOption('Option 2')
  displayUI(box)
end

addEvent(init)