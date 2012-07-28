Bit = {}

function Bit.bit(p)
  return 2 ^ p
end

function Bit.hasBit(x, p)
  return x % (p + p) >= p
end

function Bit.setbit(x, p)
  return Bit.hasBit(x, p) and x or x + p
end

function Bit.clearbit(x, p)
  return Bit.hasBit(x, p) and x - p or x
end