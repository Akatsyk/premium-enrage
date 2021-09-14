#include "World.hpp"
#include "../Render.hpp"
#include "../Settings.hpp"
#include "../Tools/Tools.hpp"
#include "../Grenades/Warning.hpp"

void C_World::Instance( ClientFrameStage_t Stage )
{
	if ( Stage != ClientFrameStage_t::FRAME_RENDER_START )
		return;

	if ( g_Settings->m_bUnhideConvars )
	{
		if ( !m_bDidUnlockConvars )
		{
			auto pIterator = **reinterpret_cast<ConCommandBase***>(reinterpret_cast<DWORD>(g_Globals.m_Interfaces.m_CVar) + 0x34);
			for (auto c = pIterator->m_pNext; c != nullptr; c = c->m_pNext) 
			{
				c->m_nFlags &= ~FCVAR_DEVELOPMENTONLY;
				c->m_nFlags &= ~FCVAR_HIDDEN;
			}

			m_bDidUnlockConvars = true;
		}
	}	
	
	if ( g_Settings->m_iViewmodelX != g_Globals.m_ConVars.m_ViewmodelX->GetInt( ) )
		g_Globals.m_ConVars.m_ViewmodelX->SetValue( g_Settings->m_iViewmodelX );

	if ( g_Settings->m_iViewmodelY != g_Globals.m_ConVars.m_ViewmodelY->GetInt( ) )
		g_Globals.m_ConVars.m_ViewmodelY->SetValue( g_Settings->m_iViewmodelY );

	if ( g_Settings->m_iViewmodelZ != g_Globals.m_ConVars.m_ViewmodelZ->GetInt( ) )
		g_Globals.m_ConVars.m_ViewmodelZ->SetValue( g_Settings->m_iViewmodelZ );

	g_Globals.m_Interfaces.m_CVar->FindVar(_S("con_filter_text"))->SetValue(_S("eqwoie2ue398129e8wuq12we9yw98d"));
	if (g_Globals.m_Interfaces.m_CVar->FindVar(_S("con_filter_enable"))->GetBool() != g_Settings->m_bFilterConsole)
	{
		g_Globals.m_Interfaces.m_CVar->FindVar(_S("con_filter_enable"))->SetValue(g_Settings->m_bFilterConsole);
		g_Globals.m_Interfaces.m_EngineClient->ExecuteClientCmd(_S("clear"));
	}

	this->RemoveHandShaking( );
	this->RemoveShadows( );
	this->RemoveSmokeAndPostProcess( );

	static C_Material* pBlurOverlay = g_Globals.m_Interfaces.m_MaterialSystem->FindMaterial( _S( "dev/scope_bluroverlay" ), TEXTURE_GROUP_OTHER );
	static C_Material* pScopeDirt = g_Globals.m_Interfaces.m_MaterialSystem->FindMaterial( _S( "models/weapons/shared/scope/scope_lens_dirt" ), TEXTURE_GROUP_OTHER );
	
	pBlurOverlay->SetMaterialVarFlag( MATERIAL_VAR_NO_DRAW, false );	
	pScopeDirt->SetMaterialVarFlag( MATERIAL_VAR_NO_DRAW, false );

	C_BaseCombatWeapon* pCombatWeapon = g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( );
	if ( pCombatWeapon )
	{
		if ( pCombatWeapon->IsSniper( ) )
		{
			if ( g_Globals.m_LocalPlayer->m_bIsScoped( ) )
			{
				if ( g_Settings->m_aWorldRemovals[ REMOVALS_VISUAL_SCOPE ] )
				{
					pBlurOverlay->SetMaterialVarFlag( MATERIAL_VAR_NO_DRAW, true );
					pScopeDirt->SetMaterialVarFlag( MATERIAL_VAR_NO_DRAW, true );
				}
			}
		}
	}

	if ( g_Settings->m_bForceCrosshair )
		g_Globals.m_ConVars.m_WeaponDebugShowSpread->SetValue( 3 );
	else
		g_Globals.m_ConVars.m_WeaponDebugShowSpread->SetValue( 0 );

	this->DrawClientImpacts( );
	return this->SkyboxChanger( );
}

