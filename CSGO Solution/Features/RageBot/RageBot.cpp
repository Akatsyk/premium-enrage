#include "RageBot.hpp"
#include "NewAutowall.hpp"
#include "Antiaim.hpp"

#include "../Markers/HitMarker.hpp"
#include "../Packet/PacketManager.hpp"

#include "../Exploits/Exploits.hpp"
#include "../Tools/Tools.hpp"
#include "../SDK/Math/Math.hpp"
#include "../Prediction/EnginePrediction.hpp"
#include "../Visuals/ShotChams.hpp"
#include "../Log Manager/LogManager.hpp"
#include "../Animations/LocalAnimations.hpp"
#include "../Visuals/World.hpp"
#include "../Networking/Networking.hpp"
#include "../Settings.hpp"

void C_RageBot::Instance( )
{
	if ( !g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( ) || !g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( )->GetWeaponData( ) )
		return;

	g_Globals.m_RageData.m_CurrentTarget = C_TargetData( );
	if ( ( g_Globals.m_LocalPlayer->m_fFlags( ) & FL_FROZEN ) || ( *g_Globals.m_Interfaces.m_GameRules )->IsFreezePeriod( ) )
		return;

	if ( !g_Globals.m_AccuracyData.m_bCanFire_Default || !g_Globals.m_AccuracyData.m_bCanFire_Shift )
	{
		g_PacketManager->GetModifableCommand( )->m_nButtons &= ~IN_ATTACK;
		return;
	}
	
	this->SetupPacket( );
	if ( !m_RageSettings.m_bEnabled )
		return;

	if ( g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( )->m_iItemDefinitionIndex( ) == WEAPON_TASER )
		return this->TaserBot( );
	
	this->AutoRevolver( );
	if ( !g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( )->IsGun( ) )
		return;

	this->ScanPlayers( );
	if ( !g_Globals.m_RageData.m_CurrentTarget.m_Player )
		return;

	int32_t nHitChance = m_RageSettings.m_iHitChance;
	if ( g_Globals.m_AccuracyData.m_bDoingSecondShot )
		nHitChance = m_RageSettings.m_iDoubleTapHitChance;
	g_Globals.m_AccuracyData.m_bDoingSecondShot = false;

	float_t flCalculatedHitchance = this->GetHitChance( g_Globals.m_RageData.m_CurrentTarget );
	if ( flCalculatedHitchance < nHitChance )
		return;

	float_t flCalculatedAccuracy = this->GetAccuracyBoost( g_Globals.m_RageData.m_CurrentTarget );
	if ( flCalculatedAccuracy < m_RageSettings.m_iAccuracyBoost )
		return;
	
	QAngle angCalculatedAngle = Math::CalcAngle( g_Globals.m_LocalData.m_vecShootPosition, g_Globals.m_RageData.m_CurrentTarget.m_Hitbox.m_vecPoint );
	if ( !g_Globals.m_Packet.m_bFakeDuck )
		g_PacketManager->GetModifablePacket( ) = true;

	g_PacketManager->GetModifableCommand( )->m_nTickCount = TIME_TO_TICKS( g_Globals.m_RageData.m_CurrentTarget.m_LagRecord.m_SimulationTime + g_LagCompensation->GetLerpTime( ) );
	g_PacketManager->GetModifableCommand( )->m_angViewAngles = angCalculatedAngle;
	g_PacketManager->GetModifableCommand( )->m_nButtons |= IN_ATTACK;

	if ( g_Globals.m_ConVars.m_WeaponRecoilScale->GetFloat( ) > 0.0f )
		g_PacketManager->GetModifableCommand( )->m_angViewAngles -= g_Globals.m_ConVars.m_WeaponRecoilScale->GetFloat( ) * g_Globals.m_LocalPlayer->m_aimPunchAngle( );

	C_ShotData ShotData;

	ShotData.m_Target = g_Globals.m_RageData.m_CurrentTarget;
	ShotData.m_vecStartPosition = g_Globals.m_LocalData.m_vecShootPosition;
	ShotData.m_vecEndPosition = g_Globals.m_RageData.m_CurrentTarget.m_Hitbox.m_vecPoint;
	ShotData.m_iShotTick = g_Networking->GetServerTick( );
	ShotData.m_bHasMaximumAccuracy = this->HasMaximumAccuracy( );

	g_Globals.m_ShotData.emplace_back( ShotData );

	return g_ShotChams->OnRageBotFire( g_Globals.m_RageData.m_CurrentTarget.m_Player );
}

void C_RageBot::UpdatePeekState( )
{
	g_Globals.m_Peek.m_bIsPeeking = false;
	if ( !m_RageSettings.m_bEnabled && ( !g_Settings->m_bFakeLagEnabled || !g_Settings->m_aFakelagTriggers[ 2 ] ) )
		return;

	if ( g_Globals.m_LocalPlayer->m_vecVelocity( ).Length2D( ) < 23.40f )
		return;

	for ( int32_t PlayerID = 1; PlayerID <= g_Globals.m_Interfaces.m_GlobalVars->m_iMaxClients; PlayerID++ )
	{
		C_BasePlayer* Player = C_BasePlayer::GetPlayerByIndex( PlayerID );
		if ( !Player || !Player->IsPlayer( ) || !Player->IsAlive( ) || Player->IsDormant( ) || Player->m_iTeamNum( ) == g_Globals.m_LocalPlayer->m_iTeamNum( ) )
			continue;

		if ( Player->m_bGunGameImmunity( ) || ( Player->m_fFlags( ) & FL_FROZEN ) )
			continue;

		const auto m_LagRecords = g_Globals.m_CachedPlayerRecords[ PlayerID ];
		if ( m_LagRecords.empty( ) )
			continue;

		// force player
		this->AdjustPlayerRecord( Player, m_LagRecords.back( ) );
	
		// next shoot pos
		Vector vecNextShootPosition = g_Globals.m_LocalData.m_vecShootPosition + ( g_Globals.m_LocalPlayer->m_vecVelocity( ) * g_Globals.m_Interfaces.m_GlobalVars->m_flIntervalPerTick ) * 4.0f;

		C_RageSettings Settings;
		std::memcpy( &Settings, &m_RageSettings, sizeof( C_RageSettings ) );

		// set hitboxes
		m_RageSettings.m_Hitboxes[ 0 ] = false;
		m_RageSettings.m_Hitboxes[ 1 ] = false;
		m_RageSettings.m_Hitboxes[ 2 ] = false;
		m_RageSettings.m_Hitboxes[ 3 ] = true;
		m_RageSettings.m_Hitboxes[ 4 ] = true;
		m_RageSettings.m_Hitboxes[ 5 ] = false;

		// set safe hitboxes
		m_RageSettings.m_SafeHitboxes[ 0 ] = false;
		m_RageSettings.m_SafeHitboxes[ 1 ] = false;
		m_RageSettings.m_SafeHitboxes[ 2 ] = false;
		m_RageSettings.m_SafeHitboxes[ 3 ] = false;
		m_RageSettings.m_SafeHitboxes[ 4 ] = false;
		m_RageSettings.m_SafeHitboxes[ 5 ] = false;

		// set multipoints
		m_RageSettings.m_Multipoints[ 0 ] = false;
		m_RageSettings.m_Multipoints[ 1 ] = false;
		m_RageSettings.m_Multipoints[ 2 ] = false;
		m_RageSettings.m_Multipoints[ 3 ] = true;
		m_RageSettings.m_Multipoints[ 4 ] = true;
		m_RageSettings.m_Multipoints[ 5 ] = false;

		// disable delay things
		m_RageSettings.m_bLethalSafety = false;
		m_RageSettings.m_iMinDamage = 1;
		m_RageSettings.m_iMinDamageOverride = 1;
		m_RageSettings.m_iHeadScale = 100;
		m_RageSettings.m_iBodyScale = 100;

		// scan record from next position
		C_HitboxData HitboxData = this->ScanPlayerRecord( Player, m_LagRecords.back( ), vecNextShootPosition );
		if ( !HitboxData.m_flDamage )
		{
			// restore settings
			std::memcpy( &m_RageSettings, &Settings, sizeof( C_RageSettings ) );

			// we didn't find the player that we peek
			continue;
		}

		// we are actually peeking somebody
		g_Globals.m_Peek.m_Player = Player;
		g_Globals.m_Peek.m_bIsPeeking = true;

		// restore settings
		std::memcpy( &m_RageSettings, &Settings, sizeof( C_RageSettings ) );
		break;
	}
}

