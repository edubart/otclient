Struct = {}

function Struct.pack(format, ...)
  local stream = {}
  local vars = {...}
  local endianness = true

  for i = 1, string.len(format) do
    local opt = string.sub(format, i, i)

    if opt == '<' or opt == '>' then
      endianness = opt == '<' and true or false
    elseif opt == 'b' or opt == 'B' or opt == 'h' or opt == 'H' or opt == 'i' or opt == 'I' or opt == 'l' or opt == 'L' then
      local n = ((opt == 'h' or opt == 'H') and 2) or ((opt == 'i' or opt == 'I') and 4) or ((opt == 'l' or opt == 'L') and 8) or 1
      local val = tonumber(table.remove(vars, 1))

      if val < 0 then
        val = val + 2 ^ (n * 8 - 1)
      end

      local bytes = {}
      for j = 1, n do
        table.insert(bytes, string.char(val % (2 ^ 8)))
        val = math.floor(val / (2 ^ 8))
      end

      if not endianness then
        table.insert(stream, string.reverse(table.concat(bytes)))
      else
        table.insert(stream, table.concat(bytes))
      end
    elseif opt == 'f' or opt == 'd' then
      local val = tonumber(table.remove(vars, 1))
      local sign = 0

      if val < 0 then
        sign = 1 
        val = -val 
      end

      local mantissa, exponent = math.frexp(val)
      if val == 0 then
        mantissa = 0
        exponent = 0
      else
        mantissa = (mantissa * 2 - 1) * math.ldexp(0.5, (opt == 'd') and 53 or 24)
        exponent = exponent + ((opt == 'd') and 1022 or 126)
      end

      local bytes = {}
      if opt == 'd' then
        val = mantissa
        for i = 1, 6 do
          table.insert(bytes, string.char(math.floor(val) % (2 ^ 8)))
          val = math.floor(val / (2 ^ 8))
        end
      else
        table.insert(bytes, string.char(math.floor(mantissa) % (2 ^ 8)))
        val = math.floor(mantissa / (2 ^ 8))
        table.insert(bytes, string.char(math.floor(val) % (2 ^ 8)))
        val = math.floor(val / (2 ^ 8))
      end

      table.insert(bytes, string.char(math.floor(exponent * ((opt == 'd') and 16 or 128) + val) % (2 ^ 8)))
      val = math.floor((exponent * ((opt == 'd') and 16 or 128) + val) / (2 ^ 8))
      table.insert(bytes, string.char(math.floor(sign * 128 + val) % (2 ^ 8)))
      val = math.floor((sign * 128 + val) / (2 ^ 8))

      if not endianness then
        table.insert(stream, string.reverse(table.concat(bytes)))
      else
        table.insert(stream, table.concat(bytes))
      end
    elseif opt == 's' then
      table.insert(stream, tostring(table.remove(vars, 1)))
      table.insert(stream, string.char(0))
    end
  end

  return table.concat(stream)
end

function Struct.unpack(format, stream)
  local vars = {}
  local iterator = 1
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
        local byte = string.byte(string.sub(stream, iterator, iterator))
        if endianness then
          val = val + byte * (2 ^ ((j - 1) * 8))
        else
          val = val + byte * (2 ^ ((n - j) * 8))
        end
        iterator = iterator + 1
      end

      if signed then
        val = val - 2 ^ (n * 8 - 1)
      end

      table.insert(vars, val)
    elseif opt == 'f' or opt == 'd' then
      local n = (opt == 'd') and 8 or 4
      local x = string.sub(stream, iterator, iterator + n - 1)
      iterator = iterator + n

      if not endianness then
        x = string.reverse(x)
      end

      local sign = 1
      local mantissa = string.byte(x, (opt == 'd') and 7 or 3) % ((opt == 'd') and 16 or 128)
      for i = n - 2, 1, -1 do
        mantissa = mantissa * (2 ^ 8) + string.byte(x, i)
      end

      if string.byte(x, n) > 127 then
        sign = -1
      end

      local exponent = (string.byte(x, n) % 128) * ((opt == 'd') and 16 or 2) + math.floor(string.byte(x, n - 1) / ((opt == 'd') and 16 or 128))
      if exponent == 0 then
        table.insert(vars, 0.0)
      else
        mantissa = (math.ldexp(mantissa, (opt == 'd') and -52 or -23) + 1) * sign
        table.insert(vars, math.ldexp(mantissa, exponent - ((opt == 'd') and 1023 or 127)))
      end
    elseif opt == 's' then
      local bytes = {}
      for j = iterator, string.len(stream) do
        if string.sub(stream, j, j) == string.char(0) then
          break
        end

        table.insert(bytes, string.sub(stream, j, j))
      end

      local str = table.concat(bytes)
      iterator = iterator + string.len(str) + 1
      table.insert(vars, str)
    end
  end

  return unpack(vars)
end
