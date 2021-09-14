#include "Setup.hpp"
#include "Hooks.hpp"
#include "Render.hpp"
#include "Features/Visuals/Chams.hpp"

#include "Multithread/threading.h"
#include "Tools/Tools.hpp"

void C_SetUp::Instance( HINSTANCE hInstance )
{
	while ( !GetModuleHandleA( _S( "serverbrowser.dll" ) ) )
		Sleep( 100 );

	// init sdk
	SetupModuleList( );
	SetupInterfaceList( );
	SetupAddressList( );
	SetupConvarList( );
	SetupImGui( );

	// привет от детей шлюх модул€р сука
	MultiThread::InitThreads( );

	// setup menu
	g_Menu->Initialize( );

	// setup render
	g_Render->Initialize( );

	// create materials
	g_Chams->CreateMaterials( );

	// scan netvars
	g_NetvarManager->Instance( );

	// setup hooks
	SetupHooks( );

	// get out of thread
	return ExitThread( EXIT_SUCCESS );
}

#define CREATE_HOOK( Address, Function, Original ) if ( MH_CreateHook( ( LPVOID )( Address ), ( LPVOID )( Function ), reinterpret_cast< void** >( &Original ) ) ) MessageBoxA( NULL, _S( #Function ), _S( #Function ), 64 ); 
void C_SetUp::SetupHooks( )
{
	if ( MH_Initialize( ) != MH_OK )
		return;
	
	LPVOID pPresent						= ( LPVOID )( ( *( uintptr_t** )( g_Globals.m_Interfaces.m_DirectDevice ) )[ 17 ] );
	LPVOID pReset						= ( LPVOID )( ( *( uintptr_t** )( g_Globals.m_Interfaces.m_DirectDevice ) )[ 16 ] );
	LPVOID pInPrediction				= ( LPVOID )( ( *( uintptr_t** )( g_Globals.m_Interfaces.m_Prediction ) )[ 14 ] );
	LPVOID pIsHLTV						= ( LPVOID )( ( *( uintptr_t** )( g_Globals.m_Interfaces.m_EngineClient ) )[ 93 ] );
	LPVOID pIsPaused					= ( LPVOID )( ( *( uintptr_t** )( g_Globals.m_Interfaces.m_EngineClient ) )[ 90 ] );
	LPVOID pFireEvents					= ( LPVOID )( ( *( uintptr_t** )( g_Globals.m_Interfaces.m_EngineClient ) )[ 59 ] );
	LPVOID pIsConnected					= ( LPVOID )( ( *( uintptr_t** )( g_Globals.m_Interfaces.m_EngineClient ) )[ 27 ] );
	LPVOID pGetAspectRatio				= ( LPVOID )( ( *( uintptr_t** )( g_Globals.m_Interfaces.m_EngineClient ) )[ 101 ] );
	LPVOID pListLeavesInBox				= ( LPVOID )( ( *( uintptr_t** )( g_Globals.m_Interfaces.m_EngineClient->GetBSPTreeQuery( ) ) )[ 6 ] );
	LPVOID pLockCursor					= ( LPVOID )( ( *( uintptr_t** )( g_Globals.m_Interfaces.m_Surface ) )[ 67 ] );
	LPVOID pDispatchUserMessage			= ( LPVOID )( ( *( uintptr_t** )( g_Globals.m_Interfaces.m_CHLClient ) )[ 38 ] );
	LPVOID pWriteUsercmdDeltaToBuffer	= ( LPVOID )( ( *( uintptr_t** )( g_Globals.m_Interfaces.m_CHLClient ) )[ 24 ] );
	LPVOID pPaintTraverse				= ( LPVOID )( ( *( uintptr_t** )( g_Globals.m_Interfaces.m_VGUIPanel ) )[ 41 ] );
	LPVOID pProcessMovement				= ( LPVOID )( ( *( uintptr_t** )( g_Globals.m_Interfaces.m_GameMovement ) )[ 1 ] );
	LPVOID pDrawModelExecute_Studio		= ( LPVOID )( ( *( uintptr_t** )( g_Globals.m_Interfaces.m_StudioRender ) )[ 29 ] );
	LPVOID pDrawModelExecute_Model		= ( LPVOID )( ( *( uintptr_t** )( g_Globals.m_Interfaces.m_ModelRender ) )[ 21 ] );
	LPVOID pSvCheats_GetBool			= ( LPVOID )( ( *( uintptr_t** )( g_Globals.m_Interfaces.m_CVar->FindVar( _S( "sv_cheats" ) ) ) )[ 13 ] );
	LPVOID pClDoResetLatch_GetBool		= ( LPVOID )( ( *( uintptr_t** )( g_Globals.m_Interfaces.m_CVar->FindVar( _S( "cl_pred_doresetlatch" ) ) ) )[ 13 ] );

	LPVOID pCL_Move						= g_Tools->FindPattern( g_Globals.m_ModuleList.m_EngineDll, _S( "55 8B EC 81 EC ? ? ? ? 53 56 57 8B 3D ? ? ? ? 8A" ) );
	LPVOID pCreateMove					= g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "55 8B EC 83 EC 08 FF 15 ? ? ? ? 84" ) );
	LPVOID pFrameStageNotify			= g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "55 8B EC 8B 0D ? ? ? ? 8B 01 8B 80 ? ? ? ? FF D0 A2" ) );
	LPVOID pPacketStart					= g_Tools->FindPattern( g_Globals.m_ModuleList.m_EngineDll, _S( "56 8B F1 E8 ? ? ? ? 8B 8E ? ? ? ? 3B 8E ? ? ? ?" ) ) - 0x20;
	LPVOID pPacketEnd					= g_Tools->FindPattern( g_Globals.m_ModuleList.m_EngineDll, _S( "56 8B F1 E8 ? ? ? ? 8B 8E ? ? ? ? 3B 8E ? ? ? ?" ) );
	LPVOID pModifyEyePosition			= g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "55 8B EC 83 E4 F8 83 EC 5C 53 8B D9 56 57 83" ) );
	LPVOID pDoExtraBoneProcessing		= g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 56 8B F1 57 89 74 24 1C" ) );
	LPVOID pTraceFilterForHeadCollision	= g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "55 8B EC 56 8B 75 0C 57 8B F9 F7 C6 ? ? ? ?" ) );
	LPVOID pSetupBones					= g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "55 8B EC 83 E4 F0 B8 D8" ) );
	LPVOID pUpdateClientSideAnimation	= g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "55 8B EC 51 56 8B F1 80 BE ? ? ? ? ? 74" ) );
	LPVOID pStandardBlendingRules		= g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "55 8B EC 83 E4 F0 B8 F8 10" ) );
	LPVOID pSetupAliveLoop				= g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "55 8B EC 51 56 8B 71 60 83 BE 8C 29 00 00 00 0F 84 93" ) );
	LPVOID pSetupVelocity				= g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "55 8B EC 83 E4 F8 83 EC 30 56 57 8B 3D" ) );
	LPVOID pPhysicsSimulate				= g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "56 8B F1 8B 8E ? ? ? ? 83 F9 FF 74 23" ) );
	LPVOID pDoPostScreenEffects			= g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "55 8B EC 8B 49 18 56 8B" ) );
	LPVOID pOverrideView				= g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "55 8B EC 83 E4 F8 83 EC 58 56 57 8B 3D ? ? ? ? 85" ) );
	LPVOID pGetViewmodelFOV				= g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "55 8B EC 8B 0D ? ? ? ? 83 EC 08 57" ) );
	LPVOID pSendNetMessage				= g_Tools->FindPattern( g_Globals.m_ModuleList.m_EngineDll, _S( "55 8B EC 83 EC 08 56 8B F1 8B 86 ? ? ? ? 85" ) );
	LPVOID pCalcView					= g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "55 8B EC 83 EC 14 53 56 57 FF 75 18" ) );
	LPVOID pGetColorModulation			= g_Tools->FindPattern( g_Globals.m_ModuleList.m_MatSysDll, _S( "55 8B EC 83 EC ? 56 8B F1 8A 46" ) );
	LPVOID pGetAlphaModulation			= g_Tools->FindPattern( g_Globals.m_ModuleList.m_MatSysDll, _S( "56 8B F1 8A 46 20 C0 E8 02 A8 01 75 0B 6A 00 6A 00 6A 00 E8 ? ? ? ? 80 7E 22 05 76 0E" ) );
	LPVOID pShouldDrawFOG				= g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "55 8B EC 8B 0D ? ? ? ? 83 EC 0C 8B 01 53 56 57 FF" ) );
	LPVOID pGetClientModelRenderable	= g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "56 8B F1 80 BE F8 26" ) );
	LPVOID pIsUsingDebugStaticProps		= g_Tools->FindPattern( g_Globals.m_ModuleList.m_EngineDll, _S( "8B 0D ? ? ? ? 81 F9 ? ? ? ? 75 ? A1 ? ? ? ? 35 ? ? ? ? EB ? 8B 01 FF 50 ? 83 F8 ? 0F 85 ? ? ? ? 8B 0D" ) );
	LPVOID pClReadPackets				= g_Tools->FindPattern( g_Globals.m_ModuleList.m_EngineDll, _S( "53 8A D9 8B 0D A0 B5 43 10 56" ) );
	LPVOID pHostShouldRun				= g_Tools->FindPattern( g_Globals.m_ModuleList.m_EngineDll, _S( "8B 0D ?? ?? ?? ?? 81 F9 ?? ?? ?? ?? 75 0C A1 ?? ?? ?? ?? 35 ?? ?? ?? ?? EB 05 8B 01 FF 50 34 85 C0 75 03 B0" ) );
	LPVOID pPerformScreenOverlay		= g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "55 8B EC 51 A1 ? ? ? ? 53 56 8B D9" ) );
	LPVOID pCalcViewmodelBob			= g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "55 8B EC A1 ? ? ? ? 83 EC 10 56 8B F1 B9" ) );
	LPVOID pViewmodel_Interpolate		= g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "55 8B EC 83 E4 F8 83 EC 0C 53 56 8B F1 57 83 BE 4C" ) );

	CREATE_HOOK( pPresent						,		C_Hooks::hkPresent						,		g_Globals.m_Hooks.m_Originals.m_Present						);
	CREATE_HOOK( pReset							,		C_Hooks::hkReset						,		g_Globals.m_Hooks.m_Originals.m_Reset						);
	CREATE_HOOK( pSvCheats_GetBool				,		C_Hooks::hkSvCheats_GetBool				,		g_Globals.m_Hooks.m_Originals.m_SvCheats_GetBool			);
	CREATE_HOOK( pClDoResetLatch_GetBool		,		C_Hooks::hkClDoResetLatch_GetBool		,		g_Globals.m_Hooks.m_Originals.m_ClDoResetLatch_GetBool		);
	CREATE_HOOK( pInPrediction					,		C_Hooks::hkInPrediction					,		g_Globals.m_Hooks.m_Originals.m_InPrediction				);
	CREATE_HOOK( pIsHLTV						,		C_Hooks::hkIsHLTV						,		g_Globals.m_Hooks.m_Originals.m_IsHLTV						);
	CREATE_HOOK( pIsPaused						,		C_Hooks::hkIsPaused						,		g_Globals.m_Hooks.m_Originals.m_IsPaused					);
	CREATE_HOOK( pFireEvents					,		C_Hooks::hkFireEvents					,		g_Globals.m_Hooks.m_Originals.m_FireEvents					);
	CREATE_HOOK( pCL_Move						,		C_Hooks::hkCL_Move						,		g_Globals.m_Hooks.m_Originals.m_CL_Move						);
	CREATE_HOOK( pCreateMove					,		C_Hooks::hkCreateMove_Proxy				,		g_Globals.m_Hooks.m_Originals.m_CreateMove					);
	CREATE_HOOK( pFrameStageNotify				,		C_Hooks::hkFrameStageNotify				,		g_Globals.m_Hooks.m_Originals.m_FrameStageNotify			);
	CREATE_HOOK( pPacketStart					,		C_Hooks::hkPacketStart					,		g_Globals.m_Hooks.m_Originals.m_PacketStart					);
	CREATE_HOOK( pPacketEnd						,		C_Hooks::hkPacketEnd					,		g_Globals.m_Hooks.m_Originals.m_PacketEnd					);
	CREATE_HOOK( pModifyEyePosition				,		C_Hooks::hkModifyEyePosition			,		g_Globals.m_Hooks.m_Originals.m_ModifyEyePosition			);
	CREATE_HOOK( pDoExtraBoneProcessing			,		C_Hooks::hkDoExtraBoneProcessing		,		g_Globals.m_Hooks.m_Originals.m_DoExtraBoneProcessing		);
	CREATE_HOOK( pLockCursor					,		C_Hooks::hkLockCursor					,		g_Globals.m_Hooks.m_Originals.m_LockCursor					);
	CREATE_HOOK( pPaintTraverse					,		C_Hooks::hkPaintTraverse				,		g_Globals.m_Hooks.m_Originals.m_PaintTraverse				);
	CREATE_HOOK( pTraceFilterForHeadCollision	,		C_Hooks::hkTraceFilterForHeadCollision	,		g_Globals.m_Hooks.m_Originals.m_TraceFilterForHeadCollision	);
	CREATE_HOOK( pProcessMovement				,		C_Hooks::hkProcessMovement				,		g_Globals.m_Hooks.m_Originals.m_ProcessMovement				);
	CREATE_HOOK( pSetupBones					,		C_Hooks::hkSetupBones					,		g_Globals.m_Hooks.m_Originals.m_SetupBones					);
	CREATE_HOOK( pUpdateClientSideAnimation		,		C_Hooks::hkUpdateClientSideAnimation	,		g_Globals.m_Hooks.m_Originals.m_UpdateClientSideAnimation	);
	CREATE_HOOK( pStandardBlendingRules			,		C_Hooks::hkStandardBlendingRules		,		g_Globals.m_Hooks.m_Originals.m_StandardBlendingRules		);
	CREATE_HOOK( pSetupAliveLoop				,		C_Hooks::hkSetupAliveLoop				,		g_Globals.m_Hooks.m_Originals.m_SetupAliveLoop				);
	CREATE_HOOK( pPhysicsSimulate				,		C_Hooks::hkPhysicsSimulate				,		g_Globals.m_Hooks.m_Originals.m_PhysicsSimulate				);
	CREATE_HOOK( pDoPostScreenEffects			,		C_Hooks::hkDoPostScreenEffects			,		g_Globals.m_Hooks.m_Originals.m_DoPostScreenEffects			);
	CREATE_HOOK( pOverrideView					,		C_Hooks::hkOverrideView					,		g_Globals.m_Hooks.m_Originals.m_OverrideView				);
	CREATE_HOOK( pSendNetMessage				,		C_Hooks::hkSendNetMessage				,		g_Globals.m_Hooks.m_Originals.m_SendNetMessage				);
	CREATE_HOOK( pGetViewmodelFOV				,		C_Hooks::hkGetViewmodelFOV				,		g_Globals.m_Hooks.m_Originals.m_GetViewmodelFOV				);
	CREATE_HOOK( pIsConnected					,		C_Hooks::hkIsConnected					,		g_Globals.m_Hooks.m_Originals.m_IsConnected					);
	CREATE_HOOK( pCalcView						,		C_Hooks::hkCalcView						,		g_Globals.m_Hooks.m_Originals.m_CalcView					);
	CREATE_HOOK( pDrawModelExecute_Model		,		C_Hooks::hkDrawModelExecute_Model		,		g_Globals.m_Hooks.m_Originals.m_DrawModelExecute_Model		);
	CREATE_HOOK( pDrawModelExecute_Studio		,		C_Hooks::hkDrawModelExecute_Studio		,		g_Globals.m_Hooks.m_Originals.m_DrawModelExecute_Studio		);
	CREATE_HOOK( pGetColorModulation			,		C_Hooks::hkGetColorModulation			,		g_Globals.m_Hooks.m_Originals.m_GetColorModulation			);
	CREATE_HOOK( pGetAlphaModulation			,		C_Hooks::hkGetAlphaModulation			,		g_Globals.m_Hooks.m_Originals.m_GetAlphaModulation			);
	CREATE_HOOK( pGetClientModelRenderable		,		C_Hooks::hkGetClientModelRenderable		,		g_Globals.m_Hooks.m_Originals.m_Useless						);
	CREATE_HOOK( pIsUsingDebugStaticProps		,		C_Hooks::hkIsUsingDebugStaticProps		,		g_Globals.m_Hooks.m_Originals.m_IsUsingDebugStaticProps		);
	CREATE_HOOK( pDispatchUserMessage			,		C_Hooks::hkDispatchUserMessage			,		g_Globals.m_Hooks.m_Originals.m_DispatchUserMessage			);
	CREATE_HOOK( pPerformScreenOverlay			,		C_Hooks::hkPerformScreenOverlay			,		g_Globals.m_Hooks.m_Originals.m_PerformScreenOverlay		);
	CREATE_HOOK( pCalcViewmodelBob				,		C_Hooks::hkCalcViewmodelBob				,		g_Globals.m_Hooks.m_Originals.m_CalcViewmodelBob			);
	CREATE_HOOK( pShouldDrawFOG					,		C_Hooks::hkShouldDrawFOG				,		g_Globals.m_Hooks.m_Originals.m_ShouldDrawFog				);
	CREATE_HOOK( pListLeavesInBox				,		C_Hooks::hkListLeavesInBox				,		g_Globals.m_Hooks.m_Originals.m_ListLeavesInBox				);
	CREATE_HOOK( pGetAspectRatio				,		C_Hooks::hkGetScreenSizeAspectRatio		,		g_Globals.m_Hooks.m_Originals.m_GetScreenSizeAspectRatio	);
	CREATE_HOOK( pWriteUsercmdDeltaToBuffer		,		C_Hooks::hkWriteUsercmdDeltaToBuffer	,		g_Globals.m_Hooks.m_Originals.m_WriteUsercmdDeltaToBuffer	);

	MH_EnableHook( MH_ALL_HOOKS );

	C_CustomEventListener* pEventListener = new C_CustomEventListener( );
	g_Globals.m_Interfaces.m_EventManager->AddListener( pEventListener, _S( "bomb_beginplant" ), false );
	g_Globals.m_Interfaces.m_EventManager->AddListener( pEventListener, _S( "bomb_begindefuse" ), false );
	g_Globals.m_Interfaces.m_EventManager->AddListener( pEventListener, _S( "bullet_impact" ), false );
	g_Globals.m_Interfaces.m_EventManager->AddListener( pEventListener, _S( "player_hurt" ), false );
	g_Globals.m_Interfaces.m_EventManager->AddListener( pEventListener, _S( "player_death" ), false );
	g_Globals.m_Interfaces.m_EventManager->AddListener( pEventListener, _S( "weapon_fire" ), false );
	g_Globals.m_Interfaces.m_EventManager->AddListener( pEventListener, _S( "item_purchase" ), false );
	g_Globals.m_Interfaces.m_EventManager->AddListener( pEventListener, _S( "round_start" ), false );
	g_Globals.m_Interfaces.m_EventManager->AddListener( pEventListener, _S( "round_end" ), false );

	g_Globals.m_Hooks.m_Originals.m_FlashDuration = new C_RecvHook( g_NetvarManager->GetNetProp( _S( "DT_CSPlayer" ), _S( "m_flFlashDuration" ) ), C_Hooks::hkFlashDuration );
}

