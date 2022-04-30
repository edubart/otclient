local ADDON_SETS = {
    [1] = {1},
    [2] = {2},
    [3] = {1, 2},
    [4] = {3},
    [5] = {1, 3},
    [6] = {2, 3},
    [7] = {1, 2, 3}
}

local addons = nil
local outfit = nil
local outfits = nil
local outfitWindow = nil
local outfitCreature = nil
local mountCreature = nil
local mounts = nil
local currentColorBox = nil
local currentClotheButtonBox = nil

local currentOutfit = 1
local currentMount = 1

local colorBoxes = {}

localPlayerEvent = EventController:new(LocalPlayer, {
    onOutfitChange = function(creature)
        creature = creature or g_game.getLocalPlayer()
        local selectedOutfit = outfits[currentOutfit]

        local selectedAddons
        local availableAddons

        outfit = creature:getOutfit()
        selectedAddons = outfit.addons
        availableAddons = selectedOutfit[3]

        if table.empty(outfits) or not outfit then return end

        local nameWidget = outfitWindow:getChildById('outfitName')
        nameWidget:setText(selectedOutfit[2])

        for k, addon in pairs(addons) do
            addon.widget:setChecked(false)
            addon.widget:setEnabled(false)
        end

        outfit.addons = 0

        for k, addon in pairs(addons) do
            local isEnabled = availableAddons == 3 or addon.value == availableAddons
            addon.widget:setEnabled(isEnabled)
            addon.widget:setChecked(isEnabled and (selectedAddons == 3 or addon.value == selectedAddons))
        end

        outfit.type = selectedOutfit[1]
        outfitCreature:setOutfit(outfit)

        if table.empty(mounts) or not mount then return end

        local nameMountWidget = outfitWindow:getChildById('mountName')
        nameMountWidget:setText(mounts[currentMount][2])

        mount.type = mounts[currentMount][1]
        mountCreature:setOutfit(mount)
    end
})

controller = Controller:new()

controller:onGameEnd(function() destroy() end)

