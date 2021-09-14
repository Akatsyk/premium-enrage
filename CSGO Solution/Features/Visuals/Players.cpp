#include "Players.hpp"
#include "../Render.hpp"
#include "../SDK/Math/Math.hpp"
#include "../RageBot/RageBot.hpp"

RECT GetBBox(C_BaseEntity* ent)
{
	RECT aRect{ };

	auto pCollideable = ent->GetCollideable();
	if (!pCollideable)
		return aRect;

	auto min = pCollideable->OBBMins();
	auto max = pCollideable->OBBMaxs();

	const matrix3x4_t& trans = ent->m_rgflCoordinateFrame();
	Vector points[] = {
		Vector(min.x, min.y, min.z),
		Vector(min.x, max.y, min.z),
		Vector(max.x, max.y, min.z),
		Vector(max.x, min.y, min.z),
		Vector(max.x, max.y, max.z),
		Vector(min.x, max.y, max.z),
		Vector(min.x, min.y, max.z),
		Vector(max.x, min.y, max.z)
	};

	Vector aPointsTransformed[8];
	for (int i = 0; i < 8; i++)
		Math::VectorTransform(points[i], trans, aPointsTransformed[i]);

	Vector aScreenPoints[8] = { };
	for (int i = 0; i < 8; i++)
		if (g_Globals.m_Interfaces.m_DebugOverlay->ScreenPosition(aPointsTransformed[i], aScreenPoints[i]))
			return aRect;

	auto flLeft = aScreenPoints[0].x;
	auto flTop = aScreenPoints[0].y;
	auto flRight = aScreenPoints[0].x;
	auto flBottom = aScreenPoints[0].y;

	for (int i = 1; i < 8; i++)
	{
		if (flLeft > aScreenPoints[i].x)
			flLeft = aScreenPoints[i].x;
		if (flTop < aScreenPoints[i].y)
			flTop = aScreenPoints[i].y;
		if (flRight < aScreenPoints[i].x)
			flRight = aScreenPoints[i].x;
		if (flBottom > aScreenPoints[i].y)
			flBottom = aScreenPoints[i].y;
	}

	return RECT{ (long)(flLeft), (long)(flBottom), (long)(flRight), (long)(flTop) };
}

void C_PlayerESP::Instance( )
{
	m_CurrentSoundData.RemoveAll( );
	if ( !g_Globals.m_LocalPlayer )
		return;

	this->DormantPlayers( );
	for ( int i = 1; i < 65; ++i )
	{
		C_BasePlayer* pPlayer = C_BasePlayer::GetPlayerByIndex( i );
		if ( !pPlayer || !pPlayer->IsAlive( ) || !pPlayer->IsPlayer( ) )
		{
			m_aPlayerData[ i ].m_flPreviousAmmo = 0.0f;
			m_aPlayerData[ i ].m_flPreviousHealth = 0.0f;

			m_DormantPlayers[ i ].m_InDormant = 0.0f;
			m_DormantPlayers[ i ].m_ReceiveTime = 0.0f;
			m_DormantPlayers[ i ].m_Origin = Vector( 0, 0, 0 );

			continue;
		}

		if ( pPlayer->IsDormant( ) )
		{
			if ( !ForceDormant( i ) )
				continue;
		}

		m_aPlayerData[ pPlayer->EntIndex( ) ].m_aSettings = g_Settings->m_Enemies;
		if ( pPlayer->m_iTeamNum( ) == g_Globals.m_LocalPlayer->m_iTeamNum( ) )
		{
			m_aPlayerData[ pPlayer->EntIndex( ) ].m_aSettings = g_Settings->m_Teammates;
			if ( pPlayer == g_Globals.m_LocalPlayer ) 
				m_aPlayerData[ pPlayer->EntIndex( ) ].m_aSettings = g_Settings->m_LocalPlayer;
		}
		
		m_DormantPlayers[ i ].m_InDormant = g_Globals.m_Interfaces.m_GlobalVars->m_flRealTime;
		m_DormantPlayers[ i ].m_LastOrigin = pPlayer->GetAbsOrigin( );

		int32_t iScreenSizeX = 0;
		int32_t iScreenSizeY = 0;

		g_Globals.m_Interfaces.m_EngineClient->GetScreenSize( iScreenSizeX, iScreenSizeY );
		if ( pPlayer == g_Globals.m_LocalPlayer )
		{
			if ( !g_Globals.m_Interfaces.m_Input->m_bCameraInThirdPerson )
				continue;
		}
		else
		{
			Vector vecToScreen = Vector( 0, 0, 0 );
			if ( g_Globals.m_Interfaces.m_DebugOverlay->ScreenPosition( pPlayer->GetAbsOrigin( ), vecToScreen ) )
			{
				RenderOutOfView( pPlayer );
				continue;
			}

			if ( vecToScreen.x < 0 || vecToScreen.x > iScreenSizeX || vecToScreen.y < 0 || vecToScreen.y > iScreenSizeY )
				continue;
		}

		m_aPlayerData[ pPlayer->EntIndex( ) ].m_aBBox = GetBBox( pPlayer );
		m_aPlayerData[ pPlayer->EntIndex( ) ].m_iWidth = abs( m_aPlayerData[ pPlayer->EntIndex( ) ].m_aBBox.right - m_aPlayerData[ pPlayer->EntIndex( ) ].m_aBBox.left );
		m_aPlayerData[ pPlayer->EntIndex( ) ].m_iHeight = abs( m_aPlayerData[ pPlayer->EntIndex( ) ].m_aBBox.top - m_aPlayerData[ pPlayer->EntIndex( ) ].m_aBBox.bottom );

		RenderBox( pPlayer );
		RenderName( pPlayer );
		RenderHealth( pPlayer );
		RenderWeapon( pPlayer );
		RenderAmmo( pPlayer );
		//RenderFlags( pPlayer );
	}
}