void C_RageBot::ScanPlayers( )
{
	for ( int32_t iPlayerID = 1; iPlayerID <= g_Globals.m_Interfaces.m_GlobalVars->m_iMaxClients; iPlayerID++ )
	{
		C_BasePlayer* pPlayer = C_BasePlayer::GetPlayerByIndex( iPlayerID );
		if ( !pPlayer || !pPlayer->IsPlayer( ) || !pPlayer->IsAlive( ) || pPlayer->m_iTeamNum( ) == g_Globals.m_LocalPlayer->m_iTeamNum( ) )
			continue;
		
		Vector vecShootPosition = g_Globals.m_LocalData.m_vecShootPosition;
		if ( pPlayer->m_bGunGameImmunity( ) || ( pPlayer->m_fFlags( ) & FL_FROZEN ) )
			continue;

		const auto m_LagRecords = g_Globals.m_CachedPlayerRecords[ iPlayerID ];
		if ( m_LagRecords.empty( ) )
			continue;

		C_LagRecord LagRecord = this->GetFirstAvailableRecord( pPlayer );
		if ( !g_LagCompensation->IsValidTime( LagRecord.m_SimulationTime ) )
		{
			if ( m_RageSettings.m_AutoStopOptions[ AUTOSTOP_EARLY ] && m_RageSettings.m_bAutoStop )
			{
				this->AdjustPlayerRecord( pPlayer, m_LagRecords.back( ) );
				if ( g_Globals.m_MovementData.m_TicksToStop > 0)
				{
					C_HitboxData HitboxData = this->ScanPlayerRecord( pPlayer, m_LagRecords.back( ), vecShootPosition + ( g_Globals.m_LocalPlayer->m_vecVelocity( ) * g_Globals.m_Interfaces.m_GlobalVars->m_flIntervalPerTick ) * g_Globals.m_MovementData.m_TicksToStop );
					if ( HitboxData.m_flDamage >= this->GetMinDamage( pPlayer ) )
					{
						float_t flDistance = g_Globals.m_LocalPlayer->m_vecOrigin( ).DistTo( pPlayer->m_vecOrigin( ) );
						if ( flDistance )
						{
							int nIndex = g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( )->m_iItemDefinitionIndex( );
							if ( ( nIndex != WEAPON_SCAR20 && nIndex != WEAPON_G3SG1 ) || flDistance > 430.0f )
								this->AutoScope( );
						}

						if ( this->CanAutoStop( ) )
							g_Globals.m_AccuracyData.m_bRestoreAutoStop = false;
					}
				}
			}

			continue;
		}

		this->AdjustPlayerRecord( pPlayer, LagRecord );
		if ( m_RageSettings.m_AutoStopOptions[ AUTOSTOP_EARLY ] && m_RageSettings.m_bAutoStop )
		{
			if ( g_Globals.m_MovementData.m_TicksToStop > 0 )
			{
				C_HitboxData HitboxData = this->ScanPlayerRecord( pPlayer, LagRecord, vecShootPosition + ( g_Globals.m_LocalPlayer->m_vecVelocity( ) * g_Globals.m_Interfaces.m_GlobalVars->m_flIntervalPerTick ) * g_Globals.m_MovementData.m_TicksToStop );
				if ( HitboxData.m_flDamage >= this->GetMinDamage( pPlayer ) )
				{
					float_t flDistance = g_Globals.m_LocalPlayer->m_vecOrigin( ).DistTo( pPlayer->m_vecOrigin( ) );
					if ( flDistance )
					{
						int nIndex = g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( )->m_iItemDefinitionIndex( );
						if ( ( nIndex != WEAPON_SCAR20 && nIndex != WEAPON_G3SG1 ) || flDistance > 430.0f )
							this->AutoScope( );
					}

					if ( this->CanAutoStop( ) )
						g_Globals.m_AccuracyData.m_bRestoreAutoStop = false;
				}
			}
		}

		C_HitboxData HitboxData = this->ScanPlayerRecord( pPlayer, LagRecord, vecShootPosition );
		if ( HitboxData.m_flDamage >= this->GetMinDamage( pPlayer ) )
		{
			g_Globals.m_RageData.m_CurrentTarget.m_Hitbox = HitboxData;
			g_Globals.m_RageData.m_CurrentTarget.m_LagRecord = LagRecord;
			g_Globals.m_RageData.m_CurrentTarget.m_Player = pPlayer;

			if ( this->CanAutoStop( ) )
				g_Globals.m_AccuracyData.m_bRestoreAutoStop = false;

			float_t flDistance = g_Globals.m_LocalPlayer->m_vecOrigin( ).DistTo( pPlayer->m_vecOrigin( ) );
			if ( flDistance )
			{
				int nIndex = g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( )->m_iItemDefinitionIndex( );
				if ( ( nIndex != WEAPON_SCAR20 && nIndex != WEAPON_G3SG1 ) || flDistance > 430.0f )
					this->AutoScope( );
			}

			this->AdjustPlayerRecord( g_Globals.m_RageData.m_CurrentTarget.m_Player, g_Globals.m_RageData.m_CurrentTarget.m_LagRecord );
			break;
		}
		
		C_LagRecord BTRecord = C_LagRecord( );
		C_HitboxData BTHitscan = C_HitboxData( );
		
		if ( !this->FindPlayerRecord( pPlayer, &BTRecord, &BTHitscan ) )
		{
			this->AdjustPlayerRecord( pPlayer, m_LagRecords.back( ) );
			continue;
		}

		g_Globals.m_RageData.m_CurrentTarget.m_Player = pPlayer;
		g_Globals.m_RageData.m_CurrentTarget.m_LagRecord = BTRecord;
		g_Globals.m_RageData.m_CurrentTarget.m_Hitbox = BTHitscan;
		
		if ( this->CanAutoStop( ) )
			g_Globals.m_AccuracyData.m_bRestoreAutoStop = false;
		
		float_t flDistance = g_Globals.m_LocalPlayer->GetAbsOrigin( ).DistTo( pPlayer->GetAbsOrigin( ) );
		if ( flDistance )
		{
			int nIndex = g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( )->m_iItemDefinitionIndex( );
			if ( ( nIndex != WEAPON_SCAR20 && nIndex != WEAPON_G3SG1 ) || flDistance > 450.0f )
				this->AutoScope( );
		}

		this->AdjustPlayerRecord( g_Globals.m_RageData.m_CurrentTarget.m_Player, g_Globals.m_RageData.m_CurrentTarget.m_LagRecord );
		break;
	}
}

bool C_RageBot::FindPlayerRecord( C_BasePlayer* pPlayer, C_LagRecord* OutRecord, C_HitboxData* OutHitbox )
{
	bool bDidScanLastRecord = false;
	for ( auto LagRecord : g_Globals.m_CachedPlayerRecords[ pPlayer->EntIndex( ) ] )
	{
		if ( !g_LagCompensation->IsValidTime( LagRecord.m_SimulationTime ) )
			continue;

		Vector vecShootPosition = g_Globals.m_LocalData.m_vecShootPosition;
		if ( !LagRecord.m_bIsShooting && bDidScanLastRecord )
			continue;
		
		bDidScanLastRecord = true;
		this->AdjustPlayerRecord( pPlayer, LagRecord );

		C_HitboxData HitboxData = this->ScanPlayerRecord( pPlayer, LagRecord, vecShootPosition );
		if ( HitboxData.m_flDamage < this->GetMinDamage( pPlayer ) )
			continue;

		*OutRecord = LagRecord;
		*OutHitbox = HitboxData;

		return true;
	}

	return false;
}

int32_t C_RageBot::GetHitgroupFromHitbox( int32_t iHitbox )
{
	int32_t iHitgroup = 0;
	switch ( iHitbox )
	{
		case HITBOX_HEAD:
			iHitgroup = HITGROUP_HEAD;
		break;

		case HITBOX_CHEST:
			iHitgroup = HITGROUP_CHEST;
		break;

		case HITBOX_PELVIS:
			iHitgroup = HITGROUP_GENERIC;
		break;

		case HITBOX_LEFT_CALF:
		case HITBOX_LEFT_FOOT:
		case HITBOX_LEFT_THIGH:
			iHitgroup = HITGROUP_LEFTLEG;
		break;

		case HITBOX_RIGHT_CALF:
		case HITBOX_RIGHT_FOOT:
		case HITBOX_RIGHT_THIGH:
			iHitgroup = HITGROUP_RIGHTLEG;
		break;

		case HITBOX_LEFT_UPPER_ARM:
		case HITBOX_LEFT_HAND:
		case HITBOX_LEFT_FOREARM:
			iHitgroup = HITGROUP_LEFTARM;
		break;

		case HITBOX_RIGHT_UPPER_ARM:
		case HITBOX_RIGHT_HAND:
		case HITBOX_RIGHT_FOREARM:
			iHitgroup = HITGROUP_RIGHTARM;
		break;

		case HITBOX_STOMACH:
			iHitgroup = HITGROUP_STOMACH;
		break;

		default: break;
	}

	return iHitgroup;
}

bool SortHitboxes( C_HitboxHitscanData First, C_HitboxHitscanData Second )
{
	if ( First.m_iHitbox == HITBOX_HEAD )
		return false;
	else if ( Second.m_iHitbox == HITBOX_HEAD )
		return true;

	return First.m_flWeaponDamage > Second.m_flWeaponDamage;
}