controller:gameEvent('onOpenOutfitWindow',
                     function(creatureOutfit, outfitList, creatureMount, mountList, creatureFamiliar, familiarList)

    if outfitWindow and not outfitWindow:isHidden() then return end

    localPlayerEvent:connect()

    outfitCreature = creatureOutfit
    mountCreature = creatureMount
    outfits = outfitList
    mounts = mountList
    destroy()

    outfitWindow = g_ui.displayUI('outfitwindow')

    local outfitCreatureBox = outfitWindow:getChildById('outfitCreatureBox')

    if outfitCreature then
        outfit = outfitCreature:getOutfit()
        outfitCreatureBox:setCreature(outfitCreature)
    else
        outfitCreatureBox:hide()
        outfitWindow:getChildById('outfitName'):hide()
        outfitWindow:getChildById('outfitNextButton'):hide()
        outfitWindow:getChildById('outfitPrevButton'):hide()
    end

    local mountCreatureBox = outfitWindow:getChildById('mountCreatureBox')
    if mountCreature then
        mount = mountCreature:getOutfit()
        mountCreatureBox:setCreature(mountCreature)
    else
        mountCreatureBox:hide()
        outfitWindow:getChildById('mountName'):hide()
        outfitWindow:getChildById('mountNextButton'):hide()
        outfitWindow:getChildById('mountPrevButton'):hide()
    end

    -- set addons
    addons = {
        [1] = {
            widget = outfitWindow:getChildById('addon1'),
            value = 1
        },
        [2] = {
            widget = outfitWindow:getChildById('addon2'),
            value = 2
        }
    }

    for _, addon in pairs(addons) do
        addon.widget.onCheckChange = function(self) onAddonCheckChange(self, addon.value) end
    end

    -- hook outfit sections
    currentClotheButtonBox = outfitWindow:getChildById('head')
    currentClotheButtonBox.onCheckChange = onClotheCheckChange
    outfitWindow:getChildById('primary').onCheckChange = onClotheCheckChange
    outfitWindow:getChildById('secondary').onCheckChange = onClotheCheckChange
    outfitWindow:getChildById('detail').onCheckChange = onClotheCheckChange

    -- populate color panel
    local colorBoxPanel = outfitWindow:getChildById('colorBoxPanel')
    for j = 0, 6 do
        for i = 0, 18 do
            local colorId = j * 19 + i

            local colorBox = g_ui.createWidget('ColorBox', colorBoxPanel)
            colorBox:setImageColor(getOutfitColor(colorId))
            colorBox:setId('colorBox' .. colorId)
            colorBox.colorId = colorId

            if colorId == outfit.head then
                currentColorBox = colorBox
                colorBox:setChecked(true)
            end

            colorBox.onCheckChange = onColorCheckChange
            colorBoxes[#colorBoxes + 1] = colorBox
        end
    end

    currentOutfit = 1
    currentMount = 1

    if outfit then
        for i = 1, #outfitList do
            if outfitList[i][1] == outfit.type then
                currentOutfit = i
                break
            end
        end

        for i = 1, #mountList do
            if mountList[i][1] == mount.type then
                currentMount = i
                break
            end
        end
    end

    localPlayerEvent:execute('onOutfitChange')
end)

function destroy()
    if not outfitWindow then return end

    outfitWindow:destroy()

    outfitWindow = nil
    outfitCreature = nil
    currentColorBox = nil
    currentClotheButtonBox = nil

    colorBoxes = {}
    addons = {}

    localPlayerEvent:disconnect()
end

function randomize()
    local outfitTemplate = {outfitWindow:getChildById('detail'), outfitWindow:getChildById('secondary'),
                            outfitWindow:getChildById('primary'), outfitWindow:getChildById('head')}

    for i, template in pairs(outfitTemplate) do
        template:setChecked(true)
        colorBoxes[math.random(1, #colorBoxes)]:setChecked(true)
        template:setChecked(false)
    end
end

function accept()
    if mount then outfit.mount = mount.type end
    g_game.changeOutfit(outfit)
    destroy()
end

function nextOutfitType()
    if not outfits then return end

    currentOutfit = currentOutfit + 1
    if currentOutfit > #outfits then currentOutfit = 1 end

    localPlayerEvent:execute('onOutfitChange')
end

function previousOutfitType()
    if not outfits then return end

    currentOutfit = currentOutfit - 1
    if currentOutfit <= 0 then currentOutfit = #outfits end

    localPlayerEvent:execute('onOutfitChange')
end

function nextMountType()
    if not mounts then return end

    currentMount = currentMount + 1
    if currentMount > #mounts then currentMount = 1 end
    localPlayerEvent:execute('onOutfitChange')
end

function previousMountType()
    if not mounts then return end

    currentMount = currentMount - 1
    if currentMount <= 0 then currentMount = #mounts end

    print(#mounts)

    localPlayerEvent:execute('onOutfitChange')
end

function onAddonCheckChange(addon, value)
    if addon:isChecked() then
        outfit.addons = outfit.addons + value
    else
        outfit.addons = outfit.addons - value
    end

    outfitCreature:setOutfit(outfit)
end

function onColorCheckChange(colorBox)
    if colorBox == currentColorBox then
        colorBox.onCheckChange = nil
        colorBox:setChecked(true)
        colorBox.onCheckChange = onColorCheckChange
    else
        currentColorBox.onCheckChange = nil
        currentColorBox:setChecked(false)
        currentColorBox.onCheckChange = onColorCheckChange

        currentColorBox = colorBox

        if currentClotheButtonBox:getId() == 'head' then
            outfit.head = currentColorBox.colorId
        elseif currentClotheButtonBox:getId() == 'primary' then
            outfit.body = currentColorBox.colorId
        elseif currentClotheButtonBox:getId() == 'secondary' then
            outfit.legs = currentColorBox.colorId
        elseif currentClotheButtonBox:getId() == 'detail' then
            outfit.feet = currentColorBox.colorId
        end

        outfitCreature:setOutfit(outfit)
    end
end

function onClotheCheckChange(clotheButtonBox)
    if clotheButtonBox == currentClotheButtonBox then
        clotheButtonBox.onCheckChange = nil
        clotheButtonBox:setChecked(true)
        clotheButtonBox.onCheckChange = onClotheCheckChange
    else
        currentClotheButtonBox.onCheckChange = nil
        currentClotheButtonBox:setChecked(false)
        currentClotheButtonBox.onCheckChange = onClotheCheckChange

        currentClotheButtonBox = clotheButtonBox

        local colorId = 0
        if currentClotheButtonBox:getId() == 'head' then
            colorId = outfit.head
        elseif currentClotheButtonBox:getId() == 'primary' then
            colorId = outfit.body
        elseif currentClotheButtonBox:getId() == 'secondary' then
            colorId = outfit.legs
        elseif currentClotheButtonBox:getId() == 'detail' then
            colorId = outfit.feet
        end
        outfitWindow:recursiveGetChildById('colorBox' .. colorId):setChecked(true)
    end
end