void C_PlayerESP::RenderBox(C_BasePlayer* pPlayer)
{
	if ( !m_aPlayerData[ pPlayer->EntIndex( ) ].m_aSettings.m_BoundaryBox )
		return;

	g_Render->RenderRect( m_aPlayerData[ pPlayer->EntIndex( ) ].m_aBBox.left + 1, m_aPlayerData[ pPlayer->EntIndex( ) ].m_aBBox.top + 1, m_aPlayerData[ pPlayer->EntIndex( ) ].m_aBBox.right - 1, m_aPlayerData[ pPlayer->EntIndex( ) ].m_aBBox.bottom - 1, Color( 10, 10, 10, 150 ) );
	g_Render->RenderRect( m_aPlayerData[ pPlayer->EntIndex( ) ].m_aBBox.left - 1, m_aPlayerData[ pPlayer->EntIndex( ) ].m_aBBox.top - 1, m_aPlayerData[ pPlayer->EntIndex( ) ].m_aBBox.right + 1, m_aPlayerData[ pPlayer->EntIndex( ) ].m_aBBox.bottom + 1, Color( 10, 10, 10, 150 ) );
	g_Render->RenderRect( m_aPlayerData[ pPlayer->EntIndex( ) ].m_aBBox.left, m_aPlayerData[ pPlayer->EntIndex( ) ].m_aBBox.top, m_aPlayerData[ pPlayer->EntIndex( ) ].m_aBBox.right, m_aPlayerData[ pPlayer->EntIndex( ) ].m_aBBox.bottom, Color(m_aPlayerData[ pPlayer->EntIndex( ) ].m_aSettings.m_aBoundaryBox ));
}

void C_PlayerESP::RenderName( C_BasePlayer* pPlayer )
{
	C_PlayerInfo info;

	g_Globals.m_Interfaces.m_EngineClient->GetPlayerInfo( pPlayer->EntIndex( ), &info );

	std::string strName = info.m_strName;
	if ( strName.length( ) > 36 )
	{
		strName.erase( 36, strName.length( ) - 36 );
		strName.append( _S( "..." ) );
	}

	if ( m_aPlayerData[ pPlayer->EntIndex( ) ].m_aSettings.m_RenderName )
		g_Render->RenderText( strName, ImVec2( m_aPlayerData[ pPlayer->EntIndex( ) ].m_aBBox.left + m_aPlayerData[ pPlayer->EntIndex( ) ].m_iWidth * 0.5f, m_aPlayerData[ pPlayer->EntIndex( ) ].m_aBBox.top - 17 ), Color(m_aPlayerData[ pPlayer->EntIndex( ) ].m_aSettings.m_aNameColor), true, true, g_Globals.m_Fonts.m_SegoeUI );
}

