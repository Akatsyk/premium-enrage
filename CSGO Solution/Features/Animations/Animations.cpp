#include "Animations.hpp"
#include "BoneManager.hpp"
#include "../SDK/Math/Math.hpp"

void C_AnimationSync::Instance( ClientFrameStage_t Stage )
{
	if ( Stage != ClientFrameStage_t::FRAME_NET_UPDATE_END )
		return;

	for ( int32_t iPlayerID = 1; iPlayerID <= g_Globals.m_Interfaces.m_GlobalVars->m_iMaxClients; iPlayerID++ )
	{
		C_BasePlayer* pPlayer = C_BasePlayer::GetPlayerByIndex( iPlayerID );
		if ( !pPlayer || !pPlayer->IsPlayer( ) || !pPlayer->IsAlive( ) || pPlayer->m_iTeamNum( ) == g_Globals.m_LocalPlayer->m_iTeamNum( ) )
		{
			g_Globals.m_ResolverData.m_AnimResoled[ iPlayerID ] = false;
			g_Globals.m_ResolverData.m_MissedShots[ iPlayerID ] = 0;
			g_Globals.m_ResolverData.m_LastMissedShots[ iPlayerID ] = 0;

			continue;
		}

		bool bHasPreviousRecord = false;
		if ( pPlayer->m_flOldSimulationTime( ) >= pPlayer->m_flSimulationTime( ) )
		{
			if ( pPlayer->m_flOldSimulationTime( ) > pPlayer->m_flSimulationTime( ) )
				this->UnmarkAsDormant( iPlayerID );

			continue;
		}

		auto& LagRecords = g_Globals.m_CachedPlayerRecords[ iPlayerID ];
		if ( LagRecords.empty( ) )
			continue;

		C_LagRecord PreviousRecord = m_PreviousRecord[ iPlayerID ];
		if ( TIME_TO_TICKS( fabs( pPlayer->m_flSimulationTime( ) - PreviousRecord.m_SimulationTime ) ) <= 17 )
			bHasPreviousRecord = true;

		C_LagRecord& LatestRecord = LagRecords.back( );
		if ( this->HasLeftOutOfDormancy( iPlayerID ) )
			bHasPreviousRecord = false;

		if ( LatestRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_ALIVELOOP ).m_flCycle == PreviousRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_ALIVELOOP ).m_flCycle )
		{
			pPlayer->m_flSimulationTime( ) = pPlayer->m_flOldSimulationTime( );
			continue;
		}

		LatestRecord.m_UpdateDelay = TIME_TO_TICKS( pPlayer->m_flSimulationTime( ) - this->GetPreviousRecord( iPlayerID ).m_SimulationTime );
		if ( LatestRecord.m_UpdateDelay > 17 )
			LatestRecord.m_UpdateDelay = 1;

		C_PlayerInfo PlayerInfo;
		g_Globals.m_Interfaces.m_EngineClient->GetPlayerInfo( iPlayerID, &PlayerInfo );

		if ( PlayerInfo.m_bIsFakePlayer || LatestRecord.m_UpdateDelay < 1 )
			LatestRecord.m_UpdateDelay = 1;
		
		Vector vecVelocity = LatestRecord.m_Velocity;
		if ( bHasPreviousRecord )
		{
			float_t flCurrentCycle = LatestRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_ALIVELOOP ).m_flCycle;
			float_t flPreviousCycle = this->GetPreviousRecord( pPlayer->EntIndex( ) ).m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_ALIVELOOP ).m_flCycle;
			if ( flCurrentCycle != flPreviousCycle )
			{
				float_t flPreviousRate = this->GetPreviousRecord( pPlayer->EntIndex( ) ).m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_ALIVELOOP ).m_flPlaybackRate;
				float_t flCurrentRate = LatestRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_ALIVELOOP ).m_flPlaybackRate;

				if ( flPreviousRate == flCurrentRate )
				{
					if ( flPreviousCycle > flCurrentCycle )
						flCurrentCycle += 1.0f;

					int32_t iCycleTiming = TIME_TO_TICKS( flCurrentCycle - flPreviousCycle );
					if ( iCycleTiming <= 17 )
						if ( iCycleTiming > LatestRecord.m_UpdateDelay )
							LatestRecord.m_UpdateDelay = iCycleTiming;
				}
			}

			if ( LatestRecord.m_UpdateDelay > 17 )
				LatestRecord.m_UpdateDelay = 1;

			if ( PlayerInfo.m_bIsFakePlayer || LatestRecord.m_UpdateDelay < 1 )
				LatestRecord.m_UpdateDelay = 1;

			if ( !( LatestRecord.m_Flags & FL_ONGROUND ) )
			{
				vecVelocity = ( LatestRecord.m_Origin - this->GetPreviousRecord( pPlayer->EntIndex( ) ).m_Origin ) / LatestRecord.m_UpdateDelay;

				float_t flWeight = 1.0f - LatestRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_ALIVELOOP ).m_flWeight;
				if ( flWeight > 0.0f )
				{
					float_t flPreviousRate = this->GetPreviousRecord( pPlayer->EntIndex( ) ).m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_ALIVELOOP ).m_flPlaybackRate;
					float_t flCurrentRate = LatestRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_ALIVELOOP ).m_flPlaybackRate;

					if ( flPreviousRate == flCurrentRate )
					{
						int32_t iPreviousSequence = this->GetPreviousRecord( pPlayer->EntIndex( ) ).m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_ALIVELOOP ).m_nSequence;
						int32_t iCurrentSequence = LatestRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_ALIVELOOP ).m_nSequence;

						if ( iPreviousSequence == iCurrentSequence )
						{
							float_t flSpeedNormalized = ( flWeight / 2.8571432f ) + 0.55f;
							if ( flSpeedNormalized > 0.0f )
							{
								float_t flSpeed = flSpeedNormalized * pPlayer->GetMaxPlayerSpeed( );
								if ( flSpeed > 0.0f )
								{
									if ( vecVelocity.Length2D( ) > 0.0f )
									{
										vecVelocity.x /= vecVelocity.Length2D( ) / flSpeed;
										vecVelocity.y /= vecVelocity.Length2D( ) / flSpeed;
									}
								}
							}
						}
					} 
				}
				
				vecVelocity.z -= g_Globals.m_ConVars.m_SvGravity->GetFloat( ) * 0.5f * TICKS_TO_TIME( LatestRecord.m_UpdateDelay );
			}
			else
				vecVelocity.z = 0.0f;
		}
			
		LatestRecord.m_Velocity = vecVelocity;
			
		std::array < C_AnimationLayer, ANIMATION_LAYER_COUNT > AnimationLayers;
		std::array < float_t, MAXSTUDIOPOSEPARAM > PoseParameters;
		C_CSGOPlayerAnimationState AnimationState;

		std::memcpy( AnimationLayers.data( ), pPlayer->m_AnimationLayers( ), sizeof( C_AnimationLayer ) * ANIMATION_LAYER_COUNT );
		std::memcpy( PoseParameters.data( ), pPlayer->m_aPoseParameters( ).data( ), sizeof( float_t ) * MAXSTUDIOPOSEPARAM );
		std::memcpy( &AnimationState, pPlayer->m_PlayerAnimStateCSGO( ), sizeof( AnimationState ) );

		for ( int32_t i = ROTATE_LEFT; i <= ROTATE_LOW_RIGHT; i++ )
		{
			this->UpdatePlayerAnimations( pPlayer, LatestRecord, PreviousRecord, bHasPreviousRecord, i );

			std::memcpy( LatestRecord.m_AnimationLayers.at( i ).data( ), pPlayer->m_AnimationLayers( ), sizeof( C_AnimationLayer ) * ANIMATION_LAYER_COUNT );

			std::memcpy( pPlayer->m_AnimationLayers( ), AnimationLayers.data( ), sizeof( C_AnimationLayer ) * ANIMATION_LAYER_COUNT );

			if ( i < ROTATE_LOW_LEFT )
				g_BoneManager->BuildMatrix( pPlayer, LatestRecord.m_Matricies[ i ].data( ), true );

			std::memcpy( pPlayer->m_aPoseParameters( ).data( ), PoseParameters.data( ), sizeof( float_t ) * MAXSTUDIOPOSEPARAM );
			std::memcpy( pPlayer->m_PlayerAnimStateCSGO( ), &AnimationState, sizeof( AnimationState ) );
		}

		if ( !LatestRecord.m_bIsShooting )
		{
			if ( LatestRecord.m_UpdateDelay > 1 && bHasPreviousRecord )
			{
				LatestRecord.m_RotationMode = g_Globals.m_ResolverData.m_LastBruteSide[ iPlayerID ];
				if ( LatestRecord.m_Velocity.Length2D( ) > 1.0f )
				{
					float_t flLeftDelta = fabsf( LatestRecord.m_AnimationLayers.at( ROTATE_LEFT ).at( ANIMATION_LAYER_MOVEMENT_MOVE ).m_flPlaybackRate - LatestRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_MOVEMENT_MOVE ).m_flPlaybackRate );
					float_t flLowLeftDelta = fabsf( LatestRecord.m_AnimationLayers.at( ROTATE_LOW_LEFT ).at( ANIMATION_LAYER_MOVEMENT_MOVE ).m_flPlaybackRate - LatestRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_MOVEMENT_MOVE ).m_flPlaybackRate );
					float_t flLowRightDelta = fabsf( LatestRecord.m_AnimationLayers.at( ROTATE_LOW_RIGHT ).at( ANIMATION_LAYER_MOVEMENT_MOVE ).m_flPlaybackRate - LatestRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_MOVEMENT_MOVE ).m_flPlaybackRate );
					float_t flRightDelta = fabsf( LatestRecord.m_AnimationLayers.at( ROTATE_RIGHT ).at( ANIMATION_LAYER_MOVEMENT_MOVE ).m_flPlaybackRate - LatestRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_MOVEMENT_MOVE ).m_flPlaybackRate );
					float_t flCenterDelta = fabsf( LatestRecord.m_AnimationLayers.at( ROTATE_CENTER ).at( ANIMATION_LAYER_MOVEMENT_MOVE ).m_flPlaybackRate - LatestRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_MOVEMENT_MOVE ).m_flPlaybackRate );
				
					LatestRecord.m_bAnimResolved = false;
					{
						int RotationMode = LatestRecord.m_RotationMode;
						if ( flLeftDelta > flCenterDelta && flLeftDelta > flLowLeftDelta )
							LatestRecord.m_RotationMode = ROTATE_LEFT;

						if ( flRightDelta > flCenterDelta && flRightDelta > flLowRightDelta )
							LatestRecord.m_RotationMode = ROTATE_RIGHT;

						if ( flLowLeftDelta > flLeftDelta && flLowLeftDelta > flCenterDelta )
							LatestRecord.m_RotationMode = ROTATE_LOW_LEFT;

						if ( flLowRightDelta > flRightDelta && flLowRightDelta > flCenterDelta )
							LatestRecord.m_RotationMode = ROTATE_LOW_RIGHT;
					
						if ( RotationMode != LatestRecord.m_RotationMode )
							LatestRecord.m_bAnimResolved = true;
					}
			
					bool bIsValidResolved = true;
					if ( bHasPreviousRecord )
					{
						if ( fabs( ( LatestRecord.m_Velocity - PreviousRecord.m_Velocity ).Length2D( ) ) > 5.0f || PreviousRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_MOVEMENT_STRAFECHANGE ).m_flWeight < 1.0f )
							bIsValidResolved = false;
					}

					g_Globals.m_ResolverData.m_AnimResoled[ iPlayerID ] = LatestRecord.m_bAnimResolved;
					if ( LatestRecord.m_bAnimResolved && LatestRecord.m_RotationMode != ROTATE_SERVER )
						g_Globals.m_ResolverData.m_LastBruteSide[ iPlayerID ] = LatestRecord.m_RotationMode;
			
					if ( LatestRecord.m_RotationMode == ROTATE_SERVER )
						LatestRecord.m_RotationMode = g_Globals.m_ResolverData.m_LastBruteSide[ iPlayerID ];
				}
				else
				{
					if ( g_Globals.m_ResolverData.m_LastBruteSide[ iPlayerID ] < 0 )
					{
						float_t flFeetDelta = Math::AngleNormalize( Math::AngleDiff( Math::AngleNormalize( pPlayer->m_flLowerBodyYaw( ) ), Math::AngleNormalize( pPlayer->m_angEyeAngles( ).yaw ) ) );
						if ( flFeetDelta > 0.0f )
							LatestRecord.m_RotationMode = ROTATE_LEFT;
						else
							LatestRecord.m_RotationMode = ROTATE_RIGHT;

						g_Globals.m_ResolverData.m_LastBruteSide[ iPlayerID ] = LatestRecord.m_RotationMode;
					}
				}

				g_Globals.m_ResolverData.m_AnimResoled[ iPlayerID ] = LatestRecord.m_bAnimResolved;
				if ( LatestRecord.m_RotationMode == g_Globals.m_ResolverData.m_BruteSide[ iPlayerID ] )
				{
					if ( g_Globals.m_ResolverData.m_MissedShots[ iPlayerID ] > 0 )
					{
						int iNewRotation = 0;
						switch ( LatestRecord.m_RotationMode )
						{
							case ROTATE_LEFT: iNewRotation = ROTATE_RIGHT; break;
							case ROTATE_RIGHT: iNewRotation = ROTATE_LEFT; break;
							case ROTATE_LOW_RIGHT: iNewRotation = ROTATE_LOW_LEFT; break;
							case ROTATE_LOW_LEFT: iNewRotation = ROTATE_LOW_RIGHT; break;
						}

						LatestRecord.m_RotationMode = iNewRotation;
					}
				}

				this->UpdatePlayerAnimations( pPlayer, LatestRecord, PreviousRecord, bHasPreviousRecord, LatestRecord.m_RotationMode );
			}
			else
				this->UpdatePlayerAnimations( pPlayer, LatestRecord, PreviousRecord, bHasPreviousRecord, ROTATE_SERVER );
		}
		else
			this->UpdatePlayerAnimations( pPlayer, LatestRecord, PreviousRecord, bHasPreviousRecord, ROTATE_SERVER );

		std::memcpy( pPlayer->m_AnimationLayers( ), AnimationLayers.data( ), sizeof( C_AnimationLayer ) * ANIMATION_LAYER_COUNT );

		std::memcpy( LatestRecord.m_PoseParameters.data( ), pPlayer->m_aPoseParameters( ).data( ), sizeof( float_t ) * MAXSTUDIOPOSEPARAM );

		g_BoneManager->BuildMatrix( pPlayer, LatestRecord.m_Matricies[ ROTATE_SERVER ].data( ), false );

		for ( int i = 0; i < MAXSTUDIOBONES; i++ )
			m_BoneOrigins[ iPlayerID ][ i ] = pPlayer->GetAbsOrigin( ) - LatestRecord.m_Matricies[ ROTATE_SERVER ][ i ].GetOrigin( );

		std::memcpy( m_CachedMatrix[ iPlayerID ].data( ), LatestRecord.m_Matricies[ ROTATE_SERVER ].data( ), sizeof( matrix3x4_t ) * MAXSTUDIOBONES );
		
		this->UnmarkAsDormant( iPlayerID );
	}
}

