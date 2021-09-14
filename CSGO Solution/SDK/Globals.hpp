#pragma once
#include <d3d9.h>
#include <Windows.h>
#include <string>
#include <deque>
#include <array>
#include <vector>
#include <unordered_map>
#include <tuple>

#include "../Tools/Obfuscation/XorStr.hpp"

#include "Interfaces/IVEngineClient.hpp"
#include "Interfaces/IEngineSound.hpp"
#include "Interfaces/IBaseClientDll.hpp"
#include "Interfaces/IClientEntityList.hpp"
#include "Interfaces/IEngineTrace.hpp"
#include "Interfaces/IPrediction.hpp"
#include "Interfaces/IGlobalVars.hpp"
#include "Interfaces/IGameEventmanager.hpp"
#include "Interfaces/ISurface.hpp"
#include "Interfaces/IRenderView.hpp"
#include "Interfaces/IVDebugOverlay.hpp"
#include "Interfaces/IVModelInfoClient.hpp"
#include "Interfaces/IMaterialSystem.hpp"
#include "Interfaces/IClientState.hpp"
#include "Interfaces/IFileSystem.hpp"
#include "Interfaces/ILocalize.hpp"
#include "Interfaces/IMemAlloc.hpp"
#include "Interfaces/IMDLCache.hpp"
#include "Interfaces/IPanel.hpp"
#include "Interfaces/IPhysics.hpp"
#include "Interfaces/ICvar.hpp"
#include "Interfaces/Input.hpp"
#include "Interfaces/Convar.hpp"
#include "Interfaces/IGameRules.hpp"
#include "Interfaces/IViewRenderBeams.hpp"
#include "Interfaces/IGlowObjectManager.hpp"
#include "Game/NetMessage.hpp"

#ifdef __READY__
inline unsigned long ByteSwap( unsigned long uValue )
{
	volatile uint32_t Swap = uValue;
	__asm
	{
		mov ecx, uValue
		bswap ecx
		mov Swap ecx
	}
	return Swap;
}

template < class T >
class C_XorPtr
{
public:
	__forceinline T& Get( )
	{
		volatile uint32_t uRor = 0x54F12F43;
		volatile uint32_t uDexor = 0x54F12F44;
		volatile uint32_t uSub = 0x54F12F45;
		volatile uint32_t uRolAmount = 0x54F12F46;
		return *( T* )( ( _rotl( uRor, uRolAmount ) ^ _byteswap_ulong( uDexor ) ) - uSub );
	}
};
#endif

struct VarMapEntry_t
{
public:
	unsigned short		type;
	unsigned short		m_bNeedsToInterpolate;	// Set to false when this var doesn't
												// need Interpolate() called on it anymore.
	void				*data;
	void				*watcher;
};
struct VarMapping_t
{
	VarMapping_t()
	{
		m_nInterpolatedEntries = 0;
	}

	CUtlVector< VarMapEntry_t >	m_Entries;
	int							m_nInterpolatedEntries;
	float						m_lastInterpolationTime;
};
struct GrenadeData_t
{
	__forceinline GrenadeData_t( ) = default;
	__forceinline GrenadeData_t( C_BasePlayer* OwnerPlayer, int32_t Index, const Vector& vecOrigin, const Vector& vecVelocity, float_t flThrowTime, int32_t iOffset ) : GrenadeData_t( )
	{
		m_OwnerPlayer = OwnerPlayer;
		m_Index = Index;

		m_vecOrigin = vecOrigin;
		m_vecVelocity = vecVelocity;

		m_ThrowTime = flThrowTime;
	}

	bool m_bHasBeenDetonated = false;

	C_BasePlayer* m_OwnerPlayer = nullptr;

	Vector m_vecOrigin = Vector( 0, 0, 0 );
	Vector m_vecVelocity = Vector( 0, 0, 0 );

	C_ClientEntity* m_LastEntity = nullptr;

	int32_t m_CollisionGroup = -1;

	float_t m_ExplodeTime = 0.0f;
	float_t m_ExpireTime = 0.0f;
	float_t m_ThrowTime = 0.0f;