void C_SetUp::SetupImGui()
{
	D3DDEVICE_CREATION_PARAMETERS lParams;
	g_Globals.m_Interfaces.m_DirectDevice->GetCreationParameters( &lParams );

	HWND hCSGO = lParams.hFocusWindow;
	if ( !hCSGO )
		return;

	ImGui::CreateContext( );
	ImGui::GetIO( ).ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
	ImGui::GetStyle( ).WindowRounding = 0.0f;

	if ( !ImGui_ImplDX9_Init( g_Globals.m_Interfaces.m_DirectDevice ) )
		return;

	if ( !ImGui_ImplWin32_Init( hCSGO ) )
		return;

	g_Globals.m_Render.m_OldWndProc = SetWindowLongA( hCSGO, GWLP_WNDPROC, ( LONG_PTR )( C_Hooks::hkWndProc ) );
}

void C_SetUp::SetupAddressList( )
{
	g_Globals.m_AddressList.m_ClDoResetLatch_GetBool_Call			=	g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "85 C0 75 28 8B 0D ? ? ? ? 81" ) );
	g_Globals.m_AddressList.m_SvCheats_GetBool_Call					=	g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "85 C0 75 30 38 86" ) );
	g_Globals.m_AddressList.m_IsPaused_Call							=	g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "0F B6 0D ? ? ? ? 84 C0 0F 44" ) );
	g_Globals.m_AddressList.m_InPrediction_Call						=	g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "84 C0 74 0A F3 0F 10 05 ? ? ? ? EB 05" ) );
	g_Globals.m_AddressList.m_SetupVelocity_Call					=	g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "84 C0 75 38 8B 0D ? ? ? ? 8B 01 8B 80 ? ? ? ? FF D0" ) );
	g_Globals.m_AddressList.m_AccumulateLayers_Call					=	g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "84 C0 75 0D F6 87" ) );
	g_Globals.m_AddressList.m_IsConnected_Call						=	g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "84 C0 75 04 B0 01 5F" ) );
	g_Globals.m_AddressList.m_SetupBonesForAttachmentQueries		=	g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "55 8B EC 83 EC 14 83 3D ? ? ? ? ? 53" ) );
	g_Globals.m_AddressList.m_DisableRenderTargetAllocationForever	=	g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "80 B9 ? ? ? ? ? 74 0F" ) ) + 0x2;
	g_Globals.m_AddressList.m_CreateModel							=	g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "53 8B D9 56 57 8D 4B 04 C7 03 ? ? ? ? E8 ? ? ? ? 6A" ) );
	g_Globals.m_AddressList.m_LookupSequence						=	g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "55 8B EC 56 8B F1 83 BE  4C 29 00 00 00 75 14 8B 46 04 8D 4E 04 FF 50 20 85 C0 74 07 8B CE E8 BD 65" ) );
	g_Globals.m_AddressList.m_SetAbsAngles							=	g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8" ) );
	g_Globals.m_AddressList.m_SetAbsOrigin							=	g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "55 8B EC 83 E4 F8 51 53  56 57 8B F1 E8 1F" ) );
	g_Globals.m_AddressList.m_SetMergedMDL							=	g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "55 8B EC 57 8B F9 8B 0D ? ? ? ? 85 C9 75" ) );
	g_Globals.m_AddressList.m_PredictionPlayer						=	g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "89 35 ? ? ? ? F3 0F 10 48 20" ) ) + 0x2;
	g_Globals.m_AddressList.m_PredictionSeed						=	g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "8B 0D ? ? ? ? BA ? ? ? ? E8 ? ? ? ? 83 C4 04" ) ) + 0x2;
	g_Globals.m_AddressList.m_GetSequenceActivity					=	g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "55 8B EC 53 8B 5D 08 56 8B F1 83" ) );
	g_Globals.m_AddressList.m_SetupBones_AttachmentHelper			=	g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "55 8B EC 83 EC 48 53 8B 5D 08 89 4D F4" ) );
	g_Globals.m_AddressList.m_InvalidateBoneCache					=	g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "80 3D ? ? ? ? ? 74 16 A1 ? ? ? ? 48 C7 81" ) );
	g_Globals.m_AddressList.m_SequenceDuration						=	g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "55 8B EC 56 8B F1 83 BE 4C 29 00 00 00 75 14 8B 46 04 8D 4E 04 FF 50 20 85 C0 74 07 8B CE E8 BD" ) );
	g_Globals.m_AddressList.m_SequenceDescriptor					=	g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "55 8B EC 83 79 04 00 75" ) );
	g_Globals.m_AddressList.m_GetFirstSequenceAnimationTag			=	g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "55 8B EC 56 8B F1 83 BE  4C 29 00 00 00 75 14 8B 46 04 8D 4E 04 FF 50 20 85 C0 74 07 8B CE E8 6D" ) );
	g_Globals.m_AddressList.m_KeyValuesFindKey						=	g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "55 8B EC 83 EC 1C 53 8B D9 85 DB" ) );
	g_Globals.m_AddressList.m_KeyValuesGetString					=	g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "55 8B EC 83 E4 C0 81 EC ? ? ? ? 53 8B 5D 08" ) );
	g_Globals.m_AddressList.m_KeyValuesSetString					=	g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "55 8B EC A1 ? ? ? ? 53 56 57 8B F9 8B 08 8B 01" ) );
	g_Globals.m_AddressList.m_KeyValuesLoadFromBuffer				=	g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "55 8B EC 83 E4 F8 83 EC 34 53 8B 5D 0C 89 4C 24 04" ) );
	g_Globals.m_AddressList.m_InvalidatePhysicsRecursive			=	g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "55 8B EC 83 E4 F8 83 EC 0C 53 8B 5D 08 8B C3" ) );
	g_Globals.m_AddressList.m_UpdateClientSideAnimation				=	g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "55 8B EC 51 56 8B F1 80 BE ? ? ? ? ? 74" ) );
	g_Globals.m_AddressList.m_PostProcess							=	g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "83 EC 4C 80 3D" ) ) + 0x5;
	g_Globals.m_AddressList.m_SmokeCount							=	g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "A3 ? ? ? ? 57 8B CB" ) ) + 0x1;
	g_Globals.m_AddressList.m_IsBreakableEntity						=	g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "55 8B EC 51 56 8B F1 85 F6 74 68" ) );
	g_Globals.m_AddressList.m_FindHudElement						=	g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28" ) );
	g_Globals.m_AddressList.m_FindHudElement_Ptr					=	g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 5D 08" ) ) + 0x1;
	g_Globals.m_AddressList.m_ClipRayToHitbox						=	g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "55 8B EC 83 E4 F8 F3 0F 10 42" ) );
	g_Globals.m_AddressList.m_ClipTraceToEntity						=	g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 81 EC D8 00 00 00" ) );
	g_Globals.m_AddressList.m_TraceFilterSimple						=	g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "55 8B EC 83 E4 F0 83 EC 7C 56 52" ) ) + 0x3D;
	g_Globals.m_AddressList.m_TraceToExit							=	g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "55 8B EC 83 EC 30 F3 0F 10 75" ) );
	g_Globals.m_AddressList.m_WriteUsercmd							=	g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "55 8B EC 83 E4 F8 51 53 56 8B D9 8B 0D" ) );
	g_Globals.m_AddressList.m_ClearDeathList						=	g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "55 8B EC 83 EC 0C 53 56 8B 71" ) );
	g_Globals.m_AddressList.m_TraceFilterSkipTwoEntities			=	g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "55 8B EC 81 EC BC 00 00 00 56 8B F1 8B 86" ) ) + 0x21E;
	g_Globals.m_AddressList.m_HostShouldRun_Call					=	g_Tools->FindPattern( g_Globals.m_ModuleList.m_EngineDll, _S( "84 C0 0F 84 E0 01 00 00 A1 E8" ) );
	g_Globals.m_AddressList.m_DispatchSounds						=	g_Tools->FindPattern( g_Globals.m_ModuleList.m_EngineDll, _S( "56 8B 35 ?? ?? ?? ?? 57  8B 3D ?? ?? ?? ?? 66 90 83 Fe FF 74 3D 6B" ) );
	g_Globals.m_AddressList.m_LoadSkybox							=	g_Tools->FindPattern( g_Globals.m_ModuleList.m_EngineDll, _S( "55 8B EC 81 EC ? ? ? ? 56 57 8B F9 C7 45" ) );
}

