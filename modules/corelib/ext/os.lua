function os.execute(command)
  local f = assert(io.popen(command, 'r'))
  local data = assert(f:read('*a'))
  f:close()
  print(data)
end