	int32_t m_Index = -1;
	int32_t m_CurrentTick = -1;
	int32_t m_NextThinkTick = -1;
	int32_t m_LastUpdateTick = -1;
	int32_t m_BounceCount = -1;

	std::vector < std::pair < Vector, bool > > m_Bounces = { };
};

#include "Game/AnimationLayer.hpp"
#include "Game/AnimationState.hpp"

class C_LagRecord
{
public:
	float_t m_SimulationTime = 0.0f;
	float_t m_LowerBodyYaw = 0.0f;
	float_t m_DuckAmount = 0.0f;
	float_t m_BruteYaw = 0.0f;

	int32_t m_UpdateDelay = 0;
	int32_t m_RotationMode = ROTATE_SERVER;
	int32_t m_Flags = 0;
	int32_t m_AdjustTick = 0;

	QAngle m_EyeAngles = QAngle( 0, 0, 0 );
	QAngle m_AbsAngles = QAngle( 0, 0, 0 );
	Vector m_Velocity = Vector( 0, 0, 0 );
	Vector m_Origin = Vector( 0, 0, 0 );
	Vector m_AbsOrigin = Vector( 0, 0, 0 );
	Vector m_Mins = Vector( 0, 0, 0 );
	Vector m_Maxs = Vector( 0, 0, 0 );
	
	std::array < std::array < C_AnimationLayer, ANIMATION_LAYER_COUNT >, 6 > m_AnimationLayers = { };
	std::array < float_t, MAXSTUDIOPOSEPARAM > m_PoseParameters = { };
	std::array < std::array < matrix3x4_t, MAXSTUDIOBONES >, 4 > m_Matricies = { };

	bool m_bIsShooting = false;
	bool m_bAnimResolved = false;
	bool m_bJumped = false;
};
class C_HitboxData
{
public:
	int32_t m_iHitbox = -1;
	float_t m_flDamage = 0.0f;
	Vector m_vecPoint = Vector(0, 0, 0);
	bool m_bIsSafeHitbox = false;
	bool m_bForcedToSafeHitbox = false;
	
	C_BasePlayer* pPlayer = NULL;
	C_LagRecord LagRecord = C_LagRecord( );
};
class C_HitboxHitscanData
{
public:
	C_HitboxHitscanData( int32_t iHitbox = -1, bool bForceSafe = false )
	{
		this->m_iHitbox = iHitbox;
		this->m_flWeaponDamage = 0.0f;
		this->m_bForceSafe = bForceSafe;
	}

	int32_t m_iHitbox = -1;
	float_t m_flWeaponDamage = 0.0f;
	bool m_bForceSafe = false;
};
class C_TargetData
{
public:
	C_BasePlayer* m_Player = NULL;
	C_LagRecord m_LagRecord = C_LagRecord();
	C_HitboxData m_Hitbox = C_HitboxData();
};
class C_ShotData
{
public:
	C_TargetData m_Target = C_TargetData( );

	Vector m_vecStartPosition = Vector( 0, 0, 0 );
	Vector m_vecEndPosition = Vector( 0, 0, 0 );

	int32_t m_iShotTick = 0;

	bool m_bDidIntersectPlayer = false;
	bool m_bHasBeenFired = false;
	bool m_bHasBeenRegistered = false;
	bool m_bHasBeenHurted = false;
	bool m_bHasMaximumAccuracy = false;

	std::vector < Vector > m_vecImpacts = { };
};
class C_RecvHook;

