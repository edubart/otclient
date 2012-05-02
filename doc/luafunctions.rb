# @group Global functions

# Converts rect to string
# @return [Rect]
def torect(str); end
# Converts point to string
# @return [Point]
def topoint(str); end
# Converts color to string
# @return [Color]
def tocolor(str); end
# @return [Size]
def tosize(str); end
# @return [String]
def recttostring(rect); end
# @return [String]
def pointtostring(point); end
# @return [String]
def colortostring(color); end
# @return [String]
def sizetostring(size); end

# @endgroup


# All classes used by the engine is derived from this class
class LuaObject
  # Return the number of references that this object have
  # @return [Integer]
  def getUseCount(); end
  # Get the derived class name
  # @return [String]
  def getClassName(); end
  # Return that hold all fields for this object
  # @return [Table]
  def getFieldsTable(); end
end

# Class that holds dispatcher events
class Event < LuaObject
  # Cancel the event execution
  # @return [Void]
  def cancel(); end
  # Force event execution, as an event can only be executed once,
  # it will not be executed again in the next poll
  # @return [Void]
  def execute(); end
  # Checks if the event is canceled
  # @return [Boolean]
  def isCanceled(); end
  # Check if the event is executed
  # @return [Boolean]
  def isExecuted(); end
end

# Class that holds dispatcher scheduled events
class ScheduledEvent < Event
  # Return ticks in milliseconds remaining to the event execution
  # @return [Integer]
  def reamaningTicks(); end
  # Return ticks when this event will be executed
  # @return [Integer]
  def ticks(); end
end

