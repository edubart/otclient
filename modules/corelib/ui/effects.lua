Effects = {}

function Effects.fadeIn(widget, time, elapsed)
  if not elapsed then elapsed = 0 end
  if not time then time = 300 end
  widget:setOpacity(math.min(elapsed/time, 1))
  removeEvent(widget.fadeEvent)
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
  if not time then time = 300 end
  elapsed = math.max((1 - widget:getOpacity()) * time, elapsed)
  removeEvent(widget.fadeEvent)
  widget:setOpacity(math.max((time - elapsed)/time, 0))
  if elapsed < time then
    widget.fadeEvent = scheduleEvent(function()
      Effects.fadeOut(widget, time, elapsed + 30)
    end, 30)
  else
    widget.fadeEvent = nil
  end
end

function Effects.cancelFade(widget)
  removeEvent(widget.fadeEvent)
  widget.fadeEvent = nil
end