C_HitboxData C_RageBot::ScanPlayerRecord( C_BasePlayer* pPlayer, C_LagRecord LagRecord, Vector vecStartPosition )
{
	C_BaseCombatWeapon* pCombatWeapon = g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( );
	if ( !pCombatWeapon )
		return C_HitboxData( );

	C_CSWeaponData* pWeaponData = pCombatWeapon->GetWeaponData( );
	if ( !pWeaponData )
		return C_HitboxData( );

	std::vector < C_HitboxHitscanData > aHitboxesData;
	
	bool bForcedSafety = g_Tools->IsBindActive( g_Settings->m_aSafePoint );
	if ( m_RageSettings.m_Hitboxes[ 0 ] )
		aHitboxesData.emplace_back( C_HitboxHitscanData( HITBOX_HEAD, m_RageSettings.m_SafeHitboxes[ HITBOX_HEAD ] || bForcedSafety ) );
	
	if ( m_RageSettings.m_Hitboxes[ 1 ] )
		aHitboxesData.emplace_back( C_HitboxHitscanData( HITBOX_CHEST, m_RageSettings.m_SafeHitboxes[ 1 ] || bForcedSafety ) );
	
	if ( m_RageSettings.m_Hitboxes[ 2 ] )
	{
		aHitboxesData.emplace_back( C_HitboxHitscanData( HITBOX_LEFT_FOREARM, m_RageSettings.m_SafeHitboxes[ 2 ] || bForcedSafety ) );
		aHitboxesData.emplace_back( C_HitboxHitscanData( HITBOX_RIGHT_FOREARM, m_RageSettings.m_SafeHitboxes[ 2 ] || bForcedSafety ) );
	}

	if ( m_RageSettings.m_Hitboxes[ 3 ] )
		aHitboxesData.emplace_back( C_HitboxHitscanData( HITBOX_PELVIS, m_RageSettings.m_SafeHitboxes[ 3 ] || bForcedSafety ) );

	if ( m_RageSettings.m_Hitboxes[ 4 ] )
		aHitboxesData.emplace_back( C_HitboxHitscanData( HITBOX_STOMACH, m_RageSettings.m_SafeHitboxes[ 4 ] || bForcedSafety ) );
		
	if ( m_RageSettings.m_Hitboxes[ 5 ] )
	{
		aHitboxesData.emplace_back( C_HitboxHitscanData( HITBOX_LEFT_THIGH, m_RageSettings.m_SafeHitboxes[ 5 ] || bForcedSafety ) );
		aHitboxesData.emplace_back( C_HitboxHitscanData( HITBOX_RIGHT_THIGH, m_RageSettings.m_SafeHitboxes[ 5 ] || bForcedSafety ) );
		aHitboxesData.emplace_back( C_HitboxHitscanData( HITBOX_LEFT_CALF, m_RageSettings.m_SafeHitboxes[ 5 ] || bForcedSafety ) );
		aHitboxesData.emplace_back( C_HitboxHitscanData( HITBOX_RIGHT_CALF, m_RageSettings.m_SafeHitboxes[ 5 ] || bForcedSafety ) );
	}
	
	std::vector < C_HitboxData > m_ScanData;
	for ( auto Hitbox : aHitboxesData )
	{
		std::vector < Vector > m_Hitboxes = this->GetHitboxPoints( pPlayer, LagRecord, vecStartPosition, Hitbox.m_iHitbox );
		if ( m_Hitboxes.empty( ) )
			continue;

		for ( auto Point : m_Hitboxes )
		{
			if ( !this->DoesIntersectHitbox( pPlayer, Hitbox.m_iHitbox, vecStartPosition, Point ) )
				continue;

			NewAutoWall::pen_argument_t Argument;
			Argument.m_from = g_Globals.m_LocalPlayer;
			Argument.m_to = pPlayer;
			Argument.m_can_penetrate = true;
			Argument.m_damage = this->GetMinDamage( pPlayer );
			Argument.m_damage_penetration = this->GetMinDamage( pPlayer );
			Argument.m_pos = Point;
			Argument.m_shoot_position = vecStartPosition;
			NewAutoWall::run_penetration( &Argument );

			float_t flDamage = Argument.m_out_data.m_damage;
			if ( flDamage < this->GetMinDamage( pPlayer ) )
				continue;

			C_HitboxData ScanData;
			ScanData.pPlayer = pPlayer;
			ScanData.LagRecord = LagRecord;
			ScanData.m_bForcedToSafeHitbox = Hitbox.m_bForceSafe;
			ScanData.m_flDamage = flDamage;
			ScanData.m_iHitbox = Hitbox.m_iHitbox;
			ScanData.m_vecPoint = Point;
			ScanData.m_bIsSafeHitbox = this->IsSafePoint( pPlayer, LagRecord, vecStartPosition, Point, Hitbox.m_iHitbox );

			if ( flDamage >= pPlayer->m_iHealth( ) || NewAutoWall::scale_damage( pPlayer, pWeaponData->m_iDamage, pWeaponData->m_flArmorRatio, GetHitgroupFromHitbox( Hitbox.m_iHitbox ) ) >= pPlayer->m_iHealth( ) )
				if ( m_RageSettings.m_bLethalSafety && !ScanData.m_bIsSafeHitbox )
					continue;

			if ( Hitbox.m_bForceSafe && !ScanData.m_bIsSafeHitbox )
				continue;

			m_ScanData.emplace_back( ScanData );
		}
	}

	if ( m_ScanData.empty( ) )
		return C_HitboxData( );

	bool bHasLethalDamage = false;
	int nLethalPosition = 0;

	for ( int i = 0; i < m_ScanData.size( ); i++ )
	{
		if ( m_ScanData[ i ].m_flDamage < pPlayer->m_iHealth( ) || m_ScanData[ i ].m_iHitbox == HITBOX_HEAD )
			continue;

		if ( !m_ScanData[ i ].m_bIsSafeHitbox )
			if ( m_ScanData[ i ].m_bForcedToSafeHitbox || m_RageSettings.m_bLethalSafety )
				continue;

		nLethalPosition = i;
		bHasLethalDamage = true;

		break;
	}

	if ( bHasLethalDamage )
		return m_ScanData[ nLethalPosition ];

	bool bHasHeadSafety = false;
	for ( int i = 0; i < m_ScanData.size( ); i++ )
	{
		if ( m_ScanData[ i ].m_iHitbox != HITBOX_HEAD || m_ScanData[ i ].m_flDamage < pPlayer->m_iHealth( ) )
			continue;

		if ( !m_ScanData[ i ].m_bIsSafeHitbox && !LagRecord.m_bAnimResolved || !( pPlayer->m_fFlags( ) & FL_ONGROUND ) )
			continue;

		return m_ScanData[ i ];
	}

	auto SortSafety = [ ]( C_HitboxData First, C_HitboxData Second ) -> bool
	{
		if ( First.m_bIsSafeHitbox )
			return false;

		return true;
	};

	auto SortBody = [ ]( C_HitboxData First, C_HitboxData Second ) -> bool
	{
		if ( First.m_iHitbox == HITBOX_HEAD )
			return false;

		return true;
	};

	auto SortDamage = [ ]( C_HitboxData First, C_HitboxData Second ) -> bool
	{
		return First.m_flDamage > Second.m_flDamage;
	};
	
	std::sort( m_ScanData.begin( ), m_ScanData.end( ), SortDamage );
	if ( m_RageSettings.m_bPreferBody )
		std::sort( m_ScanData.begin( ), m_ScanData.end( ), SortBody );

	if ( m_RageSettings.m_bPreferSafe )
		std::sort( m_ScanData.begin( ), m_ScanData.end( ), SortSafety );

	return m_ScanData.back( );
}

C_LagRecord C_RageBot::GetFirstAvailableRecord( C_BasePlayer* pPlayer )
{
	const auto m_LagRecords = g_Globals.m_CachedPlayerRecords[ pPlayer->EntIndex( ) ];
	if ( m_LagRecords.empty( ) )
		return C_LagRecord( );
	
	C_LagRecord LagRecord = C_LagRecord( );
	for ( int32_t i = 0; i < m_LagRecords.size( ); i++ )
	{
		auto m_Record = m_LagRecords[ i ];
		if ( !g_LagCompensation->IsValidTime( m_Record.m_SimulationTime ) )
			continue;

		LagRecord = m_LagRecords[ i ];
	}

	return LagRecord;
}

void C_RageBot::SaveMovementData( )
{
	m_flForwardMove = g_PacketManager->GetModifableCommand( )->m_flForwardMove;
	m_flSideMove = g_PacketManager->GetModifableCommand( )->m_flSideMove;
}

void C_RageBot::ForceMovementData( )
{
	if ( !g_Globals.m_AccuracyData.m_bRestoreAutoStop )
		return;

	g_PacketManager->GetModifableCommand( )->m_flForwardMove = m_flForwardMove;
	g_PacketManager->GetModifableCommand( )->m_flSideMove = m_flSideMove;
	
	g_PredictionSystem->RestoreNetvars( g_PacketManager->GetModifableCommand( )->m_nCommand );
	return g_PredictionSystem->Repredict( );
}

void C_RageBot::FakeDuck( )
{
	if ( ( g_Globals.m_LocalPlayer->m_fFlags( ) & FL_FROZEN ) || ( *( g_Globals.m_Interfaces.m_GameRules ) )->IsFreezePeriod( ) )
		return;

	if ( ( *( g_Globals.m_Interfaces.m_GameRules ) )->IsValveDS( ) && g_Settings->m_bAntiUntrusted )
		return;

	if ( g_Settings->m_bInfinityDuck )
		if ( !( *g_Globals.m_Interfaces.m_GameRules )->IsValveDS( ) || !g_Settings->m_bAntiUntrusted )
			g_PacketManager->GetModifableCommand( )->m_nButtons |= IN_BULLRUSH;

	bool bShouldFakeDuck = g_Tools->IsBindActive( g_Settings->m_aFakeDuck ) && ( g_Globals.m_LocalPlayer->m_fFlags( ) & FL_ONGROUND );
	bool bFakeDuckBackup = g_Globals.m_Packet.m_bFakeDuck;

	if ( bShouldFakeDuck )
		g_Globals.m_Packet.m_bVisualFakeDuck = true;
	else if ( g_Globals.m_LocalPlayer->m_flDuckAmount( ) == 0.0f || g_Globals.m_LocalPlayer->m_flDuckAmount( ) >= 1.0f )
		g_Globals.m_Packet.m_bVisualFakeDuck = false;

	g_Globals.m_Packet.m_bFakeDuck = bShouldFakeDuck;
	if ( !bFakeDuckBackup && bShouldFakeDuck )
	{
		g_Globals.m_Packet.m_bFakeDuck = true;
		if ( g_Globals.m_Interfaces.m_ClientState->m_nChokedCommands( ) )
			g_Globals.m_Packet.m_bFakeDuck = false;
		else
			g_PacketManager->GetModifableCommand( )->m_nButtons &= ~IN_DUCK;

		return;
	}
	else if ( !bShouldFakeDuck )
	{
		float m_flAwaitedDuck = ( g_PacketManager->GetModifableCommand( )->m_nButtons & IN_DUCK ) ? 1.0f : 0.0f;
		if ( m_flAwaitedDuck != g_Globals.m_LocalPlayer->m_flDuckAmount( ) )
		{
			g_PacketManager->GetModifableCommand( )->m_nButtons |= IN_BULLRUSH;

			if ( m_flAwaitedDuck < g_Globals.m_LocalPlayer->m_flDuckAmount( ) )
				g_PacketManager->GetModifableCommand( )->m_nButtons &= ~IN_DUCK;
			else
				g_PacketManager->GetModifableCommand( )->m_nButtons |= IN_DUCK;

			g_Globals.m_Packet.m_bFakeDuck = true;
		}
		
		if ( !g_Globals.m_Packet.m_bFakeDuck )
			return;
	}
	else
	{
		g_PacketManager->GetModifableCommand( )->m_nButtons |= IN_BULLRUSH;

		if ( g_Globals.m_Interfaces.m_ClientState->m_nChokedCommands( ) < 7 )
			g_PacketManager->GetModifableCommand( )->m_nButtons &= ~IN_DUCK;
		else
			g_PacketManager->GetModifableCommand( )->m_nButtons |= IN_DUCK;
	}

	if ( !bShouldFakeDuck )
	{
		g_Globals.m_Packet.m_bFakeDuck = false;
		return;
	}

	if ( g_Globals.m_Interfaces.m_ClientState->m_nChokedCommands( ) < 14 )
	{
		g_PacketManager->GetModifablePacket( ) = false;
		return;
	}

	g_PacketManager->GetModifablePacket( ) = true;
}

