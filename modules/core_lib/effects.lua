Effects = {}

function Effects.fadeIn(widget, time, elapsed)
    if not elapsed then elapsed = 0 end
    if not time then time = 250 end
    widget:setOpacity(math.min(elapsed/time, 1))
    if elapsed < time then
        scheduleEvent(function()
                        Effects.fadeIn(widget, time, elapsed + 30)
                      end, 30)
    end
end

function Effects.fadeOut(widget, time, elapsed)
    if not elapsed then elapsed = 0 end
    if not time then time = 250 end
    widget:setOpacity((time - elapsed)/time)
    if elapsed < time then
        scheduleEvent(function()
                        Effects.fadeOut(widget, time, elapsed + 30)
                      end, 30)
    else
        widget:destroy()
    end
end