void C_World::OnBulletImpact( C_GameEvent* pEvent )
{
	Vector vecPosition = Vector( pEvent->GetInt( _S( "x" ) ), pEvent->GetInt( _S( "y" ) ), pEvent->GetInt( _S( "z" ) ) );

	C_BasePlayer* pPlayer = C_BasePlayer::GetPlayerByIndex( g_Globals.m_Interfaces.m_EngineClient->GetPlayerForUserID( pEvent->GetInt( _S( "userid" ) ) ) );
	if ( !pPlayer || !pPlayer->IsPlayer( ) || !pPlayer->IsAlive( ) )
		return;

	if ( pPlayer == g_Globals.m_LocalPlayer )
	{
		if ( g_Settings->m_bDrawServerImpacts )
			g_Globals.m_Interfaces.m_DebugOverlay->BoxOverlay
			(
				vecPosition,
				Vector( -2.0f, -2.0f, -2.0f ),
				Vector( 2.0f, 2.0f, 2.0f ),
				QAngle( 0.0f, 0.0f, 0.0f ),
				g_Settings->m_ServerImpacts->r( ),
				g_Settings->m_ServerImpacts->g( ),
				g_Settings->m_ServerImpacts->b( ),
				g_Settings->m_ServerImpacts->a( ),
				4.0f
			);

		return;
	}

	if ( !g_Settings->m_bDrawEnemyTracers || pPlayer->m_iTeamNum( ) == g_Globals.m_LocalPlayer->m_iTeamNum( ) )
		return;

	C_BulletTrace BulletTrace;
	
	BulletTrace.m_bIsLocalTrace = false;
	BulletTrace.m_vecEndPosition = vecPosition;
	BulletTrace.m_vecStartPosition = pPlayer->GetAbsOrigin( ) + pPlayer->m_vecViewOffset( );

	m_BulletTracers.emplace_back( BulletTrace );
}

void C_World::PreserveKillfeed( )
{
	if ( !g_Globals.m_Interfaces.m_EngineClient->IsInGame( ) || !g_Globals.m_Interfaces.m_EngineClient->IsConnected( ) )
		return;

	static PDWORD pHudDeathNotice = NULL;
	if ( !g_Globals.m_LocalPlayer || !g_Globals.m_LocalPlayer->IsAlive( ) )
	{
		pHudDeathNotice = NULL;
		return;
	}

	if ( g_Globals.m_LocalPlayer->m_fFlags( ) & FL_FROZEN || g_Globals.m_LocalPlayer->m_bGunGameImmunity( ) || ( *g_Globals.m_Interfaces.m_GameRules )->IsFreezePeriod( ) )
	{
		pHudDeathNotice = NULL;
		return;
	}
	
	if ( !pHudDeathNotice )
	{
		pHudDeathNotice = g_Tools->FindHudElement( _S( "CCSGO_HudDeathNotice" ) );
		return;
	}

	PFLOAT pNoticeExpireTime = ( PFLOAT )( ( DWORD )( pHudDeathNotice ) + 0x50 );
	if ( pNoticeExpireTime )
		*pNoticeExpireTime = g_Settings->m_bPreserveKillfeed ? FLT_MAX : 1.5f;

	if ( g_Globals.m_RoundInfo.m_bShouldClearDeathNotices )
		( ( void( __thiscall* )( DWORD ) )( g_Globals.m_AddressList.m_ClearDeathList ) )( ( DWORD )( pHudDeathNotice ) - 20 );

	g_Globals.m_RoundInfo.m_bShouldClearDeathNotices = false;
}

void C_World::PostFrame( ClientFrameStage_t Stage )
{
	if ( Stage != ClientFrameStage_t::FRAME_START )
		return;

	return this->DrawBulletTracers( );
}

void C_World::OnRageBotFire( Vector vecStartPosition, Vector vecEndPosition )
{
	if ( !g_Settings->m_bDrawLocalTracers )
		return;

	C_BulletTrace BulletTrace;
	
	BulletTrace.m_bIsLocalTrace = true;
	BulletTrace.m_vecStartPosition = vecStartPosition;
	BulletTrace.m_vecEndPosition = vecEndPosition;

	m_BulletTracers.emplace_back( BulletTrace );
}