typedef void( __cdecl* ShouldDrawFog_t )( );
typedef void( __thiscall* GetColorModulation_t )( LPVOID, float_t*, float_t*, float_t* );
typedef long( __stdcall* Present_t ) ( IDirect3DDevice9*, RECT*, RECT*, HWND, RGNDATA* );
typedef long( __stdcall* Reset_t ) ( IDirect3DDevice9*, D3DPRESENT_PARAMETERS* );
typedef bool( __thiscall* GetBool_t ) ( LPVOID );
typedef void( __thiscall* PacketEnd_t ) ( LPVOID );
typedef void( __thiscall* FireEvents_t ) ( LPVOID );
typedef void( __thiscall* PacketStart_t )( LPVOID, int32_t, int32_t );
typedef void( __stdcall* FrameStageNotify_t )( ClientFrameStage_t );
typedef void( __stdcall* CreateMove_t )( int32_t, float_t, bool );
typedef void( __cdecl* CL_Move_t )( float_t, bool );
typedef void( __thiscall* ModifyEyePosition_t )( LPVOID, Vector& );
typedef void( __thiscall* DoExtraBoneProcessing_t )( LPVOID );
typedef bool( __thiscall* SetupBones_t )( LPVOID, LPVOID, int32_t, int32_t, float_t );
typedef void( __thiscall* StandardBlendingRules_t )( LPVOID, C_StudioHDR*, Vector*, Quaternion*, float_t, int32_t );
typedef void( __thiscall* PaintTraverse_t )( LPVOID, VGUI::VPANEL, bool, bool );
typedef void( __thiscall* ProcessMovement_t )( LPVOID, LPVOID, C_MoveData* );
typedef bool( __thiscall* TraceFilterForHeadCollision_t ) ( LPVOID, C_BasePlayer*, LPVOID );
typedef void( __thiscall* OverrideView_t )( LPVOID, C_ViewSetup* );
typedef void( __thiscall* PlayFootstepSound_t )( LPVOID, Vector&, surfacedata_t*, float_t, bool );
typedef int( __thiscall* DoPostScreenEffects_t )( LPVOID );
typedef void( __thiscall* CalcView_t )( LPVOID, Vector&, QAngle&, float_t&, float_t&, float_t& );
typedef int32_t( __thiscall* SendDatagram_t )( LPVOID, LPVOID );
typedef void( __thiscall* ProcessPacket_t )( LPVOID, LPVOID, bool );
typedef bool( __thiscall* SendNetMessage_t )( LPVOID, C_NetMessage&, bool, bool );
typedef float_t( __thiscall* GetViewmodelFov_t )( LPVOID );
typedef void( __thiscall* DrawModelExecute_Studio_t )( LPVOID, LPVOID, DrawModelInfo_t*, matrix3x4_t*, float_t*, float_t*, Vector&, int32_t );
typedef void( __thiscall* DrawModelExecute_Model_t )( LPVOID, IMatRenderContext*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4_t* );
typedef bool( __thiscall* DispatchUserMessage_t )( LPVOID, int32_t, int32_t, int32_t, LPVOID );
typedef void( __thiscall* PerformScreenOverlay_t )( LPVOID, int32_t, int32_t, int32_t, int32_t );
typedef unsigned long( __thiscall* GCRetrieveMessage_t)(void*, uint32_t*, void*, uint32_t, uint32_t*);
typedef unsigned long( __thiscall* GCSendMessage_t)(void*, uint32_t, const void*, uint32_t);
typedef bool ( __thiscall* WriteUsercmdDeltaToBuffer_t ) ( LPVOID, int32_t, bf_write*, int32_t, int32_t, bool );
typedef float_t( __thiscall* GetScreenSizeAspectRatio_t )( LPVOID, int32_t, int32_t );
typedef void( __thiscall* LevelInitPreEntity_t )( LPVOID, const char* );
typedef int32_t( __thiscall* ListLeavesInBox_t )( LPVOID, const Vector&, const Vector&, unsigned short*, int32_t );
typedef bool( __cdecl* Host_ShouldRun_t )( );
typedef void( __thiscall* ViewmodelInterpolate_t )( LPVOID, float_t );

class C_CommandContext
{
public:
	bool m_bNeedsProcessing;
	C_UserCmd m_Cmd;
	int m_nCommandNumber;
};