void C_PlayerESP::RenderHealth(C_BasePlayer* pPlayer)
{
	float_t flBoxHeight = static_cast<float>(m_aPlayerData[ pPlayer->EntIndex( ) ].m_aBBox.bottom - m_aPlayerData[ pPlayer->EntIndex( ) ].m_aBBox.top);

	Color aColor = Color( m_aPlayerData[ pPlayer->EntIndex( ) ].m_aSettings.m_aHealthBar );
	if (pPlayer->IsDormant())
		aColor = Color(150, 150, 150, 150);

	float_t flColoredBarHeight = ((flBoxHeight * min( pPlayer->m_iHealth( ), 100 ) ) / 100.0f);
	float_t flColoredBarMaxHeight = ( ( flBoxHeight * 100.0f) / 100.0f);

	if ( m_aPlayerData[ pPlayer->EntIndex( ) ].m_aSettings.m_RenderHealthBar )
	{
		g_Render->RenderRectFilled( m_aPlayerData[ pPlayer->EntIndex( ) ].m_aBBox.left - 6.0f, m_aPlayerData[ pPlayer->EntIndex( ) ].m_aBBox.top - 1, m_aPlayerData[ pPlayer->EntIndex( ) ].m_aBBox.left - 2.0f, m_aPlayerData[ pPlayer->EntIndex( ) ].m_aBBox.top + flColoredBarMaxHeight + 1, Color(0.0f, 0.0f, 0.0f, 100.0f));
		g_Render->RenderRectFilled( m_aPlayerData[ pPlayer->EntIndex( ) ].m_aBBox.left - 5.0f, m_aPlayerData[ pPlayer->EntIndex( ) ].m_aBBox.top + (flColoredBarMaxHeight - flColoredBarHeight), m_aPlayerData[ pPlayer->EntIndex( ) ].m_aBBox.left - 3.0f, m_aPlayerData[ pPlayer->EntIndex( ) ].m_aBBox.top + flColoredBarMaxHeight, aColor);
	}

	if ( m_aPlayerData[ pPlayer->EntIndex( ) ].m_aSettings.m_RenderHealthText )
		g_Render->RenderText
		( 
			std::to_string( pPlayer->m_iHealth( ) ), 
			ImVec2( m_aPlayerData[ pPlayer->EntIndex( ) ].m_aBBox.left - ( m_aPlayerData[ pPlayer->EntIndex( ) ].m_aSettings.m_RenderHealthBar ? 27 : 22 ),
			m_aPlayerData[ pPlayer->EntIndex( ) ].m_aBBox.top - 4.0f ), 
			Color( m_aPlayerData[ pPlayer->EntIndex( ) ].m_aSettings.m_aHealthText ), 
			false, 
			true, 
			g_Globals.m_Fonts.m_SegoeUI 
		);
}

void C_PlayerESP::RenderWeapon( C_BasePlayer* pPlayer )
{
	RECT aBox = m_aPlayerData[ pPlayer->EntIndex( ) ].m_aBBox;
	
	C_BaseCombatWeapon* pWeapon = pPlayer->m_hActiveWeapon( ).Get( );
	if ( !pWeapon )
		return;

	int32_t iOffset = 0;
	if ( m_aPlayerData[ pPlayer->EntIndex( ) ].m_aSettings.m_RenderAmmoBar )
		if ( pWeapon->IsGun( ) )
			iOffset = 2;

	if ( !m_aPlayerData[ pPlayer->EntIndex( ) ].m_aSettings.m_RenderWeaponIcon && !m_aPlayerData[ pPlayer->EntIndex( ) ].m_aSettings.m_RenderWeaponText )
		return;
	
	std::string strIcon = this->GetWeaponIcon( pPlayer, pWeapon );
	if ( m_aPlayerData[ pPlayer->EntIndex( ) ].m_aSettings.m_RenderWeaponText )
	{
		g_Render->RenderText( this->GetWeaponName( pWeapon ), ImVec2( aBox.left + ( abs( aBox.right - aBox.left ) * 0.5f ), aBox.bottom + iOffset + 3 ), Color(m_aPlayerData[ pPlayer->EntIndex( ) ].m_aSettings.m_aWeaponText), true, true, g_Globals.m_Fonts.m_SegoeUI );

		if ( pWeapon->IsGun( ) )
			iOffset += 18;
		else 
			iOffset += 16;
	}

	if ( m_aPlayerData[ pPlayer->EntIndex( ) ].m_aSettings.m_RenderWeaponIcon )
		g_Render->RenderText( strIcon, ImVec2( aBox.left + ( abs( aBox.right - aBox.left ) * 0.5f ) + 1, aBox.bottom + iOffset ), Color(m_aPlayerData[ pPlayer->EntIndex( ) ].m_aSettings.m_aWeaponIcon), true, true, g_Globals.m_Fonts.m_WeaponIcon );
}

