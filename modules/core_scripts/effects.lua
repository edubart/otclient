Effects = {}

function Effects.fadeIn(widget, time, elapsed)
    if not elapsed then elapsed = 0 end
    if not time then time = 250 end
    widget:setOpacity(math.min((255*elapsed)/time, 255))
    if elapsed < time then
        scheduleEvent(function()
                        Effects.fadeIn(widget, time, elapsed + 30)
                      end, 30)
    end
end

function Effects.fadeOut(widget, time, elapsed)
    if not elapsed then elapsed = 0 end
    if not time then time = 250 end
    widget:setOpacity((255*(time - elapsed))/time)
    if elapsed < time then
        scheduleEvent(function()
                        Effects.fadeOut(widget, time, elapsed + 30)
                      end, 30)
    else
        widget:destroy()
    end
end

