BugReport = {}

local bugReportWindow
local bugTextEdit
local HOTKEY = 'Ctrl+Z'

function BugReport.init()
  g_ui.importStyle('bugreport.otui')

  bugReportWindow = g_ui.createWidget('BugReportWindow', rootWidget)
  bugReportWindow:hide()

  bugTextEdit = bugReportWindow:getChildById('bugTextEdit')

  g_keyboard.bindKeyDown(HOTKEY, BugReport.show)
end

function BugReport.terminate()
  g_keyboard.unbindKeyDown(HOTKEY)
  bugReportWindow:destroy()
  bugReportWindow = nil
  bugTextEdit = nil
end

function BugReport.doReport()
  g_game.reportBug(bugTextEdit:getText())
  bugReportWindow:hide()
  TextMessage.displayEventAdvance(tr('Bug report sent.'))
end

function BugReport.show()
  bugTextEdit:setText('')
  bugReportWindow:show()
  bugReportWindow:raise()
  bugReportWindow:focus()
end
