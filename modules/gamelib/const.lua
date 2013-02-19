-- @docconsts @{

FloorHigher = 0
FloorLower = 15

SkullNone = 0
SkullYellow = 1
SkullGreen = 2
SkullWhite = 3
SkullRed = 4
SkullBlack = 5
SkullOrange = 6

ShieldNone = 0
ShieldWhiteYellow = 1
ShieldWhiteBlue = 2
ShieldBlue = 3
ShieldYellow = 4
ShieldBlueSharedExp = 5
ShieldYellowSharedExp = 6
ShieldBlueNoSharedExpBlink = 7
ShieldYellowNoSharedExpBlink = 8
ShieldBlueNoSharedExp = 9
ShieldYellowNoSharedExp = 10

EmblemNone = 0
EmblemGreen = 1
EmblemRed = 2
EmblemBlue = 3

North = 0
East = 1
South = 2
West = 3
NorthEast = 4
SouthEast = 5
SouthWest = 6
NorthWest = 7

FightOffensive = 1
FightBalanced = 2
FightDefensive = 3

DontChase = 0
ChaseOpponent = 1

GameProtocolChecksum = 1
GameAccountNames = 2
GameChallengeOnLogin = 3
GamePenalityOnDeath = 4
GameNameOnNpcTrade = 5
GameDoubleFreeCapacity = 6
GameDoubleExperience = 7
GameTotalCapacity = 8
GameSkillsBase = 9
GamePlayerRegenerationTime = 10
GameChannelPlayerList = 11
GamePlayerMounts = 12
GameEnvironmentEffect = 13
GameCreatureEmblems = 14
GameItemAnimationPhase = 15
GameMagicEffectU16 = 16
GamePlayerMarket = 17
GameSpritesU32 = 18
GameChargeableItems = 19
GameOfflineTrainingTime = 20
GamePurseSlot = 21
GameFormatCreatureName = 22
GameSpellList = 23
GameClientPing = 24
GameExtendedClientPing = 25
GameDoubleHealth = 28
GameDoubleSkills = 29
GameChangeMapAwareRange = 30
GameMapMovePosition = 31
GameAttackSeq = 32
GameBlueNpcNameColor = 33
GameDiagonalAnimatedText = 34
GameLoginPending = 35
GameNewSpeedLaw = 36
GameForceFirstAutoWalkStep = 37
GameMinimapRemove = 38
GameDoubleShopSellAmount = 39

TextColors = {
  red       = '#f55e5e', --'#c83200'
  orange    = '#f36500', --'#c87832'
  yellow    = '#ffff00', --'#e6c832'
  green     = '#00EB00', --'#3fbe32'
  lightblue = '#5ff7f7',
  blue      = '#9f9dfd',
  --blue1     = '#6e50dc',
  --blue2     = '#3264c8',
  --blue3     = '#0096c8',
  white     = '#ffffff', --'#bebebe'
}

MessageModes = {
  None                    = 0,
  Say                     = 1,
  Whisper                 = 2,
  Yell                    = 3,
  PrivateFrom             = 4,
  PrivateTo               = 5,
  ChannelManagement       = 6,
  Channel                 = 7,
  ChannelHighlight        = 8,
  Spell                   = 9,
  NpcFrom                 = 10,
  NpcTo                   = 11,
  GamemasterBroadcast     = 12,
  GamemasterChannel       = 13,
  GamemasterPrivateFrom   = 14,
  GamemasterPrivateTo     = 15,
  Login                   = 16,
  Warning                 = 17,
  Game                    = 18,
  Failure                 = 19,
  Look                    = 20,
  DamageDealed            = 21,
  DamageReceived          = 22,
  Heal                    = 23,
  Exp                     = 24,
  DamageOthers            = 25,
  HealOthers              = 26,
  ExpOthers               = 27,
  Status                  = 28,
  Loot                    = 29,
  TradeNpc                = 30,
  Guild                   = 31,
  PartyManagement         = 32,
  Party                   = 33,
  BarkLow                 = 34,
  BarkLoud                = 35,
  Report                  = 36,
  HotkeyUse               = 37,
  TutorialHint            = 38,
  Thankyou                = 39,
  Market                  = 40,
  BeyondLast              = 41,
  MonsterYell             = 42,
  MonsterSay              = 43,
  Red                     = 44,
  Blue                    = 45,
  RVRChannel              = 46,
  RVRAnswer               = 47,
  RVRContinue             = 48,
  Last                    = 49,
  Invalid                 = 255,
}

OTSERV_RSA  = "1091201329673994292788609605089955415282375029027981291234687579" ..
              "3726629149257644633073969600111060390723088861007265581882535850" ..
              "3429057592827629436413108566029093628212635953836686562675849720" ..
              "6207862794310902180176810615217550567108238764764442605581471797" ..
              "07119674283982419152118103759076030616683978566631413"

CIPSOFT_RSA = "1321277432058722840622950990822933849527763264961655079678763618" ..
              "4334395343554449668205332383339435179772895415509701210392836078" ..
              "6959821132214473291575712138800495033169914814069637740318278150" ..
              "2907336840325241747827401343576296990629870233111328210165697754" ..
              "88792221429527047321331896351555606801473202394175817"

-- set to the latest Tibia.pic signature to make otclient compatible with official tibia
PIC_SIGNATURE = 0x50a6469d

OsTypes = {
  Linux = 1,
  Windows = 2,
  Flash = 3,
  OtclientLinux = 10,
  OtclientWindows = 11,
  OtclientMac = 12,
}

PathFindResults = {
  Ok = 0,
  Position = 1,
  Impossible = 2,
  TooFar = 3,
  NoWay = 4,
}

PathFindFlags = {
  AllowNullTiles = 1,
  AllowCreatures = 2,
  AllowNonPathable = 4,
  AllowNonWalkable = 8,
}

VipState = {
  Offline = 0,
  Online = 1,
  Pending = 2,
}

ExtendedIds = {
  Activate = 0,
  Locale = 1,
  Ping = 2,
  Sound = 3,
  Game = 4,
  Particles = 5,
  MapShader = 6,
  NeedsUpdate = 7
}

-- @}