void C_World::DrawScopeLines( )
{
	if ( !g_Globals.m_LocalPlayer || !g_Globals.m_LocalPlayer->IsAlive( ) )
		return;
	
	C_BaseCombatWeapon* pCombatWeapon = g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( );
	if ( pCombatWeapon )
	{
		if ( pCombatWeapon->IsSniper( ) )
		{
			if ( g_Globals.m_LocalPlayer->m_bIsScoped( ) )
			{
				if ( g_Settings->m_aWorldRemovals[ REMOVALS_VISUAL_SCOPE ] )
				{
					int32_t iScreenSizeX, iScreenSizeY;
					g_Globals.m_Interfaces.m_EngineClient->GetScreenSize( iScreenSizeX, iScreenSizeY );

					g_Render->RenderLine( 0, iScreenSizeY / 2, iScreenSizeX, iScreenSizeY / 2, Color( 0, 0, 0 ), 1.0f );
					g_Render->RenderLine( iScreenSizeX / 2, 0, iScreenSizeX / 2, iScreenSizeY, Color( 0, 0, 0 ), 1.0f );
				}
			}
		}
	}
}

void C_World::Grenades( )
{
	if ( !g_Globals.m_LocalPlayer || !g_Globals.m_LocalPlayer->IsAlive( ) )
		return;

	if ( !g_Settings->m_bPredictGrenades )
		return;

	static auto last_server_tick = g_Globals.m_Interfaces.m_ClientState->m_ClockDriftMgr( ).m_nServerTick;
	if (last_server_tick != g_Globals.m_Interfaces.m_ClientState->m_ClockDriftMgr( ).m_nServerTick) {
		g_GrenadePrediction->get_list( ).clear();

		last_server_tick = g_Globals.m_Interfaces.m_ClientState->m_ClockDriftMgr( ).m_nServerTick;
	}

	for ( int32_t i = 1; i < g_Globals.m_Interfaces.m_EntityList->GetHighestEntityIndex( ); i++ )
	{
		C_BaseEntity* pBaseEntity = static_cast< C_BaseEntity* >( g_Globals.m_Interfaces.m_EntityList->GetClientEntity( i ) );
		if ( !pBaseEntity || pBaseEntity->IsDormant( ) )
			continue;

		const auto client_class = pBaseEntity->GetClientClass();
		if ( pBaseEntity->GetClientClass( )->m_ClassID == ClassId_CInferno )
		{
			float_t flSpawnTime = *( float_t* )( ( DWORD )( pBaseEntity ) + 0x20 );
			float_t flPercentage = ( ( flSpawnTime + 7.0f ) - g_Globals.m_Interfaces.m_GlobalVars->m_flCurTime ) / 7.0f;

			Vector vecScreenPosition = Vector( 0, 0, 0 );
			if ( !g_Globals.m_Interfaces.m_DebugOverlay->ScreenPosition( pBaseEntity->m_vecOrigin( ), vecScreenPosition ) )
			{
				ImVec2 vecTextSize = g_Globals.m_Fonts.m_BigIcons->CalcTextSizeA( 20.0f, FLT_MAX, NULL, _S( "l" ) );
				g_Render->RenderText( _S( "l" ), ImVec2( vecScreenPosition.x - vecTextSize.x / 2 + 1, vecScreenPosition.y - vecTextSize.y / 2 ), Color::White, false, true, g_Globals.m_Fonts.m_BigIcons );
				
				if ( g_Settings->m_GrenadeTimers )
				{
					g_Render->RenderRectFilled( vecScreenPosition.x - 21, vecScreenPosition.y + 14, vecScreenPosition.x + 21, vecScreenPosition.y + 20, Color( 0.0f, 0.0f, 0.0f, 100.0f ) );
					g_Render->RenderRectFilled( vecScreenPosition.x - 20, vecScreenPosition.y + 15, vecScreenPosition.x - 19.0f + 39.0f * flPercentage, vecScreenPosition.y + 19, Color( g_Settings->m_GrenadeWarningTimer ) );
				}

				g_Render->RenderCircle3D( pBaseEntity->m_vecOrigin( ), 32, 170, Color( 200, 0, 0 ) );
			}
		}
		else if ( pBaseEntity->GetClientClass( )->m_ClassID == 157 )
		{
			float_t flSpawnTime = *( float_t* )( ( DWORD )( pBaseEntity ) + 0x20 );
			if ( flSpawnTime > 0.0f )
			{
				float_t flPercentage = ( ( flSpawnTime + 17.0f ) - g_Globals.m_Interfaces.m_GlobalVars->m_flCurTime ) / 17.0f;

				Vector vecScreenPosition = Vector( 0, 0, 0 );
				if ( !g_Globals.m_Interfaces.m_DebugOverlay->ScreenPosition( pBaseEntity->m_vecOrigin( ), vecScreenPosition ) )
				{
					ImVec2 vecTextSize = g_Globals.m_Fonts.m_BigIcons->CalcTextSizeA( 20.0f, FLT_MAX, NULL, _S( "k" ) );
					g_Render->RenderText( _S( "k" ), ImVec2( vecScreenPosition.x - vecTextSize.x / 2 + 1, vecScreenPosition.y - vecTextSize.y / 2 ), Color::White, false, true, g_Globals.m_Fonts.m_BigIcons );
					
					if ( g_Settings->m_GrenadeTimers )
					{
						g_Render->RenderRectFilled( vecScreenPosition.x - 21, vecScreenPosition.y + 14, vecScreenPosition.x + 21, vecScreenPosition.y + 20, Color( 0.0f, 0.0f, 0.0f, 100.0f ) );
						g_Render->RenderRectFilled( vecScreenPosition.x - 20, vecScreenPosition.y + 15, vecScreenPosition.x - 19.0f + 39.0f * flPercentage, vecScreenPosition.y + 19, Color( g_Settings->m_GrenadeWarningTimer ) );
					}

					g_Render->RenderCircle3D( pBaseEntity->m_vecOrigin( ), 32, 170, Color( 0, 128, 255 ) );
				}
			}
		}

		if ( !client_class
			|| client_class->m_ClassID != 114 && client_class->m_ClassID != ClassId_CBaseCSGrenadeProjectile )
			continue;

		if ( client_class->m_ClassID == ClassId_CBaseCSGrenadeProjectile ) {
			const auto model = pBaseEntity->GetModel( );
			if ( !model )
				continue;

			const auto studio_model = g_Globals.m_Interfaces.m_ModelInfo->GetStudiomodel( model );
			if ( !studio_model
				|| std::string_view( studio_model->szName ).find( "fraggrenade" ) == std::string::npos )
				continue;
		}

		const auto handle = pBaseEntity->GetRefEHandle( ).ToLong( );
		if ( pBaseEntity->m_nExplodeEffectTickBegin( ) ) 
		{
			g_GrenadePrediction->get_list( ).erase( handle );
			continue;
		}

		int32_t m_GrenadeType = -1;
		if ( client_class->m_ClassID == WEAPON_HEGRENADE )
			m_GrenadeType = WEAPON_HEGRENADE;
		else if ( client_class->m_ClassID == WEAPON_INCGRENADE )
			m_GrenadeType = WEAPON_INCGRENADE;
		else if ( client_class->m_ClassID == WEAPON_MOLOTOV )
			m_GrenadeType = WEAPON_MOLOTOV;

		if ( g_GrenadePrediction->get_list( ).find( handle ) == g_GrenadePrediction->get_list( ).end( ) ) {
			g_GrenadePrediction->get_list( )[ handle ] =
				C_GrenadePrediction::data_t
				(
					reinterpret_cast< C_BaseCombatWeapon* >( pBaseEntity )->m_hThrower( ).Get( ),
					client_class->m_ClassID == 114 ? WEAPON_MOLOTOV : WEAPON_HEGRENADE,
					pBaseEntity->m_vecOrigin( ), 
					reinterpret_cast< C_BasePlayer* >( pBaseEntity )->m_vecVelocity( ),
					pBaseEntity->m_flCreationTime( ), 
					TIME_TO_TICKS( reinterpret_cast< C_BasePlayer* >( pBaseEntity )->m_flSimulationTime( ) - pBaseEntity->m_flCreationTime( ) )
				);
		}

		if ( g_GrenadePrediction->get_list( ).at( handle ).draw( ) )
			continue;

		g_GrenadePrediction->get_list( ).erase( handle );
	}

	g_GrenadePrediction->get_local_data( ).draw( );
}

