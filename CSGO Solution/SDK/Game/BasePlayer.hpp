#pragma once
#include "AnimationState.hpp"
#include "BoneAccessor.hpp"

class C_BasePlayer : public C_BaseEntity
{
public:
	PushVirtual( GetLayerSequenceCycleRate( C_AnimationLayer* AnimationLayer, int32_t iLayerSequence ), 222, float_t( __thiscall* )( void*, C_AnimationLayer*, int32_t ), AnimationLayer, iLayerSequence );

	NETVAR( m_vecVelocity,			Vector		, FNV32( "DT_BasePlayer" ), FNV32( "m_vecVelocity[0]" ) );
	NETVAR( m_vecViewOffset,		Vector		, FNV32( "DT_BasePlayer" ), FNV32( "m_vecViewOffset[0]" ) );
	NETVAR( m_aimPunchAngleVel,		Vector		, FNV32( "DT_BasePlayer" ), FNV32( "m_aimPunchAngleVel" ) );
	NETVAR( m_viewPunchAngle,		QAngle		, FNV32( "DT_BasePlayer" ), FNV32( "m_viewPunchAngle" ) );
	NETVAR( m_aimPunchAngle,		QAngle		, FNV32( "DT_BasePlayer" ), FNV32( "m_aimPunchAngle" ) );
	NETVAR( m_angEyeAngles,			QAngle		, FNV32( "DT_CSPlayer" ), FNV32( "m_angEyeAngles[0]" ) );
	NETVAR( m_nTickBase,			int32_t		, FNV32( "DT_BasePlayer" ), FNV32( "m_nTickBase" ) );
	NETVAR( m_fLifeState,			int32_t		, FNV32( "DT_BasePlayer" ), FNV32( "m_lifeState" ) );
	NETVAR( m_iHealth,				int32_t		, FNV32( "DT_BasePlayer" ), FNV32( "m_iHealth" ) );
	NETVAR(	m_iHideHUD,				int32_t		, FNV32( "DT_BasePlayer" ), FNV32( "m_iHideHUD" ) );
	NETVAR( m_fFlags,				int32_t		, FNV32( "DT_BasePlayer" ), FNV32( "m_fFlags" ) );
	NETVAR( m_iObserverMode,		int32_t		, FNV32( "DT_BasePlayer" ), FNV32( "m_iObserverMode" ) );
	NETVAR( m_ArmourValue,			int32_t		, FNV32( "DT_CSPlayer" ), FNV32( "m_ArmorValue" ) );
	NETVAR( m_iAccount,				int32_t		, FNV32( "DT_CSPlayer" ), FNV32( "m_iAccount" ) );
	NETVAR( m_iMoveState,			int32_t		, FNV32( "DT_CSPlayer" ), FNV32( "m_iMoveState" ) );
	NETVAR(	m_flFallVelocity,		float_t		, FNV32( "DT_BasePlayer" ), FNV32( "m_flFallVelocity" ) );
	NETVAR( m_flLowerBodyYaw,		float_t		, FNV32( "DT_CSPlayer" ), FNV32( "m_flLowerBodyYawTarget" ) );
	NETVAR( m_flVelocityModifier,	float_t		, FNV32( "DT_CSPlayer" ), FNV32( "m_flVelocityModifier" ) );
	NETVAR( m_flThirdpersonRecoil,	float_t		, FNV32( "DT_CSPlayer" ), FNV32( "m_flThirdpersonRecoil" ) );
	NETVAR(	m_flNextAttack,			float_t		, FNV32( "DT_BaseCombatCharacter" ), FNV32( "m_flNextAttack" ) );
	NETVAR( m_flDuckSpeed,			float_t		, FNV32( "DT_BasePlayer" ), FNV32( "m_flDuckSpeed" ) );
	NETVAR( m_flDuckAmount,			float_t		, FNV32( "DT_BasePlayer" ), FNV32( "m_flDuckAmount" ) );
	NETVAR( m_hGroundEntity,		CHandle < C_BaseEntity >, FNV32( "DT_BasePlayer" ), FNV32( "m_hGroundEntity" ) );
	NETVAR( m_hActiveWeapon,		CHandle < C_BaseCombatWeapon >, FNV32( "DT_BaseCombatCharacter" ), FNV32( "m_hActiveWeapon" ) );
	NETVAR( m_hViewModel,			CHandle < C_BaseViewModel >, FNV32( "DT_BasePlayer" ), FNV32( "m_hViewModel[0]" ) );
	NETVAR( m_bHasHeavyArmor,		bool		, FNV32( "DT_CSPlayer" ), FNV32( "m_bHasHeavyArmor" ) );
	NETVAR( m_bHasHelmet,			bool		, FNV32( "DT_CSPlayer" ), FNV32( "m_bHasHelmet" ) );
	NETVAR( m_bIsScoped,			bool		, FNV32( "DT_CSPlayer" ), FNV32( "m_bIsScoped" ) );
	NETVAR( m_bGunGameImmunity,		bool		, FNV32( "DT_CSPlayer" ), FNV32( "m_bGunGameImmunity" ) );
	NETVAR( m_bIsWalking,			bool		, FNV32( "DT_CSPlayer" ), FNV32( "m_bIsWalking" ) );
	NETVAR( m_bStrafing,			bool		, FNV32( "DT_CSPlayer" ), FNV32( "m_bStrafing" ) );
	NETVAR( m_bClientSideAnimation,	bool		, FNV32( "DT_BaseAnimating" ), FNV32( "m_bClientSideAnimation" ) );
	NETVAR( m_nHitboxSet,			int32_t		, FNV32( "DT_BaseAnimating" ), FNV32( "m_nHitboxSet" ) );
	NETVAR( m_szLastPlaceName	,	const char*	, FNV32( "DT_BasePlayer" ), FNV32( "m_szLastPlaceName" ) );
	