class ImFont;
class C_Globals
{
public:
	struct
	{
		C_ClientEntityList* m_EntityList = nullptr;
		C_BaseClientDLL* m_CHLClient = nullptr;
		C_EngineClient* m_EngineClient = nullptr;
		C_EngineSound* m_EngineSound = nullptr;
		C_EngineTrace* m_EngineTrace = nullptr;
		C_Localize* m_Localize = nullptr;
		C_Prediction* m_Prediction = nullptr;
		C_MoveHelper* m_MoveHelper = nullptr;
		C_GameMovement* m_GameMovement = nullptr;
		C_GlobalVarsBase* m_GlobalVars = nullptr;
		C_Surface* m_Surface = nullptr;
		C_DebugOverlay* m_DebugOverlay = nullptr;
		C_FileSystem* m_FileSystem = nullptr;
		C_Cvar* m_CVar = nullptr;
		C_ClientState* m_ClientState = nullptr;
		C_Input* m_Input = nullptr;
		C_GameRules** m_GameRules = nullptr;
		C_ViewRenderBeams* m_ViewRenderBeams = nullptr;
		C_PhysicsSurfaceProps* m_PropPhysics = nullptr;
		C_MDLCache* m_MDLCache = nullptr;
		C_Panel* m_VGUIPanel = nullptr;
		C_MaterialSystem* m_MaterialSystem = nullptr;
		C_ModelInfoClient* m_ModelInfo = nullptr;
		C_ModelRender* m_ModelRender = nullptr;
		C_StudioRender* m_StudioRender = nullptr;
		C_GameEventManager* m_EventManager = nullptr;
		C_RenderView* m_RenderView = nullptr;
		C_MemAlloc* m_MemAlloc = nullptr;
		IDirect3DDevice9* m_DirectDevice = nullptr;
		IGlowObjectManager* m_GlowObjManager = nullptr;
	} m_Interfaces;

	std::deque < C_ShotData > m_ShotData;
	struct
	{
		std::array < float_t, 256 > m_aInaccuracy;
		std::array < float_t, 256 > m_aSpread;
		std::array < float_t, 256 > m_aMultiValues;

		float_t m_flInaccuracy = 0.0f;
		float_t m_flSpread = 0.0f;
		float_t m_flMinInaccuracy = 0.0f;

		bool m_bCanFire_Shift = false;
		bool m_bCanFire_Default = false;
		bool m_bRestoreAutoStop = false;
		bool m_bHasValidAccuracyData = false;
		bool m_bHasMaximumAccuracy = false;
		bool m_bDoingSecondShot = false;
	} m_AccuracyData;

	struct
	{
		C_TargetData m_CurrentTarget;
	} m_RageData;

	std::array < std::deque < C_LagRecord >, 65 > m_CachedPlayerRecords;
	struct 
	{
		int m_PeekTick = -1;
		bool m_bIsPeeking = false;
		C_BasePlayer* m_Player = NULL;
	} m_Peek;

	struct
	{
		float_t m_VelocityLength = 0;
		int32_t m_TicksToStop = 0;
	} m_MovementData;

	struct
	{
		bool m_bShouldClearDeathNotices = false;
		int32_t m_BombSite = 0;
	} m_RoundInfo;

	struct
	{
		LONG m_OldWndProc = NULL;
	} m_Render;
	
	std::unordered_map < unsigned long, GrenadeData_t > m_GrenadeData = { };
	struct
	{
		std::array < C_AnimationLayer, 13 > m_AnimationLayers;
		std::array < C_AnimationLayer, 13 > m_FakeAnimationLayers;

		std::array < float_t, 24 > m_PoseParameters;
		std::array < float_t, 24 > m_FakePoseParameters;
		C_CSGOPlayerAnimationState m_FakeAnimationState;

		float_t m_flSpawnTime = 0.0f;
		float_t m_flLowerBodyYaw = 0.0f;
		float_t m_flNextLowerBodyYawUpdateTime = 0.0f;

		std::array < int32_t, 2 > m_iMoveType;
		std::array < int32_t, 2 > m_iFlags;

		std::array < Vector, MAXSTUDIOBONES > m_vecBoneOrigins;
		std::array < Vector, MAXSTUDIOBONES > m_vecFakeBoneOrigins;
	
		Vector m_vecNetworkedOrigin = Vector( 0, 0, 0 );
		Vector m_vecShootPosition = Vector( 0, 0, 0 );

		bool m_bDidShotAtChokeCycle = false;
		QAngle m_angShotChokedAngle = QAngle( 0, 0, 0 );