# This is the base class for all UI elements
class UIWidget < LuaObject
  # Creates a new UIWidget
  # @return [UIWidget]
  def self.create(); end

  def addChild(); end
  def insertChild(); end
  def removeChild(); end
  def focusChild(); end
  def focusNextChild(); end
  def focusPreviousChild(); end
  def lowerChild(); end
  def raiseChild(); end
  def moveChildToIndex(); end
  def lockChild(); end
  def unlockChild(); end
  def mergeStyle(); end
  def applyStyle(); end
  def addAnchor(); end
  def removeAnchor(); end
  def fill(); end
  def centerIn(); end
  def breakAnchors(); end
  def updateParentLayout(); end
  def updateLayout(); end
  def lock(); end
  def unlock(); end
  def focus(); end
  def lower(); end
  def raise(); end
  def grabMouse(); end
  def ungrabMouse(); end
  def grabKeyboard(); end
  def ungrabKeyboard(); end
  def bindRectToParent(); end
  def destroy(); end
  def destroyChildren(); end
  def setId(); end
  def setParent(); end
  def setLayout(); end
  def setRect(); end
  def setStyle(); end
  def setStyleFromNode(); end
  def setEnabled(); end
  def setVisible(); end
  def setOn(); end
  def setChecked(); end
  def setFocusable(); end
  def setPhantom(); end
  def setDragable(); end
  def setFixedSize(); end
  def setClipping(); end
  def setLastFocusReason(); end
  def setAutoRepeatDelay(); end
  def setVirtualOffset(); end
  def isVisible(); end
  def isChildLocked(); end
  def hasChild(); end
  def getChildIndex(); end
  def getMarginRect(); end
  def getClippingRect(); end
  def getChildrenRect(); end
  def getAnchoredLayout(); end
  def getRootParent(); end
  def getChildAfter(); end
  def getChildBefore(); end
  def getChildById(); end
  def getChildByPos(); end
  def getChildByIndex(); end
  def recursiveGetChildById(); end
  def recursiveGetChildByPos(); end
  def recursiveGetChildrenByPos(); end
  def backwardsGetWidgetById(); end
  def asUIWidget(); end
  def resize(); end
  def move(); end
  def hide(); end
  def show(); end
  def disable(); end
  def enable(); end
  def isActive(); end
  def isEnabled(); end
  def isDisabled(); end
  def isFocused(); end
  def isHovered(); end
  def isPressed(); end
  def isFirst(); end
  def isMiddle(); end
  def isLast(); end
  def isAlternate(); end
  def isChecked(); end
  def isOn(); end
  def isDragging(); end
  def isHidden(); end
  def isExplicitlyEnabled(); end
  def isExplicitlyVisible(); end
  def isFocusable(); end
  def isPhantom(); end
  def isDragable(); end
  def isFixedSize(); end
  def isClipping(); end
  def isDestroyed(); end
  def hasChildren(); end
  def containsPoint(); end
  def getId(); end
  def getParent(); end
  def getFocusedChild(); end
  def getChildren(); end
  def getFirstChild(); end
  def getLastChild(); end
  def getLayout(); end
  def getStyle(); end
  def getChildCount(); end
  def getLastFocusReason(); end
  def getAutoRepeatDelay(); end
  def getVirtualOffset(); end
  def getStyleName(); end
  def getLastClickPosition(); end
  def setX(); end
  def setY(); end
  def setWidth(); end
  def setHeight(); end
  def setSize(); end
  def setPosition(); end
  def setColor(); end
  def setBackgroundColor(); end
  def setBackgroundOffsetX(); end
  def setBackgroundOffsetY(); end
  def setBackgroundOffset(); end
  def setBackgroundWidth(); end
  def setBackgroundHeight(); end
  def setBackgroundSize(); end
  def setBackgroundRect(); end
  def setIcon(); end
  def setIconColor(); end
  def setIconOffsetX(); end
  def setIconOffsetY(); end
  def setIconOffset(); end
  def setIconWidth(); end
  def setIconHeight(); end
  def setIconSize(); end
  def setIconRect(); end
  def setBorderWidth(); end
  def setBorderWidthTop(); end
  def setBorderWidthRight(); end
  def setBorderWidthBottom(); end
  def setBorderWidthLeft(); end
  def setBorderColor(); end
  def setBorderColorTop(); end
  def setBorderColorRight(); end
  def setBorderColorBottom(); end
  def setBorderColorLeft(); end
  def setMargin(); end
  def setMarginHorizontal(); end
  def setMarginVertical(); end
  def setMarginTop(); end
  def setMarginRight(); end
  def setMarginBottom(); end
  def setMarginLeft(); end
  def setPadding(); end
  def setPaddingHorizontal(); end
  def setPaddingVertical(); end
  def setPaddingTop(); end
  def setPaddingRight(); end
  def setPaddingBottom(); end
  def setPaddingLeft(); end
  def setOpacity(); end
  def getX(); end
  def getY(); end
  def getPosition(); end
  def getWidth(); end
  def getHeight(); end
  def getSize(); end
  def getRect(); end
  def getColor(); end
  def getBackgroundColor(); end
  def getBackgroundOffsetX(); end
  def getBackgroundOffsetY(); end
  def getBackgroundOffset(); end
  def getBackgroundWidth(); end
  def getBackgroundHeight(); end
  def getBackgroundSize(); end
  def getBackgroundRect(); end
  def getIconColor(); end
  def getIconOffsetX(); end
  def getIconOffsetY(); end
  def getIconOffset(); end
  def getIconWidth(); end
  def getIconHeight(); end
  def getIconSize(); end
  def getIconRect(); end
  def getBorderTopColor(); end
  def getBorderRightColor(); end
  def getBorderBottomColor(); end
  def getBorderLeftColor(); end
  def getBorderTopWidth(); end
  def getBorderRightWidth(); end
  def getBorderBottomWidth(); end
  def getBorderLeftWidth(); end
  def getMarginTop(); end
  def getMarginRight(); end
  def getMarginBottom(); end
  def getMarginLeft(); end
  def getPaddingTop(); end
  def getPaddingRight(); end
  def getPaddingBottom(); end
  def getPaddingLeft(); end
  def getOpacity(); end
  def setImageSource(); end
  def setImageClip(); end
  def setImageOffsetX(); end
  def setImageOffsetY(); end
  def setImageOffset(); end
  def setImageWidth(); end
  def setImageHeight(); end
  def setImageSize(); end
  def setImageRect(); end
  def setImageColor(); end
  def setImageFixedRatio(); end
  def setImageRepeated(); end
  def setImageSmooth(); end
  def setImageBorderTop(); end
  def setImageBorderRight(); end
  def setImageBorderBottom(); end
  def setImageBorderLeft(); end
  def setImageBorder(); end
  def getImageClip(); end
  def getImageOffsetX(); end
  def getImageOffsetY(); end
  def getImageOffset(); end
  def getImageWidth(); end
  def getImageHeight(); end
  def getImageSize(); end
  def getImageRect(); end
  def getImageColor(); end
  def isImageFixedRatio(); end
  def isImageSmooth(); end
  def getImageBorderTop(); end
  def getImageBorderRight(); end
  def getImageBorderBottom(); end
  def getImageBorderLeft(); end
  def resizeToText(); end
  def clearText(); end
  def setText(); end
  def setTextAlign(); end
  def setTextOffset(); end
  def setTextWrap(); end
  def setTextAutoResize(); end
  def setFont(); end
  def getText(); end
  def getDrawText(); end
  def getTextAlign(); end
  def getTextOffset(); end
  def getTextWrap(); end
  def getFont(); end
  def getTextSize(); end

  # @group Class Events

  # Triggered when a style is applied to the widget
  # @param styleName Name of the style
  # @param styleNode Table containing all style tags and values
  # @return [Void]
  def onStyleApply(styleName, styleNode); end
  # Triggered when widget size or position has changed
  # @param oldRect Table containing the old geometry rect
  # @param newRect Table containing the new geometry rect
  # @return [Void]
  def onGeometryChange(oldRect, newRect); end
  # Triggered when widget's layout updates its contents
  # @return [Void]
  def onLayoutUpdate(); end
  # Triggered when widget focus state changes
  # @param focused Boolean representing the focus state
  # @param reason One of the constants: *ActiveFocusReason* *MouseFocusReason* *KeyboardFocusReason*
  # @return [Void]
  def onFocusChange(focused, reason); end
  # Triggered when child focus changes
  # @return [Void]
  def onChildFocusChange(focusedChild, unfocusedChild, reason); end
  # Triggered when mouse enter or leaves widget area
  # @return [Void]
  def onHoverChange(hovered); end
  # Triggered when this widget dragging begin
  # @return [Boolean]
  def onDragEnter(mousePos); end
  # Triggered when this widget dragging end
  # @return [Boolean]
  def onDragLeave(droppedWidget, mousePos); end
  # Triggered when moving mouse while in dragging state
  # @return [Boolean]
  def onDragMove(mousePos, mouseMoved); end
  # Triggered when leaving other widget dragging state while the mouse is over this widget
  # @return [Boolean]
  def onDrop(draggedWidget, mousePos); end
  # @return [Boolean]
  def onKeyText(keyText); end
  # @return [Boolean]
  def onKeyDown(keyCode, keyboardModifiers); end
  # @return [Boolean]
  def onKeyPress(keyCode, keyboardModifiers, autoRepeatTicks); end
  # @return [Boolean]
  def onKeyUp(keyCode, keyboardModifiers); end
  # @return [Boolean]
  def onMousePress(mousePos, utton); end
  # @return [Boolean]
  def onMouseRelease(mousePos, button); end
  # @return [Boolean]
  def onMouseMove(mousePos, mouseMoved); end
  # @return [Boolean]
  def onMouseWheel(mousePos, direction); end
  # Triggered when clicking and releasing the mouse left button inside widget area
  # @return [Boolean]
  def onClick(mousePos); end
  # Triggered when pressing mouse left button two times inside widget area
  # @return [Boolean]
  def onDoubleClick(mousePos); end

  # @endgroup
