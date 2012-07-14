QuestLog = {}

local questLogButton
local questLogWindow
local questLineWindow

local function onGameQuestLog(quests)
  QuestLog.destroyWindows()

  questLogWindow = g_ui.createWidget('QuestLogWindow', rootWidget)
  local questList = questLogWindow:getChildById('questList')

  for i,questEntry in pairs(quests) do
    local id, name, completed = unpack(questEntry)

    local questLabel = g_ui.createWidget('QuestLabel', questList)
    questLabel:setOn(completed)
    questLabel:setText(name)
    questLabel.onDoubleClick = function()
      questLogWindow:hide()
      g_game.requestQuestLine(id)
    end
  end

  questLogWindow.onDestroy = function()
    questLogWindow = nil
  end
end

local function onGameQuestLine(questId, questMissions)
  if questLogWindow then questLogWindow:hide() end
  if questLineWindow then questLineWindow:destroy() end

  questLineWindow = g_ui.createWidget('QuestLineWindow', rootWidget)
  local missionList = questLineWindow:getChildById('missionList')
  local missionDescription = questLineWindow:getChildById('missionDescription')

  connect(missionList, { onChildFocusChange = function(self, focusedChild)
    if focusedChild == nil then return end
    missionDescription:setText(focusedChild.description)
  end })

  for i,questMission in pairs(questMissions) do
    local name, description = unpack(questMission)

    local missionLabel = g_ui.createWidget('MissionLabel', missionList)
    missionLabel:setText(name)
    missionLabel.description = description
  end

  questLineWindow.onDestroy = function()
    if questLogWindow then questLogWindow:show() end
    questLineWindow = nil
  end
end

function QuestLog.init()
  g_ui.importStyle('questlogwindow.otui')
  g_ui.importStyle('questlinewindow.otui')

  questLogButton = TopMenu.addLeftGameButton('questLogButton', tr('Quest Log'), 'questlog.png', function() g_game.requestQuestLog() end)

  connect(g_game, { onQuestLog = onGameQuestLog,
                    onQuestLine = onGameQuestLine,
                    onGameEnd = QuestLog.destroyWindows})
end

function QuestLog.destroyWindows()
  if questLogWindow then
    questLogWindow:destroy()
    questLogWindow = nil
  end

  if questLineWindow then
    questLineWindow:destroy()
    questLineWindow = nil
  end
end

function QuestLog.terminate()
  disconnect(g_game, { onQuestLog = onGameQuestLog,
                       onQuestLine = onGameQuestLine,
                       onGameEnd = QuestLog.destroyWindows})

  QuestLog.destroyWindows()

  questLogButton:destroy()
  questLogButton = nil
end