		float_t m_flShotTime = 0.0f;
		QAngle m_angForcedAngles = QAngle( 0, 0, 0 );

		std::array < matrix3x4_t, MAXSTUDIOBONES > m_aMainBones;
		std::array < matrix3x4_t, MAXSTUDIOBONES > m_aDesyncBones;
		std::array < matrix3x4_t, MAXSTUDIOBONES > m_aLagBones;
	} m_LocalData;

	struct
	{
		std::array < int32_t, 65 > m_MissedShots = { };
		std::array < int32_t, 65 > m_LastMissedShots = { };
		std::array < int32_t, 65 > m_BruteSide = { };
		std::array < int32_t, 65 > m_LastBruteSide = { };
		std::array < int32_t, 65 > m_LastTickbaseShift = { };
		std::array < float_t, 65 > m_LastValidTime = { };
		std::array < Vector, 65 > m_LastValidOrigin = { };
		std::array < bool, 65 > m_AnimResoled = { };
		std::array < bool, 65 > m_FirstSinceTickbaseShift = { };
	} m_ResolverData;

	struct
	{
		uint8_t* m_KeyValuesFindKey = nullptr;
		uint8_t* m_KeyValuesFindString = nullptr;
		uint8_t* m_KeyValuesGetString = nullptr;
		uint8_t* m_KeyValuesLoadFromBuffer = nullptr;
		uint8_t* m_KeyValuesSetString = nullptr;
	
		uint8_t* m_HostShouldRun_Call = nullptr;
		uint8_t* m_SvCheats_GetBool_Call = nullptr;
		uint8_t* m_ClDoResetLatch_GetBool_Call = nullptr;
		uint8_t* m_InPrediction_Call = nullptr;
		uint8_t* m_IsPaused_Call = nullptr;
		uint8_t* m_IsConnected_Call = nullptr;
		uint8_t* m_SetupVelocity_Call = nullptr;
		uint8_t* m_UpdateClientSideAnimation = nullptr;
		uint8_t* m_AccumulateLayers_Call = nullptr;
		uint8_t* m_DisableRenderTargetAllocationForever = nullptr;
		uint8_t* m_SetMergedMDL = nullptr;
		uint8_t* m_SetupBonesForAttachmentQueries = nullptr;
		uint8_t* m_CreateModel = nullptr;
		uint8_t* m_LookupSequence = nullptr;
		uint8_t* m_SetAbsAngles = nullptr;
		uint8_t* m_SetAbsOrigin = nullptr;
		uint8_t* m_PredictionSeed = nullptr;
		uint8_t* m_PredictionPlayer = nullptr;
		uint8_t* m_GetSequenceActivity = nullptr;
		uint8_t* m_SetupBones_AttachmentHelper = nullptr;
		uint8_t* m_InvalidateBoneCache = nullptr;
		uint8_t* m_SequenceDuration = nullptr;
		uint8_t* m_SequenceDescriptor = nullptr;
		uint8_t* m_GetFirstSequenceAnimationTag = nullptr;
		uint8_t* m_InvalidatePhysicsRecursive = nullptr;
		uint8_t* m_LoadSkybox = nullptr;
		uint8_t* m_SmokeCount = nullptr;
		uint8_t* m_PostProcess = nullptr;
		uint8_t* m_IsBreakableEntity = nullptr;
		uint8_t* m_FindHudElement = nullptr;
		uint8_t* m_FindHudElement_Ptr = nullptr;
		uint8_t* m_ClipRayToHitbox = nullptr;
		uint8_t* m_TraceFilterSimple = nullptr;
		uint8_t* m_ClipTraceToEntity = nullptr;
		uint8_t* m_TraceToExit = nullptr;
		uint8_t* m_WriteUsercmd = nullptr;
		uint8_t* m_ClearDeathList = nullptr;
		uint8_t* m_DispatchSounds = nullptr;
		uint8_t* m_TraceFilterSkipTwoEntities = nullptr;
	} m_AddressList;

	struct
	{
		std::array< bool, 256 > m_aHoldedKeys = { };
		std::array< bool, 256 > m_aToggledKeys = { };
	} m_KeyData;