void C_AnimationSync::UpdatePlayerAnimations( C_BasePlayer* pPlayer, C_LagRecord& LagRecord, C_LagRecord PreviousRecord, bool bHasPreviousRecord, int32_t iRotationMode )
{
	float_t flCurTime = g_Globals.m_Interfaces.m_GlobalVars->m_flCurTime;
	float_t flRealTime = g_Globals.m_Interfaces.m_GlobalVars->m_flRealTime;
	float_t flAbsFrameTime = g_Globals.m_Interfaces.m_GlobalVars->m_flAbsFrameTime;
	float_t flFrameTime = g_Globals.m_Interfaces.m_GlobalVars->m_flFrameTime;
	float_t iFrameCount = g_Globals.m_Interfaces.m_GlobalVars->m_iFrameCount;
	float_t iTickCount = g_Globals.m_Interfaces.m_GlobalVars->m_iTickCount;
	float_t flInterpolationAmount = g_Globals.m_Interfaces.m_GlobalVars->m_flInterpolationAmount;

	float_t flLowerBodyYaw = LagRecord.m_LowerBodyYaw;
	float_t flDuckAmount = LagRecord.m_DuckAmount;
	int32_t iFlags = LagRecord.m_Flags;
	int32_t iEFlags = pPlayer->m_iEFlags( );
	
	if ( this->HasLeftOutOfDormancy( pPlayer->EntIndex( ) ) )
	{
		float_t flLastUpdateTime = LagRecord.m_SimulationTime - g_Globals.m_Interfaces.m_GlobalVars->m_flIntervalPerTick;
		if ( pPlayer->m_fFlags( ) & FL_ONGROUND )
		{
			pPlayer->m_PlayerAnimStateCSGO( )->m_bLanding = false;
			pPlayer->m_PlayerAnimStateCSGO( )->m_bOnGround = true;

			float_t flLandTime = 0.0f;
			if ( LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB ).m_flCycle > 0.0f && 
				 LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB ).m_flPlaybackRate > 0.0f )
			{ 
				int32_t iLandActivity = pPlayer->GetSequenceActivity( LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB ).m_nSequence );
				if ( iLandActivity == ACT_CSGO_LAND_LIGHT || iLandActivity == ACT_CSGO_LAND_HEAVY )
				{
					flLandTime = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB ).m_flCycle / LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB ).m_flPlaybackRate;
					if ( flLandTime > 0.0f )
						flLastUpdateTime = LagRecord.m_SimulationTime - flLandTime;
				}
			}

			LagRecord.m_Velocity.z = 0.0f;
		}
		else
		{
			float_t flJumpTime = 0.0f;
			if ( LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL ).m_flCycle > 0.0f && 
				 LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL ).m_flPlaybackRate > 0.0f )
			{ 
				int32_t iJumpActivity = pPlayer->GetSequenceActivity( LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL ).m_nSequence );
				if ( iJumpActivity == ACT_CSGO_JUMP )
				{
					flJumpTime = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL ).m_flCycle / LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL ).m_flPlaybackRate;
					if ( flJumpTime > 0.0f )
						flLastUpdateTime = LagRecord.m_SimulationTime - flJumpTime;
				}
			}
			
			pPlayer->m_PlayerAnimStateCSGO( )->m_bOnGround = false;
			pPlayer->m_PlayerAnimStateCSGO( )->m_flDurationInAir = flJumpTime - g_Globals.m_Interfaces.m_GlobalVars->m_flIntervalPerTick;
		}

		float_t flWeight = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_MOVEMENT_MOVE ).m_flWeight;
		if ( LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_MOVEMENT_MOVE ).m_flPlaybackRate < 0.00001f )
			LagRecord.m_Velocity.Zero( );
		else
		{
			float_t flPostVelocityLength = pPlayer->m_vecVelocity( ).Length( );
			if ( flWeight > 0.0f && flWeight < 0.95f )
		{
				float_t flMaxSpeed = pPlayer->GetMaxPlayerSpeed( );
				if ( flPostVelocityLength > 0.0f )
				{
					float_t flMaxSpeedMultiply = 1.0f;
					if ( pPlayer->m_fFlags( ) & 6 )
						flMaxSpeedMultiply = 0.34f;
					else if ( pPlayer->m_bIsWalking( ) )
						flMaxSpeedMultiply = 0.52f;

					LagRecord.m_Velocity.x = ( LagRecord.m_Velocity.x / flPostVelocityLength ) * ( flWeight * ( flMaxSpeed * flMaxSpeedMultiply ) );
					LagRecord.m_Velocity.y = ( LagRecord.m_Velocity.y / flPostVelocityLength ) * ( flWeight * ( flMaxSpeed * flMaxSpeedMultiply ) );
				}
			}
		}

		pPlayer->m_PlayerAnimStateCSGO( )->m_flLastUpdateTime = flLastUpdateTime;
	}

	if ( bHasPreviousRecord )
	{
		pPlayer->m_PlayerAnimStateCSGO( )->m_flStrafeChangeCycle = PreviousRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_MOVEMENT_STRAFECHANGE ).m_flCycle;
		pPlayer->m_PlayerAnimStateCSGO( )->m_flStrafeChangeWeight = PreviousRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_MOVEMENT_STRAFECHANGE ).m_flWeight;
		pPlayer->m_PlayerAnimStateCSGO( )->m_nStrafeSequence = PreviousRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_MOVEMENT_STRAFECHANGE ).m_nSequence;
		pPlayer->m_PlayerAnimStateCSGO( )->m_flPrimaryCycle = PreviousRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_MOVEMENT_MOVE ).m_flCycle;
		pPlayer->m_PlayerAnimStateCSGO( )->m_flMoveWeight = PreviousRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_MOVEMENT_MOVE ).m_flWeight;
		pPlayer->m_PlayerAnimStateCSGO( )->m_flAccelerationWeight = PreviousRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_LEAN ).m_flWeight;
		std::memcpy( pPlayer->m_AnimationLayers( ), PreviousRecord.m_AnimationLayers.at( ROTATE_SERVER ).data( ), sizeof( C_AnimationLayer ) * ANIMATION_LAYER_COUNT );
	}
	else
	{
		pPlayer->m_PlayerAnimStateCSGO( )->m_flStrafeChangeCycle = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_MOVEMENT_STRAFECHANGE ).m_flCycle;
		pPlayer->m_PlayerAnimStateCSGO( )->m_flStrafeChangeWeight = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_MOVEMENT_STRAFECHANGE ).m_flWeight;
		pPlayer->m_PlayerAnimStateCSGO( )->m_nStrafeSequence = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_MOVEMENT_STRAFECHANGE ).m_nSequence;
		pPlayer->m_PlayerAnimStateCSGO( )->m_flPrimaryCycle = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_MOVEMENT_MOVE ).m_flCycle;
		pPlayer->m_PlayerAnimStateCSGO( )->m_flMoveWeight = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_MOVEMENT_MOVE ).m_flWeight;
		pPlayer->m_PlayerAnimStateCSGO( )->m_flAccelerationWeight = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_LEAN ).m_flWeight;
		std::memcpy( pPlayer->m_AnimationLayers( ), LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).data( ), sizeof( C_AnimationLayer ) * ANIMATION_LAYER_COUNT );
	}

	if ( LagRecord.m_UpdateDelay > 1 )
	{
		int32_t iActivityTick = 0;
		int32_t iActivityType = 0;

		if ( LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB ).m_flWeight > 0.0f && PreviousRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB ).m_flWeight <= 0.0f )
		{
			int32_t iLandSequence = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB ).m_nSequence;
			if ( iLandSequence > 2 )
			{
				int32_t iLandActivity = pPlayer->GetSequenceActivity( iLandSequence );
				if ( iLandActivity == ACT_CSGO_LAND_LIGHT || iLandActivity == ACT_CSGO_LAND_HEAVY )
				{
					float_t flCurrentCycle = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB ).m_flCycle;
					float_t flCurrentRate = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB ).m_flPlaybackRate;
	
					if ( flCurrentCycle > 0.0f && flCurrentRate > 0.0f )
					{	
						float_t flLandTime = ( flCurrentCycle / flCurrentRate );
						if ( flLandTime > 0.0f )
						{
							iActivityTick = TIME_TO_TICKS( LagRecord.m_SimulationTime - flLandTime ) + 1;
							iActivityType = ACTIVITY_LAND;
						}
					}
				}
			}
		}

		if ( LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL ).m_flCycle > 0.0f && LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL ).m_flPlaybackRate > 0.0f )
		{
			int32_t iJumpSequence = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL ).m_nSequence;
			if ( iJumpSequence > 2 )
			{
				int32_t iJumpActivity = pPlayer->GetSequenceActivity( iJumpSequence );
				if ( iJumpActivity == ACT_CSGO_JUMP )
				{
					float_t flCurrentCycle = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL ).m_flCycle;
					float_t flCurrentRate = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL ).m_flPlaybackRate;
	
					if ( flCurrentCycle > 0.0f && flCurrentRate > 0.0f )
					{	
						float_t flJumpTime = ( flCurrentCycle / flCurrentRate );
						if ( flJumpTime > 0.0f )
						{
							iActivityTick = TIME_TO_TICKS( LagRecord.m_SimulationTime - flJumpTime ) + 1;
							iActivityType = ACTIVITY_JUMP;
						}
					}
				}
			}
		}

		if ( LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL ).m_flCycle > 0.0f )
			LagRecord.m_bJumped = true;

		for ( int32_t iSimulationTick = 1; iSimulationTick <= LagRecord.m_UpdateDelay; iSimulationTick++ )
		{
			float_t flSimulationTime = PreviousRecord.m_SimulationTime + TICKS_TO_TIME( iSimulationTick );
			g_Globals.m_Interfaces.m_GlobalVars->m_flCurTime = flSimulationTime;
			g_Globals.m_Interfaces.m_GlobalVars->m_flRealTime = flSimulationTime;
			g_Globals.m_Interfaces.m_GlobalVars->m_flFrameTime = g_Globals.m_Interfaces.m_GlobalVars->m_flIntervalPerTick;
			g_Globals.m_Interfaces.m_GlobalVars->m_flAbsFrameTime = g_Globals.m_Interfaces.m_GlobalVars->m_flIntervalPerTick;
			g_Globals.m_Interfaces.m_GlobalVars->m_iFrameCount = TIME_TO_TICKS( flSimulationTime );
			g_Globals.m_Interfaces.m_GlobalVars->m_iTickCount = TIME_TO_TICKS( flSimulationTime );
			g_Globals.m_Interfaces.m_GlobalVars->m_flInterpolationAmount = 0.0f;
			
			pPlayer->m_flDuckAmount( ) = Interpolate( PreviousRecord.m_DuckAmount, LagRecord.m_DuckAmount, iSimulationTick, LagRecord.m_UpdateDelay );
			pPlayer->m_vecVelocity( ) = Interpolate( PreviousRecord.m_Velocity, LagRecord.m_Velocity, iSimulationTick, LagRecord.m_UpdateDelay );
			pPlayer->m_vecAbsVelocity( ) = Interpolate( PreviousRecord.m_Velocity, LagRecord.m_Velocity, iSimulationTick, LagRecord.m_UpdateDelay );

			if ( iSimulationTick < LagRecord.m_UpdateDelay )
			{
				int32_t iCurrentSimulationTick = TIME_TO_TICKS( flSimulationTime );
				if ( iActivityType > ACTIVITY_NONE )
				{
					bool bIsOnGround = pPlayer->m_fFlags( ) & FL_ONGROUND;
					if ( iActivityType == ACTIVITY_JUMP )
					{
						if ( iCurrentSimulationTick == iActivityTick - 1 )
							bIsOnGround = true;
						else if ( iCurrentSimulationTick == iActivityTick )
						{
							// reset animation layer
							pPlayer->m_AnimationLayers( )[ ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL ].m_flCycle = 0.0f;
							pPlayer->m_AnimationLayers( )[ ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL ].m_nSequence = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL ).m_nSequence;
							pPlayer->m_AnimationLayers( )[ ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL ].m_flPlaybackRate = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL ).m_flPlaybackRate;

							// reset player ground state
							bIsOnGround = false;
						}
						
					}
					else if ( iActivityType == ACTIVITY_LAND )
					{
						if ( iCurrentSimulationTick == iActivityTick - 1 )
							bIsOnGround = false;
						else if ( iCurrentSimulationTick == iActivityTick )
						{
							// reset animation layer
							pPlayer->m_AnimationLayers( )[ ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB ].m_flCycle = 0.0f;
							pPlayer->m_AnimationLayers( )[ ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB ].m_nSequence = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB ).m_nSequence;
							pPlayer->m_AnimationLayers( )[ ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB ].m_flPlaybackRate = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB ).m_flPlaybackRate;

							// reset player ground state
							bIsOnGround = true;
						}
					}

					if ( bIsOnGround )
						pPlayer->m_fFlags( ) |= FL_ONGROUND;
					else
						pPlayer->m_fFlags( ) &= ~FL_ONGROUND;
				}

				if ( iRotationMode )
				{
					LagRecord.m_BruteYaw = Math::NormalizeAngle( LagRecord.m_BruteYaw );
					switch ( iRotationMode )
					{
						case ROTATE_LEFT: LagRecord.m_BruteYaw = Math::NormalizeAngle( LagRecord.m_BruteYaw - 60.0f ); break;
						case ROTATE_RIGHT: LagRecord.m_BruteYaw = Math::NormalizeAngle( LagRecord.m_BruteYaw + 60.0f ); break;
						case ROTATE_LOW_LEFT: LagRecord.m_BruteYaw = Math::NormalizeAngle( LagRecord.m_BruteYaw - 25.0f ); break;
						case ROTATE_LOW_RIGHT: LagRecord.m_BruteYaw = Math::NormalizeAngle( LagRecord.m_BruteYaw + 25.0f ); break;
					}

					pPlayer->m_PlayerAnimStateCSGO( )->m_flFootYaw = LagRecord.m_BruteYaw;
				}
			}
			else
			{
				pPlayer->m_vecVelocity( ) = LagRecord.m_Velocity;
				pPlayer->m_vecAbsVelocity( ) = LagRecord.m_Velocity;
				pPlayer->m_flDuckAmount( ) = LagRecord.m_DuckAmount;
				pPlayer->m_fFlags( ) = LagRecord.m_Flags;
			}

			if ( pPlayer->m_PlayerAnimStateCSGO( )->m_nLastUpdateFrame > g_Globals.m_Interfaces.m_GlobalVars->m_iFrameCount - 1 )
				pPlayer->m_PlayerAnimStateCSGO( )->m_nLastUpdateFrame = g_Globals.m_Interfaces.m_GlobalVars->m_iFrameCount - 1;

			bool bClientSideAnimation = pPlayer->m_bClientSideAnimation( );
			pPlayer->m_bClientSideAnimation( ) = true;
		
			for ( int32_t iLayer = NULL; iLayer < ANIMATION_LAYER_COUNT; iLayer++ )
				pPlayer->m_AnimationLayers( )[ iLayer ].m_pOwner = pPlayer;

			g_Globals.m_AnimationData.m_bAnimationUpdate = true;
			pPlayer->UpdateClientSideAnimation( );
			g_Globals.m_AnimationData.m_bAnimationUpdate = false;
		
			pPlayer->m_bClientSideAnimation( ) = bClientSideAnimation;
		}
	}
	else
	{
		g_Globals.m_Interfaces.m_GlobalVars->m_flCurTime = LagRecord.m_SimulationTime;
		g_Globals.m_Interfaces.m_GlobalVars->m_flRealTime = LagRecord.m_SimulationTime;
		g_Globals.m_Interfaces.m_GlobalVars->m_flFrameTime = g_Globals.m_Interfaces.m_GlobalVars->m_flIntervalPerTick;
		g_Globals.m_Interfaces.m_GlobalVars->m_flAbsFrameTime = g_Globals.m_Interfaces.m_GlobalVars->m_flIntervalPerTick;
		g_Globals.m_Interfaces.m_GlobalVars->m_iFrameCount = TIME_TO_TICKS( LagRecord.m_SimulationTime );
		g_Globals.m_Interfaces.m_GlobalVars->m_iTickCount = TIME_TO_TICKS( LagRecord.m_SimulationTime );
		g_Globals.m_Interfaces.m_GlobalVars->m_flInterpolationAmount = 0.0f;

		pPlayer->m_vecVelocity( ) = LagRecord.m_Velocity;
		pPlayer->m_vecAbsVelocity( ) = LagRecord.m_Velocity;

		if ( iRotationMode )
		{
			LagRecord.m_BruteYaw = Math::NormalizeAngle( LagRecord.m_BruteYaw );
			switch ( iRotationMode )
			{
				case ROTATE_LEFT: LagRecord.m_BruteYaw = Math::NormalizeAngle( LagRecord.m_BruteYaw - 60.0f ); break;
				case ROTATE_RIGHT: LagRecord.m_BruteYaw = Math::NormalizeAngle( LagRecord.m_BruteYaw + 60.0f ); break;
				case ROTATE_LOW_LEFT: LagRecord.m_BruteYaw = Math::NormalizeAngle( LagRecord.m_BruteYaw - 25.0f ); break;
				case ROTATE_LOW_RIGHT: LagRecord.m_BruteYaw = Math::NormalizeAngle( LagRecord.m_BruteYaw + 25.0f ); break;
			}

			pPlayer->m_PlayerAnimStateCSGO( )->m_flFootYaw = LagRecord.m_BruteYaw;
		}

		if ( pPlayer->m_PlayerAnimStateCSGO( )->m_nLastUpdateFrame > g_Globals.m_Interfaces.m_GlobalVars->m_iFrameCount - 1 )
			pPlayer->m_PlayerAnimStateCSGO( )->m_nLastUpdateFrame = g_Globals.m_Interfaces.m_GlobalVars->m_iFrameCount - 1;

		bool bClientSideAnimation = pPlayer->m_bClientSideAnimation( );
		pPlayer->m_bClientSideAnimation( ) = true;
		
		for ( int32_t iLayer = NULL; iLayer < ANIMATION_LAYER_COUNT; iLayer++ )
			pPlayer->m_AnimationLayers( )[ iLayer ].m_pOwner = pPlayer;

		g_Globals.m_AnimationData.m_bAnimationUpdate = true;
		pPlayer->UpdateClientSideAnimation( );
		g_Globals.m_AnimationData.m_bAnimationUpdate = false;
		
		pPlayer->m_bClientSideAnimation( ) = bClientSideAnimation;
	}

	pPlayer->m_flLowerBodyYaw( ) = flLowerBodyYaw;
	pPlayer->m_flDuckAmount( ) = flDuckAmount;
	pPlayer->m_iEFlags( ) = iEFlags;
	pPlayer->m_fFlags( ) = iFlags;

	g_Globals.m_Interfaces.m_GlobalVars->m_flCurTime = flCurTime;
	g_Globals.m_Interfaces.m_GlobalVars->m_flRealTime = flRealTime;
	g_Globals.m_Interfaces.m_GlobalVars->m_flAbsFrameTime = flAbsFrameTime;
	g_Globals.m_Interfaces.m_GlobalVars->m_flFrameTime = flFrameTime;
	g_Globals.m_Interfaces.m_GlobalVars->m_iFrameCount = iFrameCount;
	g_Globals.m_Interfaces.m_GlobalVars->m_iTickCount = iTickCount;
	g_Globals.m_Interfaces.m_GlobalVars->m_flInterpolationAmount = flInterpolationAmount;

	return pPlayer->InvalidatePhysicsRecursive( ANIMATION_CHANGED );
}

