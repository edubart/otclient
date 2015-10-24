GameServerOpcodes = {
    GameServerInitGame                  = 10,
    GameServerGMActions                 = 11,
    GameServerEnterGame                 = 15,
    GameServerLoginError                = 20,
    GameServerLoginAdvice               = 21,
    GameServerLoginWait                 = 22,
    GameServerAddCreature               = 23,
    GameServerPingBack                  = 29,
    GameServerPing                      = 30,
    GameServerChallenge                 = 31,
    GameServerDeath                     = 40,

    -- all in game opcodes must be greater than 50
    GameServerFirstGameOpcode           = 50,

    -- otclient ONLY
    GameServerExtendedOpcode            = 50,

    -- NOTE: add any custom opcodes in this range
    -- 51 - 99

    -- original tibia ONLY
    GameServerFullMap                   = 100,
    GameServerMapTopRow                 = 101,
    GameServerMapRightRow               = 102,
    GameServerMapBottomRow              = 103,
    GameServerMapLeftRow                = 104,
    GameServerUpdateTile                = 105,
    GameServerCreateOnMap               = 106,
    GameServerChangeOnMap               = 107,
    GameServerDeleteOnMap               = 108,
    GameServerMoveCreature              = 109,
    GameServerOpenContainer             = 110,
    GameServerCloseContainer            = 111,
    GameServerCreateContainer           = 112,
    GameServerChangeInContainer         = 113,
    GameServerDeleteInContainer         = 114,
    GameServerSetInventory              = 120,
    GameServerDeleteInventory           = 121,
    GameServerOpenNpcTrade              = 122,
    GameServerPlayerGoods               = 123,
    GameServerCloseNpcTrade             = 124,
    GameServerOwnTrade                  = 125,
    GameServerCounterTrade              = 126,
    GameServerCloseTrade                = 127,
    GameServerAmbient                   = 130,
    GameServerGraphicalEffect           = 131,
    GameServerTextEffect                = 132,
    GameServerMissleEffect              = 133,
    GameServerMarkCreature              = 134,
    GameServerTrappers                  = 135,
    GameServerCreatureHealth            = 140,
    GameServerCreatureLight             = 141,
    GameServerCreatureOutfit            = 142,
    GameServerCreatureSpeed             = 143,
    GameServerCreatureSkull             = 144,
    GameServerCreatureParty             = 145,
    GameServerCreatureUnpass            = 146,
    GameServerEditText                  = 150,
    GameServerEditList                  = 151,
    GameServerPlayerDataBasic           = 159, -- 910
    GameServerPlayerData                = 160,
    GameServerPlayerSkills              = 161,
    GameServerPlayerState               = 162,
    GameServerClearTarget               = 163,
    GameServerSpellDelay                = 164, -- 870
    GameServerSpellGroupDelay           = 165, -- 870
    GameServerMultiUseDelay             = 166, -- 870
    GameServerTalk                      = 170,
    GameServerChannels                  = 171,
    GameServerOpenChannel               = 172,
    GameServerOpenPrivateChannel        = 173,
    GameServerRuleViolationChannel      = 174,
    GameServerRuleViolationRemove       = 175,
    GameServerRuleViolationCancel       = 176,
    GameServerRuleViolationLock         = 177,
    GameServerOpenOwnChannel            = 178,
    GameServerCloseChannel              = 179,
    GameServerTextMessage               = 180,
    GameServerCancelWalk                = 181,
    GameServerWalkWait                  = 182,
    GameServerFloorChangeUp             = 190,
    GameServerFloorChangeDown           = 191,
    GameServerChooseOutfit              = 200,
    GameServerVipAdd                    = 210,
    GameServerVipLogin                  = 211,
    GameServerVipLogout                 = 212,
    GameServerTutorialHint              = 220,
    GameServerAutomapFlag               = 221,
    GameServerCoinBalance               = 223, -- 1080
    GameServerStoreError                = 224, -- 1080
    GameServerRequestPurchaseData       = 225, -- 1080
    GameServerQuestLog                  = 240,
    GameServerQuestLine                 = 241,
    GameServerCoinBalanceUpdating       = 242, -- 1080
    GameServerChannelEvent              = 243, -- 910
    GameServerItemInfo                  = 244, -- 910
    GameServerPlayerInventory           = 245, -- 910
    GameServerMarketEnter               = 246, -- 944
    GameServerMarketLeave               = 247, -- 944
    GameServerMarketDetail              = 248, -- 944
    GameServerMarketBrowse              = 249, -- 944
    GameServerShowModalDialog           = 250, -- 960
    GameServerStore                     = 251, -- 1080
    GameServerStoreOffers               = 252, -- 1080
    GameServerStoreTransactionHistory   = 253, -- 1080
    GameServerStoreCompletePurchase     = 254  -- 1080
}

