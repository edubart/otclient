HOTKEY = 'Ctrl+Z'

bugReportWindow = nil
bugTextEdit = nil

function init()
  g_ui.importStyle('bugreport.otui')

  bugReportWindow = g_ui.createWidget('BugReportWindow', rootWidget)
  bugReportWindow:hide()

  bugTextEdit = bugReportWindow:getChildById('bugTextEdit')

  g_keyboard.bindKeyDown(HOTKEY, show)
end

function terminate()
  g_keyboard.unbindKeyDown(HOTKEY)
  bugReportWindow:destroy()
end

function doReport()
  g_game.reportBug(bugTextEdit:getText())
  bugReportWindow:hide()
  modules.game_textmessage.displayGameMessage(tr('Bug report sent.'))
end

function show()
  bugTextEdit:setText('')
  bugReportWindow:show()
  bugReportWindow:raise()
  bugReportWindow:focus()
end