void C_SetUp::SetupConvarList( )
{
	g_Globals.m_ConVars.m_3DSky							= g_Globals.m_Interfaces.m_CVar->FindVar( _S( "r_3dsky" ) );
	g_Globals.m_ConVars.m_WeaponRecoilScale				= g_Globals.m_Interfaces.m_CVar->FindVar( _S( "weapon_recoil_scale" ) );
	g_Globals.m_ConVars.m_Yaw							= g_Globals.m_Interfaces.m_CVar->FindVar( _S( "m_yaw" ) );
	g_Globals.m_ConVars.m_Pitch							= g_Globals.m_Interfaces.m_CVar->FindVar( _S( "m_pitch" ) );
	g_Globals.m_ConVars.m_Sensitivity					= g_Globals.m_Interfaces.m_CVar->FindVar( _S( "sensitivity" ) );
	g_Globals.m_ConVars.m_ClInterp						= g_Globals.m_Interfaces.m_CVar->FindVar( _S( "cl_interp" ) );
	g_Globals.m_ConVars.m_DamageBulletPenetration		= g_Globals.m_Interfaces.m_CVar->FindVar( _S( "ff_damage_bullet_penetration" ) );
	g_Globals.m_ConVars.m_DamageReduction				= g_Globals.m_Interfaces.m_CVar->FindVar( _S( "ff_damage_reduction_bullets" ) );
	g_Globals.m_ConVars.m_ClInterpRatio					= g_Globals.m_Interfaces.m_CVar->FindVar( _S( "cl_interp_ratio" ) );
	g_Globals.m_ConVars.m_ClUpdateRate					= g_Globals.m_Interfaces.m_CVar->FindVar( _S( "cl_updaterate" ) );
	g_Globals.m_ConVars.m_SvMinUpdateRate				= g_Globals.m_Interfaces.m_CVar->FindVar( _S( "sv_minupdaterate" ) );
	g_Globals.m_ConVars.m_SvMaxUpdateRate				= g_Globals.m_Interfaces.m_CVar->FindVar( _S( "sv_maxupdaterate" ) );
	g_Globals.m_ConVars.m_SvAcceleration				= g_Globals.m_Interfaces.m_CVar->FindVar( _S( "sv_accelerate" ) );
	g_Globals.m_ConVars.m_SvClientMinInterpRatio		= g_Globals.m_Interfaces.m_CVar->FindVar( _S( "sv_client_min_interp_ratio" ) );
	g_Globals.m_ConVars.m_SvClientMaxInterpRatio		= g_Globals.m_Interfaces.m_CVar->FindVar( _S( "sv_client_max_interp_ratio" ) );
	g_Globals.m_ConVars.m_SvGravity						= g_Globals.m_Interfaces.m_CVar->FindVar( _S( "sv_gravity" ) );
	g_Globals.m_ConVars.m_SvFriction					= g_Globals.m_Interfaces.m_CVar->FindVar( _S( "sv_friction" ) );
	g_Globals.m_ConVars.m_SvStopSpeed					= g_Globals.m_Interfaces.m_CVar->FindVar( _S( "sv_stopspeed" ) );
	g_Globals.m_ConVars.m_SvFootsteps					= g_Globals.m_Interfaces.m_CVar->FindVar( _S( "sv_footsteps" ) );
	g_Globals.m_ConVars.m_ClWpnSwayAmount				= g_Globals.m_Interfaces.m_CVar->FindVar( _S( "cl_wpn_sway_scale" ) );
	g_Globals.m_ConVars.m_ClCsmShadows					= g_Globals.m_Interfaces.m_CVar->FindVar( _S( "cl_csm_shadows" ) );
	g_Globals.m_ConVars.m_ClFootContactShadows			= g_Globals.m_Interfaces.m_CVar->FindVar( _S( "cl_foot_contact_shadows" ) );
	g_Globals.m_ConVars.m_ClCsmStaticPropShadows		= g_Globals.m_Interfaces.m_CVar->FindVar( _S( "cl_csm_static_prop_shadows" ) );
	g_Globals.m_ConVars.m_ClCsmWorldShadows				= g_Globals.m_Interfaces.m_CVar->FindVar( _S( "cl_csm_world_shadows" ) );
	g_Globals.m_ConVars.m_ClCsmViewmodelShadows			= g_Globals.m_Interfaces.m_CVar->FindVar( _S( "cl_csm_viewmodel_shadows" ) );
	g_Globals.m_ConVars.m_ClCsmSpriteShadows			= g_Globals.m_Interfaces.m_CVar->FindVar( _S( "cl_csm_sprite_shadows" ) );
	g_Globals.m_ConVars.m_ClCsmRopeShadows				= g_Globals.m_Interfaces.m_CVar->FindVar( _S( "cl_csm_rope_shadows" ) );
	g_Globals.m_ConVars.m_ViewmodelX					= g_Globals.m_Interfaces.m_CVar->FindVar( _S( "viewmodel_offset_x" ) );
	g_Globals.m_ConVars.m_ViewmodelY					= g_Globals.m_Interfaces.m_CVar->FindVar( _S( "viewmodel_offset_y" ) );
	g_Globals.m_ConVars.m_ViewmodelZ					= g_Globals.m_Interfaces.m_CVar->FindVar( _S( "viewmodel_offset_z" ) );
	g_Globals.m_ConVars.m_RainDensity					= g_Globals.m_Interfaces.m_CVar->FindVar( _S( "r_raindensity" ) );
	g_Globals.m_ConVars.m_RainSpeed						= g_Globals.m_Interfaces.m_CVar->FindVar( _S( "r_rainspeed" ) );
	g_Globals.m_ConVars.m_RainLength					= g_Globals.m_Interfaces.m_CVar->FindVar( _S( "r_rainlength" ) );
	g_Globals.m_ConVars.m_RainWidth						= g_Globals.m_Interfaces.m_CVar->FindVar( _S( "r_rainwidth" ) );
	g_Globals.m_ConVars.m_RainAlpha						= g_Globals.m_Interfaces.m_CVar->FindVar( _S( "r_rainalpha" ) );
	g_Globals.m_ConVars.m_RainSideVel					= g_Globals.m_Interfaces.m_CVar->FindVar( _S( "r_RainSideVel" ) );
	g_Globals.m_ConVars.m_WeaponDebugShowSpread			= g_Globals.m_Interfaces.m_CVar->FindVar( _S( "weapon_debug_spread_show" ) );

	g_Globals.m_ConVars.m_ViewmodelX->m_fnChangeCallbacks.m_Size = 0;
	g_Globals.m_ConVars.m_ViewmodelY->m_fnChangeCallbacks.m_Size = 0;
	g_Globals.m_ConVars.m_ViewmodelZ->m_fnChangeCallbacks.m_Size = 0;
	
	g_Globals.m_ConVars.m_ViewmodelX->m_fMinVal = INT_MIN;
	g_Globals.m_ConVars.m_ViewmodelY->m_fMinVal = INT_MIN;
	g_Globals.m_ConVars.m_ViewmodelZ->m_fMinVal = INT_MIN;
	
	g_Globals.m_ConVars.m_ViewmodelX->m_fMaxVal = INT_MAX;
	g_Globals.m_ConVars.m_ViewmodelY->m_fMaxVal = INT_MAX;
	g_Globals.m_ConVars.m_ViewmodelZ->m_fMaxVal = INT_MAX;
}

