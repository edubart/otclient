-- @docclass
UIActionSlot = extends(UIItem, 'UIActionSlot')

function UIActionSlot.create()
    local slot = UIActionSlot.internalCreate()
    slot.itemId = nil
    slot.words = nil
    slot.text = nil
    slot.hotkey = nil
    slot.useType = nil
    slot.autoSend = nil
    slot.parameter = nil
    return slot
end