void C_RageBot::SetupPacket( )
{
	C_BaseCombatWeapon* pCombatWeapon = g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( );
	if ( !pCombatWeapon )
		return;

	C_CSWeaponData* pWeaponData = pCombatWeapon->GetWeaponData( );
	if ( !pWeaponData )
		return;

	if ( !g_Globals.m_AccuracyData.m_bHasValidAccuracyData )
	{
		for ( int32_t iSeed = 0; iSeed < 256; iSeed++ )
		{
			g_Tools->RandomSeed( iSeed + 1 );

			g_Globals.m_AccuracyData.m_aMultiValues[ iSeed ] = g_Tools->RandomFloat( 0.f, 1.f );
			g_Globals.m_AccuracyData.m_aInaccuracy[ iSeed ] = g_Tools->RandomFloat( 0.f, 2.f * PI_F );
			g_Globals.m_AccuracyData.m_aSpread[ iSeed ] = g_Tools->RandomFloat( 0.f, 2.f * PI_F );
		}

		g_Globals.m_AccuracyData.m_bHasValidAccuracyData = true;
	}

	float_t flDefaultInaccuracy = 0.0f;
	if ( g_Globals.m_LocalPlayer->m_flDuckAmount( ) )
	{
		if ( g_Globals.m_LocalPlayer->m_bIsScoped( ) )
			flDefaultInaccuracy = pWeaponData->m_flInaccuracyCrouchAlt;
		else
			flDefaultInaccuracy = pWeaponData->m_flInaccuracyCrouch;
	}
	else
	{
		if ( g_Globals.m_LocalPlayer->m_bIsScoped( ) )
			flDefaultInaccuracy = pWeaponData->m_flInaccuracyStandAlt;
		else
			flDefaultInaccuracy = pWeaponData->m_flInaccuracyStand;
	}
	g_Globals.m_AccuracyData.m_flMinInaccuracy = flDefaultInaccuracy;

	int32_t iCurrentWeapon = -1;
	switch ( pCombatWeapon->m_iItemDefinitionIndex( ) )
	{
		case WEAPON_AK47:
		case WEAPON_M4A1:
		case WEAPON_M4A1_SILENCER:
		case WEAPON_FAMAS:
		case WEAPON_SG553:
		case WEAPON_GALILAR:
			iCurrentWeapon = RAGE_WEAPON::RIFLE; break;
		case WEAPON_MAG7:
		case WEAPON_NOVA:
		case WEAPON_XM1014:
		case WEAPON_SAWEDOFF:
			iCurrentWeapon = RAGE_WEAPON::SHOTGUN; break;
		case WEAPON_MP7:
		case WEAPON_MP9:
		case WEAPON_P90:
		case WEAPON_M249:
		case WEAPON_NEGEV:
		case WEAPON_UMP45:
			iCurrentWeapon = RAGE_WEAPON::SMG; break;
		case WEAPON_SCAR20:
		case WEAPON_G3SG1:
			iCurrentWeapon = RAGE_WEAPON::AUTO; break;
		case WEAPON_GLOCK:
		case WEAPON_HKP2000: 
		case WEAPON_USP_SILENCER:
		case WEAPON_CZ75A:
		case WEAPON_TEC9:
		case WEAPON_ELITE:
		case WEAPON_FIVESEVEN:
		case WEAPON_P250:
			iCurrentWeapon = RAGE_WEAPON::PISTOL; break;
		case WEAPON_SSG08:
			iCurrentWeapon = RAGE_WEAPON::SCOUT; break;
		case WEAPON_AWP:
			iCurrentWeapon = RAGE_WEAPON::AWP; break;
		case WEAPON_DEAGLE:
			iCurrentWeapon = RAGE_WEAPON::DEAGLE; break;
		case WEAPON_REVOLVER:
			iCurrentWeapon = RAGE_WEAPON::REVOLVER; break;
		default: iCurrentWeapon = -1;
	}

	g_Globals.m_AccuracyData.m_bHasMaximumAccuracy = this->HasMaximumAccuracy( );
	if ( pCombatWeapon->m_iItemDefinitionIndex( ) == WEAPON_TASER )
	{
		m_RageSettings = C_RageSettings( );
		m_RageSettings.m_bEnabled = true;
		m_RageSettings.m_bAutoStop = true;

		m_RageSettings.m_bPreferSafe = true;
		m_RageSettings.m_bPreferBody = true;
		m_RageSettings.m_iBodyScale = 80;
		m_RageSettings.m_bLethalSafety = true;
		m_RageSettings.m_iHitChance = 85;

		m_RageSettings.m_Hitboxes[ 0 ] = false;
		m_RageSettings.m_Hitboxes[ 1 ] = false;
		m_RageSettings.m_Hitboxes[ 2 ] = false;
		m_RageSettings.m_Hitboxes[ 3 ] = true;
		m_RageSettings.m_Hitboxes[ 4 ] = true;
		m_RageSettings.m_Hitboxes[ 5 ] = false;
		m_RageSettings.m_Hitboxes[ 6 ] = false;

		m_RageSettings.m_SafeHitboxes[ 0 ] = false;
		m_RageSettings.m_SafeHitboxes[ 1 ] = false;
		m_RageSettings.m_SafeHitboxes[ 2 ] = false;
		m_RageSettings.m_SafeHitboxes[ 3 ] = true;
		m_RageSettings.m_SafeHitboxes[ 4 ] = true;
		m_RageSettings.m_SafeHitboxes[ 5 ] = false;
		m_RageSettings.m_SafeHitboxes[ 6 ] = false;
		
		m_RageSettings.m_Multipoints[ 0 ] = false;
		m_RageSettings.m_Multipoints[ 1 ] = false;
		m_RageSettings.m_Multipoints[ 2 ] = false;
		m_RageSettings.m_Multipoints[ 3 ] = true;
		m_RageSettings.m_Multipoints[ 4 ] = true;
		m_RageSettings.m_Multipoints[ 5 ] = false;
		m_RageSettings.m_Multipoints[ 6 ] = false;

		return; 
	}

	if ( iCurrentWeapon <= -1 )
	{
		m_RageSettings = C_RageSettings( );
		return;
	}

	m_RageSettings = g_Settings->m_aRageSettings[ iCurrentWeapon ];
}

int32_t C_RageBot::GetMinDamage( C_BasePlayer* pPlayer )
{
	int32_t iMinDamage = m_RageSettings.m_iMinDamage;
	if ( g_Tools->IsBindActive( g_Settings->m_aMinDamage ) )
		iMinDamage = m_RageSettings.m_iMinDamageOverride;

	iMinDamage = min( iMinDamage, pPlayer->m_iHealth( ) );
	if ( iMinDamage <= 0 )
		iMinDamage = this->GetAutoDamage( pPlayer->m_ArmourValue( ) != 0 );
	
	if ( iMinDamage > 99 )
		iMinDamage = pPlayer->m_iHealth( ) + ( iMinDamage - 100 );

	return iMinDamage;
}

int32_t C_RageBot::GetAutoDamage( bool bIsPlayerArmoured )
{
	int32_t nItemID = g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( )->m_iItemDefinitionIndex( );
	switch ( nItemID )
	{
	case WEAPON_SCAR20:
	case WEAPON_G3SG1:
		return bIsPlayerArmoured ? 42 : 33;
	case WEAPON_SSG08:
		return 84;
	case WEAPON_AWP:
		return 100;
	case WEAPON_REVOLVER:
	case WEAPON_DEAGLE:
		return 44;
	case WEAPON_TEC9:
	case WEAPON_HKP2000:
	case WEAPON_USP_SILENCER:
	case WEAPON_P250:
	case WEAPON_FLASHBANG:
	case WEAPON_CZ75A:
	case WEAPON_GLOCK:
	case WEAPON_ELITE:
		return bIsPlayerArmoured ? 20 : 34; 
	}

	return 0;
}