	PNETVAR( CHandle< C_BaseCombatWeapon >, m_hMyWeapons, FNV32( "DT_BaseCombatCharacter" ), FNV32( "m_hMyWeapons" ) );
	PNETVAR( CHandle< C_BaseAttributableItem >, m_hMyWearables, FNV32( "DT_BaseCombatCharacter" ), FNV32( "m_hMyWearables" ) );

	__forceinline void UpdateClientSideAnimation( )
	{
		return ( ( void( __thiscall* )( LPVOID ) )( g_Globals.m_AddressList.m_UpdateClientSideAnimation ) )( this );
	}

	__forceinline void InvalidatePhysicsRecursive( int32_t iFlags )
	{
		( ( void( __thiscall* )( LPVOID, int32_t ) )( g_Globals.m_AddressList.m_InvalidatePhysicsRecursive ) )( this, iFlags );
	}

	__forceinline mstudioseqdesc_t& GetSequenceDescription( int32_t iSequence )
	{
		return *( ( mstudioseqdesc_t*( __thiscall* )( void*, int32_t ) )( g_Globals.m_AddressList.m_SequenceDescriptor ) )( this->GetStudioHdr( ), iSequence );
	}

	__forceinline int32_t GetFirstSequenceAnimationTag( int32_t iSequence, int32_t iAnimationTag )
	{
		return ( ( int32_t( __thiscall* )( void*, int, int, int ) )( g_Globals.m_AddressList.m_GetFirstSequenceAnimationTag ) )( this, iSequence, iAnimationTag, 1 );
	}

	__forceinline float_t SequenceDuration( int32_t iSequence )
	{
		return ( ( float_t( __thiscall* )( void*, int32_t ) )( g_Globals.m_AddressList.m_SequenceDuration ) )( this, iSequence );
	}

	__forceinline void SetupBones_AttachmentHelper( )
	{
		return ( ( void( __thiscall* )( LPVOID, LPVOID ) )( g_Globals.m_AddressList.m_SetupBones_AttachmentHelper ) )( this, this->GetStudioHdr( ) );
	}

	__forceinline int32_t GetSequenceActivity( int32_t iSequence )
	{
		studiohdr_t* pStudioHDR = g_Globals.m_Interfaces.m_ModelInfo->GetStudiomodel( this->GetModel( ) );
		if ( !pStudioHDR )
			return -1;

		return ( ( int32_t( __fastcall* )( void*, void*, int ) )( g_Globals.m_AddressList.m_GetSequenceActivity ) )( this, pStudioHDR, iSequence );
	}


	DATAMAP( float_t, m_surfaceFriction );
	__forceinline float_t C_BasePlayer::GetMaxPlayerSpeed( )
	{
		// get max speed of player from his weapon
		C_BaseCombatWeapon* pWeapon = this->m_hActiveWeapon( ).Get( );
		if ( pWeapon )
		{
			C_CSWeaponData* pWeaponData = pWeapon->GetWeaponData( );
			if ( pWeaponData )
				return this->m_bIsScoped( ) ? pWeaponData->m_flMaxPlayerSpeedAlt : pWeaponData->m_flMaxPlayerSpeed;
		}

		return 260.0f;
	}

	__forceinline bool CanFire( int32_t ShiftAmount = 0, bool bCheckRevolver = false )
	{
		C_BaseCombatWeapon* pCombatWeapon = this->m_hActiveWeapon( ).Get( );
		if ( !pCombatWeapon || ( pCombatWeapon->m_iItemDefinitionIndex( ) != WEAPON_TASER && !pCombatWeapon->IsGun( ) ) )
			return true;

		float_t flServerTime = ( this->m_nTickBase( ) - ShiftAmount ) * g_Globals.m_Interfaces.m_GlobalVars->m_flIntervalPerTick;
		if ( pCombatWeapon->m_iClip1( ) <= 0 )
			return false;

		if ( bCheckRevolver )
			if ( pCombatWeapon->m_flPostponeFireReadyTime( ) >= flServerTime )
				return false;	
		
		if ( this->m_flNextAttack( ) > flServerTime )
			return false;
		
		return pCombatWeapon->m_flNextPrimaryAttack( ) <= flServerTime;
	}

