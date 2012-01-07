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

function string.starts(s, start)
  return string.sub(s, 1, #start) == start
end

function string.trim(s)
  return string.match(s, '^%s*(.*%S)') or ''
end
