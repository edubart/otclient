function UIWidget:setMargin(...)
    local params = {...}
    if #params == 1 then
        self.marginTop = params[1]
        self.marginRight = params[1]
        self.marginBottom = params[1]
        self.marginLeft = params[1]
    elseif #params == 2 then
        self.marginTop = params[1]
        self.marginRight = params[2]
        self.marginBottom = params[1]
        self.marginLeft = params[2]
    elseif #params == 4 then
        self.marginTop = params[1]
        self.marginRight = params[2]
        self.marginBottom = params[3]
        self.marginLeft = params[4]
    end
end