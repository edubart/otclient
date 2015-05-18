Struct = {}

function Struct.pack(format, ...)
  local stream = ''
  local vars = {...}
  local endianness = true

  for i = 1, string.len(format) do
    local opt = string.sub(format, i, i)

    if opt == '<' or opt == '>' then
      endianness = opt == '<' and true or false
    elseif opt == 'b' or opt == 'B' or opt == 'h' or opt == 'H' or opt == 'i' or opt == 'I' or opt == 'l' or opt == 'L' then
      local val = tonumber(table.remove(vars, 1))
      local n = ((opt == 'h' or opt == 'H') and 2) or ((opt == 'i' or opt == 'I') and 4) or ((opt == 'l' or opt == 'L') and 8) or 1

      if val < 0 then
        val = val + 2 ^ (n * 8 - 1)
      end

      local binary = ''
      for j = 1, n do
        binary = binary .. string.char(val % (2 ^ 8))
        val = math.floor(val / (2 ^ 8))
      end

      if not endianness then
        binary = string.reverse(binary)
      end

      stream = stream .. binary
    elseif opt == 's' then
      stream = stream .. tostring(table.remove(vars, 1))
      stream = stream .. string.char(0)
    end
  end

  return stream
end

function Struct.unpack(format, stream)
  local vars = {}
  local endianness = true

  for i = 1, string.len(format) do
    local opt = string.sub(format, i, i)

    if opt == '<' or opt == '>' then
      endianness = opt == '<' and true or false
    elseif opt == 'b' or opt == 'B' or opt == 'h' or opt == 'H' or opt == 'i' or opt == 'I' or opt == 'l' or opt == 'L' then
      local n = ((opt == 'h' or opt == 'H') and 2) or ((opt == 'i' or opt == 'I') and 4) or ((opt == 'l' or opt == 'L') and 8) or 1
      local signed = opt == 'b' or opt == 'h' or opt == 'i'

      local val = 0
      for j = 1, n do
        local byte = string.byte(string.sub(stream, 1, 1))
        if endianness then
          val = val + byte * (2 ^ ((j - 1) * 8))
        else
          val = val + byte * (2 ^ ((n - j) * 8))
        end
        stream = string.sub(stream, 2)
      end

      if signed then
        val = val - 2 ^ (n * 8 - 1)
      end

      table.insert(vars, val)
    elseif opt == 's' then
      local str = ''
      for j = 1, string.len(stream) do
        if string.sub(stream, j, j) == string.char(0) then
          break
        end

        str = str .. string.sub(stream, j, j)
      end

      stream = string.sub(stream, string.len(str) + 2)
      table.insert(vars, str)
    end
  end

  return unpack(vars)
end
