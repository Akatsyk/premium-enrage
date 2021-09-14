#include "BoneManager.hpp"

void C_BoneManager::BuildMatrix( C_BasePlayer* pPlayer, matrix3x4_t* aMatrix, bool bSafeMatrix )
{
	std::array < C_AnimationLayer, 13 > aAnimationLayers;

	float_t flCurTime = g_Globals.m_Interfaces.m_GlobalVars->m_flCurTime;
	float_t flRealTime = g_Globals.m_Interfaces.m_GlobalVars->m_flRealTime;
	float_t flFrameTime = g_Globals.m_Interfaces.m_GlobalVars->m_flFrameTime;
	float_t flAbsFrameTime = g_Globals.m_Interfaces.m_GlobalVars->m_flAbsFrameTime;
	int32_t iFrameCount = g_Globals.m_Interfaces.m_GlobalVars->m_iFrameCount;
	int32_t iTickCount = g_Globals.m_Interfaces.m_GlobalVars->m_iTickCount;
	float_t flInterpolation = g_Globals.m_Interfaces.m_GlobalVars->m_flInterpolationAmount;

	g_Globals.m_Interfaces.m_GlobalVars->m_flCurTime = pPlayer->m_flSimulationTime( );
	g_Globals.m_Interfaces.m_GlobalVars->m_flRealTime = pPlayer->m_flSimulationTime( );
	g_Globals.m_Interfaces.m_GlobalVars->m_flFrameTime = g_Globals.m_Interfaces.m_GlobalVars->m_flIntervalPerTick;
	g_Globals.m_Interfaces.m_GlobalVars->m_flAbsFrameTime = g_Globals.m_Interfaces.m_GlobalVars->m_flIntervalPerTick;
	g_Globals.m_Interfaces.m_GlobalVars->m_iFrameCount = TIME_TO_TICKS( pPlayer->m_flSimulationTime( ) );
	g_Globals.m_Interfaces.m_GlobalVars->m_iTickCount = TIME_TO_TICKS( pPlayer->m_flSimulationTime( ) );
	g_Globals.m_Interfaces.m_GlobalVars->m_flInterpolationAmount = 0.0f;

	LPVOID pInverseKinematics = pPlayer->m_pInverseKinematics( );
	int32_t nClientEffects = pPlayer->m_nClientEffects( );
	int32_t nLastSkipFramecount = pPlayer->m_nLastSkipFramecount( );
	int32_t nOcclusionMask = pPlayer->m_nOcclusionMask( );
	int32_t nOcclusionFrame = pPlayer->m_nOcclusionFrame( );
	int32_t iEffects = pPlayer->m_fEffects( );
	bool bJiggleBones = pPlayer->m_bJiggleBones( );
	bool bMaintainSequenceTransition = pPlayer->m_bMaintainSequenceTransition( );
	Vector vecAbsOrigin = pPlayer->GetAbsOrigin( );

	int32_t iMask = BONE_USED_BY_ANYTHING;
	if ( bSafeMatrix )
		iMask = BONE_USED_BY_HITBOX;

	std::memcpy( aAnimationLayers.data( ), pPlayer->m_AnimationLayers( ), sizeof( C_AnimationLayer ) * ANIMATION_LAYER_COUNT );

	pPlayer->InvalidateBoneCache( );
	pPlayer->GetBoneAccessor( ).m_ReadableBones = NULL;
	pPlayer->GetBoneAccessor( ).m_WritableBones = NULL;

	if ( pPlayer->m_PlayerAnimStateCSGO( ) )
		pPlayer->m_PlayerAnimStateCSGO( )->m_pWeaponLast = pPlayer->m_PlayerAnimStateCSGO( )->m_pWeapon;

	pPlayer->m_nOcclusionFrame( ) = 0;
	pPlayer->m_nOcclusionMask( ) = 0;
	pPlayer->m_nLastSkipFramecount( ) = 0;

	if ( pPlayer != g_Globals.m_LocalPlayer )
		pPlayer->SetAbsoluteOrigin( pPlayer->m_vecOrigin( ) );

	pPlayer->m_fEffects( ) |= EF_NOINTERP;
	pPlayer->m_nClientEffects( ) |= EF_BRIGHTLIGHT;
	pPlayer->m_pInverseKinematics( ) = nullptr;
	pPlayer->m_bJiggleBones( ) = false;
	pPlayer->m_bMaintainSequenceTransition( ) = false;
	
	pPlayer->m_AnimationLayers( )[ ANIMATION_LAYER_LEAN ].m_flWeight = 0.0f;
	if ( bSafeMatrix )
		pPlayer->m_AnimationLayers( )[ ANIMATION_LAYER_ADJUST ].m_pOwner = NULL;
	else if ( pPlayer == g_Globals.m_LocalPlayer )
	{
		if ( pPlayer->GetSequenceActivity( pPlayer->m_AnimationLayers( )[ ANIMATION_LAYER_ADJUST ].m_nSequence ) == ACT_CSGO_IDLE_TURN_BALANCEADJUST )
		{
			pPlayer->m_AnimationLayers( )[ ANIMATION_LAYER_ADJUST ].m_flCycle = 0.0f;
			pPlayer->m_AnimationLayers( )[ ANIMATION_LAYER_ADJUST ].m_flWeight = 0.0f;
		}
	}

	g_Globals.m_AnimationData.m_bSetupBones = true;
	pPlayer->SetupBones( aMatrix, MAXSTUDIOBONES, iMask, 0.0f );
	g_Globals.m_AnimationData.m_bSetupBones = false;

	pPlayer->m_bMaintainSequenceTransition( ) = bMaintainSequenceTransition;
	pPlayer->m_pInverseKinematics( ) = pInverseKinematics;
	pPlayer->m_nClientEffects( ) = nClientEffects;
	pPlayer->m_bJiggleBones( ) = bJiggleBones;
	pPlayer->m_fEffects( ) = iEffects;
	pPlayer->m_nLastSkipFramecount( ) = nLastSkipFramecount;
	pPlayer->m_nOcclusionFrame( ) = nOcclusionFrame;
	pPlayer->m_nOcclusionMask( ) = nOcclusionMask;
	
	if ( pPlayer != g_Globals.m_LocalPlayer )
		pPlayer->SetAbsoluteOrigin( vecAbsOrigin );

	std::memcpy( pPlayer->m_AnimationLayers( ), aAnimationLayers.data( ), sizeof( C_AnimationLayer ) * ANIMATION_LAYER_COUNT );

	g_Globals.m_Interfaces.m_GlobalVars->m_flCurTime = flCurTime;
	g_Globals.m_Interfaces.m_GlobalVars->m_flRealTime = flRealTime;
	g_Globals.m_Interfaces.m_GlobalVars->m_flFrameTime = flFrameTime;
	g_Globals.m_Interfaces.m_GlobalVars->m_flAbsFrameTime = flAbsFrameTime;
	g_Globals.m_Interfaces.m_GlobalVars->m_iFrameCount = iFrameCount;
	g_Globals.m_Interfaces.m_GlobalVars->m_iTickCount = iTickCount;
	g_Globals.m_Interfaces.m_GlobalVars->m_flInterpolationAmount = flInterpolation;
}