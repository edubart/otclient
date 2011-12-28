-- place any code for testing purposes here

function init()
  local box = UIComboBox.create()
  box:setStyle('ComboBox')
  box:moveTo({x=100, y=8})
  UI.display(box)
end

addEvent(init)