#include "../SDK/Includes.hpp"
#include "../Settings.hpp"
#include "../Log Manager/LogManager.hpp"
#include "../BuyBot/BuyBot.hpp"
#include "../Animations/LagCompensation.hpp"
#include "../RageBot/RageBot.hpp"
#include "../Visuals/World.hpp"
#include "../Networking/Fakelag.hpp"

#include "../Visuals/Players.hpp"
#include "../Networking/Networking.hpp"
#include "../Animations/LocalAnimations.hpp"
#include "../Packet/PacketManager.hpp"

void C_CustomEventListener::FireGameEvent( C_GameEvent* pEvent )
{
	if ( !g_Globals.m_LocalPlayer )
		return;

	auto GetHitboxByHitgroup = [ ]( int32_t iHitgroup ) -> int
	{
		switch ( iHitgroup )
		{
		case HITGROUP_HEAD:
			return HITBOX_HEAD;
		case HITGROUP_CHEST:
			return HITBOX_CHEST;
		case HITGROUP_STOMACH:
			return HITBOX_STOMACH;
		case HITGROUP_LEFTARM:
			return HITBOX_LEFT_HAND;
		case HITGROUP_RIGHTARM:
			return HITBOX_RIGHT_HAND;
		case HITGROUP_LEFTLEG:
			return HITBOX_RIGHT_CALF;
		case HITGROUP_RIGHTLEG:
			return HITBOX_LEFT_CALF;
		default:
			return HITBOX_PELVIS;
		}
	};

	auto GetHitboxNameFromHitgroup = [ GetHitboxByHitgroup ]( int32_t iHitgroup ) -> std::string
	{
		switch ( GetHitboxByHitgroup( iHitgroup ) )
		{
		case HITBOX_HEAD:
			return _S( "head" );
		case HITBOX_CHEST:
			return _S( "chest" );
		case HITBOX_STOMACH:
			return _S( "stomach" );
		case HITBOX_PELVIS:
			return _S( "pelvis" );
		case HITBOX_RIGHT_UPPER_ARM:
		case HITBOX_RIGHT_FOREARM:
		case HITBOX_RIGHT_HAND:
			return _S( "left arm" );
		case HITBOX_LEFT_UPPER_ARM:
		case HITBOX_LEFT_FOREARM:
		case HITBOX_LEFT_HAND:
			return _S( "right arm" );
		case HITBOX_RIGHT_THIGH:
		case HITBOX_RIGHT_CALF:
			return _S( "left leg" );
		case HITBOX_LEFT_THIGH:
		case HITBOX_LEFT_CALF:
			return _S( "right leg" );
		case HITBOX_RIGHT_FOOT:
			return _S( "left foot" );
		case HITBOX_LEFT_FOOT:
			return _S( "right foot" );
		}
	};

	if ( strstr( pEvent->GetName( ), _S( "weapon_fire" ) ) )
		g_RageBot->OnWeaponFire( pEvent );
	else if ( strstr( pEvent->GetName( ), _S( "player_hurt" ) ) )
	{
		C_BasePlayer* pHurtPlayer = C_BasePlayer::GetPlayerByIndex( g_Globals.m_Interfaces.m_EngineClient->GetPlayerForUserID( pEvent->GetInt( _S( "userid" ) ) ) );
		if ( !pHurtPlayer || !pHurtPlayer->IsPlayer( ) )
			return;
		
		g_RageBot->OnPlayerHurt( pEvent );
		if ( g_Globals.m_Interfaces.m_EngineClient->GetPlayerForUserID( pEvent->GetInt( _S( "attacker" ) ) ) == g_Globals.m_Interfaces.m_EngineClient->GetLocalPlayer( ) )
		{
			if ( g_Settings->m_bHitSound )
				g_Globals.m_Interfaces.m_Surface->PlaySound_( _S( "buttons\\arena_switch_press_02.wav" ) );

			if ( g_Settings->m_bLogHurts )
			{
				C_PlayerInfo Info;
				g_Globals.m_Interfaces.m_EngineClient->GetPlayerInfo( pHurtPlayer->EntIndex( ), &Info );

				std::string strHurtMessage = _S( "Hurt " );
				strHurtMessage += Info.m_strName;
				strHurtMessage += _S(" in the ");
				strHurtMessage += GetHitboxNameFromHitgroup( pEvent->GetInt( _S( "hitgroup" ) ) );
				strHurtMessage += _S( " for " );
				strHurtMessage += std::to_string( pEvent->GetInt( "dmg_health" ) );
				strHurtMessage += _S( "hp ( " );
				strHurtMessage += std::to_string( pEvent->GetInt( "health" ) );
				strHurtMessage += _S( "hp remain )" );

				g_LogManager->PushLog( strHurtMessage, _S( "h" ) );
			}
		}
		else if ( g_Settings->m_bLogHarms && ( g_Globals.m_Interfaces.m_EngineClient->GetPlayerForUserID( pEvent->GetInt( _S( "attacker" ) ) ) != g_Globals.m_Interfaces.m_EngineClient->GetLocalPlayer( ) ) && pHurtPlayer == g_Globals.m_LocalPlayer )
		{
			C_BasePlayer* pPlayer = C_BasePlayer::GetPlayerByIndex( g_Globals.m_Interfaces.m_EngineClient->GetPlayerForUserID( pEvent->GetInt( _S( "attacker" ) ) ) );
			if ( !pPlayer || !pPlayer->IsPlayer( ) )
				return;

			C_PlayerInfo Info;
			g_Globals.m_Interfaces.m_EngineClient->GetPlayerInfo( g_Globals.m_Interfaces.m_EngineClient->GetPlayerForUserID( pEvent->GetInt( _S( "attacker" ) ) ), &Info );

			std::string strHurtMessage = _S( "Harmed for " );
			strHurtMessage += std::to_string( pEvent->GetInt( _S( "dmg_health" ) ) ) + _S( " damage from " ) + Info.m_strName + _S( " in the " );
			strHurtMessage += GetHitboxNameFromHitgroup( pEvent->GetInt( _S( "hitgroup" ) ) );
			
			g_LogManager->PushLog( strHurtMessage, _S( "h" ) );
		}
	}
	else if ( strstr( pEvent->GetName( ), _S( "bullet_impact" ) ) )
	{
		g_RageBot->OnBulletImpact( pEvent );
		g_World->OnBulletImpact( pEvent );
	}
	else if ( strstr( pEvent->GetName( ), _S( "bomb_beginplant" ) ) )
	{
		C_PlayerInfo PlayerInfo;
		g_Globals.m_Interfaces.m_EngineClient->GetPlayerInfo( g_Globals.m_Interfaces.m_EngineClient->GetPlayerForUserID( pEvent->GetInt( _S( "userid" ) ) ), &PlayerInfo );

		if ( g_Settings->m_bLogBomb )
			g_LogManager->PushLog( std::string( PlayerInfo.m_strName ) + _S( " started planting the bomb" ), _S( "d" ) );
	}
	else if ( strstr( pEvent->GetName( ), _S( "bomb_begindefuse" ) ) )
	{
		C_PlayerInfo PlayerInfo;
		g_Globals.m_Interfaces.m_EngineClient->GetPlayerInfo( g_Globals.m_Interfaces.m_EngineClient->GetPlayerForUserID( pEvent->GetInt( _S( "userid" ) ) ), &PlayerInfo );

		if ( g_Settings->m_bLogBomb )
			g_LogManager->PushLog( std::string( PlayerInfo.m_strName ) + _S( " started defusing the bomb" ), _S( "d" ) );
	}
	else if ( strstr( pEvent->GetName( ), _S( "round_start" ) ) )
	{
		g_BuyBot->OnRoundStart( );
		g_Networking->ResetData( );
		g_LocalAnimations->ResetData( );
		g_PredictionSystem->ResetData( );
		g_RageBot->ResetData( );
		g_PlayerESP->ResetData( );

		g_Globals.m_RoundInfo.m_bShouldClearDeathNotices = true;
	}
	else if ( strstr( pEvent->GetName( ), _S( "player_death" ) ) )
	{
		if ( g_Globals.m_Interfaces.m_EngineClient->GetPlayerForUserID( pEvent->GetInt( _S( "userid" ) ) ) != g_Globals.m_Interfaces.m_EngineClient->GetLocalPlayer( ) )
			return;
		
		g_Globals.m_RoundInfo.m_bShouldClearDeathNotices = true;
	}
	else if ( strstr( pEvent->GetName( ), _S( "item_purchase" ) ) && g_Settings->m_bLogPurchases )
	{
		C_BasePlayer* pPlayer = C_BasePlayer::GetPlayerByIndex( g_Globals.m_Interfaces.m_EngineClient->GetPlayerForUserID( pEvent->GetInt( _S( "userid" ) ) ) );
		if ( !pPlayer || pPlayer->m_iTeamNum( ) == g_Globals.m_LocalPlayer->m_iTeamNum( ) )
			return;

		C_PlayerInfo Info;
		g_Globals.m_Interfaces.m_EngineClient->GetPlayerInfo( g_Globals.m_Interfaces.m_EngineClient->GetPlayerForUserID( pEvent->GetInt( _S( "userid" ) ) ), &Info );

		std::string strWeaponName = pEvent->GetString( _S( "weapon" ) );
		if ( strstr( strWeaponName.c_str( ), _S( "unknown" ) ) || strstr( strWeaponName.c_str( ), _S( "assaultsuit" ) ) || strstr( strWeaponName.c_str( ), _S( "kevlar" ) ) )
			return;

		if ( strstr( strWeaponName.c_str( ), _S( "weapon_" ) ) )
			strWeaponName.erase( strWeaponName.begin( ), strWeaponName.begin( ) + 7 );
		else if ( strstr( strWeaponName.c_str( ), _S( "item_" ) ) )
			strWeaponName.erase( strWeaponName.begin( ), strWeaponName.begin( ) + 4 );

		if ( strstr( strWeaponName.c_str( ), _S( "_defuser" ) ) )
			strWeaponName = _S( "defuser" );

		std::string strMessage = Info.m_strName;
		strMessage += _S(" bought ");
		strMessage += strWeaponName;
		strMessage += _S( "." );

		g_LogManager->PushLog( strMessage, _S( "b" ) );
	}
}