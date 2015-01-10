-- @docclass
UIRadioGroup = newclass("UIRadioGroup")

function UIRadioGroup.create()
  local radiogroup = UIRadioGroup.internalCreate()
  radiogroup.widgets = {}
  radiogroup.selectedWidget = nil
  return radiogroup
end

function UIRadioGroup:destroy()
  for k,widget in pairs(self.widgets) do
    widget.onClick = nil
  end
  self.widgets = {}
end

function UIRadioGroup:addWidget(widget)
  table.insert(self.widgets, widget)
  widget.onClick = function(widget) self:selectWidget(widget) end
end

function UIRadioGroup:removeWidget(widget)
  if self.selectedWidget == widget then
    self:selectWidget(nil)
  end
  widget.onClick = nil
  table.removevalue(self.widgets, widget)
end

function UIRadioGroup:selectWidget(selectedWidget, dontSignal)
  if selectedWidget == self.selectedWidget then return end

  local previousSelectedWidget = self.selectedWidget
  self.selectedWidget = selectedWidget

  if previousSelectedWidget then
    previousSelectedWidget:setChecked(false)
  end

  if selectedWidget then
    selectedWidget:setChecked(true)
  end

  if not dontSignal then
    signalcall(self.onSelectionChange, self, selectedWidget, previousSelectedWidget)
  end
end

function UIRadioGroup:clearSelected()
  if not self.selectedWidget then return end

  local previousSelectedWidget = self.selectedWidget
  self.selectedWidget:setChecked(false)
  self.selectedWidget = nil

  signalcall(self.onSelectionChange, self, nil, previousSelectedWidget)
end

function UIRadioGroup:getSelectedWidget()
  return self.selectedWidget
end

function UIRadioGroup:getFirstWidget()
  return self.widgets[1]
end