void C_World::RemoveHandShaking( )
{
	if ( g_Settings->m_aWorldRemovals[ REMOVALS_VISUAL_HAND_SHAKING ] )
		return g_Globals.m_ConVars.m_ClWpnSwayAmount->SetValue( 0.0f );
		
	return g_Globals.m_ConVars.m_ClWpnSwayAmount->SetValue( 1.6f );
}

void C_World::RemoveSmokeAndPostProcess( )
{
	static std::vector< std::string > aMaterialList =
	{
		_S( "particle/vistasmokev1/vistasmokev1_emods" ),
		_S( "particle/vistasmokev1/vistasmokev1_emods_impactdust" ),
		_S( "particle/vistasmokev1/vistasmokev1_fire" ),
		_S( "particle/vistasmokev1/vistasmokev1_smokegrenade" ),
	};

	for ( auto strSmokeMaterial : aMaterialList )
	{
		C_Material* pMaterial = g_Globals.m_Interfaces.m_MaterialSystem->FindMaterial( strSmokeMaterial.c_str( ), _S( TEXTURE_GROUP_OTHER ) );
		if ( !pMaterial || pMaterial->GetMaterialVarFlag( MATERIAL_VAR_NO_DRAW ) )
			continue;

		pMaterial->SetMaterialVarFlag( MATERIAL_VAR_NO_DRAW, g_Settings->m_aWorldRemovals[ REMOVALS_VISUAL_SMOKE ] );
	}

	if ( *( int32_t* )( *reinterpret_cast < uint32_t** >( ( uint32_t )( g_Globals.m_AddressList.m_SmokeCount ) ) ) != 0 )
	{
		if ( g_Settings->m_aWorldRemovals[ REMOVALS_VISUAL_SMOKE ] )
			*( int32_t* )( *reinterpret_cast < uint32_t** >( ( uint32_t )( g_Globals.m_AddressList.m_SmokeCount ) ) ) = 0;
	}

	**reinterpret_cast < bool** > ( reinterpret_cast < uint32_t > ( g_Globals.m_AddressList.m_PostProcess ) ) = g_Settings->m_aWorldRemovals[ REMOVALS_VISUAL_POSTPROCESS ];
}

