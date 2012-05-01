BugReport = {}

local bugReportWindow
local bugTextEdit
local HOTKEY = 'Ctrl+Z'

function BugReport.init()
  importStyle 'bugreport.otui'

  bugReportWindow = createWidget('BugReportWindow', rootWidget)
  bugReportWindow:hide()

  bugTextEdit = bugReportWindow:getChildById('bugTextEdit')

  Keyboard.bindKeyDown(HOTKEY, BugReport.show)
end

function BugReport.terminate()
  Keyboard.unbindKeyDown(HOTKEY)
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
