-- @docclass
UITable = extends(UIScrollArea)

function UITable.create()
  local table = UITable.internalCreate()
  table.rows = {}
  table.rows.columns = {}
  table.rowBaseStyle = nil
  table.columBaseStyle = nil
  table.selectedRow = nil
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
        self:setColumnStyle(value)
      end)
    elseif name == 'row-style' then
      addEvent(function()
        self:setRowStyle(value)
      end)
    end
  end
end

function UITable:addRow(columns)
  if not columns or type(columns) ~= 'table' then
    g_logger.error('UITable:addRow - table columns must be provided in a table')
  end

  -- TODO: table header rows as buttons.
  --[[if #self.rows < 1 then
    g_ui.createWidget(self.rowBaseStyle, self)
  end]]

  local row = g_ui.createWidget(self.rowBaseStyle, self)
  row.columns = {}

  for _, column in pairs(columns) do
    local col = g_ui.createWidget(self.columBaseStyle, row)
    if column[1] then
      col:setText(column[1])
    end
    if #column > 1 then
      col:setWidth(column[2])
    end
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
  self.rowBaseStyle = style
  --[[for k, row in pairs(self.rows) do
    row:setStyle(style)
  end]]
end

function UITable:setColumnStyle(style)
  self.columBaseStyle = style
  --[[for k, col in pairs(self.rows.columns) do
    col:setStyle(style)
  end]]
end