#include "Autowall.hpp"
#include "../SDK/Math/Math.hpp"

void C_AutoWall::CacheWeaponData( )
{
	m_PenetrationData.m_Weapon = g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( );
	if ( !m_PenetrationData.m_Weapon )
		return;

	m_PenetrationData.m_WeaponData = m_PenetrationData.m_Weapon->GetWeaponData( );
	if ( !m_PenetrationData.m_WeaponData )
		return;

	m_PenetrationData.m_flMaxRange = m_PenetrationData.m_WeaponData->m_flRange;
	m_PenetrationData.m_flWeaponDamage = m_PenetrationData.m_WeaponData->m_iDamage;
	m_PenetrationData.m_flPenetrationPower = m_PenetrationData.m_WeaponData->m_flPenetration;
	m_PenetrationData.m_flPenetrationDistance = 3000.0f;
}

bool C_AutoWall::HandleBulletPenetration( )
{
	bool bSolidSurf = ( ( m_PenetrationData.m_EnterTrace.contents >> 3 ) & CONTENTS_SOLID );
	bool bLightSurf = ( m_PenetrationData.m_EnterTrace.surface.flags >> 7 ) & SURF_LIGHT;
	
	int32_t nEnterMaterial = g_Globals.m_Interfaces.m_PropPhysics->GetSurfaceData( m_PenetrationData.m_EnterTrace.surface.surfaceProps )->game.material;
	if ( !m_PenetrationData.m_PenetrationCount && !bLightSurf && !bSolidSurf && nEnterMaterial != 9 )
	{
		if ( nEnterMaterial != 71 )
			return false;
	}

	Vector vecEnd;
	if ( m_PenetrationData.m_PenetrationCount <= 0 || m_PenetrationData.m_flPenetrationPower <= 0.0f )
		return false;

	CGameTrace pExitTrace;
	if ( !TraceToExit( m_PenetrationData.m_EnterTrace.endpos, vecEnd, &m_PenetrationData.m_EnterTrace, &pExitTrace ) )
	{
		if ( !( g_Globals.m_Interfaces.m_EngineTrace->GetPointContents( vecEnd, 0x600400B ) & 0x600400B ) )
		{
			m_PenetrationData.m_flCurrentDamage = 0.0f;
			m_PenetrationData.m_PenetrationCount = 0;

			return false;
		}
	}

	surfacedata_t* pEnterSurfaceData = g_Globals.m_Interfaces.m_PropPhysics->GetSurfaceData(m_PenetrationData.m_EnterTrace.surface.surfaceProps);
	unsigned short pEnterMaterial = pEnterSurfaceData->game.material;
	float_t flEnterSurfacePenetrationModifier = pEnterSurfaceData->game.flPenetrationModifier;

	surfacedata_t* pExitSurfaceData = g_Globals.m_Interfaces.m_PropPhysics->GetSurfaceData( pExitTrace.surface.surfaceProps );
	unsigned short pExitMaterial = pExitSurfaceData->game.material;

	float_t flExitSurfacePenetration = pExitSurfaceData->game.flPenetrationModifier;
	float_t flExitSurfaceModifier = pExitSurfaceData->game.flDamageModifier;

	float flFinalDamageModifier = 0.16f;
	float flCombinedPenetrationModifier = 0.0f;

	if ( pEnterMaterial == CHAR_TEX_GRATE || pEnterMaterial == CHAR_TEX_GLASS )
	{
		flCombinedPenetrationModifier = 3.f;
		flFinalDamageModifier = 0.05f;
	}
	else if ( bSolidSurf || bLightSurf )
	{
		flCombinedPenetrationModifier = 1.f;
		flFinalDamageModifier = 0.16f;
	}
	else if ( pEnterMaterial == CHAR_TEX_FLESH )
	{
		flCombinedPenetrationModifier = g_Globals.m_ConVars.m_DamageBulletPenetration->GetFloat( );
		flFinalDamageModifier = 0.16f;
	}
	else
	{
		flCombinedPenetrationModifier = ( flEnterSurfacePenetrationModifier + flExitSurfacePenetration ) / 2.f;
		flFinalDamageModifier = 0.16f;
	}
 
	//Do our materials line up?
	if ( pEnterMaterial == pExitMaterial )
	{
		if ( pExitMaterial == CHAR_TEX_CARDBOARD || pExitMaterial == CHAR_TEX_WOOD )
			flCombinedPenetrationModifier = 3.f;
		else if ( pExitMaterial == CHAR_TEX_PLASTIC )
			flCombinedPenetrationModifier = 2.f;
	}

	float flModifier = fmaxf( 0.0f, 1.0f / flCombinedPenetrationModifier );
	float flThickness = ( pExitTrace.endpos - m_PenetrationData.m_EnterTrace.endpos ).LengthSqr( );
	float flLostDamage = fmaxf(
			( ( flModifier * flThickness ) / 24.f )
			+ ( ( m_PenetrationData.m_flCurrentDamage * flFinalDamageModifier )
				+ ( fmaxf( 3.75f / m_PenetrationData.m_flPenetrationPower, 0.f ) * 3.f * flModifier ) ), 0.f );

	if ( flLostDamage > m_PenetrationData.m_flCurrentDamage )
		return false;

	if ( flLostDamage > 0.0f )
		m_PenetrationData.m_flCurrentDamage -= flLostDamage;

	if ( m_PenetrationData.m_flCurrentDamage < 1.0f )
		return false;

	m_PenetrationData.m_vecShootPosition = pExitTrace.endpos;
	m_PenetrationData.m_PenetrationCount--;

	return true;
}