bool C_AnimationSync::GetCachedMatrix( C_BasePlayer* pPlayer, matrix3x4_t* aMatrix )
{
	std::memcpy( aMatrix, m_CachedMatrix[ pPlayer->EntIndex( ) ].data( ), sizeof( matrix3x4_t ) * pPlayer->m_CachedBoneData( ).Count( ) );
	return true;
}

void C_AnimationSync::OnUpdateClientSideAnimation( C_BasePlayer* pPlayer )
{
	for ( int i = 0; i < MAXSTUDIOBONES; i++ )
		m_CachedMatrix[ pPlayer->EntIndex( ) ][ i ].SetOrigin( pPlayer->GetAbsOrigin( ) - m_BoneOrigins[ pPlayer->EntIndex( ) ][ i ] );

	std::memcpy( pPlayer->m_CachedBoneData( ).Base( ), m_CachedMatrix[ pPlayer->EntIndex( ) ].data( ), sizeof( matrix3x4_t ) * pPlayer->m_CachedBoneData( ).Count( ) );
	std::memcpy( pPlayer->GetBoneAccessor( ).GetBoneArrayForWrite( ), m_CachedMatrix[ pPlayer->EntIndex( ) ].data( ), sizeof( matrix3x4_t ) * pPlayer->m_CachedBoneData( ).Count( ) );
	
	return pPlayer->SetupBones_AttachmentHelper( );
}