void C_SetUp::SetupModuleList( )
{
	g_Globals.m_ModuleList.m_ClientDll			= GetModuleHandleA( _S( "client.dll" ) );
	g_Globals.m_ModuleList.m_EngineDll			= GetModuleHandleA( _S( "engine.dll" ) );
	g_Globals.m_ModuleList.m_TierDll			= GetModuleHandleA( _S( "tier0.dll" ) );
	g_Globals.m_ModuleList.m_LocalizeDll		= GetModuleHandleA( _S( "localize.dll" ) );
	g_Globals.m_ModuleList.m_FileSystemDll		= GetModuleHandleA( _S( "filesystem_stdio.dll" ) );
	g_Globals.m_ModuleList.m_ShaderDll			= GetModuleHandleA( _S( "shaderapidx9.dll" ) );
	g_Globals.m_ModuleList.m_VGuiDll			= GetModuleHandleA( _S( "vguimatsurface.dll" ) );
	g_Globals.m_ModuleList.m_VGui2Dll			= GetModuleHandleA( _S( "vgui2.dll" ) );
	g_Globals.m_ModuleList.m_PhysicsDll			= GetModuleHandleA( _S( "vphysics.dll" ) );
	g_Globals.m_ModuleList.m_StdDll				= GetModuleHandleA( _S( "vstdlib.dll" ) );
	g_Globals.m_ModuleList.m_MatSysDll			= GetModuleHandleA( _S( "materialsystem.dll" ) );
	g_Globals.m_ModuleList.m_DataCacheDll		= GetModuleHandleA( _S( "datacache.dll" ) );
	g_Globals.m_ModuleList.m_StudioRenderDll	= GetModuleHandleA( _S( "studiorender.dll" ) );
}