bool C_AutoWall::TraceToExit( Vector vecStart, Vector& vecEnd, CGameTrace* pEnterTrace, CGameTrace* pExitTrace )
{
	int	nFirstContents = 0;
		
	std::array < std::uintptr_t, 4 > aFilter
	= 
	{
		*( std::uintptr_t* )( g_Globals.m_AddressList.m_TraceFilterSimple ),
		NULL,
		NULL,
		NULL
	};

	float_t flDistance = 0.0f;
	while ( flDistance <= 90.0f )
	{
		// increase distance
		flDistance += 4.0f;

		// calc new end
		vecEnd = vecStart + ( m_PenetrationData.m_vecDirection * flDistance );

		// cache contents
		int nContents = g_Globals.m_Interfaces.m_EngineTrace->GetPointContents( vecEnd, 0x4600400B, nullptr );
		if ( !nFirstContents )
			nFirstContents = nContents;

		// check contents
		if ( nContents & 0x600400B && ( !( nContents & 0x40000000 ) || nFirstContents == nContents ) )
			continue;

		// trace line
		g_Globals.m_Interfaces.m_EngineTrace->TraceLine( vecEnd, vecStart, 0x4600400B, g_Globals.m_LocalPlayer, NULL, pExitTrace );
		
		// clip trace to player
		ClipTraceToPlayers( ( C_BasePlayer* )( pExitTrace->hit_entity ), ( vecEnd - ( m_PenetrationData.m_vecDirection * 4.0f ) ), vecEnd, pExitTrace, ( CTraceFilter* )( aFilter.data( ) ), 0x4600400B );
			
		// check solid and falgs
		if ( pExitTrace->startsolid && pExitTrace->surface.flags & SURF_HITBOX )
		{
			std::array < uintptr_t, 5 > aSkipTwoEntities 
			=
			{
				*( std::uintptr_t* )( g_Globals.m_AddressList.m_TraceFilterSkipTwoEntities ),
				( uintptr_t )( g_Globals.m_LocalPlayer ),
				NULL,
				NULL,
				( std::uintptr_t )( pExitTrace->hit_entity ),
			};

			Ray_t NewRay;
			NewRay.Init( vecEnd, vecStart );

			g_Globals.m_Interfaces.m_EngineTrace->TraceRay( NewRay, 0x600400B, ( CTraceFilter* )( aSkipTwoEntities.data( ) ), pExitTrace );
			if ( pExitTrace->DidHit( ) && !pExitTrace->startsolid )
			{
				vecEnd = pExitTrace->endpos;
				return true;
			}

			continue;
		}

		if ( !pExitTrace->DidHit( ) || pExitTrace->startsolid )
		{
			if ( pEnterTrace->hit_entity != g_Globals.m_Interfaces.m_EntityList->GetClientEntity( NULL ) ) 
			{
				if ( pExitTrace->hit_entity && ( ( C_BaseEntity* )( pExitTrace->hit_entity ) )->IsBreakableEntity( ) )
				{
					pExitTrace->surface.surfaceProps = pEnterTrace->surface.surfaceProps;
					pExitTrace->endpos = vecStart + m_PenetrationData.m_vecDirection;

					return true;
				}
			}

			continue;
		}

		if ( pExitTrace->surface.flags & 0x80 )
		{
			if ( pEnterTrace->hit_entity && ( ( C_BaseEntity* )( pEnterTrace->hit_entity ) )->IsBreakableEntity( ) && pExitTrace->hit_entity && ( ( C_BaseEntity* )( pExitTrace->hit_entity ) )->IsBreakableEntity( ) )
			{
				vecEnd = pExitTrace->endpos;
				return true;
			}
				
			if ( !( pEnterTrace->surface.flags & 0x80u ) )
				continue;
		}

		if ( pExitTrace->plane.normal.Dot( m_PenetrationData.m_vecDirection ) > 1.0f )
			return false;
			
		vecEnd -= m_PenetrationData.m_vecDirection * ( pExitTrace->fraction * 4.0f );
		return true;
	}

	return false;
}

