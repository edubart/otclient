QuestLog = {}

-- g_game.requestQuestLog()
-- g_game.requestQuestLine(questId)

local function onGameQuestLog(questList)
  for i,questEntry in pairs(questList) do
    local id, name, done = unpack(questEntry)
    print(id, name, done)
  end
end

local function onGameQuestLine(questId, questMissions)
  for i,questMission in pairs(questMissions) do
    local name, description = unpack(questMission)
    print(name, description)
  end
end

function QuestLog.init()
  importStyle 'questlogwindow.otui'

  connect(g_game, { onQuestLog = onGameQuestLog })
  connect(g_game, { onQuestLine= onGameQuestLine })
end

function QuestLog.terminate()
  disconnect(g_game, { onQuestLog = onGameQuestLog })
  disconnect(g_game, { onQuestLine= onGameQuestLine })
end
