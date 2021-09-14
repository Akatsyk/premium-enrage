#include "../Includes.hpp"
#include "../SDK/Math/Math.hpp"

void C_CSGOPlayerAnimationState::IncrementLayerCycle( C_AnimationLayer* Layer, bool bIsLoop )
{
	float_t flNewCycle = ( Layer->m_flPlaybackRate * this->m_flLastUpdateIncrement ) + Layer->m_flCycle;
	if ( !bIsLoop && flNewCycle >= 1.0f )
		flNewCycle = 0.999f;

	flNewCycle -= ( int32_t )( flNewCycle );
	if ( flNewCycle < 0.0f )
		flNewCycle += 1.0f;
		
	if ( flNewCycle > 1.0f )
		flNewCycle -= 1.0f;

	Layer->m_flCycle = flNewCycle;
}

bool C_CSGOPlayerAnimationState::IsLayerSequenceFinished( C_AnimationLayer* Layer, float_t flTime )
{
	return ( Layer->m_flPlaybackRate * flTime ) + Layer->m_flCycle >= 1.0f;
}

void C_CSGOPlayerAnimationState::SetLayerCycle( C_AnimationLayer* pAnimationLayer, float_t flCycle )
{
	if ( pAnimationLayer )
		pAnimationLayer->m_flCycle = flCycle;
}

void C_CSGOPlayerAnimationState::SetLayerRate( C_AnimationLayer* pAnimationLayer, float_t flRate )
{
	if ( pAnimationLayer )
		pAnimationLayer->m_flPlaybackRate = flRate;
}

void C_CSGOPlayerAnimationState::SetLayerWeight( C_AnimationLayer* pAnimationLayer, float_t flWeight )
{
	if ( pAnimationLayer )
		pAnimationLayer->m_flWeight = flWeight;
}

void C_CSGOPlayerAnimationState::SetLayerWeightRate( C_AnimationLayer* pAnimationLayer, float_t flPrevious )
{
	if ( pAnimationLayer )
		pAnimationLayer->m_flWeightDeltaRate = ( pAnimationLayer->m_flWeight - flPrevious ) / m_flLastUpdateIncrement;
}

void C_CSGOPlayerAnimationState::SetLayerSequence( C_AnimationLayer* pAnimationLayer, int32_t iActivity )
{
	int32_t iSequence = this->SelectSequenceFromActivityModifier( iActivity );
	if ( iSequence < 2 )
		return;

	pAnimationLayer->m_flCycle = 0.0f;
	pAnimationLayer->m_flWeight = 0.0f;
	pAnimationLayer->m_nSequence = iSequence;
	pAnimationLayer->m_flPlaybackRate = m_pBasePlayer->GetLayerSequenceCycleRate( pAnimationLayer, iSequence );
}

float_t C_CSGOPlayerAnimationState::GetLayerIdealWeightFromSeqCycle( C_AnimationLayer* pAnimationLayer )
{
	float flCycle = pAnimationLayer->m_flCycle;
	if ( flCycle >= 0.999f )
		flCycle = 1;

	float flEaseIn = 0.2f;
	float flEaseOut = 0.2f;
	float flIdealWeight = 1;
	
	if ( flEaseIn > 0 && flCycle < flEaseIn )
	{
		flIdealWeight = Math::SmoothstepBounds( 0, flEaseIn, flCycle );
	}
	else if ( flEaseOut < 1 && flCycle > flEaseOut )
	{
		flIdealWeight = Math::SmoothstepBounds( 1.0f, flEaseOut, flCycle );
	}

	if ( flIdealWeight < 0.0015f )
		return 0;

	return std::clamp( flIdealWeight, 0.0f, 1.0f );
}


void C_CSGOPlayerAnimationState::IncrementLayerCycleWeightRateGeneric( C_AnimationLayer* pAnimationLayer )
{
	float flWeightPrevious = pAnimationLayer->m_flWeight;;
	IncrementLayerCycle( pAnimationLayer, false );
	SetLayerWeight( pAnimationLayer, GetLayerIdealWeightFromSeqCycle( pAnimationLayer ) );
	SetLayerWeightRate( pAnimationLayer, flWeightPrevious );
}

int32_t C_CSGOPlayerAnimationState::SelectSequenceFromActivityModifier( int32_t iActivity )
{
	bool bIsPlayerDucked = m_flAnimDuckAmount > 0.55f;
	bool bIsPlayerRunning = m_flSpeedAsPortionOfWalkTopSpeed > 0.25f;

	int32_t iLayerSequence = -1;
	switch ( iActivity )
	{
		case ACT_CSGO_JUMP:
		{
			iLayerSequence = 15 + static_cast < int32_t >( bIsPlayerRunning );
			if ( bIsPlayerDucked )
				iLayerSequence = 17 + static_cast < int32_t >( bIsPlayerRunning );
		}
		break;

		case ACT_CSGO_ALIVE_LOOP:
		{
			iLayerSequence = 8;
			if ( m_pWeaponLast != m_pWeapon )
				iLayerSequence = 9;
		}
		break;

		case ACT_CSGO_IDLE_ADJUST_STOPPEDMOVING:
		{
			iLayerSequence = 6;
		}
		break;

		case ACT_CSGO_FALL:
		{
			iLayerSequence = 14;
		}
		break;

		case ACT_CSGO_IDLE_TURN_BALANCEADJUST:
		{
			iLayerSequence = 4;
		}
		break;

		case ACT_CSGO_LAND_LIGHT:
		{
			iLayerSequence = 20;
			if ( bIsPlayerRunning )
				iLayerSequence = 22;

			if ( bIsPlayerDucked )
			{
				iLayerSequence = 21;
				if ( bIsPlayerRunning )
					iLayerSequence = 19;
			}
		}
		break;

		case ACT_CSGO_LAND_HEAVY:
		{
			iLayerSequence = 23;
			if ( bIsPlayerDucked )
				iLayerSequence = 24;
		}
		break;

		case ACT_CSGO_CLIMB_LADDER:
		{
			iLayerSequence = 13;
		}
		break;
		default: break;
	}

	return iLayerSequence;
}