string.explode = function (str, sep, limit)
	if(type(sep) ~= 'string' or tostring(str):len() == 0 or sep:len() == 0) then
		return {}
	end

	local i, pos, tmp, t = 0, 1, "", {}
	for s, e in function() return string.find(str, sep, pos) end do
		tmp = str:sub(pos, s - 1):trim()
		table.insert(t, tmp)
		pos = e + 1

		i = i + 1
		if(limit ~= nil and i == limit) then
			break
		end
	end

	tmp = str:sub(pos):trim()
	table.insert(t, tmp)
	return t
end