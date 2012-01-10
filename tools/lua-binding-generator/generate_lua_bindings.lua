#!/usr/bin/lua

if not (#arg >= 1 and #arg <= 4) then
  print('usage: ' .. arg[0] .. ' <cpp class header> [class name] [cpp class instance] [lua class instance]')
  return false
end

cppclassheader = arg[1]
cppclassname = arg[2]
cppclassinstance = arg[3]
luaclassname = arg[3] or luaclassname

if not io.open(cppclassheader, 'r') then
  print('could not open ' .. cppclassheader)
  return false
end

function string:matchcount(pattern)
  local count = 0
  for w in self:gmatch(pattern) do count = count + 1 end
  return count
end

function string:splitlines()
  local t = {}
  local function helper(line) table.insert(t, line) return "" end
  helper((self:gsub("(.-)\r?\n", helper)))
  return t
end

classfound = false
publicmethods = false
for line in io.lines(cppclassheader) do
  foundclassname = line:match('^class ([%w_]+)')
  if foundclassname then
    if not cppclassname then
      guessedclassname = cppclassheader:match('([%w_]+)\.h$'):lower()
      if foundclassname:lower() == guessedclassname then
        cppclassname = foundclassname
      end
    end
    if foundclassname == cppclassname then
      classfound = true
      publicmethods = false

      if cppclassinstance then
        print('    g_lua.registerStaticClass("' .. luaclassname .. '");')
      else
        baseclassname = line:match(': public ([%w_]+)')
        bindline = '    g_lua.registerClass<' .. cppclassname

        if baseclassname and baseclassname ~= 'LuaObject' then
          bindline = bindline .. ', ' .. baseclassname
        end

        bindline = bindline .. '>();'
        print(bindline)

        bindline = '    g_lua.bindClassStaticFunction<' .. cppclassname .. '>("create", []{ return ' .. cppclassname .. 'Ptr(new ' .. cppclassname ..  '); });'
        print(bindline)
      end
    elseif classfound then
      return true
    end
  end
  if classfound then
    if line:match('public:') then
      publicmethods = true
    elseif line:match('private:') or line:match('protected:') then
      publicmethods = false
    elseif publicmethods then
      funcname, args = line:match('^ *[%w <>&\*:_]* ([%w_]+)%(([^%)]*%))[%w ]*[;{=].*$')
      if funcname then
        if funcname ~= cppclassname and funcname ~= 'create' then
          numargs = args:matchcount('[^,)]+[,)]')

          if cppclassinstance then
            bindline = '    g_lua.bindClassStaticFunction("' .. luaclassname .. '", "' .. funcname .. '", ' ..
                      'std::bind(&' .. cppclassname .. "::" .. funcname .. ', &' .. cppclassinstance
            for i=1,numargs do
              bindline = bindline .. ', _' .. i
            end
            bindline = bindline .. '));'
          else
            bindline = '    g_lua.bindClassMemberFunction<' .. cppclassname .. '>("' .. funcname .. '", &' ..
                      cppclassname .. '::' .. funcname .. ');'
          end
          print(bindline)
        end
      end
     end
  end
end

return true
