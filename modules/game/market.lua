MarketAction = {
  Buy = 0,
  Sell = 1
}

MarketRequest = {
  MyOffers = 0xFFFE,
  MyHistory = 0xFFFF
}

MarketOfferState = {
  Active = 0,
  Cancelled = 1,
  Expired = 2,
  Accepted = 3,
  AcceptedEx = 255
}

MarketItemDescription = {
  Armor = 1,
  Attack = 2,
  Container = 3,
  Defense = 4,
  General = 5,
  DecayTime = 6,
  Combat = 7,
  MinLevel = 8,
  MinMagicLevel = 9,
  Vocation = 10,
  Rune = 11,
  Ability = 12,
  Charges = 13,
  WeaponName = 14,
  Weight = 15,

  First = Armor,
  Last = Weight
}
