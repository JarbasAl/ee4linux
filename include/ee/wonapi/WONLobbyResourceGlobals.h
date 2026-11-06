// Target-specific original LobbyResource globals, IDs and APIs; genuine SDK global scope.
#ifndef EE_WONAPI_WONLOBBYRESOURCEGLOBALS_H
#define EE_WONAPI_WONLOBBYRESOURCEGLOBALS_H
#include "ee/wonapi/WONLobbyResource.h"

extern WONAPI::GUIString LobbyGlobal_Cancel_String; //0x566944
extern WONAPI::GUIString LobbyGlobal_Lan_String; //0x5669e0
extern WONAPI::GUIString LobbyGlobal_No_String; //0x566a58
extern WONAPI::GUIString LobbyGlobal_OK_String; //0x566468
extern WONAPI::GUIString LobbyGlobal_Yes_String; //0x566a30
extern WONAPI::GUIString LobbyGlobal_QuickPlay_String; //0x566758
extern WONAPI::SmartPtr<WONAPI::Image> LobbyGlobal_Searching_Image; //0x5668ac
extern WONAPI::SmartPtr<WONAPI::Image> LobbyGlobal_Firewall_Image; //0x566414
extern WONAPI::SmartPtr<WONAPI::Sound> LobbyGlobal_ButtonClick_Sound; //0x566770
extern WONAPI::SmartPtr<WONAPI::Sound> LobbyGlobal_LobbyMusic_Sound; //0x5664d0
extern WONAPI::SmartPtr<WONAPI::Sound> LobbyGlobal_AllReady_Sound; //0x5667d0
extern WONAPI::SmartPtr<WONAPI::Sound> LobbyGlobal_AllNotReady_Sound; //0x566438
extern WONAPI::SmartPtr<WONAPI::Sound> LobbyGlobal_AskToJoinRecv_Sound; //0x566694
extern WONAPI::SmartPtr<WONAPI::Sound> LobbyGlobal_Error_Sound; //0x566638
extern WONAPI::SmartPtr<WONAPI::Sound> LobbyGlobal_FriendJoined_Sound; //0x56646c
extern WONAPI::SmartPtr<WONAPI::Sound> LobbyGlobal_FriendLeft_Sound; //0x566a9c
extern WONAPI::SmartPtr<WONAPI::Sound> LobbyGlobal_InviteRecv_Sound; //0x566714
extern WONAPI::SmartPtr<WONAPI::Sound> LobbyGlobal_PlayerJoined_Sound; //0x566500
extern WONAPI::SmartPtr<WONAPI::Sound> LobbyGlobal_SierraLogon_Sound; //0x566a2c
extern WONAPI::SmartPtr<WONAPI::Sound> LobbyGlobal_WhisperRecv_Sound; //0x5665dc
extern WONAPI::GUIString AccountLogic_InvalidCDKeyTitle_String; //0x5666f4
extern WONAPI::GUIString AccountLogic_InvalidCDKeyDetails_String; //0x56682c
extern WONAPI::GUIString AddFriendDialog_StatusTitle_String; //0x566598
extern WONAPI::GUIString AddFriendDialog_StatusDetails_String; //0x566740
extern WONAPI::GUIString AdminActionDialog_Warn_String; //0x56640c
extern WONAPI::GUIString AdminActionDialog_Mute_String; //0x5664a0
extern WONAPI::GUIString AdminActionDialog_UnMute_String; //0x566a04
extern WONAPI::GUIString AdminActionDialog_Kick_String; //0x566604
extern WONAPI::GUIString AdminActionDialog_Ban_String; //0x566444
extern WONAPI::GUIString AdminActionDialog_ExplanationOpt_String; //0x566818
extern WONAPI::GUIString AdminActionDialog_ExplanationReq_String; //0x566548
extern WONAPI::GUIString BadUserDialog_OneDay_String; //0x5664cc
extern WONAPI::GUIString BadUserDialog_OneHour_String; //0x5665b4
extern WONAPI::GUIString BadUserDialog_OneMinute_String; //0x56675c
extern WONAPI::GUIString BadUserDialog_OneSecond_String; //0x5668dc
extern WONAPI::GUIString BadUserDialog_XDays_String; //0x566418
extern WONAPI::GUIString BadUserDialog_XHours_String; //0x566760
extern WONAPI::GUIString BadUserDialog_XMinutes_String; //0x5667b4
extern WONAPI::GUIString BadUserDialog_XSeconds_String; //0x566530
extern WONAPI::GUIString BadUserDialog_Infinite_String; //0x566828
extern WONAPI::GUIString ChatCommandLogic_Whisper_String; //0x5664bc
extern WONAPI::GUIString ChatCommandLogic_Block_String; //0x5666a0
extern WONAPI::GUIString ChatCommandLogic_Emote_String; //0x566a18
extern WONAPI::GUIString ChatCommandLogic_Ignore_String; //0x56641c
extern WONAPI::GUIString ChatCommandLogic_Away_String; //0x5668c8
extern WONAPI::GUIString ChatCommandLogic_Invite_String; //0x5668c4
extern WONAPI::GUIString ChatCommandLogic_Uninvite_String; //0x566708
extern WONAPI::GUIString ChatCommandLogic_Reply_String; //0x5665ac
extern WONAPI::GUIString ChatCommandLogic_Clear_String; //0x566a74
extern WONAPI::GUIString ChatCommandLogic_Help_String; //0x5666cc
extern WONAPI::GUIString ChatCommandLogic_ShowTeam_String; //0x5667a0
extern WONAPI::GUIString ChatCommandLogic_Moderator_String; //0x56642c
extern WONAPI::GUIString ChatCommandLogic_ServerMute_String; //0x566744
extern WONAPI::GUIString ChatCommandLogic_ServerBan_String; //0x56676c
extern WONAPI::GUIString ChatCommandLogic_Mute_String; //0x566648
extern WONAPI::GUIString ChatCommandLogic_Ban_String; //0x56689c
extern WONAPI::GUIString ChatCommandLogic_Unmute_String; //0x56679c
extern WONAPI::GUIString ChatCommandLogic_ServerUnmute_String; //0x5666e4
extern WONAPI::GUIString ChatCommandLogic_Unban_String; //0x566794
extern WONAPI::GUIString ChatCommandLogic_ServerUnban_String; //0x5663f4
extern WONAPI::GUIString ChatCommandLogic_Alert_String; //0x5665d0
extern WONAPI::GUIString ChatCommandLogic_Warn_String; //0x566a48
extern WONAPI::GUIString ChatCommandLogic_HelpHeader_String; //0x5668d8
extern WONAPI::GUIString ChatCommandLogic_HelpHelp_String; //0x566498
extern WONAPI::GUIString ChatCommandLogic_ShowTeamHelp_String; //0x5668b0
extern WONAPI::GUIString ChatCommandLogic_WhisperHelp_String; //0x56688c
extern WONAPI::GUIString ChatCommandLogic_BlockHelp_String; //0x566580
extern WONAPI::GUIString ChatCommandLogic_IgnoreHelp_String; //0x5663c4
extern WONAPI::GUIString ChatCommandLogic_ReplyHelp_String; //0x566620
extern WONAPI::GUIString ChatCommandLogic_EmoteHelp_String; //0x5667c0
extern WONAPI::GUIString ChatCommandLogic_AwayHelp_String; //0x5665f8
extern WONAPI::GUIString ChatCommandLogic_ClearHelp_String; //0x566874
extern WONAPI::GUIString ChatCommandLogic_InviteHelp_String; //0x566a98
extern WONAPI::GUIString ChatCommandLogic_UninviteHelp_String; //0x566820
extern WONAPI::GUIString ChatCommandLogic_MuteHelp_String; //0x566674
extern WONAPI::GUIString ChatCommandLogic_UnmuteHelp_String; //0x566658
extern WONAPI::GUIString ChatCommandLogic_BanHelp_String; //0x566984
extern WONAPI::GUIString ChatCommandLogic_UnbanHelp_String; //0x566920
extern WONAPI::GUIString ChatCommandLogic_ModeratorHelp_String; //0x566958
extern WONAPI::GUIString ChatCommandLogic_ServerMuteHelp_String; //0x5669f4
extern WONAPI::GUIString ChatCommandLogic_ServerUnmuteHelp_String; //0x5665cc
extern WONAPI::GUIString ChatCommandLogic_ServerBanHelp_String; //0x566ab0
extern WONAPI::GUIString ChatCommandLogic_ServerUnbanHelp_String; //0x56686c
extern WONAPI::GUIString ChatCommandLogic_AlertHelp_String; //0x566860
extern WONAPI::GUIString ChatCommandLogic_WarnHelp_String; //0x56652c
extern WONAPI::GUIString ChatCtrl_NormalChat_String; //0x5667e4
extern WONAPI::GUIString ChatCtrl_Emote_String; //0x5668b8
extern WONAPI::GUIString ChatCtrl_TeamChat_String; //0x56692c
extern WONAPI::GUIString ChatCtrl_TeamEmote_String; //0x5666e0
extern WONAPI::GUIString ChatCtrl_YouWhisperToYourself_String; //0x566718
extern WONAPI::GUIString ChatCtrl_YouWhisper_String; //0x566900
extern WONAPI::GUIString ChatCtrl_SomeoneWhispersToYou_String; //0x566398
extern WONAPI::GUIString ChatCtrl_YouWarnSomeone_String; //0x5666d8
extern WONAPI::GUIString ChatCtrl_YouWarnYourself_String; //0x566a00
extern WONAPI::GUIString ChatCtrl_SomeoneWarnsYou_String; //0x5669b0
extern WONAPI::GUIString ChatCtrl_SomeoneWarnsSomeone_String; //0x566a70
extern WONAPI::GUIString ChatCtrl_EnteringRoom_String; //0x566410
extern WONAPI::GUIString ChatCtrl_LeavingRoom_String; //0x5669c8
extern WONAPI::GUIString ChatCtrl_Disconnected_String; //0x566664
extern WONAPI::GUIString ChatCtrl_YouWereKicked_String; //0x566908
extern WONAPI::GUIString ChatCtrl_YouWereBannedMinute_String; //0x56694c
extern WONAPI::GUIString ChatCtrl_YouWereBannedMinutes_String; //0x566634
extern WONAPI::GUIString ChatCtrl_YouWereBannedHour_String; //0x566428
extern WONAPI::GUIString ChatCtrl_YouWereBannedHours_String; //0x566a0c
extern WONAPI::GUIString ChatCtrl_YouWereBannedDay_String; //0x566600
extern WONAPI::GUIString ChatCtrl_YouWereBannedDays_String; //0x566ad0
extern WONAPI::GUIString ChatCtrl_YouWereBannedIndefinitely_String; //0x56697c
extern WONAPI::GUIString ChatCtrl_SomeoneWasKicked_String; //0x5664c8
extern WONAPI::GUIString ChatCtrl_SomeoneWasBannedMinute_String; //0x5668a4
extern WONAPI::GUIString ChatCtrl_SomeoneWasBannedMinutes_String; //0x5663bc
extern WONAPI::GUIString ChatCtrl_SomeoneWasBannedHour_String; //0x5667d8
extern WONAPI::GUIString ChatCtrl_SomeoneWasBannedHours_String; //0x566684
extern WONAPI::GUIString ChatCtrl_SomeoneWasBannedDay_String; //0x566ac4
extern WONAPI::GUIString ChatCtrl_SomeoneWasBannedDays_String; //0x56660c
extern WONAPI::GUIString ChatCtrl_SomeoneWasBannedIndefinitely_String; //0x5664b8
extern WONAPI::GUIString ChatCtrl_YouWereUnmuted_String; //0x5663d4
extern WONAPI::GUIString ChatCtrl_YouWereMutedMinute_String; //0x5664e4
extern WONAPI::GUIString ChatCtrl_YouWereMutedMinutes_String; //0x5666ac
extern WONAPI::GUIString ChatCtrl_YouWereMutedHour_String; //0x5663cc
extern WONAPI::GUIString ChatCtrl_YouWereMutedHours_String; //0x566ae4
extern WONAPI::GUIString ChatCtrl_YouWereMutedDay_String; //0x566a34
extern WONAPI::GUIString ChatCtrl_YouWereMutedDays_String; //0x5665e4
extern WONAPI::GUIString ChatCtrl_YouWereMutedIndefinitely_String; //0x56649c
extern WONAPI::GUIString ChatCtrl_SomeoneWasUnmuted_String; //0x566960
extern WONAPI::GUIString ChatCtrl_SomeoneWasMutedMinute_String; //0x566424
extern WONAPI::GUIString ChatCtrl_SomeoneWasMutedMinutes_String; //0x5667a8
extern WONAPI::GUIString ChatCtrl_SomeoneWasMutedHour_String; //0x5663e8
extern WONAPI::GUIString ChatCtrl_SomeoneWasMutedHours_String; //0x566588
extern WONAPI::GUIString ChatCtrl_SomeoneWasMutedDay_String; //0x566690
extern WONAPI::GUIString ChatCtrl_SomeoneWasMutedDays_String; //0x5668e4
extern WONAPI::GUIString ChatCtrl_SomeoneWasMutedIndefinitely_String; //0x566440
extern WONAPI::GUIString ChatCtrl_ModeratorComment_String; //0x566494
extern WONAPI::GUIString ChatCtrl_AlertComment_String; //0x566898
extern WONAPI::GUIString ChatCtrl_ServerAlert_String; //0x5664b4
extern WONAPI::GUIString ChatCtrl_ServerShutdownStartedMinute_String; //0x56669c
extern WONAPI::GUIString ChatCtrl_ServerShutdownStartedMinutes_String; //0x5668d4
extern WONAPI::GUIString ChatCtrl_ServerShutdownStartedHour_String; //0x5665a4
extern WONAPI::GUIString ChatCtrl_ServerShutdownStartedHours_String; //0x566914
extern WONAPI::GUIString ChatCtrl_ServerShutdownStartedDay_String; //0x5663d0
extern WONAPI::GUIString ChatCtrl_ServerShutdownStartedDays_String; //0x5668e0
extern WONAPI::GUIString ChatCtrl_ServerShutdownAborted_String; //0x566608
extern WONAPI::GUIString ChatCtrl_SomeoneInvitesYou_String; //0x566940
extern WONAPI::GUIString ChatCtrl_SomeoneUninvitesYou_String; //0x566934
extern WONAPI::GUIString ChatCtrl_YouInviteSomeone_String; //0x566644
extern WONAPI::GUIString ChatCtrl_YouUninviteSomeone_String; //0x566478
extern WONAPI::GUIString ChatCtrl_SomeoneInvitesYouExpl_String; //0x566980
extern WONAPI::GUIString ChatCtrl_SomeoneUninvitesYouExpl_String; //0x566974
extern WONAPI::GUIString ChatCtrl_YouInviteSomeoneExpl_String; //0x5663e4
extern WONAPI::GUIString ChatCtrl_YouUninviteSomeoneExpl_String; //0x566878
extern WONAPI::GUIString ChatCtrl_Accept_String; //0x566ad4
extern WONAPI::GUIString ChatCtrl_Reject_String; //0x566574
extern WONAPI::GUIString ChatCtrl_SomeoneAsksToJoin_String; //0x5669bc
extern WONAPI::GUIString ChatCtrl_SomeoneAsksToJoinExpl_String; //0x566734
extern WONAPI::GUIString ChatCtrl_ClientNotOnServer_String; //0x5669a0
extern WONAPI::GUIString ChatCtrl_ClientInGame_String; //0x5669f8
extern WONAPI::GUIString ChatCtrl_ClientInChat_String; //0x5669dc
extern WONAPI::GUIString ChatCtrl_ClientInGameAndChat_String; //0x56693c
extern WONAPI::GUIString ChatCtrl_ClientIsAnonymous_String; //0x5664ec
extern WONAPI::GUIString ChatCtrl_FriendIsChatting_String; //0x56674c
extern WONAPI::GUIString ChatCtrl_FriendIsInGameStaging_String; //0x566768
extern WONAPI::GUIString ChatCtrl_FriendIsPlaying_String; //0x566acc
extern WONAPI::GUIString ChatCtrl_FriendIsOn_String; //0x566460
extern WONAPI::GUIString ChatCtrl_FriendIsNotOnline_String; //0x566a28
extern WONAPI::GUIString ChatCtrl_PlayerKicked_String; //0x566450
extern WONAPI::GUIString ChatCtrl_PlayerBanned_String; //0x566780
extern WONAPI::GUIString ChatCtrl_FriendJoined_String; //0x56661c
extern WONAPI::GUIString ChatCtrl_FriendLeft_String; //0x566ad8
extern WONAPI::GUIString ChatCtrl_TeamMemberJoined_String; //0x566954
extern WONAPI::GUIString ChatCtrl_TeamMemberLeft_String; //0x5666a8
extern WONAPI::GUIString ChatCtrl_TeamMembers_String; //0x566528
extern WONAPI::GUIString ChatCtrl_IgnoreList_String; //0x566a64
extern WONAPI::GUIString ChooseNetworkAdapterDialog_Default_String; //0x56681c
extern WONAPI::GUIString ChooseNetworkAdapterDialog_Unknown_String; //0x566594
extern WONAPI::GUIString ClientOptionsCtrl_Whisper_String; //0x566968
extern WONAPI::GUIString ClientOptionsCtrl_Invite_String; //0x566710
extern WONAPI::GUIString ClientOptionsCtrl_Uninvite_String; //0x566814
extern WONAPI::GUIString ClientOptionsCtrl_AddToFriends_String; //0x5667d4
extern WONAPI::GUIString ClientOptionsCtrl_RemoveFromFriends_String; //0x5665f0
extern WONAPI::GUIString ClientOptionsCtrl_ClearAFK_String; //0x566964
extern WONAPI::GUIString ClientOptionsCtrl_SetAFK_String; //0x5668f4
extern WONAPI::GUIString ClientOptionsCtrl_Ignore_String; //0x5668c0
extern WONAPI::GUIString ClientOptionsCtrl_Unignore_String; //0x56656c
extern WONAPI::GUIString ClientOptionsCtrl_Block_String; //0x5667bc
extern WONAPI::GUIString ClientOptionsCtrl_Unblock_String; //0x566400
extern WONAPI::GUIString ClientOptionsCtrl_Warn_String; //0x56653c
extern WONAPI::GUIString ClientOptionsCtrl_Mute_String; //0x566998
extern WONAPI::GUIString ClientOptionsCtrl_UnMute_String; //0x566890
extern WONAPI::GUIString ClientOptionsCtrl_Kick_String; //0x5664a8
extern WONAPI::GUIString ClientOptionsCtrl_Ban_String; //0x5667fc
extern WONAPI::GUIString DirectConnectionOptionsCtrl_Connect_String; //0x5666a4
extern WONAPI::GUIString DirectConnectionOptionsCtrl_Refresh_String; //0x5667c4
extern WONAPI::GUIString DirectConnectionOptionsCtrl_Remove_String; //0x566614
extern WONAPI::GUIString FriendOptionsCtrl_Locate_String; //0x5664e8
extern WONAPI::GUIString FriendOptionsCtrl_RefreshList_String; //0x56683c
extern WONAPI::GUIString FriendOptionsCtrl_RemoveFromFriends_String; //0x566a5c
extern WONAPI::SmartPtr<WONAPI::Image> FriendsListCtrl_Unknown_Image; //0x566aa4
extern WONAPI::SmartPtr<WONAPI::Image> FriendsListCtrl_NotFound_Image; //0x56677c
extern WONAPI::SmartPtr<WONAPI::Image> FriendsListCtrl_InChat_Image; //0x5663ec
extern WONAPI::SmartPtr<WONAPI::Image> FriendsListCtrl_InGame_Image; //0x566858
extern WONAPI::GUIString GameOptionsPopup_Join_String; //0x56647c
extern WONAPI::GUIString GameOptionsPopup_Ping_String; //0x566988
extern WONAPI::GUIString GameOptionsPopup_QueryDetails_String; //0x5665c8
extern WONAPI::GUIString GameOptionsPopup_HideDetails_String; //0x566790
extern WONAPI::GUIString GameOptionsPopup_ShowDetails_String; //0x566a90
extern WONAPI::GUIString GameOptionsPopup_Remove_String; //0x5668fc
extern WONAPI::GUIString GameStagingCtrl_Ready_String; //0x566748
extern WONAPI::GUIString GameStagingCtrl_NotReady_String; //0x56650c
extern WONAPI::GUIString GameStagingCtrl_StartGame_String; //0x566558
extern WONAPI::GUIString GameStagingCtrl_LeaveGame_String; //0x566a08
extern WONAPI::GUIString GameStagingCtrl_DissolveGame_String; //0x566730
extern WONAPI::GUIString GameStagingLogic_KickedTitle_String; //0x5669fc
extern WONAPI::GUIString GameStagingLogic_KickedDetails_String; //0x5669a4
extern WONAPI::GUIString GameStagingLogic_BannedTitle_String; //0x566704
extern WONAPI::GUIString GameStagingLogic_BannedDetails_String; //0x566970
extern WONAPI::GUIString GameStagingLogic_ConnectWarningTitle1_String; //0x5665a8
extern WONAPI::GUIString GameStagingLogic_ConnectWarningDetails1_String; //0x566458
extern WONAPI::GUIString GameStagingLogic_ConnectWarningTitle2_String; //0x566448
extern WONAPI::GUIString GameStagingLogic_ConnectWarningDetails2_String; //0x5667f0
extern WONAPI::GUIString GameStagingLogic_ConnectWarningTitle3_String; //0x5669b4
extern WONAPI::GUIString GameStagingLogic_ConnectWarningDetails3_String; //0x5666c8
extern WONAPI::GUIString GameStagingLogic_NoPingReplyError_String; //0x56667c
extern WONAPI::GUIString GameStagingLogic_GettingGameInfoTitle_String; //0x566654
extern WONAPI::GUIString GameStagingLogic_GettingGameInfoDetails_String; //0x566480
extern WONAPI::GUIString GameStagingLogic_JoinGameErrorTitle_String; //0x566938
extern WONAPI::GUIString GameStagingLogic_VerifyingPingTitle_String; //0x566a14
extern WONAPI::GUIString GameStagingLogic_VerifyingPingDetails_String; //0x566ac8
extern WONAPI::GUIString GameStagingLogic_DissolveGame_String; //0x566800
extern WONAPI::GUIString GameStagingLogic_DissolveGameConfirm_String; //0x566700
extern WONAPI::GUIString GameStagingLogic_LeaveGame_String; //0x56662c
extern WONAPI::GUIString GameStagingLogic_LeaveGameConfirm_String; //0x566568
extern WONAPI::GUIString GameStagingLogic_NoQuickPlayGameTitle_String; //0x56654c
extern WONAPI::GUIString GameStagingLogic_NoQuickPlayGameDetails_String; //0x56684c
extern WONAPI::GUIString GameUserListCtrl_Ban_String; //0x56691c
extern WONAPI::GUIString GameUserListCtrl_Kick_String; //0x566430
extern WONAPI::SmartPtr<WONAPI::Image> GameUserListCtrl_Ready_Image; //0x5665c0
extern WONAPI::GUIString InternetQuickPlay_Welcome_String; //0x566ab8
extern WONAPI::GUIString InternetQuickPlay_UserWelcome_String; //0x566544
extern WONAPI::GUIString JoinResponse_Joined_String; //0x5669d0
extern WONAPI::GUIString JoinResponse_ServerFull_String; //0x5665b8
extern WONAPI::GUIString JoinResponse_InviteOnly_String; //0x566464
extern WONAPI::GUIString JoinResponse_AskToJoin_String; //0x5667b8
extern WONAPI::GUIString JoinResponse_GameInProgress_String; //0x56668c
extern WONAPI::GUIString JoinResponse_InvalidPassword_String; //0x566a50
extern WONAPI::GUIString JoinResponse_NotOnLAN_String; //0x5663f8
extern WONAPI::GUIString JoinResponse_Blocked_String; //0x5666f8
extern WONAPI::GUIString JoinResponse_Unknown_String; //0x566848
extern WONAPI::GUIString LANLogic_SocketBindError_String; //0x5663d8
extern WONAPI::GUIString LANLogic_SocketListenError_String; //0x566808
extern WONAPI::GUIString LANLogic_ConnectToGameTitle_String; //0x566904
extern WONAPI::GUIString LANLogic_ConnectToGameDetails_String; //0x5666dc
extern WONAPI::GUIString LANLogic_CheckingAddressTitle_String; //0x56690c
extern WONAPI::GUIString LANLogic_CheckingAddressDetails_String; //0x5668a0
extern WONAPI::GUIString LANLogic_RegisterTitle_String; //0x56699c
extern WONAPI::GUIString LANLogic_RegisterDetails_String; //0x5663fc
extern WONAPI::GUIString LANLogic_JoinFailure_String; //0x566a78
extern WONAPI::GUIString LANLogic_JoinReplyError_String; //0x566a44
extern WONAPI::GUIString LANLogic_InvalidSubnet_String; //0x5666c4
extern WONAPI::GUIString LobbyContainer_InitStart_String; //0x566888
extern WONAPI::GUIString LobbyContainer_InitFailed_String; //0x5667b0
extern WONAPI::GUIString LobbyContainer_CheckingVersion_String; //0x5668d0
extern WONAPI::GUIString LobbyContainer_QueryingServers_String; //0x5669ac
extern WONAPI::GUIString LobbyContainer_NoConnection_String; //0x5664a4
extern WONAPI::GUIString LobbyContainer_DNSLookupFailed_String; //0x56663c
extern WONAPI::GUIString LobbyContainer_DirLookupFailure_String; //0x5665fc
extern WONAPI::GUIString LobbyContainer_VersionCheckFailure_String; //0x56639c
extern WONAPI::GUIString LobbyContainer_InitComplete_String; //0x5669c0
extern WONAPI::GUIString LobbyContainer_NoServers_String; //0x566aac
extern WONAPI::GUIString LobbyContainer_AuthServers_String; //0x566788
extern WONAPI::GUIString LobbyContainer_AccountServers_String; //0x566660
extern WONAPI::GUIString LobbyContainer_VersionServers_String; //0x566724
extern WONAPI::GUIString LobbyContainer_CreateAccountTitle_String; //0x566924
extern WONAPI::GUIString LobbyContainer_CreateAccountDetails_String; //0x5669a8
extern WONAPI::GUIString LobbyContainer_RetrievePasswordTitle_String; //0x566720
extern WONAPI::GUIString LobbyContainer_RetrievePasswordDetails_String; //0x5663a0
extern WONAPI::GUIString LobbyContainer_RetrieveUsernameTitle_String; //0x566804
extern WONAPI::GUIString LobbyContainer_RetrieveUsernameDetails_String; //0x566824
extern WONAPI::GUIString LobbyContainer_QueryAccountTitle_String; //0x566510
extern WONAPI::GUIString LobbyContainer_QueryAccountDetails_String; //0x56664c
extern WONAPI::GUIString LobbyContainer_UpdateAccountTitle_String; //0x5665ec
extern WONAPI::GUIString LobbyContainer_UpdateAccountDetails_String; //0x566ae0
extern WONAPI::GUIString LobbyContainer_LoginTitle_String; //0x5667f4
extern WONAPI::GUIString LobbyContainer_LoginDetails_String; //0x566a10
extern WONAPI::GUIString LobbyContainer_GettingRoomsTitle_String; //0x566610
extern WONAPI::GUIString LobbyContainer_GettingRoomsDetails_String; //0x56696c
extern WONAPI::GUIString LobbyContainer_VersionOutOfDataTitle_String; //0x5668f0
extern WONAPI::GUIString LobbyContainer_VersionOutOfDataDetails_String; //0x5668ec
extern WONAPI::GUIString LobbyGameStatus_Success_String; //0x566550
extern WONAPI::GUIString LobbyGameStatus_GameFull_String; //0x566578
extern WONAPI::GUIString LobbyGameStatus_GameInProgress_String; //0x566810
extern WONAPI::GUIString LobbyGameStatus_UnpackFailure_String; //0x566870
extern WONAPI::GUIString LobbyGameStatus_NotInvited_String; //0x5665d8
extern WONAPI::GUIString LobbyGameStatus_CaptainRejectedYou_String; //0x566640
extern WONAPI::GUIString LobbyGameStatus_InvalidPassword_String; //0x56659c
extern WONAPI::GUIString LobbyGameStatus_DuplicateName_String; //0x56648c
extern WONAPI::GUIString LobbyOptions_DefaultName_String; //0x566488
extern WONAPI::GUIString LobbyOptions_CheckingAddress_String; //0x5666f0
extern WONAPI::GUIString LobbyOptions_AddressUnavailable_String; //0x566628
extern WONAPI::GUIString RoomCtrl_QueryingServer_String; //0x566838
extern WONAPI::SmartPtr<WONAPI::Image> RoomCtrl_Password_Image; //0x5664f4
extern WONAPI::GUIString RoomLabelCtrl_LabelPrefix_String; //0x5663b8
extern WONAPI::GUIString RoomLabelCtrl_LabelServer_String; //0x5664b0
extern WONAPI::GUIString RoomLabelCtrl_LabelUser_String; //0x566688
extern WONAPI::GUIString RoomLabelCtrl_LabelUsers_String; //0x5669ec
extern WONAPI::GUIString RoomLabelCtrl_LabelDisconnected_String; //0x566764
extern WONAPI::GUIString RoutingLogic_GetClientListTitle_String; //0x5669e4
extern WONAPI::GUIString RoutingLogic_GetClientListDetails_String; //0x566a3c
extern WONAPI::GUIString RoutingLogic_GetGroupListTitle_String; //0x566ab4
extern WONAPI::GUIString RoutingLogic_GetGroupListDetails_String; //0x566a1c
extern WONAPI::GUIString RoutingLogic_GameDissolvedTitle_String; //0x566470
extern WONAPI::GUIString RoutingLogic_GameDissolvedDetails_String; //0x566994
extern WONAPI::GUIString RoutingLogic_NoServersTitle_String; //0x566534
extern WONAPI::GUIString RoutingLogic_NoServersDetails_String; //0x5664e0
extern WONAPI::GUIString RoutingLogic_TooManyFriendsTitle_String; //0x5666b8
extern WONAPI::GUIString RoutingLogic_TooManyFriendsDetails_String; //0x566678
extern WONAPI::GUIString RoutingLogic_AlreadyJoinedTitle_String; //0x5667ac
extern WONAPI::GUIString RoutingLogic_AlreadyInGame_String; //0x566504
extern WONAPI::GUIString RoutingLogic_AlreadyInRoom_String; //0x56695c
extern WONAPI::GUIString RoutingLogic_JoinGroupTitle_String; //0x5667dc
extern WONAPI::GUIString RoutingLogic_JoinGroupDetails_String; //0x5663c0
extern WONAPI::GUIString RoutingLogic_ConnectTitle_String; //0x566490
extern WONAPI::GUIString RoutingLogic_ConnectDetails_String; //0x5664fc
extern WONAPI::GUIString RoutingLogic_RegisterTitle_String; //0x5669d8
extern WONAPI::GUIString RoutingLogic_RegisterDetails_String; //0x566618
extern WONAPI::GUIString RoutingLogic_QueryGroupsTitle_String; //0x566854
extern WONAPI::GUIString RoutingLogic_QueryGroupsDetails_String; //0x566650
extern WONAPI::GUIString RoutingLogic_CreateGroupTitle_String; //0x56651c
extern WONAPI::GUIString RoutingLogic_CreateGroupDetails_String; //0x5667a4
extern WONAPI::GUIString RoutingLogic_GetBadUsersTitle_String; //0x5664d8
extern WONAPI::GUIString RoutingLogic_GetBadUsersDetails_String; //0x566abc
extern WONAPI::GUIString RoutingLogic_DisconnectedTitle_String; //0x5666d0
extern WONAPI::GUIString RoutingLogic_DisconnectedDetails_String; //0x566a6c
extern WONAPI::GUIString RoutingLogic_AskingCaptainTitle_String; //0x5665a0
extern WONAPI::GUIString RoutingLogic_AskingCaptainDetails_String; //0x566a84
extern WONAPI::GUIString RoutingLogic_CollectingPingsTitle_String; //0x566884
extern WONAPI::GUIString RoutingLogic_CollectingPingsDetails_String; //0x566778
extern WONAPI::GUIString RoutingLogic_GettingGameListTitle_String; //0x56665c
extern WONAPI::GUIString RoutingLogic_GettingGameListDetails_String; //0x56673c
extern WONAPI::GUIString RoutingLogic_MuteClientTitle_String; //0x5667e8
extern WONAPI::GUIString RoutingLogic_UnmuteClientTitle_String; //0x566844
extern WONAPI::GUIString RoutingLogic_BanClientTitle_String; //0x566880
extern WONAPI::GUIString RoutingLogic_KickClientTitle_String; //0x5663c8
extern WONAPI::GUIString RoutingLogic_ModerateDetails_String; //0x566408
extern WONAPI::GUIString RoutingLogic_FriendNotFoundTitle_String; //0x5666c0
extern WONAPI::GUIString RoutingLogic_FriendNotFoundDetails_String; //0x566738
extern WONAPI::GUIString RoutingLogic_ClientNotFound_String; //0x566784
extern WONAPI::GUIString RoutingLogic_ClientAmbiguous_String; //0x566518
extern WONAPI::GUIString RoutingLogic_InvalidCommand_String; //0x5669b8
extern WONAPI::GUIString RoutingLogic_RequireName_String; //0x566590
extern WONAPI::GUIString RoutingLogic_BadTimeSpec_String; //0x566a24
extern WONAPI::GUIString RoutingLogic_Error_String; //0x56678c
extern WONAPI::GUIString RoutingLogic_NotCaptain_String; //0x566750
extern WONAPI::GUIString ServerListCtrl_Pinging_String; //0x56657c
extern WONAPI::GUIString ServerListCtrl_PingFailed_String; //0x56687c
extern WONAPI::GUIString ServerListCtrl_Querying_String; //0x566918
extern WONAPI::GUIString ServerListCtrl_QueryFailed_String; //0x566404
extern WONAPI::GUIString ServerListCtrl_ListSummary_String; //0x5665c4
extern WONAPI::GUIString ServerListCtrl_ListPingSummary_String; //0x566950
extern WONAPI::GUIString ServerListCtrl_RefreshList_String; //0x566868
extern WONAPI::GUIString ServerListCtrl_StopRefresh_String; //0x5664dc
extern WONAPI::GUIString ServerListCtrl_NoFilter_String; //0x56671c
extern WONAPI::GUIString ServerListCtrl_AddRemove_String; //0x566540
extern WONAPI::GUIString ServerListCtrl_Add_String; //0x566474
extern WONAPI::GUIString ServerListCtrl_Remove_String; //0x566670
extern WONAPI::GUIString ServerListCtrl_FullFilter_String; //0x566a38
extern WONAPI::GUIString ServerListCtrl_NotFullFilter_String; //0x566894
extern WONAPI::GUIString ServerListCtrl_OpenFilter_String; //0x5669cc
extern WONAPI::SmartPtr<WONAPI::Image> ServerListCtrl_PingFailed_Image; //0x5666bc
extern WONAPI::SmartPtr<WONAPI::Image> ServerListCtrl_GameExpanded_Image; //0x5664f8
extern WONAPI::SmartPtr<WONAPI::Image> ServerListCtrl_GameNotExpanded_Image; //0x566930
extern WONAPI::SmartPtr<WONAPI::Image> ServerListCtrl_GameAreInvited_Image; //0x5669e8
extern WONAPI::SmartPtr<WONAPI::Image> ServerListCtrl_GameNone_Image; //0x566a80
extern WONAPI::SmartPtr<WONAPI::Image> ServerListCtrl_GameAskToJoin_Image; //0x56645c
extern WONAPI::SmartPtr<WONAPI::Image> ServerListCtrl_GameHasPassword_Image; //0x5663e0
extern WONAPI::SmartPtr<WONAPI::Image> ServerListCtrl_GameNotInvited_Image; //0x5666fc
extern WONAPI::SmartPtr<WONAPI::Image> ServerListCtrl_GameInProgress_Image; //0x566830
extern WONAPI::SmartPtr<WONAPI::Image> ServerListCtrl_SkillOpen_Image; //0x566a7c
extern WONAPI::SmartPtr<WONAPI::Image> ServerListCtrl_SkillNovice_Image; //0x5663dc
extern WONAPI::SmartPtr<WONAPI::Image> ServerListCtrl_SkillIntermediate_Image; //0x56658c
extern WONAPI::SmartPtr<WONAPI::Image> ServerListCtrl_SkillAdvanced_Image; //0x5667cc
extern WONAPI::SmartPtr<WONAPI::Image> ServerListCtrl_SkillExpert_Image; //0x5668e8
extern WONAPI::GUIString ServerOptionsCtrl_Connect_String; //0x566524
extern WONAPI::GUIString ServerOptionsCtrl_RefreshServer_String; //0x566990
extern WONAPI::GUIString ServerOptionsCtrl_HideDetails_String; //0x56685c
extern WONAPI::GUIString ServerOptionsCtrl_ShowDetails_String; //0x5663ac
extern WONAPI::GUIString ServerOptionsCtrl_ResfreshList_String; //0x5666b4
extern WONAPI::GUIString StagingLogic_DisconnectedTitle_String; //0x5663b4
extern WONAPI::GUIString StagingLogic_DisconnectedDetails_String; //0x5669d4
extern WONAPI::GUIString StagingLogic_BannedTitle_String; //0x566434
extern WONAPI::GUIString StagingLogic_BannedDetails_String; //0x566850
extern WONAPI::GUIString StagingLogic_KickedTitle_String; //0x5667c8
extern WONAPI::GUIString StagingLogic_KickedDetails_String; //0x566624
extern WONAPI::GUIString UserListCtrl_Ignore_String; //0x56644c
extern WONAPI::GUIString UserListCtrl_Unignore_String; //0x566a40
extern WONAPI::SmartPtr<WONAPI::Image> UserListCtrl_Normal_Image; //0x566698
extern WONAPI::SmartPtr<WONAPI::Image> UserListCtrl_Away_Image; //0x566560
extern WONAPI::SmartPtr<WONAPI::Image> UserListCtrl_Admin_Image; //0x566aa8
extern WONAPI::SmartPtr<WONAPI::Image> UserListCtrl_Moderator_Image; //0x5665f4
extern WONAPI::SmartPtr<WONAPI::Image> UserListCtrl_Captain_Image; //0x566aa0
extern WONAPI::SmartPtr<WONAPI::Image> UserListCtrl_Muted_Image; //0x5665e8
extern WONAPI::SmartPtr<WONAPI::Image> UserListCtrl_Ignored_Image; //0x566a20
extern WONAPI::SmartPtr<WONAPI::Image> UserListCtrl_Blocked_Image; //0x5668cc
extern WONAPI::GUIString WONStatus_Success_String; //0x566584
extern WONAPI::GUIString WONStatus_Killed_String; //0x566a68
extern WONAPI::GUIString WONStatus_TimedOut_String; //0x566630
extern WONAPI::GUIString WONStatus_InvalidAddress_String; //0x566538
extern WONAPI::GUIString WONStatus_GeneralFailure_String; //0x5663b0
extern WONAPI::GUIString WONStatus_AuthFailure_String; //0x566a4c
extern WONAPI::GUIString WONStatus_FTPFailure_String; //0x56666c
extern WONAPI::GUIString WONStatus_HTTPFailure_String; //0x5665b0
extern WONAPI::GUIString WONStatus_MOTDFailure_String; //0x566a88
extern WONAPI::GUIString WONStatus_RoutingFailure_String; //0x5668b4
extern WONAPI::GUIString WONStatus_GameSpyFailure_String; //0x566554
extern WONAPI::GUIString WONStatus_DirFailure_String; //0x566754
extern WONAPI::GUIString WONStatus_ChatFailure_String; //0x5669c4
extern WONAPI::GUIString WONStatus_DatabaseFailure_String; //0x5668a8
extern WONAPI::GUIString WONStatus_NotAllowed_String; //0x5666d4
extern WONAPI::GUIString WONStatus_NotAuthorized_String; //0x566484
extern WONAPI::GUIString WONStatus_BehindFirewall_String; //0x566ac0
extern WONAPI::GUIString WONStatus_ExpiredCertificate_String; //0x5667f8
extern WONAPI::GUIString WONStatus_InvalidCDKey_String; //0x5663a4
extern WONAPI::GUIString WONStatus_CDKeyBanned_String; //0x566520
extern WONAPI::GUIString WONStatus_CDKeyInUse_String; //0x566514
extern WONAPI::GUIString WONStatus_CRCFailed_String; //0x56670c
extern WONAPI::GUIString WONStatus_UserExists_String; //0x566680
extern WONAPI::GUIString WONStatus_UserNotFound_String; //0x566978
extern WONAPI::GUIString WONStatus_BadPassword_String; //0x5667e0
extern WONAPI::GUIString WONStatus_InvalidUserName_String; //0x5664ac
extern WONAPI::GUIString WONStatus_BadCommunity_String; //0x566a60
extern WONAPI::GUIString WONStatus_NotInCommunity_String; //0x566454
extern WONAPI::GUIString WONStatus_UserSeqInUse_String; //0x56698c
extern WONAPI::GUIString WONStatus_BlankNamesNotAllowed_String; //0x566a94
extern WONAPI::GUIString WONStatus_CaptainRejectedYou_String; //0x566a8c
extern WONAPI::GUIString WONStatus_ClientAlreadyBanned_String; //0x5663a8
extern WONAPI::GUIString WONStatus_ClientAlreadyInGroup_String; //0x5665bc
extern WONAPI::GUIString WONStatus_ClientAlreadyMuted_String; //0x566420
extern WONAPI::GUIString WONStatus_ClientBanned_String; //0x566840
extern WONAPI::GUIString WONStatus_ClientDoesNotExist_String; //0x5663f0
extern WONAPI::GUIString WONStatus_ClientNotBanned_String; //0x5665e0
extern WONAPI::GUIString WONStatus_ClientNotInGroup_String; //0x566834
extern WONAPI::GUIString WONStatus_ClientNotInvited_String; //0x566508
extern WONAPI::GUIString WONStatus_ClientNotMuted_String; //0x566864
extern WONAPI::GUIString WONStatus_ConnectFailure_String; //0x5666ec
extern WONAPI::GUIString WONStatus_GroupAlreadyClaimed_String; //0x5664c4
extern WONAPI::GUIString WONStatus_GroupAlreadyExists_String; //0x5667ec
extern WONAPI::GUIString WONStatus_GroupClosed_String; //0x5665d4
extern WONAPI::GUIString WONStatus_GroupDeleted_String; //0x566910
extern WONAPI::GUIString WONStatus_GroupDoesNotExist_String; //0x56680c
extern WONAPI::GUIString WONStatus_GroupFull_String; //0x566798
extern WONAPI::GUIString WONStatus_GuestNamesReserved_String; //0x566570
extern WONAPI::GUIString WONStatus_InvalidPassword_String; //0x5669f0
extern WONAPI::GUIString WONStatus_MustBeAdmin_String; //0x5668bc
extern WONAPI::GUIString WONStatus_MustBeCaptain_String; //0x566ae8
extern WONAPI::GUIString WONStatus_MustBeCaptainOrModerator_String; //0x56643c
extern WONAPI::GUIString WONStatus_MustBeModerator_String; //0x566668
extern WONAPI::GUIString WONStatus_ServerFull_String; //0x566a54
extern WONAPI::GUIString WONStatus_GroupNameExceedsMaximum_String; //0x5664c0
extern WONAPI::GUIString WONStatus_JoinCanceled_String; //0x5664f0
extern WONAPI::GUIString WONStatus_Throttled_String; //0x566adc
extern WONAPI::GUIString WONStatus_KeyNotUsed_String; //0x5666e8
extern WONAPI::GUIString WONStatus_EMailPasswordDuplication_String; //0x56672c
extern WONAPI::GUIString WONStatus_AccountCreateDisabled_String; //0x5666b0
extern WONAPI::GUIString WONStatus_AgeNotOldEnough_String; //0x56655c
extern WONAPI::GUIString WONStatus_ItemHasDependencies_String; //0x566948
extern WONAPI::GUIString WONStatus_OutOfDate_String; //0x566928
extern WONAPI::GUIString WONStatus_OutOfDateNoUpdate_String; //0x566774
extern WONAPI::GUIString WONStatus_ValidNotLatest_String; //0x566728
extern WONAPI::GUIString WONStatus_DataAlreadyExists_String; //0x566564
extern WONAPI::GUIString WONStatus_DBError_String; //0x5664d4
extern WONAPI::GUIString WONStatus_ConnRefused_String; //0x5668f8