void C_World::RemoveShadows( )
{
	g_Globals.m_ConVars.m_ClFootContactShadows->SetValue( !g_Settings->m_aWorldRemovals[ REMOVALS_VISUAL_SHADOWS ] );
	g_Globals.m_ConVars.m_ClCsmStaticPropShadows->SetValue( !g_Settings->m_aWorldRemovals[ REMOVALS_VISUAL_SHADOWS ] );
	g_Globals.m_ConVars.m_ClCsmWorldShadows->SetValue( !g_Settings->m_aWorldRemovals[ REMOVALS_VISUAL_SHADOWS ] );
	g_Globals.m_ConVars.m_ClCsmShadows->SetValue( !g_Settings->m_aWorldRemovals[ REMOVALS_VISUAL_SHADOWS ] );
	g_Globals.m_ConVars.m_ClCsmViewmodelShadows->SetValue( !g_Settings->m_aWorldRemovals[ REMOVALS_VISUAL_SHADOWS ] );
	g_Globals.m_ConVars.m_ClCsmSpriteShadows->SetValue( !g_Settings->m_aWorldRemovals[ REMOVALS_VISUAL_SHADOWS ] );
	g_Globals.m_ConVars.m_ClCsmRopeShadows->SetValue( !g_Settings->m_aWorldRemovals[ REMOVALS_VISUAL_SHADOWS ] );
}

void C_World::DrawBulletTracers( )
{
	for ( int32_t iPosition = 0; iPosition < m_BulletTracers.size( ); iPosition++ )
	{
		auto Trace = &m_BulletTracers[ iPosition ];

		Color aBulletColor = Color( g_Settings->m_EnemyTracers );
		if ( Trace->m_bIsLocalTrace )
			aBulletColor = Color( g_Settings->m_LocalTracers );

		BeamInfo_t BeamInfo = BeamInfo_t( );

		BeamInfo.m_vecStart = Trace->m_vecStartPosition;
		if ( Trace->m_bIsLocalTrace )
			BeamInfo.m_vecStart = Vector( Trace->m_vecStartPosition.x, Trace->m_vecStartPosition.y, Trace->m_vecStartPosition.z - 2.0f );

		BeamInfo.m_vecEnd = Trace->m_vecEndPosition;
		BeamInfo.m_nModelIndex = -1;
		BeamInfo.m_flHaloScale = 0.0f;
		BeamInfo.m_flLife = 4.0f;
		BeamInfo.m_flFadeLength = 0.0f;
		BeamInfo.m_flWidth = 0.75f;
		BeamInfo.m_flEndWidth = 0.75f;
		BeamInfo.m_flAmplitude = 1.f;

		BeamInfo.m_nStartFrame = 0;
		BeamInfo.m_flRed = aBulletColor.r( );
		BeamInfo.m_flGreen = aBulletColor.g( );
		BeamInfo.m_flBlue = aBulletColor.b( );
		BeamInfo.m_flBrightness = aBulletColor.a( );

		BeamInfo.m_bRenderable = true;
		BeamInfo.m_nSegments = 2;
		BeamInfo.m_nFlags = 0;

		BeamInfo.m_flFrameRate = 0.0f;
		BeamInfo.m_pszModelName = "sprites/physbeam.vmt";
		BeamInfo.m_nType = TE_BEAMPOINTS;

		Beam_t* Beam = g_Globals.m_Interfaces.m_ViewRenderBeams->CreateBeamPoints( BeamInfo );
		if ( Beam )
			g_Globals.m_Interfaces.m_ViewRenderBeams->DrawBeam( Beam );

		m_BulletTracers.erase( m_BulletTracers.begin( ) + iPosition );
	}
}