void C_RageBot::PredictAutoStop( )
{
	g_Globals.m_MovementData.m_TicksToStop = 0;
	if ( !this->CanAutoStop( ) || !m_RageSettings.m_bAutoStop )
		return;

	g_PredictionSystem->Instance( );

	float_t flMaxSpeed = g_Globals.m_LocalPlayer->GetMaxPlayerSpeed( );
	for ( ; g_Globals.m_MovementData.m_TicksToStop < 6; g_Globals.m_MovementData.m_TicksToStop++ )
	{
		this->AutoStop( );

		g_Globals.m_Interfaces.m_Prediction->SetupMove( g_Globals.m_LocalPlayer, g_PacketManager->GetModifableCommand( ), g_Globals.m_Interfaces.m_MoveHelper, &m_MoveData );
		g_Globals.m_Interfaces.m_GameMovement->ProcessMovement( g_Globals.m_LocalPlayer, &m_MoveData );
		g_Globals.m_Interfaces.m_Prediction->FinishMove( g_Globals.m_LocalPlayer, g_PacketManager->GetModifableCommand( ), &m_MoveData );

		if ( !m_RageSettings.m_AutoStopOptions[ AUTOSTOP_ACCURACY ] )
		{
			if ( g_Globals.m_LocalPlayer->m_vecVelocity( ).Length2D( ) < flMaxSpeed * 0.34f )
				break;
		}
		else if ( g_Globals.m_LocalPlayer->m_vecVelocity( ).Length2D( ) < 5.0f )
			break;
	}

	g_PredictionSystem->RestoreNetvars( g_PacketManager->GetModifableCommand( )->m_nCommand );
}

void C_RageBot::ResetData( )
{
	g_Globals.m_AccuracyData.m_aInaccuracy = { };
	g_Globals.m_AccuracyData.m_aMultiValues = { };
	g_Globals.m_AccuracyData.m_aSpread = { };
	
	g_Globals.m_AccuracyData.m_flSpread = 0.0f;
	g_Globals.m_AccuracyData.m_flInaccuracy = 0.0f;
	g_Globals.m_AccuracyData.m_flMinInaccuracy = 0.0f;
	g_Globals.m_AccuracyData.m_bHasValidAccuracyData = false;

	g_Globals.m_RageData.m_CurrentTarget = C_TargetData( );

	g_Globals.m_MovementData.m_TicksToStop = 0;
	g_Globals.m_MovementData.m_VelocityLength = 0;

	if ( !g_Globals.m_ShotData.empty( ) )
		return g_Globals.m_ShotData.clear( );
}

void C_RageBot::OnWeaponFire( C_GameEvent* pEvent )
{
	if ( !g_Globals.m_LocalPlayer || !g_Globals.m_LocalPlayer->IsAlive( ) )
		return;

	if ( g_Globals.m_Interfaces.m_EngineClient->GetPlayerForUserID( pEvent->GetInt( _S( "userid" ) ) ) != g_Globals.m_Interfaces.m_EngineClient->GetLocalPlayer( ) )
		return;

	C_ShotData* ShotData = nullptr;
	for ( int i = 0; i < g_Globals.m_ShotData.size( ); i++ )
	{
		if ( g_Networking->GetServerTick( ) - g_Globals.m_ShotData[ i ].m_iShotTick > g_Networking->GetTickRate( ) )
		{
			g_Globals.m_ShotData.erase( g_Globals.m_ShotData.begin( ) + i );
			continue;
		}

		ShotData = &g_Globals.m_ShotData[ i ];
	}

	if ( ShotData )
		ShotData->m_bHasBeenFired = true;
}

void C_RageBot::OnPlayerHurt( C_GameEvent* pEvent )
{
	if ( !g_Globals.m_LocalPlayer || !g_Globals.m_LocalPlayer->IsAlive( ) )
		return;

	if ( g_Globals.m_Interfaces.m_EngineClient->GetPlayerForUserID( pEvent->GetInt( _S( "attacker" ) ) ) != g_Globals.m_Interfaces.m_EngineClient->GetLocalPlayer( ) )
		return;

	C_ShotData* ShotData = nullptr;
	for ( int i = 0; i < g_Globals.m_ShotData.size( ); i++ )
	{
		if ( g_Networking->GetServerTick( ) - g_Globals.m_ShotData[ i ].m_iShotTick > g_Networking->GetTickRate( ) )
		{
			g_Globals.m_ShotData.erase( g_Globals.m_ShotData.begin( ) + i );
			continue;
		}

		ShotData = &g_Globals.m_ShotData[ i ];
	}

	if ( ShotData )
		ShotData->m_bHasBeenHurted = true;
}

void C_RageBot::OnBulletImpact( C_GameEvent* pEvent )
{
	if ( !g_Globals.m_LocalPlayer || !g_Globals.m_LocalPlayer->IsAlive( ) )
		return;
	
	Vector vecBulletImpact = Vector( pEvent->GetInt( _S( "x" ) ), pEvent->GetInt( _S( "y" ) ), pEvent->GetInt( _S( "z" ) ) );
	if ( g_Globals.m_Interfaces.m_EngineClient->GetPlayerForUserID( pEvent->GetInt( _S( "userid" ) ) ) != g_Globals.m_Interfaces.m_EngineClient->GetLocalPlayer( ) )
		return;

	C_ShotData* ShotData = nullptr;
	for ( int i = 0; i < g_Globals.m_ShotData.size( ); i++ )
	{
		if ( g_Networking->GetServerTick( ) - g_Globals.m_ShotData[ i ].m_iShotTick > g_Networking->GetTickRate( ) )
		{
			g_Globals.m_ShotData.erase( g_Globals.m_ShotData.begin( ) + i );
			continue;
		}

		ShotData = &g_Globals.m_ShotData[ i ];
	}
	
	if ( !ShotData )
		return;

	ShotData->m_vecImpacts.emplace_back( vecBulletImpact );
	ShotData->m_bHasBeenRegistered = true;
}

void C_RageBot::OnNetworkUpdate( ClientFrameStage_t Stage )
{
	if ( Stage != ClientFrameStage_t::FRAME_NET_UPDATE_END )
		return;

	if ( !g_Globals.m_LocalPlayer || !g_Globals.m_LocalPlayer->IsAlive( ) )
	{
		if ( !g_Globals.m_ShotData.empty( ) )
			g_Globals.m_ShotData.clear( );

		return;
	}

	for ( int i = 0; i < g_Globals.m_ShotData.size( ); i++ )
	{
		auto Shot = &g_Globals.m_ShotData[ i ];
		if ( !Shot->m_bHasBeenFired || !Shot->m_bHasBeenRegistered )
			continue;
	
		if ( !Shot->m_Target.m_Player )
		{
			g_Globals.m_ShotData.erase( g_Globals.m_ShotData.begin( ) + i );
			continue;
		}

		if ( Shot->m_Target.m_Player->IsAlive( ) )
		{
			const auto LagRecords = g_Globals.m_CachedPlayerRecords[ Shot->m_Target.m_Player->EntIndex( ) ];
			if ( LagRecords.empty( ) )
			{
				g_Globals.m_ShotData.erase( g_Globals.m_ShotData.begin( ) + i );
				continue;
			}

			this->AdjustPlayerRecord( Shot->m_Target.m_Player, Shot->m_Target.m_LagRecord );
			for ( auto& Impact : Shot->m_vecImpacts )
			{
				if ( !this->DoesIntersectHitbox( Shot->m_Target.m_Player, Shot->m_Target.m_Hitbox.m_iHitbox, Shot->m_vecStartPosition, Impact ) )
					continue;

				Shot->m_bDidIntersectPlayer = true;
				Shot->m_vecEndPosition = Impact;
				break;
			}
			this->AdjustPlayerRecord( Shot->m_Target.m_Player, LagRecords.back( ) );
		}

		g_World->OnRageBotFire( Shot->m_vecStartPosition, Shot->m_vecImpacts.back( ) );
		if ( Shot->m_bHasBeenHurted )
		{
			for ( auto& Impact : Shot->m_vecImpacts )
				g_HitMarkers->OnRageBotFire( Impact );
		
			g_Globals.m_ShotData.erase( g_Globals.m_ShotData.begin( ) + i );
			continue;
		}

		if ( !Shot->m_Target.m_Player->IsAlive( ) )
		{
			g_Globals.m_ShotData.erase( g_Globals.m_ShotData.begin( ) + i );
			continue;
		}

		float_t flClientYaw = Math::AngleNormalize( Math::CalcAngle( Shot->m_vecStartPosition, Shot->m_Target.m_Hitbox.m_vecPoint ).yaw );
		float_t flServerYaw = Math::AngleNormalize( Math::CalcAngle( Shot->m_vecStartPosition, Shot->m_vecImpacts.back( ) ).yaw );

		bool bMissedShotDueToOcclusion = false;
		if ( Shot->m_vecStartPosition.DistTo( Shot->m_vecImpacts.back( ) ) < Shot->m_vecStartPosition.DistTo( Shot->m_Target.m_Hitbox.m_vecPoint ) )
		{
			if ( fabs( Math::AngleNormalize( fabs( Math::AngleDiff( flClientYaw, flServerYaw ) ) ) ) <= 5.0f )
			{
				NewAutoWall::pen_argument_t Argument;
				Argument.m_from = g_Globals.m_LocalPlayer;
				Argument.m_to = Shot->m_Target.m_Player;
				Argument.m_can_penetrate = true;
				Argument.m_damage = this->GetMinDamage( Shot->m_Target.m_Player );
				Argument.m_damage_penetration = this->GetMinDamage( Shot->m_Target.m_Player );
				Argument.m_pos = Shot->m_vecImpacts.back( );
				Argument.m_shoot_position = Shot->m_vecStartPosition;
				NewAutoWall::run_penetration( &Argument );

				if ( Argument.m_out_data.m_damage < 1.0f )
					bMissedShotDueToOcclusion = true;
			}
		}

		if ( Shot->m_bDidIntersectPlayer )
		{
			g_Globals.m_ResolverData.m_BruteSide[ Shot->m_Target.m_Player->EntIndex( ) ] = Shot->m_Target.m_LagRecord.m_RotationMode;
			g_Globals.m_ResolverData.m_MissedShots[ Shot->m_Target.m_Player->EntIndex( ) ]++;

			if ( g_Settings->m_bLogMisses )
				g_LogManager->PushLog( _S( "Missed shot due to animation desync" ), _S( "h" ) );
		}
		else if ( bMissedShotDueToOcclusion )
		{
			if ( g_Settings->m_bLogMisses )
				g_LogManager->PushLog( _S( "Missed shot due to occlusion error" ), _S( "h" ) );
		}
		else
		{
			if ( g_Settings->m_bLogMisses )
				g_LogManager->PushLog( _S( "Missed shot due to weapon spread" ), _S( "h" ) );
		}

		g_Globals.m_ShotData.erase( g_Globals.m_ShotData.begin( ) + i );
	}
}