end

class g_game
  def self.login(); end
  def self.logout(); end
  def self.loginWorld(); end
  def self.cancelLogin(); end
  def self.forceLogout(); end
  def self.safeLogout(); end
  def self.walk(); end
  def self.autoWalk(); end
  def self.forceWalk(); end
  def self.turn(); end
  def self.stop(); end
  def self.look(); end
  def self.move(); end
  def self.moveToParentContainer(); end
  def self.rotate(); end
  def self.use(); end
  def self.useWith(); end
  def self.useInventoryItem(); end
  def self.useInventoryItemWith(); end
  def self.open(); end
  def self.openParent(); end
  def self.close(); end
  def self.refreshContainer(); end
  def self.attack(); end
  def self.cancelAttack(); end
  def self.follow(); end
  def self.cancelFollow(); end
  def self.cancelAttackAndFollow(); end
  def self.talk(); end
  def self.talkChannel(); end
  def self.talkPrivate(); end
  def self.openPrivateChannel(); end
  def self.requestChannels(); end
  def self.joinChannel(); end
  def self.leaveChannel(); end
  def self.closeNpcChannel(); end
  def self.openOwnChannel(); end
  def self.inviteToOwnChannel(); end
  def self.excludeFromOwnChannel(); end
  def self.partyInvite(); end
  def self.partyJoin(); end
  def self.partyRevokeInvitation(); end
  def self.partyPassLeadership(); end
  def self.partyLeave(); end
  def self.partyShareExperience(); end
  def self.requestOutfit(); end
  def self.changeOutfit(); end
  def self.addVip(); end
  def self.removeVip(); end
  def self.setChaseMode(); end
  def self.setFightMode(); end
  def self.setSafeFight(); end
  def self.getChaseMode(); end
  def self.getFightMode(); end
  def self.isSafeFight(); end
  def self.inspectNpcTrade(); end
  def self.buyItem(); end
  def self.sellItem(); end
  def self.closeNpcTrade(); end
  def self.requestTrade(); end
  def self.inspectTrade(); end
  def self.acceptTrade(); end
  def self.rejectTrade(); end
  def self.reportBug(); end
  def self.reportRuleVilation(); end
  def self.debugReport(); end
  def self.editText(); end
  def self.editList(); end
  def self.requestQuestLog(); end
  def self.requestQuestLine(); end
  def self.canPerformGameAction(); end
  def self.canReportBugs(); end
  def self.checkBotProtection(); end
  def self.isOnline(); end
  def self.isDead(); end
  def self.isAttacking(); end
  def self.isFollowing(); end
  def self.getContainer(); end
  def self.getContainers(); end
  def self.getVips(); end
  def self.getAttackingCreature(); end
  def self.getFollowingCreature(); end
  def self.getServerBeat(); end
  def self.getLocalPlayer(); end
  def self.getProtocolGame(); end
  def self.getProtocolVersion(); end
  def self.getWorldName(); end
  def self.getGMActions(); end
end
