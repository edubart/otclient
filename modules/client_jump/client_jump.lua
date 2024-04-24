local clientJumpWindow
local clientJumpButton
local clientJumpLabel

--alternative trigger for our bouncy button repositions
function onClickEvent(mousePos)
    clientJumpButton:setY(math.random()*700)
    clientJumpButton:setX(400)
end

function onClientUIMouseMove(mouseBindWidget, mousePos, mouseMove)

    --makes my life easier so I don't have to count pixels with the UI
    clientJumpButton:breakAnchors();

    --not quite sure if I should do a process like a coroutine here, 
    --(That's definitely where i initially want to go for something like this) but the implementation here is simple enough
    --I wanted to replicate the "skippiness" of the button, so i settled for an arbitrary delay.
    local percentage = clientJumpButton:getX()-0.05
    clientJumpButton:setX(percentage)

    --checks if button has hit the edge of screen then repositions within a random area within the bounds of our window (300 px)
    if(clientJumpButton:getX()<=5) then
        clientJumpButton:setY(math.random()*600)
        clientJumpButton:setX(400)
    end
end


function init()

    --I'm aware of getChildRecursive, but i've been fiddling with the UI enough to prefer
    --not using it
    clientJumpWindow = g_ui.displayUI('client_jump')
    clientJumpButton = clientJumpWindow:getChildById('clientJumpButton')   
    clientJumpLabel = clientJumpWindow:getChildById('clientJumpLabel')

    clientJumpLabel:setText("Jump!!")
    clientJumpWindow:raise()
    clientJumpButton:raise()
    clientJumpLabel:raise()

    --I prefer purely event based systems when it comes to user input, so i've subscribed/unsubscribed to any relevent events.
    connect(rootWidget, {onClick = onClickEvent, onMouseMove = onClientUIMouseMove, })
    connect(g_game, { onGameStart = hide })
    connect(g_game, { onGameEnd = show })


end

function terminate()

    disconnect(rootWidget, {onClick = onClickEvent, onMouseMove = onClientUIMouseMove,})
    disconnect(g_game, { onGameStart = hide })
    disconnect(g_game, { onGameEnd = show })
    
    --Keepin it clean :) not sure if the framework accounts for children of children as well though
    clientJumpWindow:destroy()
end
--copying some functionality from other modules with the hide/show functions.
function hide()
    clientJumpWindow:hide()
    clientJumpButton:hide()
    clientJumpLabel:hide()
  end
  
  function show()
    clientJumpWindow:show()
    clientJumpButton:show()
    clientJumpLabel:show()
  end