void C_PlayerESP::RenderOutOfView( C_BasePlayer* pPlayer )
{
	if ( !g_Settings->m_bOutOfViewArrows )
		return;

	if ( pPlayer->m_iTeamNum( ) == g_Globals.m_LocalPlayer->m_iTeamNum( ) )
		return;

	if ( !g_Globals.m_LocalPlayer->IsAlive( ) )
		return;

	float width = 7.f;
    QAngle viewangles;
    g_Globals.m_Interfaces.m_EngineClient->GetViewAngles( &viewangles );

    auto angle = viewangles.yaw - Math::CalcAngle( g_Globals.m_LocalPlayer->m_vecOrigin( ) + g_Globals.m_LocalPlayer->m_vecViewOffset( ), pPlayer->GetAbsOrigin( ) ).yaw - 90;

	int iScreenSizeX, iScreenSizeY;
	g_Globals.m_Interfaces.m_EngineClient->GetScreenSize( iScreenSizeX, iScreenSizeY );

	g_Render->RenderArc( iScreenSizeX / 2, iScreenSizeY / 2, 256, angle - width, angle + width, Color (1.f, 1.f, 1.f, 1.f), 4.f) ;
	g_Render->RenderArc( iScreenSizeX / 2, iScreenSizeY / 2, 250, angle - width, angle + width, Color (1.f, 1.f, 1.f, 0.5f), 1.5f);
}

void C_PlayerESP::RenderAmmo( C_BasePlayer* pPlayer )
{
	if ( !m_aPlayerData[ pPlayer->EntIndex( ) ].m_aSettings.m_RenderAmmoBar )
		return;

	RECT aBox = m_aPlayerData[ pPlayer->EntIndex( ) ].m_aBBox;
	C_BaseCombatWeapon* pWeapon = pPlayer->m_hActiveWeapon().Get();
	if (!pWeapon)
		return;

	if (!pWeapon->IsGun())
		return;

	C_CSWeaponData* pWeaponData = pWeapon->GetWeaponData();
	if (!pWeaponData)
		return;

	if ( m_aPlayerData[ pPlayer->EntIndex( ) ].m_flPreviousAmmo > pWeapon->m_iClip1( ) ) 
		m_aPlayerData[ pPlayer->EntIndex( ) ].m_flPreviousAmmo -= 0.01f;
	else
	{
		if ( m_aPlayerData[ pPlayer->EntIndex( ) ].m_flPreviousAmmo != pWeapon->m_iClip1( ) )
			m_aPlayerData[ pPlayer->EntIndex( ) ].m_flPreviousAmmo = pWeapon->m_iClip1( );
	}

	if ( m_aPlayerData[ pPlayer->EntIndex( ) ].m_flPreviousAmmo < pWeapon->m_iClip1( ) || m_aPlayerData[ pPlayer->EntIndex( ) ].m_flPreviousAmmo > pWeaponData->m_iMaxClip1 )
		m_aPlayerData[ pPlayer->EntIndex( ) ].m_flPreviousAmmo = pWeapon->m_iClip1( );

	float_t flBoxWidth = fabsf( aBox.right - aBox.left );
	float_t flCurrentBoxWidth = ( flBoxWidth * m_aPlayerData[ pPlayer->EntIndex( ) ].m_flPreviousAmmo ) / pWeaponData->m_iMaxClip1;
		
	if ( pPlayer->GetSequenceActivity( pPlayer->m_AnimationLayers( )[ ANIMATION_LAYER_WEAPON_ACTION ].m_nSequence ) == ACT_CSGO_RELOAD )
		flCurrentBoxWidth = flBoxWidth * pPlayer->m_AnimationLayers( )[ ANIMATION_LAYER_WEAPON_ACTION ].m_flCycle;

	Color aColor = Color( m_aPlayerData[ pPlayer->EntIndex( ) ].m_aSettings.m_aAmmoBar );
	if ( pPlayer->IsDormant( ) )
		aColor = Color( 150, 150, 150, 150 );

	g_Render->RenderRectFilled( aBox.right + 1, aBox.bottom + 2, aBox.left - 1, aBox.bottom + 6, Color( 0.0f, 0.0f, 0.0f, 100.0f ) );
	g_Render->RenderRectFilled( aBox.left, aBox.bottom + 3, aBox.left + flCurrentBoxWidth, aBox.bottom + 5, aColor );

	if ( m_aPlayerData[ pPlayer->EntIndex( ) ].m_aSettings.m_RenderAmmoBarText )
		g_Render->RenderText
		(
			std::to_string( pWeapon->m_iClip1( ) ),
			ImVec2( aBox.right + 3, aBox.bottom ),
			Color( m_aPlayerData[ pPlayer->EntIndex( ) ].m_aSettings.m_aAmmoBarText ),
			false,
			true,
			g_Globals.m_Fonts.m_SegoeUI
		);
}

