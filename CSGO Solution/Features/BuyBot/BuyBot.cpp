#include "BuyBot.hpp"
#include "../Settings.hpp"

void C_BuyBot::OnRoundStart( )
{
	if ( m_bShouldBuy )
		return;

	m_bDidBuy = false;
	m_bShouldBuy = true;
}

void C_BuyBot::OnCreateMove( )
{
	if ( m_bDidBuy || !m_bShouldBuy )
		return;

	std::string strBuyCommand = "";
	if ( !g_Settings->m_bBuyBotEnabled )
		return;

	switch ( g_Settings->m_BuyBotPrimaryWeapon )
	{
		case 1: strBuyCommand += _S( "buy scar20; buy g3sg1; " ); break;
		case 2: strBuyCommand += _S( "buy ssg08; " ); break;
		case 3: strBuyCommand += _S( "buy awp; " ); break;
		case 4: strBuyCommand += _S( "buy ak47; buy m4a1; " ); break;
	}

	if ( g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( ) )
		if ( g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( )->m_iItemDefinitionIndex( ) == WEAPON_AWP )
			if ( g_Settings->m_bBuyBotKeepAWP )
				strBuyCommand = "";

	switch ( g_Settings->m_BuyBotSecondaryWeapon )
	{
		case 1: strBuyCommand += _S( "buy fn57; buy tec9; " ); break;
		case 2: strBuyCommand += _S( "buy elite; " ); break;
		case 3: strBuyCommand += _S( "buy deagle; " ); break;
		case 4: strBuyCommand += _S( "buy usp_silencer; buy glock; " ); break;
		case 5: strBuyCommand += _S( "buy p250; " ); break;
	}

	if ( g_Settings->m_aEquipment[ 0 ] )
		strBuyCommand += _S( "buy incgrenade; buy molotov; " );

	if ( g_Settings->m_aEquipment[ 1 ] )
		strBuyCommand += _S( "buy smokegrenade; " );

	if ( g_Settings->m_aEquipment[ 2 ] )
		strBuyCommand += _S( "buy hegrenade; " );

	if ( g_Settings->m_aEquipment[ 3 ] )
		strBuyCommand += _S( "buy taser; " );

	if ( g_Settings->m_aEquipment[ 4 ] )
		strBuyCommand += _S( "buy heavyarmor; " );

	if ( g_Settings->m_aEquipment[ 5 ] )
		strBuyCommand += _S( "buy vesthelm; " );

	if ( g_Settings->m_aEquipment[ 6 ] )
		strBuyCommand += _S( "buy defuser; " );

	g_Globals.m_Interfaces.m_EngineClient->ExecuteClientCmd( strBuyCommand.c_str( ) );

	m_bDidBuy = true;
	m_bShouldBuy = false;
}