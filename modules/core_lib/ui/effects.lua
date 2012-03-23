Effects = {}

function Effects.fadeIn(widget, time, elapsed)
  if not elapsed then elapsed = 0 end
  if not time then time = 250 end
  widget:setOpacity(math.min(elapsed/time, 1))
  if elapsed < time then
    removeEvent(widget.fadeEvent)
    widget.fadeEvent = scheduleEvent(function()
      Effects.fadeIn(widget, time, elapsed + 30)
    end, 30)
  else
    widget.fadeEvent = nil
  end
end

function Effects.fadeOut(widget, time, elapsed)
  if not elapsed then elapsed = 0 end
  if not time then time = 250 end
  widget:setOpacity(math.max((time - elapsed)/time, 0))
  if elapsed < time then
    removeEvent(widget.fadeEvent)
    widget.fadeEvent = scheduleEvent(function()
      Effects.fadeOut(widget, time, elapsed + 30)
    end, 30)
  else
    widget.fadeEvent = nil
    widget:destroy()
  end
end

function Effects.cancelFade(widget)
  removeEvent(widget.fadeEvent)
end