void C_PlayerESP::RenderFlags(C_BasePlayer* pPlayer)
{
	int iOffset = 0;
	
	RECT aBox = m_aPlayerData[ pPlayer->EntIndex( ) ].m_aBBox;
	if ( pPlayer->m_bIsScoped( ) )
	{
		g_Render->RenderText( _S( "SCOPED" ), ImVec2( aBox.right + 3, aBox.top + iOffset ), Color( 255, 255, 255, 255 ), false, true, g_Globals.m_Fonts.m_SegoeUI );
		iOffset += 12;
	}

	if ( pPlayer->m_ArmourValue( ) > 1 )
	{
		g_Render->RenderText( pPlayer->m_bHasHelmet( ) && pPlayer->m_ArmourValue( ) > 0 ? _S( "HK" ) : ( pPlayer->m_bHasHelmet( ) ? _S( "H" ) : _S( "K" ) ), ImVec2( aBox.right + 3, aBox.top + iOffset ), Color( 255, 255, 255, 255 ), false, true, g_Globals.m_Fonts.m_SegoeUI );
		iOffset += 12;
	}

	if ( pPlayer->m_flFlashTime() > 0.1f )
	{
		g_Render->RenderText( _S( "FLASHED" ), ImVec2( aBox.right + 3, aBox.top + iOffset ), Color( 255, 255, 255, 255 ), false, true, g_Globals.m_Fonts.m_SegoeUI );
		iOffset += 12;
	}

	if ( strlen( pPlayer->m_szLastPlaceName( ) ) > 1 )
	{
		const char* last_place = pPlayer->m_szLastPlaceName( );
		if (last_place && *last_place)
		{
			auto st = std::string( last_place );
			std::transform( st.begin( ), st.end( ), st.begin( ), ::toupper );
			g_Render->RenderText( st, ImVec2( aBox.right + 3, aBox.top + iOffset ), Color( 255, 255, 255, 255 ), false, true, g_Globals.m_Fonts.m_SegoeUI );
		}

		iOffset += 12;
	}

	std::stringstream money;
	money << _S( "$" ) << pPlayer->m_iAccount( );
	
	g_Render->RenderText( money.str( ), ImVec2( aBox.right + 3, aBox.top + iOffset ), Color( 255, 255, 255, 255 ), false, true, g_Globals.m_Fonts.m_SegoeUI );
}

std::string C_PlayerESP::GetWeaponName( C_BaseCombatWeapon* pWeapon )
{
	auto GetCleanItemName = [](const char* name) -> const char* {
		if (name[0] == 'C')
			name++;

		auto start = strstr(name, _S("Weapon"));
		if (start != nullptr)
			name = start + 6;

		return name;
	};

	if (!pWeapon)
		return "";

	int32_t iWeaponIndex = pWeapon->m_iItemDefinitionIndex();
	if (iWeaponIndex < 0)
		return "";

	std::string strResult = "";
	switch (iWeaponIndex)
	{
	case WEAPON_REVOLVER: strResult = _S("Revolver R8"); break;
	case WEAPON_SCAR20: strResult = _S("SCAR20"); break;
	case WEAPON_DEAGLE: strResult = _S("Deagle"); break;
	case WEAPON_ELITE: strResult = _S("Dual elite"); break;
	case WEAPON_FIVESEVEN: strResult = _S("Fiveseven"); break;
	case WEAPON_FRAG_GRENADE: strResult = _S("Frag grenade"); break;
	case WEAPON_SMOKEGRENADE: strResult = _S("Smoke grenade"); break;
	case WEAPON_DECOY: strResult = _S("Decoy grenade"); break;
	case WEAPON_FLASHBANG: strResult = _S("Flashbang"); break;
	case WEAPON_HKP2000: strResult = _S("P2000"); break;
	case WEAPON_INCGRENADE: strResult = _S("Fire grenade"); break;
	case WEAPON_MOLOTOV: strResult = _S("Molotov"); break;
	case WEAPON_HEGRENADE: strResult = _S("Frag grenade"); break;
	default: strResult = GetCleanItemName(pWeapon->GetClientClass()->m_strNetworkName);
	}

	if (strResult == _S("HKP2000"))
		return _S("P2000");

	return strResult;
}

