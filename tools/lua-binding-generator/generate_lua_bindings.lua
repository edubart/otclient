#!/usr/bin/lua

-- parse options
if #arg == 0 then
  print('usage: ' .. arg[0] .. ' <headers> [--doc]')
  os.exit(1)
end

gendoc = false
for i=1,#arg do
  if arg[i] == '--doc' then
    table.remove(arg, i)
    gendoc = true
    break
  end
end

if #arg == 0 then
  print('Specify a file.')
  os.exit(1)
end

-- string utilities
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


function table.removevalue(t, value)
  for k,v in pairs(t) do
    if v == value then
      table.remove(t, k)
      break
    end
  end
end

function string.split(s, delim)
  local start = 1
  local results = {}
  while true do
    local pos = string.find(s, delim, start, true)
    if not pos then
      break
    end
    table.insert(results, string.sub(s, start, pos-1))
    start = pos + string.len(delim)
  end
  table.insert(results, string.sub(s, start))
  table.removevalue(results, '')
  return results
end

function string.trim(s)
  return string.match(s, '^%s*(.*%S)') or ''
end

function countbrackets(str)
  local ret = 0
  for _i in str:gmatch('{') do ret = ret+1 end
  for _i in str:gmatch('}') do ret = ret-1 end
  return ret
end

function filterType(arg)
  arg = arg:gsub('^const[%s]+', '')
  arg = arg:gsub('Ptr', '')
  arg = arg:gsub('&', '')
  arg = arg:gsub('.*List$', 'table')
  arg = arg:gsub('^std::string$', 'string')
  arg = arg:gsub('^OTMLNode$', 'table')
  arg = arg:gsub('^std::vector<.*>$', 'table')
  arg = arg:gsub('^std::map<.*>$', 'table')
  arg = arg:gsub('^[u]?int[0-9_t]*$', 'integer')
  arg = arg:gsub('^float$', 'number')
  arg = arg:gsub('^double$', 'number')
  arg = arg:gsub('^bool$', 'boolean')
  arg = arg:gsub('^ticks_t$', 'integer')
  arg = arg:gsub('.*\*.*', 'buffer')
  arg = arg:gsub('.*::.*', 'enum')
  return arg
end

function filterArgs(str)
  local args = str:split(',') or { str }

  newstr = ''
  for i,argstr in pairs(args) do
    argstr = argstr:gsub('[%s]*=.*','')
    local argtype, argvar = argstr:match('^(.*[%s]+[&*]?)([%w_]*)')
    newstr = newstr .. filterType(argtype:trim()) .. ' ' .. argvar:trim()

    if i ~= #args then
      newstr = newstr .. ', '
    end
  end

  return newstr
end

function filterReturn(str)
  str = str:gsub('virtual ', '')
  str = str:gsub('static ', '')
  return filterType(str:trim())
end


function emitSingletonDecl(cppclass, luaclass)
  if gendoc then
    outline = 'class ' .. luaclass .. ' {\n' ..
              'public:'
  else
    outline = '    // ' .. luaclass .. '\n' ..
              '    g_lua.registerStaticClass("' .. luaclass .. '");'
  end
  print(outline)
  return outline
end

function emitClassDecl(cppclass, luaclass, baseclass)
  local outline
  if gendoc then
    outline = 'class ' .. luaclass
    if cppclass ~= 'LuaObject' and baseclass then
      outline = outline .. ' : public ' .. baseclass
    end
    outline = outline .. ' {\npublic:'
  else
    print('    // ' .. luaclass)
    outline = '    g_lua.registerClass<' .. cppclass
    if cppclass ~= 'LuaObject' and baseclass and baseclass ~= 'LuaObject' then
      outline = outline .. ', ' .. baseclass
    end
    outline = outline .. '>();'
  end
  print(outline)
  return outline
end

function emitClassEnd()
  local outline = '\n'
  if gendoc then
    outline = '};\n'
  end
  print(outline)
  return outline
end

function emitSingletonFunctionDecl(cppclass, luaclass, funcname, funcargs, funcret)
  local outline
  if gendoc then
    outline = '    ' .. filterReturn(funcret) .. ' ' .. funcname .. '(' .. filterArgs(funcargs) .. ');'
  else
    outline = '    g_lua.bindSingletonFunction("' .. luaclass .. '", "' .. funcname .. '", &' ..  cppclass .. '::' .. funcname .. ', &' .. luaclass .. ');'
  end
  print(outline)
  return outline
end

function emitMemberFunctionDecl(cppclass, luaclass, funcname, funcargs, funcret)
  local outline
  if gendoc then
    outline = '    ' .. filterReturn(funcret) .. ' ' .. funcname .. '(' .. filterArgs(funcargs) .. ');'
  else
    outline = '    g_lua.bindClassMemberFunction("' .. funcname .. '", &' ..  cppclass .. '::' .. funcname .. ');'
  end
  print(outline)
  return outline
end

function parseClassHeader(line, param)
  cppclass = line:match('^[%s]*class[%s]+([%w_]+)')
  if not cppclass then
    print('Invalid directive at ' .. header .. ':' .. linenumber)
    os.exit(1)
  end

  luaclass = param or cppclass
  insideclass = true
  publicmethods = true
  brackets = 0
  bindnext = true

  if singleton then
    emitSingletonDecl(cppclass, luaclass)
  else
    baseclass = line:match(':[%s]+public[%s]+([%w_]+)')
    emitClassDecl(cppclass, luaclass, baseclass)
  end
end

function parseHeader(file)
  header = file
  linenumber = 0
  brackets = 0
  publicmethods = false
  insideclass = false
  singleton = false

  if not io.open(header, 'r') then
    print('Unable to open ' .. header)
    exit(1)
  end

  lines = {}
  for line in io.lines(header) do table.insert(lines, line) end

  for linenumber=1,#lines do
    local line = lines[linenumber]
    local param = line:match('^[%s]*//[%s]*@[%w]+[%s]+(.*)[%s]*')

    if not insideclass then
      if line:match('[%s]*//[%s]*@bindsingleton') then
        singleton = true
        linenumber = linenumber+1
        parseClassHeader(lines[linenumber], param)
      elseif line:match('[%s]*//[%s]*@bindclass') then
        singleton = false
        linenumber = linenumber+1
        parseClassHeader(lines[linenumber], param)
      end
    else
      if brackets > 1 then
        -- ignore
      elseif line:match('[%s]*//[%s]*@dontbind') then
        bindnext = false
      elseif line:match('[%s]*template') then
        bindnext = false
      elseif line:match('[%s]*public:') then
        publicmethods = true
      elseif line:match('[%s]*private:') or line:match('[%s]*protected:') then
        publicmethods = false
      elseif line:match('^};') then
        insideclass = false
        emitClassEnd()
      elseif bindnext then
        funcreturn, funcname, funcargs = line:match('^[%s]*([%w <>&\*:_]*) ([%w_]+)%(([^%)]*%))[%w ]*[;{=].*$')
        if funcname then
          funcargs = funcargs:match('(.*)\%)')
          if funcname ~= cppclass then
            if singleton then
              emitSingletonFunctionDecl(cppclass, luaclass, funcname, funcargs, funcreturn)
            else
              emitMemberFunctionDecl(cppclass, luaclass, funcname, funcargs, funcreturn)
            end
          end
        end
      else
        bindnext = true
      end
      brackets = brackets + countbrackets(line)
    end
  end
end

for i=1,#arg do
  parseHeader(arg[i])
end