	struct
	{
		ConVar* m_WeaponRecoilScale;
		ConVar* m_DamageBulletPenetration;
		ConVar* m_DamageReduction;
		ConVar* m_Yaw;
		ConVar* m_3DSky;
		ConVar* m_Pitch;
		ConVar* m_RainSpeed;
		ConVar* m_RainLength;
		ConVar* m_RainWidth;
		ConVar* m_RainDensity;
		ConVar* m_RainAlpha;
		ConVar* m_RainSideVel;
		ConVar* m_Sensitivity;
		ConVar* m_ClInterp;
		ConVar* m_ClInterpRatio;
		ConVar* m_ClUpdateRate;
		ConVar* m_SvClientMinInterpRatio;
		ConVar* m_SvClientMaxInterpRatio;
		ConVar* m_SvMinUpdateRate;
		ConVar* m_SvMaxUpdateRate;
		ConVar* m_SvGravity;
		ConVar* m_SvAcceleration;
		ConVar* m_SvFriction;
		ConVar* m_SvStopSpeed;
		ConVar* m_SvFootsteps;
		ConVar* m_SvJumpImpulse;
		ConVar* m_ClWpnSwayAmount;
		ConVar* m_ClFootContactShadows;
		ConVar* m_ClCsmStaticPropShadows;
		ConVar* m_ClCsmWorldShadows;
		ConVar* m_ClCsmShadows;
		ConVar* m_ClCsmViewmodelShadows;
		ConVar* m_ClCsmSpriteShadows;
		ConVar* m_ClCsmRopeShadows;
		ConVar* m_ViewmodelX;
		ConVar* m_ViewmodelY;
		ConVar* m_ViewmodelZ;
		ConVar* m_WeaponDebugShowSpread;
	} m_ConVars;

	struct
	{
		HMODULE m_EngineDll = nullptr;
		HMODULE m_ClientDll = nullptr;
		HMODULE m_VGuiDll = nullptr;
		HMODULE m_VGui2Dll = nullptr;
		HMODULE m_TierDll = nullptr;
		HMODULE m_MatSysDll = nullptr;
		HMODULE m_LocalizeDll = nullptr;
		HMODULE m_ShaderDll = nullptr;
		HMODULE m_DataCacheDll = nullptr;
		HMODULE m_StdDll = nullptr;
		HMODULE m_PhysicsDll = nullptr;
		HMODULE m_FileSystemDll = nullptr;
		HMODULE m_StudioRenderDll = nullptr;
	} m_ModuleList;

	struct
	{
		ImFont* m_SegoeUI = nullptr;
		ImFont* m_BigIcons = nullptr;
		ImFont* m_LogFont = nullptr;
		ImFont* m_LogIcons = nullptr;
		ImFont* m_MenuIcons = nullptr;
		ImFont* m_WeaponIcon = nullptr;
		ImFont* m_BombIcon = nullptr;
	} m_Fonts;

	struct
	{
		bool m_bAnimationUpdate = false;
		bool m_bSetupBones = false;
	} m_AnimationData;

	struct
	{
		bool m_bShotChams = false;
		bool m_bDrawModel = false;
		float_t m_flModelRotation = -180.0f;
	} m_Model;

	struct
	{
		bool m_bInCreateMove = false;
		bool m_bFakeDuck = false;
		bool m_bVisualFakeDuck = false;

		int m_MaxChoke = 0;
	} m_Packet;

	struct
	{
		float_t m_flPostponeFireReadyTime = 0.0f;

		std::array < bool, MULTIPLAYER_BACKUP > m_FireStates = { };
		std::array < bool, MULTIPLAYER_BACKUP > m_PostponeStates = { };
		std::array < bool, MULTIPLAYER_BACKUP > m_PrimaryAttack = { };
		std::array < bool, MULTIPLAYER_BACKUP > m_SecondaryAttack = { };
		std::array < int, MULTIPLAYER_BACKUP > m_TickRecords = { };
	} m_Revolver;