std::string C_PlayerESP::GetWeaponIcon(C_BasePlayer* pPlayer, C_BaseCombatWeapon* pWeapon)
{
	if (!pWeapon)
		return "";

	C_CSWeaponData* pWeaponData = pWeapon->GetWeaponData();
	if (!pWeaponData)
		return "";

	if (pWeapon->IsKnife())
	{
		if (pPlayer->m_iTeamNum() == 1)
			return _S("]");
		else
			return _S("[");
	}

	std::string strResult = "";
	switch (pWeapon->m_iItemDefinitionIndex())
	{
		case WEAPON_SCAR20: strResult = _S("Y"); break;
		case WEAPON_G3SG1: strResult = _S("X"); break;
		case WEAPON_AWP: strResult = _S("Z"); break;
		case WEAPON_SSG08: strResult = _S("a"); break;
		case WEAPON_DEAGLE: strResult = _S("A"); break;
		case WEAPON_REVOLVER: strResult = _S("J"); break;
		case WEAPON_HKP2000: strResult = _S("E"); break; break;
		case WEAPON_GLOCK: strResult = _S("D"); break;
		case WEAPON_USP_SILENCER: strResult = _S("G"); break;
		case WEAPON_ELITE: strResult = _S("B"); break;
		case WEAPON_C4: strResult = _S("o"); break;
		case WEAPON_P250: strResult = _S("F"); break;
		case WEAPON_AUG: strResult = _S("U"); break;
		case WEAPON_FIVESEVEN: strResult = _S("C"); break;
		case WEAPON_AK47: strResult = _S("W"); break;
		case WEAPON_GALILAR: strResult = _S("Q"); break;
		case WEAPON_CZ75A: strResult = _S("I"); break;
		case WEAPON_FAMAS: strResult = _S("R"); break;
		case WEAPON_TEC9: strResult = _S("H"); break;
		case WEAPON_BIZON: strResult = _S("M"); break;
		case WEAPON_M249: strResult = _S("g"); break;
		case WEAPON_NEGEV: strResult = _S("f"); break;
		case WEAPON_NOVA: strResult = _S("e"); break;
		case WEAPON_MAG7: strResult = _S("d"); break;
		case WEAPON_TASER: strResult = _S("h"); break;
		case WEAPON_HEGRENADE: strResult = _S("j"); break;
		case WEAPON_SMOKEGRENADE: strResult = _S("k"); break;
		case WEAPON_INCGRENADE: strResult = _S("n"); break;
		case WEAPON_MOLOTOV: strResult = _S("l"); break;
		case WEAPON_SAWEDOFF: strResult = _S("c"); break;
		case WEAPON_DECOY: strResult = _S("m"); break;
		case WEAPON_FLASHBANG: strResult = _S("i"); break;
		case WEAPON_M4A1: strResult = _S("S"); break;
		case WEAPON_M4A1_SILENCER: strResult = _S("T"); break;
		case WEAPON_FRAG_GRENADE: strResult = _S("k"); break;
		case WEAPON_MAC10: strResult = _S("K"); break;
		case WEAPON_UMP45: strResult = _S("L"); break;
		case WEAPON_MP7: strResult = _S("N"); break;
		case WEAPON_P90: strResult = _S("P"); break;
		case WEAPON_MP9: strResult = _S("N"); break;
		case WEAPON_SG553: strResult = _S("V"); break;
		case WEAPON_XM1014: strResult = _S("e"); break;
		case WEAPON_TAGRENADE: strResult = _S("i"); break;
	}

	return strResult;
}


