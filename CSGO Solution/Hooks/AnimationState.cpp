#include "../Hooks.hpp"
#include "../Features/Animations/LocalAnimations.hpp"

#include "../SDK/Math/Math.hpp"
#include "../Tools/Tools.hpp"

#define FIRSTPERSON_TO_THIRDPERSON_VERTICAL_TOLERANCE_MIN 4.0f
#define FIRSTPERSON_TO_THIRDPERSON_VERTICAL_TOLERANCE_MAX 10.0f

void __fastcall C_Hooks::hkModifyEyePosition( LPVOID pEcx, uint32_t, Vector& vecInputEyePos )
{
	C_CSGOPlayerAnimationState* m_AnimationState = ( C_CSGOPlayerAnimationState* )( pEcx );
	if ( !m_AnimationState->m_pBasePlayer || !g_Globals.m_Packet.m_bInCreateMove )
		return;

	if ( !m_AnimationState->m_bLanding && m_AnimationState->m_flAnimDuckAmount == 0.0f )
	{
		m_AnimationState->m_bSmoothHeightValid = false;
		return;
	}

	Vector vecHeadPos = Vector
	(
		m_AnimationState->m_pBasePlayer->m_CachedBoneData( )[ 8 ][ 0 ][ 3 ],
		m_AnimationState->m_pBasePlayer->m_CachedBoneData( )[ 8 ][ 1 ][ 3 ],
		m_AnimationState->m_pBasePlayer->m_CachedBoneData( )[ 8 ][ 2 ][ 3 ] + 1.7f
	);

	if ( vecHeadPos.z > vecInputEyePos.z )
		return;

	float_t flLerp = Math::RemapValClamped( abs( vecInputEyePos.z - vecHeadPos.z ),
			FIRSTPERSON_TO_THIRDPERSON_VERTICAL_TOLERANCE_MIN,
			FIRSTPERSON_TO_THIRDPERSON_VERTICAL_TOLERANCE_MAX,
			0.0f, 1.0f );

	vecInputEyePos.z = Math::Lerp( flLerp, vecInputEyePos.z, vecHeadPos.z );
}

void __fastcall C_Hooks::hkSetupAliveLoop( LPVOID pEcx, uint32_t )
{
	C_CSGOPlayerAnimationState* m_AnimationState = ( C_CSGOPlayerAnimationState* )( pEcx );
	if ( !m_AnimationState->m_pBasePlayer )
		return; 

	C_AnimationLayer* m_AliveLoop = &m_AnimationState->m_pBasePlayer->m_AnimationLayers( )[ ANIMATION_LAYER_ALIVELOOP ];
	if ( !m_AliveLoop )
		return; 

	if ( m_AnimationState->m_pBasePlayer->GetSequenceActivity( m_AliveLoop->m_nSequence ) != ACT_CSGO_ALIVE_LOOP )
	{
		m_AnimationState->SetLayerSequence( m_AliveLoop, ACT_CSGO_ALIVE_LOOP );
		m_AnimationState->SetLayerCycle( m_AliveLoop, g_Tools->RandomFloat( 0.0f, 1.0f ) );
		m_AnimationState->SetLayerRate( m_AliveLoop, m_AnimationState->m_pBasePlayer->GetLayerSequenceCycleRate( m_AliveLoop, m_AliveLoop->m_nSequence ) * g_Tools->RandomFloat( 0.8, 1.1f ) );
	}
	else
	{
		float_t flRetainCycle = m_AliveLoop->m_flCycle;
		if ( m_AnimationState->m_pWeapon != m_AnimationState->m_pWeaponLast )
		{
			m_AnimationState->SetLayerSequence( m_AliveLoop, ACT_CSGO_ALIVE_LOOP );
			m_AnimationState->SetLayerCycle( m_AliveLoop, flRetainCycle );
		}
		else if ( m_AnimationState->IsLayerSequenceFinished( m_AliveLoop, m_AnimationState->m_flLastUpdateIncrement ) )
			m_AnimationState->SetLayerRate( m_AliveLoop, m_AnimationState->m_pBasePlayer->GetLayerSequenceCycleRate( m_AliveLoop, m_AliveLoop->m_nSequence ) * g_Tools->RandomFloat( 0.8, 1.1f ) );
		else
			m_AnimationState->SetLayerWeight( m_AliveLoop, Math::RemapValClamped( m_AnimationState->m_flSpeedAsPortionOfRunTopSpeed, 0.55f, 0.9f, 1.0f, 0.0f ) );
	}
	
	return m_AnimationState->IncrementLayerCycle( m_AliveLoop, true );
}