void C_RageBot::BackupPlayers( )
{
	for ( int nPlayerID = 1; nPlayerID <= g_Globals.m_Interfaces.m_GlobalVars->m_iMaxClients; nPlayerID++ )
	{
		C_BasePlayer* pPlayer = C_BasePlayer::GetPlayerByIndex( nPlayerID );
		if ( !pPlayer || !pPlayer->IsAlive( ) || pPlayer->IsDormant( ) || pPlayer->m_iTeamNum( ) == pPlayer->m_iTeamNum( ) || !pPlayer->IsPlayer( ) )
			continue;

		if ( g_Globals.m_CachedPlayerRecords[ nPlayerID ].empty( ) )
			continue;

		m_BackupData[ nPlayerID ] = g_Globals.m_CachedPlayerRecords[ nPlayerID ].back( );
		m_BackupData[ nPlayerID ].m_AdjustTick = g_PacketManager->GetModifableCommand( )->m_nCommand;
	}
}

void C_RageBot::RestorePlayers( )
{
	for ( int nPlayerID = 1; nPlayerID <= g_Globals.m_Interfaces.m_GlobalVars->m_iMaxClients; nPlayerID++ )
	{
		C_BasePlayer* pPlayer = C_BasePlayer::GetPlayerByIndex( nPlayerID );
		if ( !pPlayer || !pPlayer->IsAlive( ) || pPlayer->IsDormant( ) || pPlayer->m_iTeamNum( ) == pPlayer->m_iTeamNum( ) || !pPlayer->IsPlayer( ) )
			continue;

		if ( m_BackupData[ nPlayerID ].m_AdjustTick != g_PacketManager->GetModifableCommand( )->m_nCommand )
			continue;

		pPlayer->m_fFlags( ) = m_BackupData[ nPlayerID ].m_Flags;
		pPlayer->m_flSimulationTime( ) = m_BackupData[ nPlayerID ].m_SimulationTime;
		pPlayer->m_angEyeAngles( ) = m_BackupData[ nPlayerID ].m_EyeAngles;

		pPlayer->SetWorldOrigin( m_BackupData[ nPlayerID ].m_Origin );
		pPlayer->SetAbsoluteOrigin( m_BackupData[ nPlayerID ].m_Origin );

		std::memcpy( pPlayer->m_AnimationLayers( ), m_BackupData[ nPlayerID ].m_AnimationLayers.at( ROTATE_SERVER ).data( ), sizeof( C_AnimationLayer ) * ANIMATION_LAYER_COUNT );
		std::memcpy( pPlayer->m_aPoseParameters( ).data( ), m_BackupData[ nPlayerID ].m_PoseParameters.data( ), sizeof( float_t ) * MAXSTUDIOPOSEPARAM );
	
		pPlayer->GetCollideable( )->OBBMaxs( ) = m_BackupData[ nPlayerID ].m_Maxs;
		pPlayer->GetCollideable( )->OBBMins( ) = m_BackupData[ nPlayerID ].m_Mins;
	
		pPlayer->InvalidateBoneCache( );
		this->AdjustPlayerBones( pPlayer, m_BackupData[ nPlayerID ].m_Matricies[ ROTATE_SERVER ] );
	}
}

void C_RageBot::AutoStop( )
{
	if ( !this->CanAutoStop( ) || !m_RageSettings.m_bAutoStop )
		return;

	if ( !m_RageSettings.m_AutoStopOptions[ AUTOSTOP_ACCURACY ] )
	{
		if ( g_Globals.m_LocalPlayer->m_vecVelocity( ).Length2D( ) < g_Globals.m_LocalPlayer->GetMaxPlayerSpeed( ) * 0.34f )
		{
			g_AntiAim->SlowWalk( true );
			return;
		}
	}

	QAngle angResistance = QAngle( 0, 0, 0 );
    Math::VectorAngles( ( g_Globals.m_LocalPlayer->m_vecVelocity( ) * -1.f ), angResistance );

	angResistance.yaw = g_PacketManager->GetModifableCommand( )->m_angViewAngles.yaw - angResistance.yaw;
	angResistance.pitch = g_PacketManager->GetModifableCommand( )->m_angViewAngles.pitch - angResistance.pitch;

    Vector vecResistance = Vector( 0, 0, 0 );
    Math::AngleVectors( angResistance, vecResistance );

    g_PacketManager->GetModifableCommand( )->m_flForwardMove = std::clamp( vecResistance.x, -450.f, 450.0f );
	g_PacketManager->GetModifableCommand( )->m_flSideMove = std::clamp( vecResistance.y, -450.f, 450.0f );
}

void C_RageBot::AutoScope( )
{
	if ( g_Globals.m_LocalPlayer->m_bIsScoped( ) || !g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( ) || !g_Globals.m_LocalPlayer->m_hActiveWeapon( )->IsSniper( ) )
		return;

	if ( m_RageSettings.m_bAutoScope )
		g_PacketManager->GetModifableCommand( )->m_nButtons |= IN_ATTACK2;
}

void C_RageBot::AdjustPlayerBones( C_BasePlayer* pPlayer, std::array < matrix3x4_t, MAXSTUDIOBONES > aMatrix )
{
	std::memcpy( pPlayer->m_CachedBoneData( ).Base( ), aMatrix.data( ), sizeof( matrix3x4_t ) * pPlayer->m_CachedBoneData( ).Count( ) );
}

void C_RageBot::AdjustPlayerRecord( C_BasePlayer* pPlayer, C_LagRecord LagRecord )
{
	pPlayer->m_fFlags( ) = LagRecord.m_Flags;
	pPlayer->m_flSimulationTime( ) = LagRecord.m_SimulationTime;
	pPlayer->m_angEyeAngles( ) = LagRecord.m_EyeAngles;

	pPlayer->SetWorldOrigin( LagRecord.m_Origin );
	pPlayer->SetAbsoluteOrigin( LagRecord.m_Origin );

	std::memcpy( pPlayer->m_AnimationLayers( ), LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).data( ), sizeof( C_AnimationLayer ) * ANIMATION_LAYER_COUNT );
	std::memcpy( pPlayer->m_aPoseParameters( ).data( ), LagRecord.m_PoseParameters.data( ), sizeof( float_t ) * MAXSTUDIOPOSEPARAM );
	
	pPlayer->GetCollideable( )->OBBMaxs( ) = LagRecord.m_Maxs;
	pPlayer->GetCollideable( )->OBBMins( ) = LagRecord.m_Mins;
	
	pPlayer->InvalidateBoneCache( );
	return this->AdjustPlayerBones( pPlayer, LagRecord.m_Matricies[ ROTATE_SERVER ] );
}

typedef int32_t( __fastcall* ClipRayToHitbox_t )( const Ray_t&, mstudiobbox_t*, matrix3x4_t&, trace_t& );
bool C_RageBot::DoesIntersectHitbox( C_BasePlayer* pPlayer, int32_t iHitbox, Vector vecStartPosition, Vector vecEndPosition )
{
	trace_t Trace;
	Ray_t Ray;

	Ray.Init( vecStartPosition, vecEndPosition );
	Trace.fraction = 1.0f;
	Trace.startsolid = false;

	studiohdr_t* pStudioHdr = ( studiohdr_t* )( g_Globals.m_Interfaces.m_ModelInfo->GetStudiomodel( pPlayer->GetModel( ) ) );
	if ( !pStudioHdr)
		return false;

	mstudiohitboxset_t* pHitset = pStudioHdr->GetHitboxSet( pPlayer->m_nHitboxSet( ) );
	if ( !pHitset )
		return false;

	mstudiobbox_t* pHitbox = pHitset->GetHitbox( iHitbox );
	if ( !pHitbox )
		return false;

	return ( ( ClipRayToHitbox_t )( g_Globals.m_AddressList.m_ClipRayToHitbox ) )( Ray, pHitbox, pPlayer->m_CachedBoneData( )[ pHitbox->m_nBone ], Trace ) > -1;
}

bool C_RageBot::IsSafePoint( C_BasePlayer* pPlayer, C_LagRecord LagRecord, Vector vecStartPosition, Vector vecEndPosition, int32_t iHitbox )
{
	this->AdjustPlayerBones( pPlayer, LagRecord.m_Matricies[ ROTATE_LEFT ] );
	if ( !this->DoesIntersectHitbox( pPlayer, iHitbox, vecStartPosition, vecEndPosition ) )
	{
		this->AdjustPlayerBones( pPlayer, LagRecord.m_Matricies[ ROTATE_SERVER ] );
		return false; 
	}

	this->AdjustPlayerBones( pPlayer, LagRecord.m_Matricies[ ROTATE_CENTER ] );
	if ( !this->DoesIntersectHitbox( pPlayer, iHitbox, vecStartPosition, vecEndPosition ) )
	{
		this->AdjustPlayerBones( pPlayer, LagRecord.m_Matricies[ ROTATE_SERVER ] );
		return false;
	}

	this->AdjustPlayerBones( pPlayer, LagRecord.m_Matricies[ ROTATE_RIGHT ] );
	if ( !this->DoesIntersectHitbox( pPlayer, iHitbox, vecStartPosition, vecEndPosition ) )
	{
		this->AdjustPlayerBones( pPlayer, LagRecord.m_Matricies[ ROTATE_SERVER ] );
		return false;
	}

	this->AdjustPlayerBones( pPlayer, LagRecord.m_Matricies[ ROTATE_SERVER ] );
	return true;
}

