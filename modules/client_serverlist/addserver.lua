AddServer = {}

-- private variables
local addServerWindow = nil

-- public functions
function AddServer.init()
  addServerWindow = g_ui.displayUI('addserver')
end

function AddServer.terminate()
  addServerWindow:destroy()
end

function AddServer.add()
  local host = addServerWindow:getChildById('host'):getText()
  local port = addServerWindow:getChildById('port'):getText()
  local protocol = addServerWindow:getChildById('protocol'):getCurrentOption().text

  local added, error = ServerList.add(host, port, protocol)
  if not added then
    displayErrorBox(tr('Error'), tr(error))
  else
    AddServer.hide()
  end
end

function AddServer.show()
  addServerWindow:show()
  addServerWindow:raise()
  addServerWindow:focus()
  addServerWindow:lock()
end

function AddServer.hide()
  addServerWindow:hide()
  addServerWindow:unlock()
end