ClientOpcodes = {
    ClientEnterAccount                  = 1,
    ClientEnterGame                     = 10,
    ClientLeaveGame                     = 20,
    ClientPing                          = 29,
    ClientPingBack                      = 30,

    -- all in game opcodes must be equal or greater than 50
    ClientFirstGameOpcode               = 50,

    -- otclient ONLY
    ClientExtendedOpcode                = 50,

    -- NOTE: add any custom opcodes in this range
    -- 51 - 99

    -- original tibia ONLY
    ClientAutoWalk                      = 100,
    ClientWalkNorth                     = 101,
    ClientWalkEast                      = 102,
    ClientWalkSouth                     = 103,
    ClientWalkWest                      = 104,
    ClientStop                          = 105,
    ClientWalkNorthEast                 = 106,
    ClientWalkSouthEast                 = 107,
    ClientWalkSouthWest                 = 108,
    ClientWalkNorthWest                 = 109,
    ClientTurnNorth                     = 111,
    ClientTurnEast                      = 112,
    ClientTurnSouth                     = 113,
    ClientTurnWest                      = 114,
    ClientEquipItem                     = 119, -- 910
    ClientMove                          = 120,
    ClientInspectNpcTrade               = 121,
    ClientBuyItem                       = 122,
    ClientSellItem                      = 123,
    ClientCloseNpcTrade                 = 124,
    ClientRequestTrade                  = 125,
    ClientInspectTrade                  = 126,
    ClientAcceptTrade                   = 127,
    ClientRejectTrade                   = 128,
    ClientUseItem                       = 130,
    ClientUseItemWith                   = 131,
    ClientUseOnCreature                 = 132,
    ClientRotateItem                    = 133,
    ClientCloseContainer                = 135,
    ClientUpContainer                   = 136,
    ClientEditText                      = 137,
    ClientEditList                      = 138,
    ClientLook                          = 140,
    ClientTalk                          = 150,
    ClientRequestChannels               = 151,
    ClientJoinChannel                   = 152,
    ClientLeaveChannel                  = 153,
    ClientOpenPrivateChannel            = 154,
    ClientCloseNpcChannel               = 158,
    ClientChangeFightModes              = 160,
    ClientAttack                        = 161,
    ClientFollow                        = 162,
    ClientInviteToParty                 = 163,
    ClientJoinParty                     = 164,
    ClientRevokeInvitation              = 165,
    ClientPassLeadership                = 166,
    ClientLeaveParty                    = 167,
    ClientShareExperience               = 168,
    ClientDisbandParty                  = 169,
    ClientOpenOwnChannel                = 170,
    ClientInviteToOwnChannel            = 171,
    ClientExcludeFromOwnChannel         = 172,
    ClientCancelAttackAndFollow         = 190,
    ClientRefreshContainer              = 202,
    ClientRequestOutfit                 = 210,
    ClientChangeOutfit                  = 211,
    ClientMount                         = 212, -- 870
    ClientAddVip                        = 220,
    ClientRemoveVip                     = 221,
    ClientBugReport                     = 230,
    ClientRuleViolation                 = 231,
    ClientDebugReport                   = 232,
    ClientTransferCoins                 = 239, -- 1080
    ClientRequestQuestLog               = 240,
    ClientRequestQuestLine              = 241,
    ClientNewRuleViolation              = 242, -- 910
    ClientRequestItemInfo               = 243, -- 910
    ClientMarketLeave                   = 244, -- 944
    ClientMarketBrowse                  = 245, -- 944
    ClientMarketCreate                  = 246, -- 944
    ClientMarketCancel                  = 247, -- 944
    ClientMarketAccept                  = 248, -- 944
    ClientAnswerModalDialog             = 249, -- 960
    ClientOpenStore                     = 250, -- 1080
    ClientRequestStoreOffers            = 251, -- 1080
    ClientBuyStoreOffer                 = 252, -- 1080
    ClientOpenTransactionHistory        = 253, -- 1080
    ClientRequestTransactionHistory     = 254  -- 1080
}