void C_PlayerESP::RenderGlow()
{
	for (auto i = 0; i < g_Globals.m_Interfaces.m_GlowObjManager->m_GlowObjectDefinitions.Count(); i++) {
		auto& glowObject = g_Globals.m_Interfaces.m_GlowObjManager->m_GlowObjectDefinitions[i];
		auto entity = reinterpret_cast<C_BasePlayer*>(glowObject.m_pEntity);

		if (glowObject.IsUnused())
			continue;

		if (!entity || entity->IsDormant())
			continue;

		auto class_id = entity->GetClientClass()->m_ClassID;
		auto color = Color{};

		auto model = entity->GetModel();

		switch (class_id)
		{
		case ClassId_CCSPlayer: {
			auto is_enemy = entity->m_iTeamNum() != g_Globals.m_LocalPlayer->m_iTeamNum();
			auto is_local = entity == g_Globals.m_LocalPlayer;

			C_PlayerSettings Settings;
			if ( entity->m_iTeamNum( ) == g_Globals.m_LocalPlayer->m_iTeamNum( ) )
			{
				Settings = g_Settings->m_Teammates;
				if ( entity == g_Globals.m_LocalPlayer ) 
					Settings = g_Settings->m_LocalPlayer;
			}

			if (!Settings.m_bRenderGlow || !entity->IsAlive())
				continue;

			color = Color(Settings.m_aGlow);

			glowObject.m_nGlowStyle = Settings.m_iGlowStyle;

			break;
		}
		case ClassId_CC4:
		case ClassId_CPlantedC4:
		{
			
			if (!g_Settings->m_bRenderC4Glow || !entity->IsAlive())
				continue;

			color = Color(g_Settings->m_aC4Glow);

			glowObject.m_nGlowStyle = g_Settings->m_iC4GlowStyle;

			break;
		}
		case ClassId_CKnife:
		case ClassId_CKnifeGG:
		case ClassId_CAK47:
		case ClassId_CDEagle:
		case ClassId_CWeaponAug:
		case ClassId_CWeaponAWP:
		case ClassId_CWeaponBizon:
		case ClassId_CWeaponElite:
		case ClassId_CWeaponFamas:
		case ClassId_CWeaponFiveSeven:
		case ClassId_CWeaponG3SG1:
		case ClassId_CWeaponGalil:
		case ClassId_CWeaponGalilAR:
		case ClassId_CWeaponGlock:
		case ClassId_CWeaponHKP2000:
		case ClassId_CWeaponM249:
		case ClassId_CWeaponM4A1:
		case ClassId_CWeaponMAC10:
		case ClassId_CWeaponMag7:
		case ClassId_CWeaponMP5Navy:
		case ClassId_CWeaponMP7:
		case ClassId_CWeaponMP9:
		case ClassId_CWeaponNegev:
		case ClassId_CWeaponNOVA:
		case ClassId_CWeaponP228:
		case ClassId_CWeaponP250:
		case ClassId_CWeaponP90:
		case ClassId_CWeaponSawedoff:
		case ClassId_CWeaponSCAR20:
		case ClassId_CWeaponScout:
		case ClassId_CWeaponSG550:
		case ClassId_CWeaponSG552:
		case ClassId_CWeaponSG556:
		case ClassId_CWeaponSSG08:
		case ClassId_CWeaponTaser:
		case ClassId_CWeaponTec9:
		case ClassId_CWeaponUMP45:
		case ClassId_CWeaponUSP:
		case ClassId_CWeaponXM1014:
		{

			if (!g_Settings->m_bRenderDroppedWeaponGlow || !entity->IsAlive())
				continue;

			color = Color(g_Settings->m_aDroppedWeaponGlow);

			glowObject.m_nGlowStyle = g_Settings->m_iDroppedWeaponGlowStyle;

			break;
		}
		case ClassId_CMolotovProjectile:
		case ClassId_CDecoyProjectile:
		case ClassId_CSmokeGrenadeProjectile:
		case ClassId_CSnowballProjectile:
		case ClassId_CBreachChargeProjectile:
		case ClassId_CBumpMineProjectile:
		case ClassId_CBaseCSGrenadeProjectile:
		case ClassId_CSensorGrenadeProjectile:
		{
			if (!g_Settings->m_bRenderProjectileGlow || !entity->IsAlive())
				continue;

			color = Color(g_Settings->m_aProjectileGlow);

			glowObject.m_nGlowStyle = g_Settings->m_iProjectileGlowStyle;
			break;
		}
		}

		if (model)
		{
			std::string name{ model->szName };
			if (name.find(_S("smoke")) != std::string::npos || name.find(_S("projectile")) != std::string::npos)
			{
				if (!g_Settings->m_bRenderProjectileGlow || !entity->IsAlive())
					continue;

				color = Color(g_Settings->m_aProjectileGlow);

				glowObject.m_nGlowStyle = g_Settings->m_iProjectileGlowStyle;
			}
		}

		glowObject.m_flRed = color.r() / 255.0f;
		glowObject.m_flGreen = color.g() / 255.0f;
		glowObject.m_flBlue = color.b() / 255.0f;
		glowObject.m_flAlpha = color.a() / 255.0f;
		glowObject.m_bRenderWhenOccluded = true;
		glowObject.m_bRenderWhenUnoccluded = false;
	}
}

