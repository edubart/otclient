RadioGroup = newclass()

function RadioGroup.create()
  local radiogroup = RadioGroup.internalCreate()
  radiogroup.widgets = {}
  return radiogroup
end

function RadioGroup:destroy()
  while #self.widgets ~= 0 do
    self:removeWidget(self.widgets[1])
  end
end

function RadioGroup:addWidget(widget)
  table.insert(self.widgets, widget)
  widget.onMousePress = function(widget) self:selectWidget(widget) end
end

function RadioGroup:removeWidget(widget)
  if self.selectedWidget == widget then
    self:selectWidget(nil)
  end
  widget.onMousePress = nil
  table.removevalue(self.widgets, widget)
end

function RadioGroup:selectWidget(selectedWidget)
  if selectedWidget == self.selectedWidget then return end

  local previousSelectedWidget = self.selectedWidget
  self.selectedWidget = selectedWidget

  if previousSelectedWidget then
    previousSelectedWidget:setChecked(false)
  end

  if selectedWidget then
    selectedWidget:setChecked(true)
  end

  signalcall(self.onSelectionChange, self, selectedWidget, previousSelectedWidget)
end