void C_SetUp::SetupInterfaceList( )
{
	g_Globals.m_Interfaces.m_EntityList			= reinterpret_cast< C_ClientEntityList*		>(		g_Tools->GetInterface( g_Globals.m_ModuleList.m_ClientDll		, _S( "VClientEntityList003" ) )	);
	g_Globals.m_Interfaces.m_CHLClient			= reinterpret_cast< C_BaseClientDLL*		>(		g_Tools->GetInterface( g_Globals.m_ModuleList.m_ClientDll		, _S( "VClient018" ) )				);
	g_Globals.m_Interfaces.m_Prediction			= reinterpret_cast< C_Prediction*			>(		g_Tools->GetInterface( g_Globals.m_ModuleList.m_ClientDll		, _S( "VClientPrediction001" ) )	);
	g_Globals.m_Interfaces.m_GameMovement		= reinterpret_cast< C_GameMovement*			>(		g_Tools->GetInterface( g_Globals.m_ModuleList.m_ClientDll		, _S( "GameMovement001" ) )			);
	g_Globals.m_Interfaces.m_ModelRender		= reinterpret_cast< C_ModelRender*			>(		g_Tools->GetInterface( g_Globals.m_ModuleList.m_EngineDll		, _S( "VEngineModel016" ) )			);
	g_Globals.m_Interfaces.m_ModelInfo			= reinterpret_cast< C_ModelInfoClient*		>(		g_Tools->GetInterface( g_Globals.m_ModuleList.m_EngineDll		, _S( "VModelInfoClient004" ) )		);
	g_Globals.m_Interfaces.m_EventManager		= reinterpret_cast< C_GameEventManager*		>(		g_Tools->GetInterface( g_Globals.m_ModuleList.m_EngineDll		, _S( "GAMEEVENTSMANAGER002" ) )	);
	g_Globals.m_Interfaces.m_RenderView			= reinterpret_cast< C_RenderView*			>(		g_Tools->GetInterface( g_Globals.m_ModuleList.m_EngineDll		, _S( "VEngineRenderView014" ) )	);
	g_Globals.m_Interfaces.m_EngineClient		= reinterpret_cast< C_EngineClient*			>(		g_Tools->GetInterface( g_Globals.m_ModuleList.m_EngineDll		, _S( "VEngineClient014" ) )		);
	g_Globals.m_Interfaces.m_EngineSound		= reinterpret_cast< C_EngineSound*			>(		g_Tools->GetInterface( g_Globals.m_ModuleList.m_EngineDll		, _S( "IEngineSoundClient003" ) )	);
	g_Globals.m_Interfaces.m_EngineTrace		= reinterpret_cast< C_EngineTrace*			>(		g_Tools->GetInterface( g_Globals.m_ModuleList.m_EngineDll		, _S( "EngineTraceClient004" ) )	);
	g_Globals.m_Interfaces.m_DebugOverlay		= reinterpret_cast< C_DebugOverlay*			>(		g_Tools->GetInterface( g_Globals.m_ModuleList.m_EngineDll		, _S( "VDebugOverlay004" ) )		);
	g_Globals.m_Interfaces.m_Surface			= reinterpret_cast< C_Surface*				>(		g_Tools->GetInterface( g_Globals.m_ModuleList.m_VGuiDll			, _S( "VGUI_Surface031" ) )			);
	g_Globals.m_Interfaces.m_VGUIPanel			= reinterpret_cast< C_Panel*				>(		g_Tools->GetInterface( g_Globals.m_ModuleList.m_VGui2Dll		, _S( "VGUI_Panel009" ) )			);
	g_Globals.m_Interfaces.m_MaterialSystem		= reinterpret_cast< C_MaterialSystem*		>(		g_Tools->GetInterface( g_Globals.m_ModuleList.m_MatSysDll		, _S( "VMaterialSystem080" ) )		);
	g_Globals.m_Interfaces.m_PropPhysics		= reinterpret_cast< C_PhysicsSurfaceProps*	>(		g_Tools->GetInterface( g_Globals.m_ModuleList.m_PhysicsDll		, _S( "VPhysicsSurfaceProps001" ) )	);
	g_Globals.m_Interfaces.m_Localize			= reinterpret_cast< C_Localize*				>(		g_Tools->GetInterface( g_Globals.m_ModuleList.m_LocalizeDll		, _S( "Localize_001" ) )			);
	g_Globals.m_Interfaces.m_FileSystem			= reinterpret_cast< C_FileSystem*			>(		g_Tools->GetInterface( g_Globals.m_ModuleList.m_FileSystemDll	, _S( "VBaseFileSystem011" ) )			);
	g_Globals.m_Interfaces.m_MDLCache			= reinterpret_cast< C_MDLCache*				>(		g_Tools->GetInterface( g_Globals.m_ModuleList.m_DataCacheDll	, _S( "MDLCache004" ) )				);
	g_Globals.m_Interfaces.m_CVar				= reinterpret_cast< C_Cvar*					>(		g_Tools->GetInterface( g_Globals.m_ModuleList.m_StdDll			, _S( "VEngineCvar007" ) )			);
	g_Globals.m_Interfaces.m_StudioRender		= reinterpret_cast< C_StudioRender*			>(		g_Tools->GetInterface( g_Globals.m_ModuleList.m_StudioRenderDll	, _S( "VStudioRender026" ) )		);

	g_Globals.m_Interfaces.m_MemAlloc			= *( C_MemAlloc** )( GetProcAddress( g_Globals.m_ModuleList.m_TierDll, _S( "g_pMemAlloc" ) ) );
	g_Globals.m_Interfaces.m_ClientState		=**( C_ClientState*** )( g_Tools->FindPattern( g_Globals.m_ModuleList.m_EngineDll, _S( "A1 ? ? ? ? 8B 80 ? ? ? ? C3" ) ) + 0x1 );
	g_Globals.m_Interfaces.m_MoveHelper			=**( C_MoveHelper*** )( g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "8B 0D ? ? ? ? 8B 45 ? 51 8B D4 89 02 8B 01" ) ) + 0x2 );
	g_Globals.m_Interfaces.m_GlobalVars			=**( C_GlobalVarsBase*** )( g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "A1 ? ? ? ? 5E 8B 40 10" ) ) + 0x1 );
	g_Globals.m_Interfaces.m_DirectDevice		=**( IDirect3DDevice9*** )( g_Tools->FindPattern( g_Globals.m_ModuleList.m_ShaderDll, _S( "A1 ? ? ? ? 50 8B 08 FF 51 0C" ) ) + 0x1 );
	g_Globals.m_Interfaces.m_Input				= *( C_Input** )( g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "B9 ? ? ? ? F3 0F 11 04 24 FF 50 10" ) ) + 0x1 );
	g_Globals.m_Interfaces.m_GameRules			= *( C_GameRules*** )( g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "E8 ? ? ? ? A1 ? ? ? ? 85 C0 0F 84 ? ? ? ?" ) ) + 0x6 );
	g_Globals.m_Interfaces.m_ViewRenderBeams	= *( C_ViewRenderBeams** )( g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S( "A1 ? ? ? ? FF 10 A1 ? ? ? ? B9" ) ) + 0x1 );
	g_Globals.m_Interfaces.m_GlowObjManager		= *( IGlowObjectManager** )( g_Tools->FindPattern( g_Globals.m_ModuleList.m_ClientDll, _S("0F 11 05 ? ? ? ? 83 C8 01") ) + 0x3 );
}