void C_PlayerESP::DormantPlayers( )
{
	g_Globals.m_Interfaces.m_EngineSound->GetActiveSounds( m_CurrentSoundData );
	if ( !m_CurrentSoundData.Count( ) )
		return;

	for ( auto i = 0; i < m_CurrentSoundData.Count( ); i++ )
	{
		auto& m_Sound = m_CurrentSoundData[ i ];
		if ( m_Sound.m_nSoundSource < 1 || m_Sound.m_nSoundSource > g_Globals.m_Interfaces.m_GlobalVars->m_iMaxClients )
			continue;

		if ( m_Sound.m_pOrigin->IsZero( ) || !IsSoundValid( m_Sound ) )
			continue;

		C_BasePlayer* pPlayer = C_BasePlayer::GetPlayerByIndex( m_Sound.m_nSoundSource );
		if ( !pPlayer || !pPlayer->IsAlive( ) || !pPlayer->IsDormant( ) )
			continue;

		Vector src3D, dst3D;
		trace_t tr;
		Ray_t ray;
		CTraceFilter filter;

		src3D = *m_Sound.m_pOrigin + Vector(0.0f, 0.0f, 1.0f);
		dst3D = src3D - Vector(0.0f, 0.0f, 100.0f);

		filter.pSkip = pPlayer;
		ray.Init(src3D, dst3D);

		g_Globals.m_Interfaces.m_EngineTrace->TraceRay(ray, MASK_PLAYERSOLID, &filter, &tr);

		if (tr.allsolid)
			m_DormantPlayers[ m_Sound.m_nSoundSource ].m_ReceiveTime = -1;

		*m_Sound.m_pOrigin = tr.fraction <= 0.97f ? tr.endpos : *m_Sound.m_pOrigin;

		m_DormantPlayers[ m_Sound.m_nSoundSource ].m_Origin = *m_Sound.m_pOrigin;
		m_DormantPlayers[ m_Sound.m_nSoundSource ].m_ReceiveTime = g_Globals.m_Interfaces.m_GlobalVars->m_flRealTime;
	}

	m_CachedSoundData = m_CurrentSoundData;
}

bool C_PlayerESP::ForceDormant( int nIndex )
{
	C_BasePlayer* pPlayer = C_BasePlayer::GetPlayerByIndex( nIndex );
	if ( !pPlayer || !pPlayer->IsAlive( ) || !pPlayer->IsDormant( ) || pPlayer->m_iTeamNum( ) == g_Globals.m_LocalPlayer->m_iTeamNum( ) )
	{
		m_DormantPlayers[ pPlayer->EntIndex( ) ].m_InDormant = 0.0f;
		m_DormantPlayers[ pPlayer->EntIndex( ) ].m_ReceiveTime = 0.0f;

		return false;
	}
	
	if ( m_DormantPlayers[ pPlayer->EntIndex( ) ].m_InDormant > m_DormantPlayers[ pPlayer->EntIndex( ) ].m_ReceiveTime )
	{
		float_t flSinceInDormant = g_Globals.m_Interfaces.m_GlobalVars->m_flRealTime - m_DormantPlayers[ pPlayer->EntIndex( ) ].m_InDormant;
		if ( flSinceInDormant < 10.0f )
			pPlayer->SetAbsoluteOrigin( m_DormantPlayers[ pPlayer->EntIndex( ) ].m_LastOrigin );
		else if ( g_Globals.m_Interfaces.m_GlobalVars->m_flRealTime - m_DormantPlayers[ pPlayer->EntIndex( ) ].m_ReceiveTime < 10.0f )
			pPlayer->SetAbsoluteOrigin( m_DormantPlayers[ pPlayer->EntIndex( ) ].m_Origin );

		return true;
	}

	if ( g_Globals.m_Interfaces.m_GlobalVars->m_flRealTime - m_DormantPlayers[ pPlayer->EntIndex( ) ].m_ReceiveTime > 10.0f )
	{
		float_t flSinceInDormant = g_Globals.m_Interfaces.m_GlobalVars->m_flRealTime - m_DormantPlayers[ pPlayer->EntIndex( ) ].m_InDormant;
		if ( flSinceInDormant < 10.0f )
		{
			pPlayer->SetAbsoluteOrigin( m_DormantPlayers[ pPlayer->EntIndex( ) ].m_LastOrigin );
			return true;
		}

		return false;
	}

	pPlayer->SetAbsoluteOrigin( m_DormantPlayers[ pPlayer->EntIndex( ) ].m_Origin );
	return true;
}

bool C_PlayerESP::IsSoundValid( SndInfo_t Sound )
{
	for ( int32_t i = 0; i < m_CachedSoundData.Count( ); i++ )
		if ( Sound.m_nGuid == m_CachedSoundData[ i ].m_nGuid )
			return false;

	return true;
}

void C_PlayerESP::ResetData( )
{
	m_CachedSoundData.RemoveAll( );
	m_CurrentSoundData.RemoveAll( );
	m_DormantPlayers = { };
}