std::vector < Vector > C_RageBot::GetHitboxPoints( C_BasePlayer* pPlayer, C_LagRecord LagRecord, Vector vecStartPosition, int32_t iHitbox )
{
	studiohdr_t* pStudioHdr = ( studiohdr_t* )( g_Globals.m_Interfaces.m_ModelInfo->GetStudiomodel( pPlayer->GetModel( ) ) );
	if ( !pStudioHdr)
		return { };

	mstudiohitboxset_t* pHitset = pStudioHdr->GetHitboxSet( pPlayer->m_nHitboxSet( ) );
	if ( !pHitset )
		return { };

	mstudiobbox_t* pHitbox = pHitset->GetHitbox( iHitbox );
	if ( !pHitbox )
		return { };

	float_t flModifier = fmaxf( pHitbox->m_flRadius, 0.f );

	Vector vecMax;
	Vector vecMin;

	Math::VectorTransform( Vector( pHitbox->m_vecBBMax.x + flModifier, pHitbox->m_vecBBMax.y + flModifier, pHitbox->m_vecBBMax.z + flModifier ), LagRecord.m_Matricies[ ROTATE_SERVER ][ pHitbox->m_nBone ], vecMax );
	Math::VectorTransform( Vector( pHitbox->m_vecBBMin.x - flModifier, pHitbox->m_vecBBMin.y - flModifier, pHitbox->m_vecBBMin.z - flModifier ), LagRecord.m_Matricies[ ROTATE_SERVER ][ pHitbox->m_nBone ], vecMin );

	Vector vecCenter = ( vecMin + vecMax ) * 0.5f;

	QAngle angAngle = Math::CalcAngle( vecStartPosition, vecCenter );

	Vector vecForward;
	Math::AngleVectors( angAngle, vecForward );

	Vector vecRight = vecForward.Cross( Vector( 0, 0, 2.33f ) );
	Vector vecLeft = Vector( -vecRight.x, -vecRight.y, vecRight.z );

	Vector vecTop = Vector( 0, 0, 3.25f );
	Vector vecBottom = Vector( 0, 0, -3.25f );

	int32_t iAngleToPlayer = ( int32_t )( fabsf( Math::NormalizeAngle( Math::NormalizeAngle( Math::NormalizeAngle( pPlayer->m_angEyeAngles( ).yaw ) - Math::NormalizeAngle( Math::CalcAngle( vecStartPosition, pPlayer->GetAbsOrigin( ) ).yaw + 180.0f ) ) ) ) );
	int32_t iDistanceToPlayer = ( int32_t )( g_Globals.m_LocalPlayer->GetAbsOrigin( ).DistTo( pPlayer->m_vecOrigin( ) ) );

	std::vector < Vector > aMultipoints = { };
	if ( iHitbox == HITBOX_HEAD )
	{
		if ( !m_RageSettings.m_Multipoints[ 0 ] )
		{
			aMultipoints.emplace_back( vecCenter );
			return aMultipoints;
		}

		float_t flScale = m_RageSettings.m_iHeadScale;

		aMultipoints.emplace_back( vecCenter );
		aMultipoints.emplace_back( vecCenter + ( ( vecTop + vecRight ) * ( flScale / 75.0f ) ) );
		aMultipoints.emplace_back( vecCenter + ( ( vecTop + vecLeft ) * ( flScale / 75.0f ) ) );
	}
	else if ( iHitbox == HITBOX_CHEST )
	{
		if ( !m_RageSettings.m_Multipoints[ 1 ] )
		{
			aMultipoints.emplace_back( vecCenter );
			return aMultipoints;
		}

		float_t flModifier = 3.05f * ( m_RageSettings.m_iBodyScale / 80.0f );
		if ( iAngleToPlayer < 140 && iAngleToPlayer > 30 )
			flModifier = 2.0f * ( m_RageSettings .m_iBodyScale / 80.0f );

		aMultipoints.emplace_back( vecCenter + Vector( 0, 0, 3 ) );
		aMultipoints.emplace_back( vecCenter + vecRight * flModifier + Vector( 0, 0, 3 ) );
		aMultipoints.emplace_back( vecCenter + vecLeft * flModifier + Vector( 0, 0, 3 ) );
	}
	else if ( iHitbox == HITBOX_STOMACH )
	{
		float_t flModifier = m_RageSettings.m_iBodyScale / 33.0f;
		if ( iAngleToPlayer < 120 && iAngleToPlayer > 60 )
			flModifier = ( m_RageSettings.m_iBodyScale / 33.0f ) - 0.10f;

		aMultipoints.emplace_back( vecCenter + Vector( 0, 0, 3.0f ) );
		aMultipoints.emplace_back( vecCenter + vecRight * flModifier + Vector( 0.0f, 0.0f, 3.0f ) );
		aMultipoints.emplace_back( vecCenter + vecLeft * flModifier + Vector( 0.0f, 0.0f, 3.0f ) );
	}
	else if ( iHitbox == HITBOX_PELVIS )
	{
		float_t flModifier = m_RageSettings.m_iBodyScale / 33.0f;
		if ( iAngleToPlayer < 120 && iAngleToPlayer > 60 )
			flModifier = ( m_RageSettings.m_iBodyScale / 33.0f ) - 0.10f;

		aMultipoints.emplace_back( vecCenter - Vector( 0.0f, 0.0f, 2.0f ) );
		aMultipoints.emplace_back( vecCenter + vecRight * flModifier - Vector( 0.0f, 0.0f, 2.0f ) );
		aMultipoints.emplace_back( vecCenter + vecLeft * flModifier - Vector( 0.0f, 0.0f, 2.0f ) );
	}
	else if ( iHitbox == HITBOX_LEFT_FOOT || iHitbox == HITBOX_RIGHT_FOOT || iHitbox == HITBOX_LEFT_THIGH || iHitbox == HITBOX_RIGHT_THIGH )
	{
		if ( !m_RageSettings.m_Multipoints[ 5 ] )
		{
			aMultipoints.emplace_back( vecCenter );
			return aMultipoints;
		}

		Vector vecAddition = vecLeft;
		if ( iHitbox == HITBOX_LEFT_FOOT || iHitbox == HITBOX_LEFT_THIGH )
			vecAddition = vecRight;
		else if ( iHitbox == HITBOX_RIGHT_FOOT || iHitbox == HITBOX_RIGHT_THIGH )
			vecAddition = vecLeft;

		if ( iHitbox == HITBOX_LEFT_THIGH || iHitbox == HITBOX_RIGHT_THIGH )
			vecCenter -= Vector( 0.0f, 0.0f, 2.5f );

		aMultipoints.emplace_back( vecCenter - ( vecAddition * 0.90f ) );
	}
	else if ( iHitbox == HITBOX_LEFT_FOREARM || iHitbox == HITBOX_RIGHT_FOREARM )
		aMultipoints.emplace_back( vecCenter - ( iHitbox == HITBOX_LEFT_FOREARM ? vecLeft : -vecLeft ) );

	return aMultipoints;
}

void C_RageBot::AutoRevolver( )
{
	if ( ( g_Globals.m_LocalPlayer->m_fFlags( ) & FL_FROZEN ) || ( *( g_Globals.m_Interfaces.m_GameRules ) )->IsFreezePeriod( ) )
		return;

	C_BaseCombatWeapon* pCombatWeapon = g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( );
	if ( !pCombatWeapon || !m_RageSettings.m_bEnabled || pCombatWeapon->m_iItemDefinitionIndex( ) != WEAPON_REVOLVER )
		return;

	g_PacketManager->GetModifableCommand( )->m_nButtons &= ~IN_ATTACK2;
	if ( !( g_PacketManager->GetModifableCommand( )->m_nButtons & IN_ATTACK ) )
	{
		if ( g_Globals.m_AccuracyData.m_bCanFire_Default )
		{
			if ( g_Globals.m_Revolver.m_flPostponeFireReadyTime <= TICKS_TO_TIME( g_Globals.m_LocalPlayer->m_nTickBase( ) ) )
			{
				if ( pCombatWeapon->m_flNextSecondaryAttack( ) <= TICKS_TO_TIME( g_Globals.m_LocalPlayer->m_nTickBase( ) ) )
					g_Globals.m_Revolver.m_flPostponeFireReadyTime = TICKS_TO_TIME( g_Globals.m_LocalPlayer->m_nTickBase( ) ) + 0.2f;
			}
			else
				g_PacketManager->GetModifableCommand( )->m_nButtons |= IN_ATTACK;
		}
	}
}

void C_RageBot::AdjustRevolverData( int32_t nCommand, int32_t nButtons )
{
	if ( ( g_Globals.m_LocalPlayer->m_fFlags( ) & FL_FROZEN ) || ( *( g_Globals.m_Interfaces.m_GameRules ) )->IsFreezePeriod( ) )
		return;

	C_BaseCombatWeapon* pCombatWeapon = g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( );
	if ( !pCombatWeapon || !pCombatWeapon->GetWeaponData( ) )
		return;

	g_Globals.m_Revolver.m_TickRecords[ nCommand % MULTIPLAYER_BACKUP ] = g_Globals.m_LocalPlayer->m_nTickBase( );
	g_Globals.m_Revolver.m_FireStates[ nCommand % MULTIPLAYER_BACKUP ] = g_Globals.m_LocalPlayer->CanFire( );
	g_Globals.m_Revolver.m_PrimaryAttack[ nCommand % MULTIPLAYER_BACKUP ] = nButtons & IN_ATTACK;

	int32_t nFireCommand = 0;
	for ( int32_t nAttackCommand = nCommand - 1; nAttackCommand > nCommand - ( g_Networking->GetTickRate( ) >> 1 ); nAttackCommand-- )
	{
		if ( !g_Globals.m_Revolver.m_FireStates[ nAttackCommand % MULTIPLAYER_BACKUP ] || !g_Globals.m_Revolver.m_PrimaryAttack[ nAttackCommand % MULTIPLAYER_BACKUP ] )
			continue;

		nFireCommand = nAttackCommand;
	}

	if ( nFireCommand )
	{
		if ( pCombatWeapon->m_iItemDefinitionIndex( ) == WEAPON_REVOLVER )
			pCombatWeapon->m_flPostponeFireReadyTime( ) = TICKS_TO_TIME( g_Globals.m_Revolver.m_TickRecords[ nFireCommand % MULTIPLAYER_BACKUP ] ) + 0.2f;
	}
}

