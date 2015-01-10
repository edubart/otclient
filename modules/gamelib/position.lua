Position = {}

function Position.equals(pos1, pos2)
  return pos1.x == pos2.x and pos1.y == pos2.y and pos1.z == pos2.z
end

function Position.greaterThan(pos1, pos2, orEqualTo)
  if orEqualTo then
    return pos1.x >= pos2.x or pos1.y >= pos2.y or pos1.z >= pos2.z
  else
    return pos1.x > pos2.x or pos1.y > pos2.y or pos1.z > pos2.z
  end
end

function Position.lessThan(pos1, pos2, orEqualTo)
  if orEqualTo then
    return pos1.x <= pos2.x or pos1.y <= pos2.y or pos1.z <= pos2.z
  else
    return pos1.x < pos2.x or pos1.y < pos2.y or pos1.z < pos2.z
  end
end

function Position.isInRange(pos1, pos2, xRange, yRange)
  return math.abs(pos1.x-pos2.x) <= xRange and math.abs(pos1.y-pos2.y) <= yRange and pos1.z == pos2.z;
end

function Position.isValid(pos)
  return not (pos.x == 65535 and pos.y == 65535 and pos.z == 255)
end

function Position.distance(pos1, pos2)
  return math.sqrt(math.pow((pos2.x - pos1.x), 2) + math.pow((pos2.y - pos1.y), 2))
end

function Position.manhattanDistance(pos1, pos2)
  return math.abs(pos2.x - pos1.x) + math.abs(pos2.y - pos1.y)
end