#pragma once
#include "SDK/Includes.hpp"

#include <MinHook.h>
#pragma comment( lib, "MinHook.lib" )

class C_Hooks
{
public:
	// directx
	static long WINAPI hkPresent( IDirect3DDevice9* pDevice, RECT* pSrcRect, RECT* pDescRect, HWND pHWND, RGNDATA* pRegion );
	static long WINAPI hkReset( IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pDeviceParameters );
	static LRESULT WINAPI hkWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

	// booleans
	static bool __fastcall hkSvCheats_GetBool( LPVOID pEcx, uint32_t );
	static bool __fastcall hkClDoResetLatch_GetBool( LPVOID pEcx, uint32_t );
	static bool __fastcall hkIsConnected( LPVOID pEcx, uint32_t );
	static bool __fastcall hkIsPaused( LPVOID pEcx, uint32_t );
	static bool __fastcall hkIsHLTV( LPVOID pEcx, uint32_t );
	static bool __fastcall hkInPrediction( LPVOID pEcx, uint32_t );

	// command related
	static void __cdecl hkCL_Move( float_t flFrametime, bool bIsFinalTick );
	static void __stdcall hkCreateMove( int32_t iSequence, float_t flFrametime, bool bIsActive, bool& bSendPacket );
	static __declspec( naked ) void __stdcall hkCreateMove_Proxy( int32_t iSequence, float_t flFrameTime, bool bIsActive );
	static void __fastcall hkPacketStart( LPVOID pEcx, uint32_t, int32_t iSequence, int32_t iCommand );
	static void __fastcall hkPacketEnd( LPVOID pEcx, uint32_t );
	static bool __fastcall hkWriteUsercmdDeltaToBuffer( LPVOID pEcx, uint32_t, int32_t iSlot, bf_write* pBuffer, int32_t iFrom, int32_t iTo, bool bNewCmd );

	// game movement
	static void __fastcall hkProcessMovement( LPVOID pEcx, uint32_t, C_BasePlayer* pPlayer, C_MoveData* pMoveData );
	static bool __fastcall hkTraceFilterForHeadCollision( LPVOID pEcx, uint32_t, C_BasePlayer* pPlayer, LPVOID pTraceParams );

	// net
	static bool __cdecl hkHost_ShouldRun( );

	// frame related
	static void __fastcall hkFrameStageNotify( LPVOID pEcx, uint32_t, ClientFrameStage_t Stage );

	// engine things
	static void __fastcall hkFireEvents( LPVOID pEcx, uint32_t );

	// surface
	static void __fastcall hkLockCursor( LPVOID pEcx, uint32_t );
	static void __fastcall hkPaintTraverse( LPVOID pEcx, uint32_t, VGUI::VPANEL pPanel, bool bForceRepaint, bool bAllowForce );

	// animstate
	static void __fastcall hkModifyEyePosition( LPVOID pEcx, uint32_t, Vector& vecEyePosition );
	static void __fastcall hkSetupAliveLoop( LPVOID pEcx, uint32_t );
	
	// player class
	static void __fastcall hkDoExtraBoneProcessing( LPVOID pEcx, uint32_t, C_StudioHDR* pHdr, Vector* vecVector, Quaternion* pSomething, matrix3x4_t* aMatrix, LPVOID aList, LPVOID pContext );
	static void __fastcall hkUpdateClientSideAnimation( LPVOID pEcx, uint32_t );
	static bool __fastcall hkSetupBones( LPVOID pEcx, uint32_t, matrix3x4_t* aMatrix, int32_t iMaxBones, int32_t iBoneMask, float_t flCurrentTime );
	static void __fastcall hkStandardBlendingRules( LPVOID pEcx, uint32_t, C_StudioHDR* pStudioHDR, Vector* vecPosition, Quaternion* quatern, float_t flTime, int32_t iBoneMask );
	static void __fastcall hkPhysicsSimulate( LPVOID pEcx, uint32_t );
	static void __fastcall hkCalcView( LPVOID pEcx, uint32_t, Vector& vecEyeOrigin, QAngle& angEyeAngles, float_t& zNear, float_t& zFar, float_t& flFov );
	static void __fastcall hkCalcViewmodelBob( LPVOID pEcx, uint32_t, Vector& vecEyePos );

	static void hkFlashDuration( const CRecvProxyData* Data, LPVOID pStruct, LPVOID pOut );
	static int32_t __fastcall hkListLeavesInBox( LPVOID pEcx, uint32_t, Vector& vecMin, Vector& vecMax, unsigned short* pList, int32_t iMax );
	static float_t __fastcall hkGetScreenSizeAspectRatio( LPVOID pEcx, uint32_t, int32_t X, int32_t Y );

	// material
	static float_t __fastcall hkGetAlphaModulation( LPVOID pEcx, uint32_t );
	static void __fastcall hkGetColorModulation( LPVOID pEcx, uint32_t, float_t* flRed, float_t* flGreen, float_t* flBlue );
	static bool __cdecl hkIsUsingDebugStaticProps( );

	// model render
	static void __fastcall hkDrawModelExecute_Studio( LPVOID pEcx, uint32_t, LPVOID pResults, DrawModelInfo_t* pInfo, matrix3x4_t* pBoneToWorld, float_t* flpFlexWeights, float_t* flpFlexDelayedWeights, Vector& vrModelOrigin, int32_t iFlags );
	static void __fastcall hkDrawModelExecute_Model( LPVOID pEcx, uint32_t, IMatRenderContext* pCtx, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4_t* aMatrix );

	// clientmode
	static void __fastcall hkOverrideView( LPVOID pEcx, uint32_t, C_ViewSetup* pSetupView );
	static float_t __fastcall hkGetViewmodelFOV( LPVOID pEcx, uint32_t );

	// net channel
	static bool __fastcall hkSendNetMessage( LPVOID pEcx, uint32_t, C_NetMessage& Message, bool bReliable, bool bVoice );

	// other
	static LPVOID __fastcall hkGetClientModelRenderable( LPVOID pEcx, uint32_t );
	static bool __fastcall hkDispatchUserMessage( LPVOID pEcx, uint32_t, int32_t iMessageType, int32_t iArgument, int32_t iSecondArgument, LPVOID pData );
	static void __fastcall hkPerformScreenOverlay( LPVOID pEcx, uint32_t, int32_t x, int32_t y, int32_t iWidth, int32_t iHeight );
	static void __cdecl hkShouldDrawFOG( );

	// effects render
	static int __fastcall hkDoPostScreenEffects(LPVOID pEcx, uint32_t);
};