enum LobbyResourceCtrlId {
    LobbyResourceCtrlId_Min=200000, //actualSDK ControlId_UserMin; native firstID200001
    ID_ShowCreateAccount,
    ID_ShowLostPassword,
    ID_ShowLostUserName,
    ID_ShowMOTD,
    ID_ShowTOU,
    ID_LoginScreenBack,
    ID_ShowLoginHelp,
    ID_ShowInternetScreen,
    ID_ShowLanScreen,
    ID_Logout,
    ID_HostGame,
    ID_ChangeRoom,
    ID_JoinGame,
    ID_LeaveGame,
    ID_StartGame,
    ID_RefreshGameList,
    ID_AddFriend,
    ID_RefreshFriends,
    ID_AddLANName,
    ID_ChangeLANBroadcast,
    ID_FilterDirtyWordsCheck,
    ID_AnonymousToFriendsCheck,
    ID_ChooseNetworkAdapter,
};

enum LobbyResourceId {
    LobbyGlobal_Cancel_String_Id,
    LobbyGlobal_Lan_String_Id,
    LobbyGlobal_No_String_Id,
    LobbyGlobal_OK_String_Id,
    LobbyGlobal_Yes_String_Id,
    LobbyGlobal_QuickPlay_String_Id,
    LobbyGlobal_Searching_Image_Id,
    LobbyGlobal_Firewall_Image_Id,
    LobbyGlobal_ButtonClick_Sound_Id,
    LobbyGlobal_LobbyMusic_Sound_Id,
    LobbyGlobal_AllReady_Sound_Id,
    LobbyGlobal_AllNotReady_Sound_Id,
    LobbyGlobal_AskToJoinRecv_Sound_Id,
    LobbyGlobal_Error_Sound_Id,
    LobbyGlobal_FriendJoined_Sound_Id,
    LobbyGlobal_FriendLeft_Sound_Id,
    LobbyGlobal_InviteRecv_Sound_Id,
    LobbyGlobal_PlayerJoined_Sound_Id,
    LobbyGlobal_SierraLogon_Sound_Id,
    LobbyGlobal_WhisperRecv_Sound_Id,
    AccountLogic_InvalidCDKeyTitle_String_Id,
    AccountLogic_InvalidCDKeyDetails_String_Id,
    AddFriendDialog_StatusTitle_String_Id,
    AddFriendDialog_StatusDetails_String_Id,
    AdminActionDialog_Warn_String_Id,
    AdminActionDialog_Mute_String_Id,
    AdminActionDialog_UnMute_String_Id,
    AdminActionDialog_Kick_String_Id,
    AdminActionDialog_Ban_String_Id,
    AdminActionDialog_ExplanationOpt_String_Id,
    AdminActionDialog_ExplanationReq_String_Id,
    BadUserDialog_OneDay_String_Id,
    BadUserDialog_OneHour_String_Id,
    BadUserDialog_OneMinute_String_Id,
    BadUserDialog_OneSecond_String_Id,
    BadUserDialog_XDays_String_Id,
    BadUserDialog_XHours_String_Id,
    BadUserDialog_XMinutes_String_Id,
    BadUserDialog_XSeconds_String_Id,
    BadUserDialog_Infinite_String_Id,
    ChatCommandLogic_Whisper_String_Id,
    ChatCommandLogic_Block_String_Id,
    ChatCommandLogic_Emote_String_Id,
    ChatCommandLogic_Ignore_String_Id,
    ChatCommandLogic_Away_String_Id,
    ChatCommandLogic_Invite_String_Id,
    ChatCommandLogic_Uninvite_String_Id,
    ChatCommandLogic_Reply_String_Id,
    ChatCommandLogic_Clear_String_Id,
    ChatCommandLogic_Help_String_Id,
    ChatCommandLogic_ShowTeam_String_Id,
    ChatCommandLogic_Moderator_String_Id,
    ChatCommandLogic_ServerMute_String_Id,
    ChatCommandLogic_ServerBan_String_Id,
    ChatCommandLogic_Mute_String_Id,
    ChatCommandLogic_Ban_String_Id,
    ChatCommandLogic_Unmute_String_Id,
    ChatCommandLogic_ServerUnmute_String_Id,
    ChatCommandLogic_Unban_String_Id,
    ChatCommandLogic_ServerUnban_String_Id,
    ChatCommandLogic_Alert_String_Id,
    ChatCommandLogic_Warn_String_Id,
    ChatCommandLogic_HelpHeader_String_Id,
    ChatCommandLogic_HelpHelp_String_Id,
    ChatCommandLogic_ShowTeamHelp_String_Id,
    ChatCommandLogic_WhisperHelp_String_Id,
    ChatCommandLogic_BlockHelp_String_Id,
    ChatCommandLogic_IgnoreHelp_String_Id,
    ChatCommandLogic_ReplyHelp_String_Id,
    ChatCommandLogic_EmoteHelp_String_Id,
    ChatCommandLogic_AwayHelp_String_Id,
    ChatCommandLogic_ClearHelp_String_Id,
    ChatCommandLogic_InviteHelp_String_Id,
    ChatCommandLogic_UninviteHelp_String_Id,
    ChatCommandLogic_MuteHelp_String_Id,
    ChatCommandLogic_UnmuteHelp_String_Id,
    ChatCommandLogic_BanHelp_String_Id,
    ChatCommandLogic_UnbanHelp_String_Id,
    ChatCommandLogic_ModeratorHelp_String_Id,
    ChatCommandLogic_ServerMuteHelp_String_Id,
    ChatCommandLogic_ServerUnmuteHelp_String_Id,
    ChatCommandLogic_ServerBanHelp_String_Id,
    ChatCommandLogic_ServerUnbanHelp_String_Id,
    ChatCommandLogic_AlertHelp_String_Id,
    ChatCommandLogic_WarnHelp_String_Id,
    ChatCtrl_NormalChat_String_Id,
    ChatCtrl_Emote_String_Id,
    ChatCtrl_TeamChat_String_Id,
    ChatCtrl_TeamEmote_String_Id,
    ChatCtrl_YouWhisperToYourself_String_Id,
    ChatCtrl_YouWhisper_String_Id,
    ChatCtrl_SomeoneWhispersToYou_String_Id,
    ChatCtrl_YouWarnSomeone_String_Id,
    ChatCtrl_YouWarnYourself_String_Id,
    ChatCtrl_SomeoneWarnsYou_String_Id,
    ChatCtrl_SomeoneWarnsSomeone_String_Id,
    ChatCtrl_EnteringRoom_String_Id,
    ChatCtrl_LeavingRoom_String_Id,
    ChatCtrl_Disconnected_String_Id,
    ChatCtrl_YouWereKicked_String_Id,
    ChatCtrl_YouWereBannedMinute_String_Id,
    ChatCtrl_YouWereBannedMinutes_String_Id,
    ChatCtrl_YouWereBannedHour_String_Id,
    ChatCtrl_YouWereBannedHours_String_Id,
    ChatCtrl_YouWereBannedDay_String_Id,
    ChatCtrl_YouWereBannedDays_String_Id,
    ChatCtrl_YouWereBannedIndefinitely_String_Id,
    ChatCtrl_SomeoneWasKicked_String_Id,
    ChatCtrl_SomeoneWasBannedMinute_String_Id,
    ChatCtrl_SomeoneWasBannedMinutes_String_Id,
    ChatCtrl_SomeoneWasBannedHour_String_Id,
    ChatCtrl_SomeoneWasBannedHours_String_Id,
    ChatCtrl_SomeoneWasBannedDay_String_Id,
    ChatCtrl_SomeoneWasBannedDays_String_Id,
    ChatCtrl_SomeoneWasBannedIndefinitely_String_Id,
    ChatCtrl_YouWereUnmuted_String_Id,
    ChatCtrl_YouWereMutedMinute_String_Id,
    ChatCtrl_YouWereMutedMinutes_String_Id,
    ChatCtrl_YouWereMutedHour_String_Id,
    ChatCtrl_YouWereMutedHours_String_Id,
    ChatCtrl_YouWereMutedDay_String_Id,
    ChatCtrl_YouWereMutedDays_String_Id,
    ChatCtrl_YouWereMutedIndefinitely_String_Id,
    ChatCtrl_SomeoneWasUnmuted_String_Id,
    ChatCtrl_SomeoneWasMutedMinute_String_Id,
    ChatCtrl_SomeoneWasMutedMinutes_String_Id,
    ChatCtrl_SomeoneWasMutedHour_String_Id,
    ChatCtrl_SomeoneWasMutedHours_String_Id,
    ChatCtrl_SomeoneWasMutedDay_String_Id,
    ChatCtrl_SomeoneWasMutedDays_String_Id,
    ChatCtrl_SomeoneWasMutedIndefinitely_String_Id,
    ChatCtrl_ModeratorComment_String_Id,
    ChatCtrl_AlertComment_String_Id,
    ChatCtrl_ServerAlert_String_Id,
    ChatCtrl_ServerShutdownStartedMinute_String_Id,
    ChatCtrl_ServerShutdownStartedMinutes_String_Id,
    ChatCtrl_ServerShutdownStartedHour_String_Id,
    ChatCtrl_ServerShutdownStartedHours_String_Id,
    ChatCtrl_ServerShutdownStartedDay_String_Id,
    ChatCtrl_ServerShutdownStartedDays_String_Id,
    ChatCtrl_ServerShutdownAborted_String_Id,
    ChatCtrl_SomeoneInvitesYou_String_Id,
    ChatCtrl_SomeoneUninvitesYou_String_Id,
    ChatCtrl_YouInviteSomeone_String_Id,
    ChatCtrl_YouUninviteSomeone_String_Id,
    ChatCtrl_SomeoneInvitesYouExpl_String_Id,
    ChatCtrl_SomeoneUninvitesYouExpl_String_Id,
    ChatCtrl_YouInviteSomeoneExpl_String_Id,
    ChatCtrl_YouUninviteSomeoneExpl_String_Id,
    ChatCtrl_Accept_String_Id,
    ChatCtrl_Reject_String_Id,
    ChatCtrl_SomeoneAsksToJoin_String_Id,
    ChatCtrl_SomeoneAsksToJoinExpl_String_Id,
    ChatCtrl_ClientNotOnServer_String_Id,
    ChatCtrl_ClientInGame_String_Id,
    ChatCtrl_ClientInChat_String_Id,
    ChatCtrl_ClientInGameAndChat_String_Id,
    ChatCtrl_ClientIsAnonymous_String_Id,
    ChatCtrl_FriendIsChatting_String_Id,
    ChatCtrl_FriendIsInGameStaging_String_Id,
    ChatCtrl_FriendIsPlaying_String_Id,
    ChatCtrl_FriendIsOn_String_Id,
    ChatCtrl_FriendIsNotOnline_String_Id,
    ChatCtrl_PlayerKicked_String_Id,
    ChatCtrl_PlayerBanned_String_Id,
    ChatCtrl_FriendJoined_String_Id,
    ChatCtrl_FriendLeft_String_Id,
    ChatCtrl_TeamMemberJoined_String_Id,
    ChatCtrl_TeamMemberLeft_String_Id,
    ChatCtrl_TeamMembers_String_Id,
    ChatCtrl_IgnoreList_String_Id,
    ChooseNetworkAdapterDialog_Default_String_Id,
    ChooseNetworkAdapterDialog_Unknown_String_Id,
    ClientOptionsCtrl_Whisper_String_Id,
    ClientOptionsCtrl_Invite_String_Id,
    ClientOptionsCtrl_Uninvite_String_Id,
    ClientOptionsCtrl_AddToFriends_String_Id,
    ClientOptionsCtrl_RemoveFromFriends_String_Id,
    ClientOptionsCtrl_ClearAFK_String_Id,
    ClientOptionsCtrl_SetAFK_String_Id,
    ClientOptionsCtrl_Ignore_String_Id,
    ClientOptionsCtrl_Unignore_String_Id,
    ClientOptionsCtrl_Block_String_Id,
    ClientOptionsCtrl_Unblock_String_Id,
    ClientOptionsCtrl_Warn_String_Id,
    ClientOptionsCtrl_Mute_String_Id,
    ClientOptionsCtrl_UnMute_String_Id,
    ClientOptionsCtrl_Kick_String_Id,
    ClientOptionsCtrl_Ban_String_Id,
    DirectConnectionOptionsCtrl_Connect_String_Id,
    DirectConnectionOptionsCtrl_Refresh_String_Id,
    DirectConnectionOptionsCtrl_Remove_String_Id,
    FriendOptionsCtrl_Locate_String_Id,
    FriendOptionsCtrl_RefreshList_String_Id,
    FriendOptionsCtrl_RemoveFromFriends_String_Id,
    FriendsListCtrl_Unknown_Image_Id,
    FriendsListCtrl_NotFound_Image_Id,
    FriendsListCtrl_InChat_Image_Id,
    FriendsListCtrl_InGame_Image_Id,
    GameOptionsPopup_Join_String_Id,
    GameOptionsPopup_Ping_String_Id,
    GameOptionsPopup_QueryDetails_String_Id,
    GameOptionsPopup_HideDetails_String_Id,
    GameOptionsPopup_ShowDetails_String_Id,
    GameOptionsPopup_Remove_String_Id,
    GameStagingCtrl_Ready_String_Id,
    GameStagingCtrl_NotReady_String_Id,
    GameStagingCtrl_StartGame_String_Id,
    GameStagingCtrl_LeaveGame_String_Id,
    GameStagingCtrl_DissolveGame_String_Id,
    GameStagingLogic_KickedTitle_String_Id,
    GameStagingLogic_KickedDetails_String_Id,
    GameStagingLogic_BannedTitle_String_Id,
    GameStagingLogic_BannedDetails_String_Id,
    GameStagingLogic_ConnectWarningTitle1_String_Id,
    GameStagingLogic_ConnectWarningDetails1_String_Id,
    GameStagingLogic_ConnectWarningTitle2_String_Id,
    GameStagingLogic_ConnectWarningDetails2_String_Id,
    GameStagingLogic_ConnectWarningTitle3_String_Id,
    GameStagingLogic_ConnectWarningDetails3_String_Id,
    GameStagingLogic_NoPingReplyError_String_Id,
    GameStagingLogic_GettingGameInfoTitle_String_Id,
    GameStagingLogic_GettingGameInfoDetails_String_Id,
    GameStagingLogic_JoinGameErrorTitle_String_Id,
    GameStagingLogic_VerifyingPingTitle_String_Id,
    GameStagingLogic_VerifyingPingDetails_String_Id,
    GameStagingLogic_DissolveGame_String_Id,
    GameStagingLogic_DissolveGameConfirm_String_Id,
    GameStagingLogic_LeaveGame_String_Id,
    GameStagingLogic_LeaveGameConfirm_String_Id,
    GameStagingLogic_NoQuickPlayGameTitle_String_Id,
    GameStagingLogic_NoQuickPlayGameDetails_String_Id,
    GameUserListCtrl_Ban_String_Id,
    GameUserListCtrl_Kick_String_Id,
    GameUserListCtrl_Ready_Image_Id,
    InternetQuickPlay_Welcome_String_Id,
    InternetQuickPlay_UserWelcome_String_Id,
    JoinResponse_Joined_String_Id,
    JoinResponse_ServerFull_String_Id,
    JoinResponse_InviteOnly_String_Id,
    JoinResponse_AskToJoin_String_Id,
    JoinResponse_GameInProgress_String_Id,
    JoinResponse_InvalidPassword_String_Id,
    JoinResponse_NotOnLAN_String_Id,
    JoinResponse_Blocked_String_Id,
    JoinResponse_Unknown_String_Id,
    LANLogic_SocketBindError_String_Id,
    LANLogic_SocketListenError_String_Id,
    LANLogic_ConnectToGameTitle_String_Id,
    LANLogic_ConnectToGameDetails_String_Id,
    LANLogic_CheckingAddressTitle_String_Id,
    LANLogic_CheckingAddressDetails_String_Id,
    LANLogic_RegisterTitle_String_Id,
    LANLogic_RegisterDetails_String_Id,
    LANLogic_JoinFailure_String_Id,
    LANLogic_JoinReplyError_String_Id,
    LANLogic_InvalidSubnet_String_Id,
    LobbyContainer_InitStart_String_Id,
    LobbyContainer_InitFailed_String_Id,
    LobbyContainer_CheckingVersion_String_Id,
    LobbyContainer_QueryingServers_String_Id,
    LobbyContainer_NoConnection_String_Id,
    LobbyContainer_DNSLookupFailed_String_Id,
    LobbyContainer_DirLookupFailure_String_Id,
    LobbyContainer_VersionCheckFailure_String_Id,
    LobbyContainer_InitComplete_String_Id,
    LobbyContainer_NoServers_String_Id,
    LobbyContainer_AuthServers_String_Id,
    LobbyContainer_AccountServers_String_Id,
    LobbyContainer_VersionServers_String_Id,
    LobbyContainer_CreateAccountTitle_String_Id,
    LobbyContainer_CreateAccountDetails_String_Id,
    LobbyContainer_RetrievePasswordTitle_String_Id,
    LobbyContainer_RetrievePasswordDetails_String_Id,
    LobbyContainer_RetrieveUsernameTitle_String_Id,
    LobbyContainer_RetrieveUsernameDetails_String_Id,
    LobbyContainer_QueryAccountTitle_String_Id,
    LobbyContainer_QueryAccountDetails_String_Id,
    LobbyContainer_UpdateAccountTitle_String_Id,
    LobbyContainer_UpdateAccountDetails_String_Id,
    LobbyContainer_LoginTitle_String_Id,
    LobbyContainer_LoginDetails_String_Id,
    LobbyContainer_GettingRoomsTitle_String_Id,
    LobbyContainer_GettingRoomsDetails_String_Id,
    LobbyContainer_VersionOutOfDataTitle_String_Id,
    LobbyContainer_VersionOutOfDataDetails_String_Id,
    LobbyGameStatus_Success_String_Id,
    LobbyGameStatus_GameFull_String_Id,
    LobbyGameStatus_GameInProgress_String_Id,
    LobbyGameStatus_UnpackFailure_String_Id,
    LobbyGameStatus_NotInvited_String_Id,
    LobbyGameStatus_CaptainRejectedYou_String_Id,
    LobbyGameStatus_InvalidPassword_String_Id,
    LobbyGameStatus_DuplicateName_String_Id,
    LobbyOptions_DefaultName_String_Id,
    LobbyOptions_CheckingAddress_String_Id,
    LobbyOptions_AddressUnavailable_String_Id,
    RoomCtrl_QueryingServer_String_Id,
    RoomCtrl_Password_Image_Id,
    RoomLabelCtrl_LabelPrefix_String_Id,
    RoomLabelCtrl_LabelServer_String_Id,
    RoomLabelCtrl_LabelUser_String_Id,
    RoomLabelCtrl_LabelUsers_String_Id,
    RoomLabelCtrl_LabelDisconnected_String_Id,
    RoutingLogic_GetClientListTitle_String_Id,
    RoutingLogic_GetClientListDetails_String_Id,
    RoutingLogic_GetGroupListTitle_String_Id,
    RoutingLogic_GetGroupListDetails_String_Id,
    RoutingLogic_GameDissolvedTitle_String_Id,
    RoutingLogic_GameDissolvedDetails_String_Id,
    RoutingLogic_NoServersTitle_String_Id,
    RoutingLogic_NoServersDetails_String_Id,
    RoutingLogic_TooManyFriendsTitle_String_Id,
    RoutingLogic_TooManyFriendsDetails_String_Id,
    RoutingLogic_AlreadyJoinedTitle_String_Id,
    RoutingLogic_AlreadyInGame_String_Id,
    RoutingLogic_AlreadyInRoom_String_Id,
    RoutingLogic_JoinGroupTitle_String_Id,
    RoutingLogic_JoinGroupDetails_String_Id,
    RoutingLogic_ConnectTitle_String_Id,
    RoutingLogic_ConnectDetails_String_Id,
    RoutingLogic_RegisterTitle_String_Id,
    RoutingLogic_RegisterDetails_String_Id,
    RoutingLogic_QueryGroupsTitle_String_Id,
    RoutingLogic_QueryGroupsDetails_String_Id,
    RoutingLogic_CreateGroupTitle_String_Id,
    RoutingLogic_CreateGroupDetails_String_Id,
    RoutingLogic_GetBadUsersTitle_String_Id,
    RoutingLogic_GetBadUsersDetails_String_Id,
    RoutingLogic_DisconnectedTitle_String_Id,
    RoutingLogic_DisconnectedDetails_String_Id,
    RoutingLogic_AskingCaptainTitle_String_Id,
    RoutingLogic_AskingCaptainDetails_String_Id,
    RoutingLogic_CollectingPingsTitle_String_Id,
    RoutingLogic_CollectingPingsDetails_String_Id,
    RoutingLogic_GettingGameListTitle_String_Id,
    RoutingLogic_GettingGameListDetails_String_Id,
    RoutingLogic_MuteClientTitle_String_Id,
    RoutingLogic_UnmuteClientTitle_String_Id,
    RoutingLogic_BanClientTitle_String_Id,
    RoutingLogic_KickClientTitle_String_Id,
    RoutingLogic_ModerateDetails_String_Id,
    RoutingLogic_FriendNotFoundTitle_String_Id,
    RoutingLogic_FriendNotFoundDetails_String_Id,
    RoutingLogic_ClientNotFound_String_Id,
    RoutingLogic_ClientAmbiguous_String_Id,
    RoutingLogic_InvalidCommand_String_Id,
    RoutingLogic_RequireName_String_Id,
    RoutingLogic_BadTimeSpec_String_Id,
    RoutingLogic_Error_String_Id,
    RoutingLogic_NotCaptain_String_Id,
    ServerListCtrl_Pinging_String_Id,
    ServerListCtrl_PingFailed_String_Id,
    ServerListCtrl_Querying_String_Id,
    ServerListCtrl_QueryFailed_String_Id,
    ServerListCtrl_ListSummary_String_Id,
    ServerListCtrl_ListPingSummary_String_Id,
    ServerListCtrl_RefreshList_String_Id,
    ServerListCtrl_StopRefresh_String_Id,
    ServerListCtrl_NoFilter_String_Id,
    ServerListCtrl_AddRemove_String_Id,
    ServerListCtrl_Add_String_Id,
    ServerListCtrl_Remove_String_Id,
    ServerListCtrl_FullFilter_String_Id,
    ServerListCtrl_NotFullFilter_String_Id,
    ServerListCtrl_OpenFilter_String_Id,
    ServerListCtrl_PingFailed_Image_Id,
    ServerListCtrl_GameExpanded_Image_Id,
    ServerListCtrl_GameNotExpanded_Image_Id,
    ServerListCtrl_GameAreInvited_Image_Id,
    ServerListCtrl_GameNone_Image_Id,
    ServerListCtrl_GameAskToJoin_Image_Id,
    ServerListCtrl_GameHasPassword_Image_Id,
    ServerListCtrl_GameNotInvited_Image_Id,
    ServerListCtrl_GameInProgress_Image_Id,
    ServerListCtrl_SkillOpen_Image_Id,
    ServerListCtrl_SkillNovice_Image_Id,
    ServerListCtrl_SkillIntermediate_Image_Id,
    ServerListCtrl_SkillAdvanced_Image_Id,
    ServerListCtrl_SkillExpert_Image_Id,
    ServerOptionsCtrl_Connect_String_Id,
    ServerOptionsCtrl_RefreshServer_String_Id,
    ServerOptionsCtrl_HideDetails_String_Id,
    ServerOptionsCtrl_ShowDetails_String_Id,
    ServerOptionsCtrl_ResfreshList_String_Id,
    StagingLogic_DisconnectedTitle_String_Id,
    StagingLogic_DisconnectedDetails_String_Id,
    StagingLogic_BannedTitle_String_Id,
    StagingLogic_BannedDetails_String_Id,
    StagingLogic_KickedTitle_String_Id,
    StagingLogic_KickedDetails_String_Id,
    UserListCtrl_Ignore_String_Id,
    UserListCtrl_Unignore_String_Id,
    UserListCtrl_Normal_Image_Id,
    UserListCtrl_Away_Image_Id,
    UserListCtrl_Admin_Image_Id,
    UserListCtrl_Moderator_Image_Id,
    UserListCtrl_Captain_Image_Id,
    UserListCtrl_Muted_Image_Id,
    UserListCtrl_Ignored_Image_Id,
    UserListCtrl_Blocked_Image_Id,
    WONStatus_Success_String_Id,
    WONStatus_Killed_String_Id,
    WONStatus_TimedOut_String_Id,
    WONStatus_InvalidAddress_String_Id,
    WONStatus_GeneralFailure_String_Id,
    WONStatus_AuthFailure_String_Id,
    WONStatus_FTPFailure_String_Id,
    WONStatus_HTTPFailure_String_Id,
    WONStatus_MOTDFailure_String_Id,
    WONStatus_RoutingFailure_String_Id,
    WONStatus_GameSpyFailure_String_Id,
    WONStatus_DirFailure_String_Id,
    WONStatus_ChatFailure_String_Id,
    WONStatus_DatabaseFailure_String_Id,
    WONStatus_NotAllowed_String_Id,
    WONStatus_NotAuthorized_String_Id,
    WONStatus_BehindFirewall_String_Id,
    WONStatus_ExpiredCertificate_String_Id,
    WONStatus_InvalidCDKey_String_Id,
    WONStatus_CDKeyBanned_String_Id,
    WONStatus_CDKeyInUse_String_Id,
    WONStatus_CRCFailed_String_Id,
    WONStatus_UserExists_String_Id,
    WONStatus_UserNotFound_String_Id,
    WONStatus_BadPassword_String_Id,
    WONStatus_InvalidUserName_String_Id,
    WONStatus_BadCommunity_String_Id,
    WONStatus_NotInCommunity_String_Id,
    WONStatus_UserSeqInUse_String_Id,
    WONStatus_BlankNamesNotAllowed_String_Id,
    WONStatus_CaptainRejectedYou_String_Id,
    WONStatus_ClientAlreadyBanned_String_Id,
    WONStatus_ClientAlreadyInGroup_String_Id,
    WONStatus_ClientAlreadyMuted_String_Id,
    WONStatus_ClientBanned_String_Id,
    WONStatus_ClientDoesNotExist_String_Id,
    WONStatus_ClientNotBanned_String_Id,
    WONStatus_ClientNotInGroup_String_Id,
    WONStatus_ClientNotInvited_String_Id,
    WONStatus_ClientNotMuted_String_Id,
    WONStatus_ConnectFailure_String_Id,
    WONStatus_GroupAlreadyClaimed_String_Id,
    WONStatus_GroupAlreadyExists_String_Id,
    WONStatus_GroupClosed_String_Id,
    WONStatus_GroupDeleted_String_Id,
    WONStatus_GroupDoesNotExist_String_Id,
    WONStatus_GroupFull_String_Id,
    WONStatus_GuestNamesReserved_String_Id,
    WONStatus_InvalidPassword_String_Id,
    WONStatus_MustBeAdmin_String_Id,
    WONStatus_MustBeCaptain_String_Id,
    WONStatus_MustBeCaptainOrModerator_String_Id,
    WONStatus_MustBeModerator_String_Id,
    WONStatus_ServerFull_String_Id,
    WONStatus_GroupNameExceedsMaximum_String_Id,
    WONStatus_JoinCanceled_String_Id,
    WONStatus_Throttled_String_Id,
    WONStatus_KeyNotUsed_String_Id,
    WONStatus_EMailPasswordDuplication_String_Id,
    WONStatus_AccountCreateDisabled_String_Id,
    WONStatus_AgeNotOldEnough_String_Id,
    WONStatus_ItemHasDependencies_String_Id,
    WONStatus_OutOfDate_String_Id,
    WONStatus_OutOfDateNoUpdate_String_Id,
    WONStatus_ValidNotLatest_String_Id,
    WONStatus_DataAlreadyExists_String_Id,
    WONStatus_DBError_String_Id,
    WONStatus_ConnRefused_String_Id,
};

WONAPI::ResourceConfigTablePtr LobbyResource_Init(WONAPI::ResourceConfigTable* = NULL);
const WONAPI::GUIString& LobbyResource_GetString(LobbyResourceId);
WONAPI::Image* LobbyResource_GetImage(LobbyResourceId);
WONAPI::Sound* LobbyResource_GetSound(LobbyResourceId);
WONAPI::Font* LobbyResource_GetFont(LobbyResourceId);
const WONAPI::Background& LobbyResource_GetBackground(LobbyResourceId);
DWORD LobbyResource_GetColor(LobbyResourceId);
int LobbyResource_GetInt(LobbyResourceId);
void LobbyResource_SafeGlueResources(WONAPI::ResourceConfigTable*);
void LobbyResource_UnloadResources();

#endif