bool C_AutoWall::SimulateFireBullet( )
{
	if ( !m_PenetrationData.m_Weapon || !m_PenetrationData.m_WeaponData )
		return false;

	while ( m_PenetrationData.m_PenetrationCount > 0 )
	{
		float_t flRemains = ( m_PenetrationData.m_flMaxRange - m_PenetrationData.m_flCurrentDistance );
		Vector vecEnd = m_PenetrationData.m_vecShootPosition + ( m_PenetrationData.m_vecDirection * flRemains );
	
		Ray_t Ray;
		Ray.Init( m_PenetrationData.m_vecShootPosition, vecEnd );
		
		g_Globals.m_Interfaces.m_EngineTrace->TraceLine( m_PenetrationData.m_vecShootPosition, vecEnd, MASK_SHOT_HULL | CONTENTS_HITBOX, NULL, NULL, &m_PenetrationData.m_EnterTrace );
		std::array < uintptr_t, 5 > aSkipTwoEntities 
		=
		{
			*( std::uintptr_t* )( g_Globals.m_AddressList.m_TraceFilterSkipTwoEntities ),
			( uintptr_t )( g_Globals.m_LocalPlayer ),
			NULL,
			NULL,
			( std::uintptr_t )( m_PenetrationData.m_EnterTrace.hit_entity ),
		};

		this->ClipTraceToPlayers( ( C_BasePlayer* )( m_PenetrationData.m_EnterTrace.hit_entity ), m_PenetrationData.m_vecShootPosition, vecEnd + ( m_PenetrationData.m_vecDirection * 40.0f ), &m_PenetrationData.m_EnterTrace, ( CTraceFilter* )( aSkipTwoEntities.data( ) ), MASK_SHOT_HULL | CONTENTS_HITBOX );
	
		surfacedata_t* pEnterSurfaceData = g_Globals.m_Interfaces.m_PropPhysics->GetSurfaceData( m_PenetrationData.m_EnterTrace.surface.surfaceProps );
		if ( ( int )( m_PenetrationData.m_EnterTrace.fraction ) )
			break;

		float_t flEnterSurfacePenetrationModifier = pEnterSurfaceData->game.flPenetrationModifier;
		if ( m_PenetrationData.m_flCurrentDistance > m_PenetrationData.m_flPenetrationDistance && m_PenetrationData.m_flPenetrationPower > 0.f || flEnterSurfacePenetrationModifier < 0.1f )
			break;

		m_PenetrationData.m_flCurrentDistance += m_PenetrationData.m_EnterTrace.fraction * flRemains;
		m_PenetrationData.m_flCurrentDamage *= std::pow( m_PenetrationData.m_WeaponData->m_flRangeModifier, ( m_PenetrationData.m_flCurrentDamage / 500.0f ) );
	
		if ( m_PenetrationData.m_EnterTrace.hit_entity )
		{
			C_BasePlayer* pPlayer = ( C_BasePlayer* )( m_PenetrationData.m_EnterTrace.hit_entity );
			if ( pPlayer && pPlayer->IsPlayer( ) )
			{
				if ( !pPlayer->IsAlive( ) || pPlayer->m_iTeamNum( ) == g_Globals.m_LocalPlayer->m_iTeamNum( ) )
					return false;
			
				if ( m_PenetrationData.m_EnterTrace.hitgroup != HITGROUP_GEAR && m_PenetrationData.m_EnterTrace.hitgroup != HITGROUP_GENERIC )
				{
					this->ScaleDamage( m_PenetrationData.m_EnterTrace, m_PenetrationData.m_flCurrentDamage );
					return true;
				}
			}
		}

		if ( this->HandleBulletPenetration( ) && m_PenetrationData.m_flCurrentDamage > 0.0f )
			continue;
		
		break;
	}

	return false;
}