	struct
	{
		struct
		{
			Present_t m_Present = NULL;
			Reset_t m_Reset = NULL;
			CreateMove_t m_CreateMove = NULL;
			CL_Move_t m_CL_Move = NULL;
			PacketStart_t m_PacketStart = NULL;
			PacketEnd_t m_PacketEnd = NULL;
			ProcessMovement_t m_ProcessMovement = NULL;
			FireEvents_t m_FireEvents = NULL;
			FireEvents_t m_LockCursor = NULL;
			FireEvents_t m_UpdateClientSideAnimation = NULL;
			FireEvents_t m_SetupAliveLoop = NULL;
			FireEvents_t m_SetupVelocity = NULL;
			FireEvents_t m_PhysicsSimulate = NULL;
			FireEvents_t m_Useless = NULL;
			DoPostScreenEffects_t m_DoPostScreenEffects = NULL;
			OverrideView_t m_OverrideView = NULL;
			SendNetMessage_t m_SendNetMessage = NULL;
			CalcView_t m_CalcView = NULL;
			GetViewmodelFov_t m_GetAlphaModulation = NULL;
			GetColorModulation_t m_GetColorModulation = NULL;
			GetViewmodelFov_t m_GetViewmodelFOV = NULL;
			ShouldDrawFog_t m_ShouldDrawFog = NULL;
			StandardBlendingRules_t m_StandardBlendingRules = NULL;
			ModifyEyePosition_t m_CalcViewmodelBob = NULL;
			SetupBones_t m_SetupBones = NULL;
			PaintTraverse_t m_PaintTraverse = NULL;
			FrameStageNotify_t m_FrameStageNotify = NULL;
			ModifyEyePosition_t m_ModifyEyePosition = NULL;
			TraceFilterForHeadCollision_t m_TraceFilterForHeadCollision = NULL;
			DoExtraBoneProcessing_t m_DoExtraBoneProcessing = NULL;
			DrawModelExecute_Studio_t m_DrawModelExecute_Studio = NULL;
			DrawModelExecute_Model_t m_DrawModelExecute_Model = NULL;
			DispatchUserMessage_t m_DispatchUserMessage = NULL;
			PerformScreenOverlay_t m_PerformScreenOverlay = NULL;
			PlayFootstepSound_t m_PlayFootstepSound = NULL;
			C_RecvHook* m_FlashDuration;
			GCRetrieveMessage_t m_GCRetrieveMessage = NULL;
			WriteUsercmdDeltaToBuffer_t m_WriteUsercmdDeltaToBuffer = NULL;
			GCSendMessage_t m_GCSendMessage = NULL;
			ListLeavesInBox_t m_ListLeavesInBox = NULL;
			GetScreenSizeAspectRatio_t m_GetScreenSizeAspectRatio = NULL;
			GetBool_t m_IsUsingDebugStaticProps = NULL;
			GetBool_t m_IsHLTV = NULL;
			GetBool_t m_IsConnected = NULL;
			GetBool_t m_IsPaused = NULL;
			GetBool_t m_InPrediction = NULL;
			GetBool_t m_SvCheats_GetBool = NULL;
			GetBool_t m_ClDoResetLatch_GetBool = NULL;
			Host_ShouldRun_t m_Host_ShouldRun = NULL;
			ViewmodelInterpolate_t m_Viewmodel_Interpolate = NULL;
		} m_Originals;
	} m_Hooks;

	struct
	{
		float_t m_flCurTime = 0.0f;
		int32_t m_nTickBase = 0;
	} m_Prediction;

	C_BasePlayer* m_LocalPlayer;
};

#ifdef __READY__
inline C_XorPtr< C_Globals > g_XoredOriginalGlobalsAddress;
#define g_Globals g_XoredOriginalGlobalsAddress.Get( )
#else
inline C_Globals g_Globals = C_Globals( );
#endif

#define TIME_TO_TICKS( flTime ) ( ( int32_t )( 0.5f + ( float_t )( ( flTime ) ) / g_Globals.m_Interfaces.m_GlobalVars->m_flIntervalPerTick ) )
#define TICKS_TO_TIME( iTick ) ( float_t )( ( iTick ) * g_Globals.m_Interfaces.m_GlobalVars->m_flIntervalPerTick )