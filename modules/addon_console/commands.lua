function dumpWidgets()
  for i=1,UI.root:getChildCount() do
    print(UI.root:getChildByIndex(i):getId())
  end
end