void C_AutoWall::ScaleDamage( CGameTrace Trace, float_t& flDamage )
{
	bool bHasHeavyArmour = ( ( C_BasePlayer* )( Trace.hit_entity ) )->m_bHasHeavyArmor( );
	switch ( Trace.hitgroup )
	{
		case HITGROUP_HEAD:

			if ( bHasHeavyArmour )
				flDamage *= 2.0f;
			else
				flDamage *= 4.f;

			break;

		case HITGROUP_CHEST:
		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM:

			flDamage *= 1.f;
			break;

		case HITGROUP_STOMACH:

			flDamage *= 1.25f;
			break;

		case HITGROUP_LEFTLEG:
		case HITGROUP_RIGHTLEG:

			flDamage *= 0.75f;
			break;
	}

	if ( !this->IsArmored( ( ( C_BasePlayer* )( Trace.hit_entity ) ), Trace.hitgroup ) )
		return;

	float fl47 = 1.f, flArmorBonusRatio = 0.5f;

	float_t flArmorRatio = m_PenetrationData.m_WeaponData->m_flArmorRatio * 0.5f;
	if ( bHasHeavyArmour )
	{
		flArmorBonusRatio = 0.33f;
		flArmorRatio = m_PenetrationData.m_WeaponData->m_flArmorRatio * 0.25f;
		fl47 = 0.33f;
	}

	float flNewDamage = flDamage * flArmorRatio;
	if ( bHasHeavyArmour )
		flNewDamage *= 0.85f;

	if ( ( ( flDamage - ( flDamage * flArmorRatio ) ) * ( fl47 * flArmorBonusRatio ) ) > ( ( C_BasePlayer* )( Trace.hit_entity ) )->m_ArmourValue( ) )
		flNewDamage = flDamage - ( ( ( C_BasePlayer* )( Trace.hit_entity ) )->m_ArmourValue( ) / flArmorBonusRatio );

	flDamage = flNewDamage;
}

bool C_AutoWall::IsArmored( C_BasePlayer* pPlayer, int32_t nHitGroup )
{
	bool bResult = false;
	if ( pPlayer->m_ArmourValue( ) > 0 )
	{
		switch ( nHitGroup )
		{
			case HITGROUP_GENERIC:
			case HITGROUP_CHEST:
			case HITGROUP_STOMACH:
			case HITGROUP_LEFTARM:
			case HITGROUP_RIGHTARM:
				bResult = true;
				break;
			case HITGROUP_HEAD:
				bResult = pPlayer->m_bHasHelmet( );
				break;
		}
	}

	return bResult;
}

void C_AutoWall::ClipTraceToPlayers( C_BasePlayer* pPlayer, Vector vecStart, Vector vecEnd, CGameTrace* Trace, CTraceFilter* pTraceFilter, uint32_t nMask )
{
	Ray_t Ray;
	Ray.Init( vecStart, vecEnd );

	CGameTrace NewTrace;
	if ( !pPlayer || !pPlayer->IsPlayer( ) || !pPlayer->IsAlive( ) || pPlayer->IsDormant( ) || pPlayer->m_iTeamNum( ) == g_Globals.m_LocalPlayer->m_iTeamNum( ) )
		return;

	if ( pTraceFilter && !pTraceFilter->ShouldHitEntity( pPlayer, NULL ) )
		return;

	float_t flRange = Math::DistanceToRay( pPlayer->WorldSpaceCenter( ), vecStart, vecEnd );
	if ( flRange < 0.0f || flRange > 60.0f )
		return;
		
	g_Globals.m_Interfaces.m_EngineTrace->ClipRayToEntity( Ray,  nMask, pPlayer, &NewTrace );
	if ( NewTrace.fraction > Trace->fraction )
		std::memcpy( Trace, &NewTrace, sizeof( CGameTrace ) );
}

float C_AutoWall::GetPointDamage( Vector vecShootPosition, Vector vecTargetPosition )
{
	if ( !m_PenetrationData.m_Weapon || !m_PenetrationData.m_WeaponData )
		return false;

	m_PenetrationData.m_vecShootPosition = vecShootPosition;
	m_PenetrationData.m_vecTargetPosition = vecTargetPosition;
	m_PenetrationData.m_vecDirection = GetPointDirection( vecShootPosition, vecTargetPosition );
	m_PenetrationData.m_flCurrentDamage = m_PenetrationData.m_flWeaponDamage;
	m_PenetrationData.m_flCurrentDistance = 0.0f;
	m_PenetrationData.m_PenetrationCount = 4;

	m_PenetrationData.m_flDamageModifier = 0.5f;
	m_PenetrationData.m_flPenetrationModifier = 1.0f;

	if ( SimulateFireBullet( ) )
		return m_PenetrationData.m_flCurrentDamage;

	return 0.0f;
}

Vector C_AutoWall::GetPointDirection( Vector vecShootPosition, Vector vecTargetPosition )
{
	Vector vecDirection;
	QAngle angDirection;

	Math::VectorAngles( vecTargetPosition - vecShootPosition, angDirection );
	Math::AngleVectors( angDirection, vecDirection );

	vecDirection.NormalizeInPlace( );
	return vecDirection;
}

bool CGameTrace::DidHitWorld( ) const
{
	return hit_entity == g_Globals.m_Interfaces.m_EntityList->GetClientEntity( NULL );
}

bool CGameTrace::DidHitNonWorldEntity( ) const
{
	return hit_entity != nullptr && !DidHitWorld();
}