void C_RageBot::TaserBot( )
{

}

float_t C_RageBot::GetHitChance( C_TargetData Target )
{
	Vector vecShootPosition = g_Globals.m_LocalData.m_vecShootPosition;
	QAngle angViewAngles = Math::CalcAngle( vecShootPosition, Target.m_Hitbox.m_vecPoint );
	Math::Normalize3( angViewAngles );

	Vector vecForward, vecRight, vecUp;
	Math::AngleVectors( angViewAngles, vecForward, vecRight, vecUp );

	C_BaseCombatWeapon* pWeapon = g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( );
	if ( !pWeapon )
		return 0.0f;

	if ( !g_Globals.m_AccuracyData.m_bHasValidAccuracyData )
		return 0.0f;

	C_CSWeaponData* pWeaponData = pWeapon->GetWeaponData( );
	if ( !pWeaponData )
		return 0.0f;

	int32_t iMaxDistance = pWeaponData->m_flRange;
	if ( vecShootPosition.DistTo( Target.m_Hitbox.m_vecPoint ) > iMaxDistance )
		return false;

	if ( g_Globals.m_AccuracyData.m_bHasMaximumAccuracy )
		return 100.0f;
		
	float_t flWeaponSpread = pWeapon->GetSpread( );
	float_t flWeaponInaccuracy = pWeapon->GetInaccuracy( );
	int32_t iHitchanceValidSeeds = 0;

	for ( int32_t iSeed = 0; iSeed < 256; iSeed++ )
	{
		float_t flInaccuracy = g_Globals.m_AccuracyData.m_aMultiValues[ iSeed ] * flWeaponInaccuracy;
		float_t flSpread = g_Globals.m_AccuracyData.m_aMultiValues[ iSeed ] * flWeaponSpread;

		Vector vecSpreadView
		( 
			( cos( g_Globals.m_AccuracyData.m_aInaccuracy[ iSeed ] ) * flInaccuracy ) + ( cos( g_Globals.m_AccuracyData.m_aSpread[ iSeed ] ) * flSpread ), 
			( sin( g_Globals.m_AccuracyData.m_aInaccuracy[ iSeed ] ) * flInaccuracy ) + ( sin( g_Globals.m_AccuracyData.m_aSpread[ iSeed ] ) * flSpread ), 
			0.0f 
		);

		Vector vecDirection = Vector( 0.0f, 0.0f, 0.0f );
		vecDirection.x = vecForward.x + ( vecSpreadView.x * vecRight.x ) + ( vecSpreadView.y * vecUp.x );
		vecDirection.y = vecForward.y + ( vecSpreadView.x * vecRight.y ) + ( vecSpreadView.y * vecUp.y );
		vecDirection.z = vecForward.z + ( vecSpreadView.x * vecRight.z ) + ( vecSpreadView.y * vecUp.z );
		vecDirection.Normalized( );

		QAngle angSpreadView;
		Math::VectorAngles( vecDirection, vecUp, angSpreadView );
		Math::Normalize3( angSpreadView );

		Vector vecForwardView;
		Math::AngleVectors( angSpreadView, vecForwardView );
		vecForwardView.NormalizeInPlace( );

		vecForwardView = vecShootPosition + vecForwardView * iMaxDistance;
		if ( this->DoesIntersectHitbox( Target.m_Player, Target.m_Hitbox.m_iHitbox, vecShootPosition, vecForwardView ) )
			iHitchanceValidSeeds++;

		if ( ( 256 - iSeed + iHitchanceValidSeeds ) < static_cast< int32_t >( ( float_t )( m_RageSettings.m_iHitChance ) * 2.56f ) )
			return ( float_t )( iHitchanceValidSeeds ) / 2.56f;
	}

	return ( float_t )( iHitchanceValidSeeds ) / 2.56f;
}

float_t C_RageBot::GetAccuracyBoost( C_TargetData Target )
{
	Vector vecShootPosition = g_Globals.m_LocalData.m_vecShootPosition;
	QAngle angViewAngles = Math::CalcAngle( vecShootPosition, Target.m_Hitbox.m_vecPoint );
	Math::Normalize3( angViewAngles );

	Vector vecForward, vecRight, vecUp;
	Math::AngleVectors( angViewAngles, vecForward, vecRight, vecUp );

	C_BaseCombatWeapon* pWeapon = g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( );
	if ( !pWeapon )
		return 0.0f;
	
	if ( pWeapon->m_iItemDefinitionIndex( ) == WEAPON_TASER )
		return 100.0f;

	C_CSWeaponData* pWeaponData = pWeapon->GetWeaponData( );
	if ( !pWeaponData || !g_Globals.m_AccuracyData.m_bHasValidAccuracyData )
		return 0.0f;

	if ( g_Globals.m_AccuracyData.m_bHasMaximumAccuracy || g_Globals.m_AccuracyData.m_bDoingSecondShot )
		return 100.0f;

	float_t flWeaponSpread = pWeapon->GetSpread( );
	float_t flWeaponInaccuracy = pWeapon->GetInaccuracy( );
	int32_t iAccuracyValidSeeds = 0;
		
	for ( int32_t iSeed = 0; iSeed < 256; iSeed++ )
	{
		float_t flInaccuracy = g_Globals.m_AccuracyData.m_aMultiValues[ iSeed ] * flWeaponInaccuracy;
		float_t flSpread = g_Globals.m_AccuracyData.m_aMultiValues[ iSeed ] * flWeaponSpread;

		Vector vecSpreadView( ( cos( g_Globals.m_AccuracyData.m_aInaccuracy[ iSeed ] ) * flInaccuracy) + ( cos( g_Globals.m_AccuracyData.m_aSpread[ iSeed ] ) * flSpread), ( sin( g_Globals.m_AccuracyData.m_aInaccuracy[ iSeed ] ) * flInaccuracy ) + ( sin( g_Globals.m_AccuracyData.m_aSpread[ iSeed ] ) * flSpread ), 0.0f );

		Vector vecDirection = Vector( 0.0f, 0.0f, 0.0f );
		vecDirection.x = vecForward.x + ( vecSpreadView.x * vecRight.x ) + ( vecSpreadView.y * vecUp.x );
		vecDirection.y = vecForward.y + ( vecSpreadView.x * vecRight.y ) + ( vecSpreadView.y * vecUp.y );
		vecDirection.z = vecForward.z + ( vecSpreadView.x * vecRight.z ) + ( vecSpreadView.y * vecUp.z );
		vecDirection.Normalized( );

		QAngle angSpreadView;
		Math::VectorAngles( vecDirection, vecUp, angSpreadView );
		Math::Normalize3( angSpreadView );

		Vector vecForwardView;
		Math::AngleVectors( angSpreadView, vecForwardView );
		vecForwardView.NormalizeInPlace( );
		
		vecForwardView = vecShootPosition + ( vecForwardView * pWeaponData->m_flRange );
		NewAutoWall::pen_argument_t Argument;
		Argument.m_from = g_Globals.m_LocalPlayer;
		Argument.m_to = Target.m_Player;
		Argument.m_can_penetrate = true;
		Argument.m_damage = this->GetMinDamage( Target.m_Player );
		Argument.m_damage_penetration = this->GetMinDamage( Target.m_Player );
		Argument.m_pos = vecForwardView;
		Argument.m_shoot_position = vecShootPosition;
		NewAutoWall::run_penetration( &Argument );

		if ( Argument.m_out_data.m_damage > 0.0f )
			iAccuracyValidSeeds++;

		if ( ( 256 - iSeed + iAccuracyValidSeeds ) < static_cast< int32_t >( ( float_t )( m_RageSettings.m_iAccuracyBoost ) * 2.56f ) )
			return ( float_t )( iAccuracyValidSeeds ) / 2.56f;
	}

	return ( float_t )( iAccuracyValidSeeds ) / 2.56f;
}

bool C_RageBot::HasMaximumAccuracy( )
{
	C_BaseCombatWeapon* pWeapon = g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( );
	if ( !pWeapon )
		return false;

	if ( !pWeapon->IsGun( ) )
		return false;

	C_CSWeaponData* pWeaponData = pWeapon->GetWeaponData( );
	if ( !pWeaponData )
		return false;

	if ( g_Globals.m_LocalPlayer->m_flVelocityModifier( ) < 1.0f )
		return false;

	return pWeapon->GetInaccuracy( ) - g_Globals.m_AccuracyData.m_flMinInaccuracy < 0.0001f;
}

bool C_RageBot::CanAutoStop( )
{
	if ( !g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( ) || !g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( )->IsGun( ) )
		return false;

	if ( !g_Globals.m_AccuracyData.m_bCanFire_Default || !g_Globals.m_AccuracyData.m_bCanFire_Shift || g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( )->m_iClip1( ) <= 0 )
		return false;

	if ( !( g_Globals.m_LocalPlayer->m_fFlags( ) & FL_ONGROUND ) )
		return false;

	if ( !g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( ) || !g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( )->GetWeaponData( ) )
		return false;

	return m_RageSettings.m_bAutoStop;
}