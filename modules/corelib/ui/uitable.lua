-- @docclass
UITable = extends(UIScrollArea)

function UITable.create()
  local table = UITable.internalCreate()
  table.rows = {}
  table.rows.columns = {}
  table.rowStyle = {}
  table.columStyle = {}
  return table
end

function UITable:destroy()
  for k,row in pairs(self.rows) do
    row.onClick = nil
  end
  self.rows = {}
end

function UITable:onStyleApply(styleName, styleNode)
  UIScrollArea.onStyleApply(self, styleName, styleNode)
  for name, value in pairs(styleNode) do
    if name == 'column-style' then
      addEvent(function()
        self:setRowStyle(self:getParent():getChildById(value))
      end)
    elseif name == 'row-style' then
      addEvent(function()
        self:setRowStyle(self:getParent():getChildById(value))
      end)
    end
  end
end

function UITable:addRow(columns, rowStyle, columStyle)
  local row = g_ui.createWidget(rowStyle, self)
  row.columns = {}

  for k, data in pairs(columns) do
    local col = g_ui.createWidget(columStyle, row)
    col:setText(data)
    table.insert(row.columns, col)
  end
  row.onClick = function(row) self:selectRow(row) end
  table.insert(self.rows, row)
  return row
end

function UITable:removeRow(row)
  if self.selectedRow == row then
    self:selectRow(nil)
  end
  row.onClick = nil
  table.removevalue(self.rows, row)
end

function UITable:selectRow(selectedRow)
  if selectedRow == self.selectedRow then return end

  local previousSelectedRow = self.selectedRow
  self.selectedRow = selectedRow

  if previousSelectedRow then
    previousSelectedRow:setChecked(false)
  end

  if selectedRow then
    selectedRow:setChecked(true)
  end

  signalcall(self.onSelectionChange, self, selectedRow, previousSelectedRow)
end

function UITable:setRowStyle(style)
  self.rowStyle = style
  for k, row in pairs(self.rows) do
    row:setStyle(style)
  end
end

function UITable:setColumnStyle(style)
  self.columStyle = style
  for k, col in pairs(self.rows.columns) do
    col:setStyle(style)
  end
end