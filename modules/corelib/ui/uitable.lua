-- @docclass
--[[
  TODO:
    * Make table headers more robust.
    * Get dynamic row heights working with text wrapping.
    * Every second row different background color applied.
]]
UITable = extends(UIWidget)

local HEADER_ID = 'row0'

function UITable.create()
  local table = UITable.internalCreate()
  table.headerRow = nil
  table.dataSpace = nil
  table.rows = {}
  table.rowBaseStyle = nil
  table.columns = {}
  table.columBaseStyle = nil
  table.headerRowBaseStyle = nil
  table.headerColumnBaseStyle = nil
  table.selectedRow = nil
  return table
end

function UITable:onDestroy()
  for k,row in pairs(self.rows) do
    row.onClick = nil
  end
  self.rows = {}
  self.columns = {}
  self.headerRow = {}
  self.selectedRow = nil

  if self.dataSpace then
    self.dataSpace:destroyChildren()
    self.dataSpace = nil
  end
end

function UITable:onStyleApply(styleName, styleNode)
  for name, value in pairs(styleNode) do
    if name == 'table-data' then
      addEvent(function()
        self:setTableData(self:getParent():getChildById(value))
      end)
    elseif name == 'column-style' then
      addEvent(function()
        self:setColumnStyle(value)
      end)
    elseif name == 'row-style' then
      addEvent(function()
        self:setRowStyle(value)
      end)
    elseif name == 'header-column-style' then
      addEvent(function()
        self:setHeaderColumnStyle(value)
      end)
    elseif name == 'header-row-style' then
      addEvent(function()
        self:setHeaderRowStyle(value)
      end)
    end
  end
end

function UITable:hasHeader()
  return self.headerRow ~= nil
end

function UITable:clearData()
  if not self.dataSpace then
    return
  end
  self.dataSpace:destroyChildren()
  self.selectedRow = nil
  self.columns = {}
  self.rows = {}
end

function UITable:addHeaderRow(data)
  if not data or type(data) ~= 'table' then
    g_logger.error('UITable:addHeaderRow - table columns must be provided in a table')
    return
  end

  -- build header columns
  local columns = {}
  for _, column in pairs(data) do
    local col = g_ui.createWidget(self.headerColumnBaseStyle)
    for type, value in pairs(column) do
      if type == 'width' then
        col:setWidth(value)
      elseif type == 'height' then
        col:setHeight(value)
      elseif type == 'text' then
        col:setText(value)
      elseif type == 'onClick' then
        col.onClick = value
      end
    end
    table.insert(columns, col)
  end

  -- create a new header
  local headerRow = g_ui.createWidget(self.headerRowBaseStyle, self)
  local newHeight = (self.dataSpace:getHeight()-headerRow:getHeight())-self.dataSpace:getMarginTop()
  self.dataSpace:applyStyle({ height = newHeight })

  headerRow:setId(HEADER_ID)
  for _, column in pairs(columns) do
    headerRow:addChild(column)
    self.columns[HEADER_ID] = column
  end

  headerRow.onClick = function(headerRow) self:selectRow(headerRow) end
  self.headerRow = headerRow
  return headerRow
end

function UITable:removeHeaderRow()
  self.headerRow:destroy()
  self.headerRow = nil
end

function UITable:addRow(data, ref, height)
  if not self.dataSpace then
    g_logger.error('UITable:addRow - table data space has not been set, cannot add rows.')
    return
  end
  if not data or type(data) ~= 'table' then
    g_logger.error('UITable:addRow - table columns must be provided in a table.')
    return
  end

  local row = g_ui.createWidget(self.rowBaseStyle)
  if ref then row.ref = ref end
  if height then row:setHeight(height) end

  local rowId = #self.rows
  row:setId('row'..(rowId < 1 and 1 or rowId))

  for _, column in pairs(data) do
    local col = g_ui.createWidget(self.columBaseStyle, row)
    for type, value in pairs(column) do
      if type == 'width' then
        col:setWidth(value)
      elseif type == 'height' then
        col:setHeight(value)
      elseif type == 'text' then
        col:setText(value)
      end
    end
    self.columns[rowId] = col
  end

  row.onFocusChange = function(row, focused)
    if focused then self:selectRow(row) end
  end
  self.dataSpace:addChild(row)

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

function UITable:setTableData(tableData)
  self.dataSpace = tableData
  self.dataSpace:applyStyle({ height = self:getHeight() })
end

function UITable:setRowStyle(style)
  self.rowBaseStyle = style
  for _, row in pairs(self.rows) do
    row:setStyle(style)
  end
end

function UITable:setColumnStyle(style)
  self.columBaseStyle = style
  for _, col in pairs(self.columns) do
    col:setStyle(style)
  end
end

function UITable:setHeaderRowStyle(style)
  self.headerRowBaseStyle = style
  if self.headerRow then
    self.headerRow:setStyle(style)
  end
end

function UITable:setHeaderColumnStyle(style)
  self.headerColumnBaseStyle = style
  if table.haskey(HEADER_ID) then
    self.columns[HEADER_ID]:setStyle(style)
  end
end
