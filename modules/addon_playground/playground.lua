-- place any code for testing purposes here

function init()
  local box = createWidget('ComboBox')
  box:moveTo({x=100, y=8})
  displayUI(box)
end

addEvent(init)