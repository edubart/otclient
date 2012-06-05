RuleViolation = {}

-- private variables
local rvreasons = {}
rvreasons[1] = tr("1a) Offensive Name")
rvreasons[2] = tr("1b) Invalid Name Format")
rvreasons[3] = tr("1c) Unsuitable Name")
rvreasons[4] = tr("1d) Name Inciting Rule Violation")
rvreasons[5] = tr("2a) Offensive Statement")
rvreasons[6] = tr("2b) Spamming")
rvreasons[7] = tr("2c) Illegal Advertising")
rvreasons[8] = tr("2d) Off-Topic Public Statement")
rvreasons[9] = tr("2e) Non-English Public Statement")
rvreasons[10] = tr("2f) Inciting Rule Violation")
rvreasons[11] = tr("3a) Bug Abuse")
rvreasons[12] = tr("3b) Game Weakness Abuse")
rvreasons[13] = tr("3c) Using Unofficial Software to Play")
rvreasons[14] = tr("3d) Hacking")
rvreasons[15] = tr("3e) Multi-Clienting")
rvreasons[16] = tr("3f) Account Trading or Sharing")
rvreasons[17] = tr("4a) Threatening Gamemaster")
rvreasons[18] = tr("4b) Pretending to Have Influence on Rule Enforcement")
rvreasons[19] = tr("4c) False Report to Gamemaster")
rvreasons[20] = tr("Destructive Behaviour")
rvreasons[21] = tr("Excessive Unjustified Player Killing")

local rvactions = {}
rvactions[0] = tr("Notation")
rvactions[1] = tr("Name Report")
rvactions[2] = tr("Banishment")
rvactions[3] = tr("Name Report + Banishment")
rvactions[4] = tr("Banishment + Final Warning")
rvactions[5] = tr("Name Report + Banishment + Final Warning")
rvactions[6] = tr("Statement Report")

local ruleViolationWindow
local reasonsTextList
local actionsTextList

-- public functions
function RuleViolation.hasWindowAccess()
  return reasonsTextList:getChildCount() > 0
end

function RuleViolation.loadReasons()
  reasonsTextList:destroyChildren()

  local actions = g_game.getGMActions()
  for reason, actionFlags in pairs(actions) do
    local label = createWidget('RVListLabel', reasonsTextList)
    print("LOAD REASON: " .. tostring(reason) .. " " .. tostring(actionFlags))
    label:setText(rvreasons[reason])
    label.reasonId = reason
    label.actionFlags = actionFlags
  end
  
  if not RuleViolation.hasWindowAccess() and ruleViolationWindow:isVisible() then RuleViolation.hide() end
end

function RuleViolation.init()  
  connect(g_game, { onGMActions = RuleViolation.loadReasons })

  ruleViolationWindow = displayUI('ruleviolation.otui')
  ruleViolationWindow:setVisible(false)
  
  reasonsTextList = ruleViolationWindow:getChildById('reasonList')
  actionsTextList = ruleViolationWindow:getChildById('actionList')
  
  Keyboard.bindKeyDown('Ctrl+Y', RuleViolation.show)
  
  if g_game.isOnline() then
    RuleViolation.loadReasons()
  end
end

function RuleViolation.terminate()
  disconnect(g_game, { onGMActions = RuleViolation.loadReasons })

  ruleViolationWindow:destroy()
  ruleViolationWindow = nil
  
  reasonsTextList = nil
  actionsTextList = nil
end

function RuleViolation.show(target, statement)
  if g_game.isOnline() and RuleViolation.hasWindowAccess() then
    if target then
      ruleViolationWindow:getChildById('nameText'):setText(target)
    end
    
    if statement then
      ruleViolationWindow:getChildById('statementText'):setText(statement)
    end
  
    ruleViolationWindow:show()
    ruleViolationWindow:raise()
  end
end

function RuleViolation.hide()
  ruleViolationWindow:hide()
  RuleViolation.clearForm()
end

function RuleViolation.onSelectReason(reasonLabel, focused)
  if reasonLabel.actionFlags and focused then
    actionsTextList:destroyChildren()
    for actionBaseFlag = 0, #rvactions do
      actionFlagString = rvactions[actionBaseFlag]
      if bit32.band(reasonLabel.actionFlags, math.pow(2, actionBaseFlag)) > 0 then
        local label = createWidget('RVListLabel', actionsTextList)
        label:setText(actionFlagString)
        label.actionId = actionBaseFlag
      end
    end
  end
end

function RuleViolation.report()
  local target = ruleViolationWindow:getChildById('nameText'):getText()
  local reason = reasonsTextList:getFocusedChild().reasonId
  local action = actionsTextList:getFocusedChild().actionId
  local comment = ruleViolationWindow:getChildById('commentText'):getText()
  local statement = ruleViolationWindow:getChildById('statementText'):getText()
  local statementId = 0 -- TODO: message unique id ?
  local ipBanishment = ruleViolationWindow:getChildById('ipBanCheckBox'):isChecked()
  if action == 6 and statement == "" then
    displayErrorBox(tr("Error"), tr("No statement has been selected."))
  elseif comment == "" then
    displayErrorBox(tr("Error"), tr("You must enter a comment."))
  else
    g_game.reportRuleVilation(target, reason, action, comment, statement, statementId, ipBanishment)
    RuleViolation.hide()
  end
end

function RuleViolation.clearForm()
  ruleViolationWindow:getChildById('nameText'):clearText()
  ruleViolationWindow:getChildById('commentText'):clearText()
  ruleViolationWindow:getChildById('statementText'):clearText()
  ruleViolationWindow:getChildById('ipBanCheckBox'):setChecked(false)
end