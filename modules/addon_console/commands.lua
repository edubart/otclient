function dumpWidgets()
  for i=1,rootWidget:getChildCount() do
    print(rootWidget:getChildByIndex(i):getId())
  end
end