-- TODO: find another hotkey for this. Ctrl+Z will be reserved to undo on textedits.
HOTKEY = 'Ctrl+Z'

bugReportWindow = nil
bugTextEdit = nil

function init()
  g_ui.importStyle('bugreport')

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
  if g_game.isOnline() then
    bugTextEdit:setText('')
    bugReportWindow:show()
    bugReportWindow:raise()
    bugReportWindow:focus()
  end
end