void C_World::DrawClientImpacts ( )
{
	if ( !g_Settings->m_bDrawClientImpacts )
		return;

	auto& aClientImpactList = *( CUtlVector< ClientImpact_t >*)( ( uintptr_t )( g_Globals.m_LocalPlayer ) + 0xBC00 );
	for ( auto Impact = aClientImpactList.Count( ); Impact > m_iLastProcessedImpact; --Impact )
		g_Globals.m_Interfaces.m_DebugOverlay->BoxOverlay( 
			aClientImpactList[ Impact - 1 ].m_vecPosition, 
			Vector( -2.0f, -2.0f, -2.0f ),
			Vector( 2.0f, 2.0f, 2.0f ), 
			QAngle( 0.0f, 0.0f, 0.0f ), 
			g_Settings->m_ClientImpacts->r( ), 
			g_Settings->m_ClientImpacts->g( ),
			g_Settings->m_ClientImpacts->b( ),
			g_Settings->m_ClientImpacts->a( ),
			4.0f );

	if ( aClientImpactList.Count( ) != m_iLastProcessedImpact )
		m_iLastProcessedImpact = aClientImpactList.Count( );
}

void C_World::SkyboxChanger( )
{
	std::string strSkyBox = g_Globals.m_Interfaces.m_CVar->FindVar( _S( "sv_skyname" ) )->GetString( );
	switch ( g_Settings->m_iSkybox )
	{
		case 1:
			strSkyBox = _S( "cs_tibet" );
			break;
		case 2:
			strSkyBox = _S( "cs_baggage_skybox_" );
			break;
		case 3:
			strSkyBox = _S( "italy" );
			break;
		case 4:
			strSkyBox = _S( "jungle" );
			break;
		case 5:
			strSkyBox = _S( "office" );
			break;
		case 6:
			strSkyBox = _S( "sky_cs15_daylight01_hdr" );
			break;
		case 7:
			strSkyBox = _S( "sky_cs15_daylight02_hdr" );
			break;
		case 8:
			strSkyBox = _S( "vertigoblue_hdr" );
			break;
		case 9:
			strSkyBox = _S( "vertigo" );
			break;
		case 10:
			strSkyBox = _S( "sky_day02_05_hdr" );
			break;
		case 11:
			strSkyBox = _S( "nukeblank" );
			break;
		case 12:
			strSkyBox = _S( "sky_venice" );
			break;
		case 13:
			strSkyBox = _S( "sky_cs15_daylight03_hdr" );
			break;
		case 14:
			strSkyBox = _S( "sky_cs15_daylight04_hdr" );
			break;
		case 15:
			strSkyBox = _S( "sky_csgo_cloudy01" );
			break;
		case 16:
			strSkyBox = _S( "sky_csgo_night02" );
			break;
		case 17:
			strSkyBox = _S( "sky_csgo_night02b" );
			break;
		case 18:
			strSkyBox = _S( "sky_csgo_night_flat" );
			break;
		case 19:
			strSkyBox = _S( "sky_dust" );
			break;
		case 20:
			strSkyBox = _S( "vietnam" );
			break;
		case 21:
			strSkyBox = g_Settings->m_szCustomSkybox;
			break;
	}

	g_Globals.m_ConVars.m_3DSky->SetValue( false );
	if ( g_Settings->m_iSkybox <= 0 )
		g_Globals.m_ConVars.m_3DSky->SetValue( true );

	return g_Tools->SetSkybox( strSkyBox.c_str( ) );
}