	Vector WorldSpaceCenter( )
	{
		Vector vecOrigin = m_vecOrigin( );

		Vector vecMins = this->GetCollideable( )->OBBMins( ) + vecOrigin;
		Vector vecMaxs = this->GetCollideable( )->OBBMaxs( ) + vecOrigin;

		Vector vecSize = vecMaxs - vecMins;
		vecSize /= 2.0f;
		vecSize += vecMins;
		return vecSize;
	}

	__forceinline int32_t& m_nFinalPredictedTick( )
	{
		return *( int32_t* )( ( DWORD )( this ) + 0x3434 );
	}

	static C_BasePlayer* GetPlayerByIndex( int32_t index )
	{
		return static_cast < C_BasePlayer* > ( g_Globals.m_Interfaces.m_EntityList->GetClientEntity( index ) );
	}

	bool IsAlive( )
	{
		return this->m_fLifeState( ) == LIFE_ALIVE && this->m_iHealth( ) > 0;
	}
	
	typedef void( __thiscall* GetShootPosition_t )( LPVOID, Vector* );
	Vector GetShootPosition( )
	{
		Vector vecShootPosition = Vector( 0, 0, 0 );
		if ( !this )
			return vecShootPosition;

		GetVirtual < GetShootPosition_t >( this, 284 )( this, &vecShootPosition );
		return vecShootPosition;
	}

	__forceinline void InvalidateBoneCache( )
	{
		int32_t iModelBoneCounter = **( int32_t** )( ( ( DWORD )( g_Globals.m_AddressList.m_InvalidateBoneCache ) ) + 10 );

		*( uintptr_t* )( ( DWORD ) this + 0x2924 ) = 0xFF7FFFFF;
		*( uintptr_t* )( ( DWORD ) this + 0x2690 ) = iModelBoneCounter - 1;
		*( uintptr_t* )( ( DWORD ) this + 0x26AC ) = 0;
	}

	CUSTOM_OFFSET( m_CachedBoneData, CUtlVector < matrix3x4_t >, FNV32( "CachedBoneData" ), 10512 );
	CUSTOM_OFFSET( GetBoneAccessor, C_BoneAccessor, FNV32( "BoneAccessor" ), 9896 );
	CUSTOM_OFFSET( m_angVisualAngles, QAngle, FNV32( "VisualAngles" ), 12760 );
	CUSTOM_OFFSET( m_flSpawnTime, float_t, FNV32( "SpawnTime" ), 41840 );
	CUSTOM_OFFSET( GetMoveType, int32_t, FNV32( "MoveType" ), 604 );
	CUSTOM_OFFSET( m_nClientEffects, int32_t, FNV32( "ClientEffects" ), 68 );
	CUSTOM_OFFSET( m_nLastSkipFramecount, int32_t, FNV32( "LastSkipFramecount" ), 2664 );
	CUSTOM_OFFSET( m_nOcclusionFrame, int32_t, FNV32( "OcclusionFrame" ), 2608 );
	CUSTOM_OFFSET( m_nOcclusionMask, int32_t, FNV32( "OcclusionMask" ), 2600 );
	CUSTOM_OFFSET( m_pInverseKinematics, LPVOID, FNV32( "InverseKinematics" ), 9840 );
	CUSTOM_OFFSET( m_bJiggleBones, bool, FNV32( "JiggleBones" ), 10540 );
	CUSTOM_OFFSET( m_bMaintainSequenceTransition, bool, FNV32( "MaintainSequenceTransition" ), 2544 );

	C_StudioHDR* GetStudioHdr( )
	{
		return *( C_StudioHDR** )( ( DWORD )( this ) + 10572 );
	}

	C_AnimationLayer* m_AnimationLayers( )
	{
		return *( C_AnimationLayer** )( ( DWORD )( this ) + 0x2980 );
	}

	C_CSGOPlayerAnimationState* m_PlayerAnimStateCSGO( )
	{
		return *( C_CSGOPlayerAnimationState** )( ( DWORD )( this ) + 14612 );
	}

	std::array < float_t, 24 >& m_aPoseParameters( )
	{
#ifdef __READY__

		volatile uint32_t uRor = 0xDEADC0DE;\
		volatile uint32_t uKey1 = FNV32( "DT_BaseAnimating" );\
		volatile uint32_t uKey2 = FNV32( "m_flPoseParameter" );\
		return *( std::array < float_t, 24 >* )( ( uintptr_t )( this ) + ( _rotl( uKey2, uRor ) ^ _byteswap_ulong( uKey1 ) ) ); \

#else

		static uint32_t g_PoseParameter = 0; \
		if ( !g_PoseParameter )\
			g_PoseParameter = g_NetvarManager->GetNetvar( FNV32( "DT_BaseAnimating" ), FNV32( "m_flPoseParameter" ) ); \
		return *( std::array < float_t, 24 >* )( ( uintptr_t )( this ) + 0x2774 );

#endif
	}
};