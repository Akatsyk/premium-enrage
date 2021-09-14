#include <d3d9.h>
#include "Tools/Obfuscation/XorStr.hpp"
#include "SDK/Globals.hpp"
#include "Data/Ava.h"

#include "../Features/Model/Model.hpp"
#include <d3dx9.h>
#pragma comment (lib, "d3dx9.lib")

#include "../Render.hpp"
#include "../Config.hpp"
#include "../Settings.hpp"

void C_Menu::Instance()
{
	if (!m_bIsMenuOpened && ImGui::GetStyle().Alpha > 0.f) {
		float fc = 255.f / 0.2f * ImGui::GetIO().DeltaTime;
		ImGui::GetStyle().Alpha = std::clamp(ImGui::GetStyle().Alpha - fc / 255.f, 0.f, 1.f);
	}

	if (m_bIsMenuOpened && ImGui::GetStyle().Alpha < 1.f) {
		float fc = 255.f / 0.2f * ImGui::GetIO().DeltaTime;
		ImGui::GetStyle().Alpha = std::clamp(ImGui::GetStyle().Alpha + fc / 255.f, 0.f, 1.f);
	}

	this->DrawKeybindList( );
	if (!m_bIsMenuOpened && ImGui::GetStyle().Alpha < 0.1f)
		return;

	int32_t iScreenSizeX, iScreenSizeY;
	g_Globals.m_Interfaces.m_EngineClient->GetScreenSize(iScreenSizeX, iScreenSizeY);

	ImGui::SetNextWindowPos(ImVec2((iScreenSizeX / 2) - 325, (iScreenSizeY / 2) - 220), ImGuiCond_Once);
	ImGui::SetNextWindowSizeConstraints(ImVec2(800, 565), ImVec2(800, 565));
	ImGui::Begin(this->GetMenuName(), NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);

	ImGui::TabButton(_S("Rage"), _S("a"), &m_iCurrentTab, MENU_RAGE, 9);
	ImGui::TabButton(_S("Anti-aim"), _S("b"), &m_iCurrentTab, MENU_ANTIAIM, 9);
	ImGui::TabButton(_S("Legit"), _S("c"), &m_iCurrentTab, MENU_LEGIT, 9);
	ImGui::TabButton(_S("Players"), _S("d"), &m_iCurrentTab, MENU_PLAYERS, 9);
	ImGui::TabButton(_S("World"), _S("e"), &m_iCurrentTab, MENU_WORLD, 9);
	ImGui::TabButton(_S("Misc"), _S("f"), &m_iCurrentTab, MENU_MISC, 9);
	ImGui::TabButton(_S("Inventory"), _S("h"), &m_iCurrentTab, MENU_INVENTORY, 9);
	ImGui::TabButton(_S("Lua"), _S("i"), &m_iCurrentTab, MENU_LUA, 9);
	ImGui::TabButton(_S("Settings"), _S("g"), &m_iCurrentTab, MENU_SETTINGS, 9);

	if ( m_iCurrentTab == MENU_RAGE )
		this->DrawRageTab( );
	else if ( m_iCurrentTab == MENU_ANTIAIM )
		this->DrawAntiAimTab( );
	else if ( m_iCurrentTab == MENU_PLAYERS )
		this->DrawPlayersTab( );
	else if ( m_iCurrentTab == MENU_MISC )
		this->DrawMiscTab( );
	else if ( m_iCurrentTab == MENU_WORLD )
		this->DrawWorldTab( );
	else if ( m_iCurrentTab == MENU_SETTINGS )
		this->DrawConfigTab( );

	ImGui::End();
}

void C_Menu::DrawRageTab()
{
	ImVec2 vecWindowPosition = ImGui::GetWindowPos();

	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
	int iMainTextSize = ImGui::CalcTextSize(this->GetMenuName()).x;
	int iChildDoubleSizeX = (800 - ImGui::CalcTextSize(this->GetMenuName()).x - 85) / 2;
	int iChildDoubleSizeY = (565 - 45) / 2;

	int iChildPosFirstX = vecWindowPosition.x + iMainTextSize + 15 + 40;
	int iChildPosSecondX = vecWindowPosition.x + iMainTextSize + 30 + iChildDoubleSizeX + 40;

	int iChildPosFirstY = vecWindowPosition.y + 15;
	int iChildPosSecondY = vecWindowPosition.y + 15 + iChildDoubleSizeY + 15;

	ImGui::PopFont();

	ImGui::SetNextWindowPos(ImVec2(iChildPosFirstX, iChildPosFirstY));
	ImGui::BeginChild("Main", ImVec2(iChildDoubleSizeX, iChildDoubleSizeY));

	std::vector< const char* > aRageWeapons =
	{
		( "Auto" ),
		( "Scout" ),
		( "AWP" ),
		( "Deagle" ),
		( "Revolver" ),
		( "Pistol" ),
		( "Rifle" )
	};

	std::vector< const char* > aHitboxes =
	{
		( "Head" ),
		( "Chest" ),
		( "Arms" ),
		( "Pelvis" ),
		( "Stomach" ),
		( "Legs" ),
	};

	static int wep = 0;
	ImGui::SingleSelect( "Weapon", &wep, aRageWeapons );

	ImGui::Checkbox( "Enable Ragebot", &g_Settings->m_aRageSettings[ wep ].m_bEnabled );

	ImGui::Checkbox( "Auto stop", &g_Settings->m_aRageSettings[ wep ].m_bAutoStop );
	ImGui::Checkbox( "Auto scope", &g_Settings->m_aRageSettings[ wep ].m_bAutoScope );

	ImGui::EndChild();

	ImGui::SetNextWindowPos( ImVec2( iChildPosFirstX, iChildPosSecondY ) );
	ImGui::BeginChild( "Accuracy", ImVec2( iChildDoubleSizeX, iChildDoubleSizeY ) );
	
	ImGui::SliderInt( "Mindamage", &g_Settings->m_aRageSettings[ wep ].m_iMinDamage, 0, 100 );
	ImGui::SliderInt( "Mindamage override", &g_Settings->m_aRageSettings[ wep ].m_iMinDamageOverride, 0, 100 );
	ImGui::SliderInt( "Hitchance", &g_Settings->m_aRageSettings[ wep ].m_iHitChance, 0, 100 );
	ImGui::SliderInt( "Double Tap Hitchance", &g_Settings->m_aRageSettings[ wep ].m_iDoubleTapHitChance, 0, 100 );
	ImGui::SliderInt( "Accuracy boost", &g_Settings->m_aRageSettings[ wep ].m_iAccuracyBoost, 0, 100 );
	ImGui::SliderFloat( "Recharge time", g_Settings->m_flRechargeTime, 0.0f, 2.0f );

	std::vector < const char* > aAutoStop =
	{
		"Force accuracy",
		"Early"
	};

	if ( ImGui::BeginCombo( _S( "Autostop options" ), _S( "Select" ), 0, aAutoStop.size( ) ) )
	{
		for ( int i = 0; i < aAutoStop.size( ); i++ )
			ImGui::Selectable( aAutoStop[ i ], &g_Settings->m_aRageSettings[ wep ].m_AutoStopOptions[ i ], ImGuiSelectableFlags_DontClosePopups );

		ImGui::EndCombo( );
	}

	std::vector < const char* > aDoubleTap =
	{
		"Move between shots",
		"Full stop"
	};

	if ( ImGui::BeginCombo( _S( "Double Tap options" ), _S( "Select" ), 0, aDoubleTap.size( ) ) )
	{
		for ( int i = 0; i < aDoubleTap.size( ); i++ )
			ImGui::Selectable( aDoubleTap[ i ], &g_Settings->m_aRageSettings[ wep ].m_DoubleTapOptions[ i ], ImGuiSelectableFlags_DontClosePopups );

		ImGui::EndCombo( );
	}

	ImGui::EndChild();

	ImGui::SetNextWindowPos(ImVec2(iChildPosSecondX, iChildPosFirstY));
	ImGui::BeginChild("Advanced", ImVec2(iChildDoubleSizeX, iChildDoubleSizeY));

	ImGui::Text(_S("Force safe"));
	ImGui::Keybind(_S("Force safe"), &g_Settings->m_aSafePoint->m_iKeySelected, &g_Settings->m_aSafePoint->m_iModeSelected);

	ImGui::Text(_S("Damage override"));
	ImGui::Keybind(_S("Damage override"), &g_Settings->m_aMinDamage->m_iKeySelected, &g_Settings->m_aMinDamage->m_iModeSelected);

	ImGui::Text(_S("Double tap"));
	ImGui::Keybind(_S("Double tap"), &g_Settings->m_aDoubleTap->m_iKeySelected, &g_Settings->m_aDoubleTap->m_iModeSelected);

	ImGui::Text(_S("Hide shots"));
	ImGui::Keybind(_S("Hide shots"), &g_Settings->m_aHideShots->m_iKeySelected, &g_Settings->m_aHideShots->m_iModeSelected);

	ImGui::EndChild();

	ImGui::SetNextWindowPos(ImVec2(iChildPosSecondX, iChildPosSecondY));
	ImGui::BeginChild("Target", ImVec2(iChildDoubleSizeX, iChildDoubleSizeY));

	if ( ImGui::BeginCombo (_S( "Hitboxes" ), _S( "Select" ), 0, aHitboxes.size( ) ) )
	{
		for ( int i = 0; i < aHitboxes.size( ); i++ )
			ImGui::Selectable( aHitboxes[ i ], &g_Settings->m_aRageSettings[ wep ].m_Hitboxes[ i ], ImGuiSelectableFlags_DontClosePopups );

		ImGui::EndCombo( );
	}

	if ( ImGui::BeginCombo (_S( "Safe Hitboxes" ), _S( "Select" ), 0, aHitboxes.size( ) ) )
	{
		for ( int i = 0; i < aHitboxes.size( ); i++ )
			ImGui::Selectable( aHitboxes[ i ], &g_Settings->m_aRageSettings[ wep ].m_SafeHitboxes[ i ], ImGuiSelectableFlags_DontClosePopups );

		ImGui::EndCombo( );
	}

	if ( ImGui::BeginCombo (_S( "Multipoints" ), _S( "Select" ), 0, aHitboxes.size( ) ) )
	{
		for ( int i = 0; i < aHitboxes.size( ); i++ )
			ImGui::Selectable( aHitboxes[ i ], &g_Settings->m_aRageSettings[ wep ].m_Multipoints[ i ], ImGuiSelectableFlags_DontClosePopups );

		ImGui::EndCombo( );
	}

	ImGui::SliderInt( "Head scale", &g_Settings->m_aRageSettings[ wep ].m_iHeadScale, 0, 100 );
	ImGui::SliderInt( "Body scale", &g_Settings->m_aRageSettings[ wep ].m_iBodyScale, 0, 100 );

	ImGui::Checkbox( _S( "Lethal safe" ), &g_Settings->m_aRageSettings[ wep ].m_bLethalSafety );
	ImGui::Checkbox( _S( "Prefer body" ), &g_Settings->m_aRageSettings[ wep ].m_bPreferBody );
	ImGui::Checkbox( _S( "Prefer safe" ), &g_Settings->m_aRageSettings[ wep ].m_bPreferSafe );

	ImGui::EndChild();
}

void C_Menu::DrawAntiAimTab( )
{
	ImVec2 vecWindowPosition = ImGui::GetWindowPos();

	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
	int iMainTextSize = ImGui::CalcTextSize(this->GetMenuName()).x;
	int iChildDoubleSizeX = (800 - ImGui::CalcTextSize(this->GetMenuName()).x - 85) / 2;
	int iChildDoubleSizeY = (565 - 45) / 2;

	int iChildSizeX = (800 - ImGui::CalcTextSize(this->GetMenuName()).x - 40) - 30;
	int iChildSizeY = (565 - 30) - 200;

	ImGui::PopFont( );

	int iChildPosFirstX = vecWindowPosition.x + iMainTextSize + 15 + 40;
	int iChildPosSecondX = vecWindowPosition.x + iMainTextSize + 30 + iChildDoubleSizeX + 40;

	int iChildPosFirstY = vecWindowPosition.y + 15;
	int iChildPosSecondY = vecWindowPosition.y + 15 + iChildDoubleSizeY + 15;

	ImGui::SetNextWindowPos( ImVec2( iChildPosFirstX, iChildPosFirstY ) );
	ImGui::BeginChild( _S( "Main" ), ImVec2( iChildDoubleSizeX, iChildDoubleSizeY - 30 ) );

	ImGui::Spacing( );
	ImGui::Spacing( );

	ImGui::Checkbox( _S( "Enable Anti-aim" ), g_Settings->m_bAntiAim );
	ImGui::Checkbox( _S( "At targets" ), g_Settings->m_bAntiAimAtTargets );
	ImGui::Checkbox( _S( "Auto direction" ), g_Settings->m_bAutoDirection );

	ImGui::SliderInt( _S( "Yaw add offset" ), g_Settings->m_iYawAddOffset, 0, 180 );
	ImGui::SliderInt( _S( "Jitter amount" ), g_Settings->m_iJitterAmount, 0, 180 );

	ImGui::Text( _S( "Manual left" ) );
	ImGui::Keybind( _S( "123" ), &g_Settings->m_aManualLeft->m_iKeySelected, &g_Settings->m_aManualRight->m_iModeSelected );

	ImGui::Text( _S( "Manual back" ) );
	ImGui::Keybind( _S( "1234" ), &g_Settings->m_aManualBack->m_iKeySelected, &g_Settings->m_aManualBack->m_iModeSelected );

	ImGui::Text( _S( "Manual right" ) );
	ImGui::Keybind( _S( "12346" ), &g_Settings->m_aManualRight->m_iKeySelected, &g_Settings->m_aManualRight->m_iModeSelected );

	ImGui::EndChild( );

	ImGui::SetNextWindowPos( ImVec2( iChildPosFirstX, iChildPosSecondY - 30 ) );
	ImGui::BeginChild( _S( "Angle settings" ), ImVec2( iChildDoubleSizeX, iChildDoubleSizeY ) );

	ImGui::Spacing( );
	ImGui::Spacing( );

	std::vector < const char* > PitchModes = { _S( "None" ), _S( "Down" ), _S( "Up" ), _S( "Fake down" ), _S( "Fake up" ) };
	ImGui::SingleSelect( _S( "Pitch" ), g_Settings->m_iPitchMode, PitchModes );

	ImGui::SliderInt( _S( "Left fake limit" ), g_Settings->m_iLeftFakeLimit, 0, 60 );
	ImGui::SliderInt( _S( "Right fake limit" ), g_Settings->m_iRightFakeLimit, 0, 60 );
	
	std::vector < std::string > InverterConditions = { _S( "Stand" ), _S( "Move" ), _S( "Air" ) };
	if ( ImGui::BeginCombo (_S( "Auto inverter" ), _S( "Select" ), 0, InverterConditions.size( ) ) )
	{
		for ( int i = 0; i < InverterConditions.size( ); i++ )
			ImGui::Selectable( InverterConditions[ i ].c_str( ), &g_Settings->m_aInverterConditions[ i ], ImGuiSelectableFlags_DontClosePopups );

		ImGui::EndCombo( );
	}

	ImGui::Text( _S( "Inverter" ) );
	ImGui::Keybind( _S( "InvertButton" ), &g_Settings->m_aInverter->m_iKeySelected, &g_Settings->m_aInverter->m_iModeSelected );

	ImGui::EndChild( );

	ImGui::SetNextWindowPos( ImVec2( iChildPosSecondX, iChildPosFirstY ) );
	ImGui::BeginChild( _S( "Fakelag settings" ), ImVec2( iChildDoubleSizeX, iChildDoubleSizeY ) );

	ImGui::Spacing( );
	ImGui::Spacing( );

	ImGui::Checkbox( _S( "Enable fakelag" ), g_Settings->m_bFakeLagEnabled );
	
	ImGui::SliderInt( _S( "Lag limit" ), g_Settings->m_iLagLimit, 1, 14 );

	std::vector < std::string > aLagTriggers
	=
	{
		_S( "Move" ),
		_S( "Air" ),
		_S( "Peek" )
	};

	if ( ImGui::BeginCombo (_S( "Lag triggers" ), _S( "Select" ), 0, aLagTriggers.size( ) ) )
	{
		for ( int i = 0; i < aLagTriggers.size( ); i++ )
			ImGui::Selectable( aLagTriggers[ i ].c_str( ), &g_Settings->m_aFakelagTriggers[ i ], ImGuiSelectableFlags_DontClosePopups );

		ImGui::EndCombo( );
	}

	ImGui::SliderInt( _S( "Trigger limit" ), g_Settings->m_iTriggerLimit, 1, 14 );

	ImGui::EndChild( );

	ImGui::SetNextWindowPos( ImVec2( iChildPosSecondX, iChildPosSecondY ) );
	ImGui::BeginChild( _S( "Movement" ), ImVec2( iChildDoubleSizeX, iChildDoubleSizeY ) );
		
	ImGui::Spacing( );
	ImGui::Spacing( );

	ImGui::Checkbox( _S( "Jitter move" ), g_Settings->m_bJitterMove );

	ImGui::Text( _S( "Slowwalk" ) );
	ImGui::Keybind( _S( "SW" ), &g_Settings->m_aSlowwalk->m_iKeySelected, &g_Settings->m_aSlowwalk->m_iModeSelected );

	ImGui::Text( _S( "Fakeduck" ) );
	ImGui::Keybind( _S( "FD" ), &g_Settings->m_aFakeDuck->m_iKeySelected, &g_Settings->m_aFakeDuck->m_iModeSelected );

	ImGui::Text( _S( "Auto peek" ) );
	ImGui::Keybind( _S( "AP" ), &g_Settings->m_aAutoPeek->m_iKeySelected, &g_Settings->m_aAutoPeek->m_iModeSelected );

	std::vector < const char* > aLegMovement
	=
	{
		_S( "Default" ),
		_S( "Slide" )
	};

	ImGui::SingleSelect( _S( "Leg movement" ), g_Settings->m_iLegMovement, aLegMovement );

	ImGui::EndChild( );
}

void C_Menu::DrawPlayersTab()
{
	ImVec2 vecWindowPosition = ImGui::GetWindowPos();

	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
	int iMainTextSize = ImGui::CalcTextSize(this->GetMenuName()).x;
	int iChildDoubleSizeX = (800 - ImGui::CalcTextSize(this->GetMenuName()).x - 85) / 2;
	int iChildDoubleSizeY = (565 - 45) / 2;

	int iChildSizeX = (800 - ImGui::CalcTextSize(this->GetMenuName()).x - 40 - 30);
	int iChildSizeY = (565 - 30);

	int iChildPosFirstX = vecWindowPosition.x + iMainTextSize + 15 + 40;
	int iChildPosSecondX = vecWindowPosition.x + iMainTextSize + 30 + iChildDoubleSizeX + 40;

	int iChildPosFirstY = vecWindowPosition.y + 15;
	int iChildPosSecondY = vecWindowPosition.y + 15 + iChildDoubleSizeY + 15;
	ImGui::PopFont();


	ImGui::SetNextWindowPos(ImVec2(iChildPosFirstX, iChildPosFirstY));
	ImGui::BeginChild("ESP", ImVec2(iChildDoubleSizeX, iChildSizeY - 240));
	static int iPlayerESPType = 0;
		
	ImGui::Spacing( );
	ImGui::Spacing( );

	ImGui::SingleSelect(_S("Player Type"), &iPlayerESPType, { _S("Enemy"), _S("Team"), _S("Local") });

	static C_PlayerSettings* Settings = NULL;
	switch ( iPlayerESPType )
	{
		case 0: Settings = g_Settings->m_Enemies; break;
		case 1: Settings = g_Settings->m_Teammates; break;
		case 2: Settings = g_Settings->m_LocalPlayer; break;
	}

	ImGui::Checkbox(_S("Box"), &Settings->m_BoundaryBox);
	this->DrawColorEdit4(_S("Box##color"), &Settings->m_aBoundaryBox);

	ImGui::Checkbox(_S("Name"), &Settings->m_RenderName);
	this->DrawColorEdit4(_S("Name##color"), &Settings->m_aNameColor);

	ImGui::Checkbox(_S("Health bar"), &Settings->m_RenderHealthBar);
	this->DrawColorEdit4(_S("m_aHealth##color"), &Settings->m_aHealthBar);

	ImGui::Checkbox(_S("Health bar text"), &Settings->m_RenderHealthText);
	this->DrawColorEdit4(_S("m_aHealthText##color"), &Settings->m_aHealthText);

	ImGui::Checkbox(_S("Ammo bar"), &Settings->m_RenderAmmoBar);
	this->DrawColorEdit4(_S("m_aAmmoBar##color"), &Settings->m_aAmmoBar);

	ImGui::Checkbox(_S("Ammo bar text"), &Settings->m_RenderAmmoBarText);
	this->DrawColorEdit4(_S("m_aAmmoBarText##color"), &Settings->m_aAmmoBarText);

	ImGui::Checkbox(_S("Weapon Text"), &Settings->m_RenderWeaponText);
	this->DrawColorEdit4(_S("m_aWeaponText##color"), &Settings->m_aWeaponText);

	ImGui::Checkbox(_S("Weapon Icon"), &Settings->m_RenderWeaponIcon);
	this->DrawColorEdit4(_S("m_aWeaponIcon##color"), &Settings->m_aWeaponIcon);
	
	ImGui::Checkbox(_S("Out of view arrows"), g_Settings->m_bOutOfViewArrows);
	this->DrawColorEdit4(_S("awerqweqw2e123412er412q4##color"), g_Settings->m_aOutOfViewArrows);

	static char aFontName[ 32 ];
	if ( ImGui::InputText( _S( "Custom font" ), aFontName, 32 ) )
		g_Settings->m_szCustomFont.Get( ) = aFontName;
	
	static int nFontSize = 16;
	ImGui::SliderInt( _S( "Font size" ), &nFontSize, 1, 32 );
	
	if ( ImGui::Button( "Apply" ) )
		g_Render->ForceReload( g_Settings->m_szCustomFont.Get( ), nFontSize );


	ImGui::EndChild();

	ImGui::SetNextWindowPos(ImVec2(iChildPosFirstX, iChildPosFirstY + iChildSizeY - 240 + 15));
	ImGui::BeginChild( _S( "Glow" ), ImVec2( iChildDoubleSizeX, 225 ) );
	
	ImGui::Spacing( );
	ImGui::Spacing( );

	ImGui::Checkbox( _S( "Player Glow" ), &Settings->m_bRenderGlow );
	this->DrawColorEdit4( _S( "##m_aGlowcolor" ), &Settings->m_aGlow );
	ImGui::SingleSelect( _S( "Glow style##1" ), &Settings->m_iGlowStyle, { _S( "Outline" ), _S( "Thin" ), _S( "Cover" ), _S( "Cover Pulse" ) } );

	ImGui::Checkbox( _S("C4 Glow" ), g_Settings->m_bRenderC4Glow );
	this->DrawColorEdit4( _S( "##C4Glowcolor" ), g_Settings->m_aC4Glow );
	ImGui::SingleSelect( _S( "Glow style##m_iC4GlowStyle" ), g_Settings->m_iC4GlowStyle, { _S( "Outline" ), _S( "Thin" ), _S( "Cover" ), _S( "Cover Pulse" ) } );

	ImGui::Checkbox( _S("Dropped Weapon Glow" ), g_Settings->m_bRenderDroppedWeaponGlow);
	this->DrawColorEdit4( _S("m_aDroppedWeaponGlow##color" ), g_Settings->m_aDroppedWeaponGlow);
	ImGui::SingleSelect( _S( "Glow style##m_iDroppedWeaponGlowStyle" ), g_Settings->m_iDroppedWeaponGlowStyle, { _S( "Outline" ), _S( "Thin" ), _S( "Cover" ), _S( "Cover Pulse" ) } );

	ImGui::Checkbox( _S( "Projectiles Glow" ), g_Settings->m_bRenderProjectileGlow );
	this->DrawColorEdit4( _S( "##Projectile" ), g_Settings->m_aProjectileGlow );
	ImGui::SingleSelect( _S( "Glow style##m_iProjectileGlowStyle" ), g_Settings->m_iProjectileGlowStyle, { _S( "Outline" ), _S( "Thin" ), _S( "Cover" ), _S( "Cover Pulse" ) } );

	ImGui::EndChild();

	g_DrawModel->SetGlow( Settings->m_iGlowStyle );
	if ( !Settings->m_bRenderGlow )
		g_DrawModel->SetGlow( -1 );
	else
		g_DrawModel->SetGlowColor( Color( Settings->m_aGlow ) );

	ImGui::SetNextWindowPos(ImVec2(iChildPosSecondX, iChildPosFirstY));
	ImGui::BeginChild(_S("Chams"), ImVec2(iChildDoubleSizeX, iChildSizeY));

	ImGui::Spacing( );
	ImGui::Spacing( );

	static int32_t iChamsGroup = 0;
	ImGui::SingleSelect( _S( "Chams group" ), &iChamsGroup, { _S( "Enemy visible" ), _S( "Enemy invisble" ), _S( "Backtrack" ), _S( "Shot chams" ), _S( "Team visible" ), _S( "Team invisible" ), _S( "Local" ), _S( "Desync" ), _S( "Lag" ), _S("Hands")});

	ImGui::Checkbox( _S( "Enable Chams" ), &g_Settings->m_aChamsSettings[ iChamsGroup ].m_bRenderChams );
	this->DrawColorEdit4( _S( "##qweqwe" ), &g_Settings->m_aChamsSettings[ iChamsGroup ].m_Color );
	ImGui::SingleSelect( _S( "Material" ), &g_Settings->m_aChamsSettings[ iChamsGroup ].m_iMainMaterial, { _S( "Flat" ), _S( "Regular" ) } );

	ImGui::Checkbox( _S( "Enable glow" ), &g_Settings->m_aChamsSettings[ iChamsGroup ].m_aModifiers[ 0 ] );
	this->DrawColorEdit4( _S( "##512414 color" ), &g_Settings->m_aChamsSettings[ iChamsGroup ].m_aModifiersColors[ 0 ] );
	ImGui::Checkbox( _S( "Enable ghost" ), &g_Settings->m_aChamsSettings[ iChamsGroup ].m_aModifiers[ 1 ] );
	this->DrawColorEdit4( _S( "##235235 color" ), &g_Settings->m_aChamsSettings[ iChamsGroup ].m_aModifiersColors[ 1 ] );
	ImGui::Checkbox( _S( "Enable glass" ), &g_Settings->m_aChamsSettings[ iChamsGroup ].m_aModifiers[ 2 ] );
	this->DrawColorEdit4( _S( "##4124124 color" ), &g_Settings->m_aChamsSettings[ iChamsGroup ].m_aModifiersColors[ 2 ] );
	ImGui::Checkbox( _S( "Enable pulsation" ), &g_Settings->m_aChamsSettings[ iChamsGroup ].m_aModifiers[ 3 ] );
	this->DrawColorEdit4( _S( "##123123 color" ), &g_Settings->m_aChamsSettings[ iChamsGroup ].m_aModifiersColors[ 3 ] );
	ImGui::Checkbox( _S( "Ragdoll chams" ), g_Settings->m_bDrawRagdolls );

	if ( iChamsGroup < 8 )
		g_DrawModel->SetChamsSettings( g_Settings->m_aChamsSettings[ iChamsGroup ] );

	ImGui::EndChild();

	ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(vecWindowPosition.x + 654 + 150, vecWindowPosition.y), ImVec2(vecWindowPosition.x + 950 + 150, vecWindowPosition.y + 440), ImColor(30.0f / 255.0f, 30.0f / 255.0f, 36.0f / 255.0f, 1.0f));
	if (g_DrawModel->GetTexture())
	{
		ImGui::GetForegroundDrawList()->AddImage(
			g_DrawModel->GetTexture()->pTextureHandles[0]->lpRawTexture,
			ImVec2(vecWindowPosition.x + 610 + 150, vecWindowPosition.y - 130),
			ImVec2(vecWindowPosition.x + 610 + 150 + g_DrawModel->GetTexture()->GetActualWidth(), vecWindowPosition.y + g_DrawModel->GetTexture()->GetActualHeight() - 130),
			ImVec2(0, 0), ImVec2(1, 1),
			ImColor(1.0f, 1.0f, 1.0f, 1.0f));
	}

	static ImVec2 vecPreviousMousePosition = ImVec2(0, 0);
	static ImVec2 vecLastMousePosition = ImVec2(0, 0);
	ImVec2 vecCurrentCursorPosition = ImGui::GetMousePos();

	static bool bIsActive = false;

	// render box
	Color aBox = Color(Settings->m_aBoundaryBox);
	if (Settings->m_BoundaryBox)
	{
		ImGui::GetOverlayDrawList()->AddRect(ImVec2(vecWindowPosition.x + 694 + 150, vecWindowPosition.y + 39), ImVec2(vecWindowPosition.x + 886 + 170, vecWindowPosition.y + 386), ImColor(0, 0, 0, 255));
		ImGui::GetOverlayDrawList()->AddRect(ImVec2(vecWindowPosition.x + 696 + 150, vecWindowPosition.y + 41), ImVec2(vecWindowPosition.x + 884 + 170, vecWindowPosition.y + 384), ImColor(0, 0, 0, 255));
		ImGui::GetOverlayDrawList()->AddRect(ImVec2(vecWindowPosition.x + 695 + 150, vecWindowPosition.y + 40), ImVec2(vecWindowPosition.x + 885 + 170, vecWindowPosition.y + 385), ImColor(aBox.r(), aBox.g(), aBox.b(), aBox.a()));
	}

	// render name
	Color aName = Color(Settings->m_aNameColor);
	if (Settings->m_RenderName)
	{
		ImGui::PushFont(g_Globals.m_Fonts.m_SegoeUI);
		ImGui::GetOverlayDrawList()->AddText(ImVec2(vecWindowPosition.x + 715 + 150 + 85 - ImGui::CalcTextSize(_S("Agent Ava")).x / 2, vecWindowPosition.y + 22), ImColor(aName.r(), aName.g(), aName.b(), aName.a()), _S("Agent Ava"));
		ImGui::PopFont();
	}

	// render health
	Color aHealthBar = Color(Settings->m_aHealthBar);
	if (Settings->m_RenderHealthBar)
	{
		ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(vecWindowPosition.x + 689 + 150, vecWindowPosition.y + 39), ImVec2(vecWindowPosition.x + 693 + 150, vecWindowPosition.y + 385), ImColor(0, 0, 0, 100));
		ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(vecWindowPosition.x + 690 + 150, vecWindowPosition.y + 40), ImVec2(vecWindowPosition.x + 692 + 150, vecWindowPosition.y + 385), ImColor(aHealthBar.r(), aHealthBar.g(), aHealthBar.b(), aHealthBar.a()));
	}

	Color aHealthText = Color(Settings->m_aHealthText);
	if (Settings->m_RenderHealthText)
	{
		ImGui::PushFont(g_Globals.m_Fonts.m_SegoeUI);

		if (Settings->m_RenderHealthBar)
			ImGui::GetOverlayDrawList()->AddText(ImVec2(vecWindowPosition.x + 687 + 150 - ImGui::CalcTextSize(_S("100")).x, vecWindowPosition.y + 37 ), ImColor(aHealthText.r(), aHealthText.g(), aHealthText.b(), aHealthText.a()), _S("100"));
		else
			ImGui::GetOverlayDrawList()->AddText(ImVec2(vecWindowPosition.x + 691 + 150 - ImGui::CalcTextSize(_S("100")).x, vecWindowPosition.y + 37 ), ImColor(aHealthText.r(), aHealthText.g(), aHealthText.b(), aHealthText.a()), _S("100"));

		ImGui::PopFont();
	}

	Color aWeaponText = Color(Settings->m_aWeaponText);
	if (Settings->m_RenderWeaponText)
	{
		ImGui::PushFont(g_Globals.m_Fonts.m_SegoeUI);
		ImGui::GetOverlayDrawList()->AddText(ImVec2(vecWindowPosition.x + 715 + 150 + 85 - ImGui::CalcTextSize(_S("P2000")).x / 2, vecWindowPosition.y + 385 + 6), ImColor(aWeaponText.r(), aWeaponText.g(), aWeaponText.b(), aWeaponText.a()), _S("P2000"));
		ImGui::PopFont();
	}

	Color aWeaponIcon = Color(Settings->m_aWeaponIcon);
	if (Settings->m_RenderWeaponIcon)
	{
		ImGui::PushFont(g_Globals.m_Fonts.m_WeaponIcon);

		if (Settings->m_RenderWeaponText)
			ImGui::GetOverlayDrawList()->AddText(ImVec2(vecWindowPosition.x + 715 + 150 + 85 - ImGui::CalcTextSize(_S("E")).x / 2, vecWindowPosition.y + 385 + 22), ImColor(aWeaponIcon.r(), aWeaponIcon.g(), aWeaponIcon.b(), aWeaponIcon.a()), _S("E"));
		else
			ImGui::GetOverlayDrawList()->AddText(ImVec2(vecWindowPosition.x + 715 + 150 + 85 - ImGui::CalcTextSize(_S("E")).x / 2, vecWindowPosition.y + 385 + 8), ImColor(aWeaponIcon.r(), aWeaponIcon.g(), aWeaponIcon.b(), aWeaponIcon.a()), _S("E"));

		ImGui::PopFont();
	}

	Color aAmmoBar = Color(Settings->m_aAmmoBar);
	if (Settings->m_RenderAmmoBar)
	{
		ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(vecWindowPosition.x + 695 + 150, vecWindowPosition.y + 387), ImVec2(vecWindowPosition.x + 906 + 150, vecWindowPosition.y + 391), ImColor(0, 0, 0, 100));
		ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(vecWindowPosition.x + 695 + 150, vecWindowPosition.y + 388), ImVec2(vecWindowPosition.x + 905 + 150, vecWindowPosition.y + 390), ImColor(aAmmoBar.r(), aAmmoBar.g(), aAmmoBar.b(), aAmmoBar.a()));
	}

	Color aAmmoText = Color(Settings->m_aAmmoBarText);
	if (Settings->m_RenderAmmoBarText)
	{
		ImGui::PushFont(g_Globals.m_Fonts.m_SegoeUI);
		ImGui::GetOverlayDrawList()->AddText(ImVec2(vecWindowPosition.x + 902 + 150 + ImGui::CalcTextSize(_S("13")).x / 2, vecWindowPosition.y + 386), ImColor(aAmmoText.r(), aAmmoText.g(), aAmmoText.b(), aAmmoText.a()), _S("13"));
		ImGui::PopFont();
	}

	ImGui::GetOverlayDrawList()->AddRect(ImVec2(vecWindowPosition.x + 653 + 150, vecWindowPosition.y + 2), ImVec2(vecWindowPosition.x + 948 + 150, vecWindowPosition.y + 438), ImColor(10.0f / 255.0f, 12.0f / 255.0f, 14.0f / 255.0f, 1.0f));
	ImGui::GetOverlayDrawList()->AddRect(ImVec2(vecWindowPosition.x + 652 + 150, vecWindowPosition.y + 1), ImVec2(vecWindowPosition.x + 949 + 150, vecWindowPosition.y + 439), ImColor(45.0f / 255.0f, 47.0f / 255.0f, 54.0f / 255.0f, 1.0f));
	ImGui::GetOverlayDrawList()->AddRect(ImVec2(vecWindowPosition.x + 651 + 150, vecWindowPosition.y), ImVec2(vecWindowPosition.x + 950 + 150, vecWindowPosition.y + 440), ImColor(10.0f / 255.0f, 12.0f / 255.0f, 14.0f / 255.0f, 1.0f));
}

static int Selected = 0;

void C_Menu::DrawMiscTab()
{
	ImVec2 vecWindowPosition = ImGui::GetWindowPos();

	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
	int iMainTextSize = ImGui::CalcTextSize(this->GetMenuName()).x;
	int iChildDoubleSizeX = (800 - ImGui::CalcTextSize(this->GetMenuName()).x - 85) / 2;
	int iChildDoubleSizeY = (565 - 45) / 2;

	int iChildPosFirstX = vecWindowPosition.x + iMainTextSize + 15 + 40;
	int iChildPosSecondX = vecWindowPosition.x + iMainTextSize + 30 + iChildDoubleSizeX + 40;

	int iChildPosFirstY = vecWindowPosition.y + 15;
	int iChildPosSecondY = vecWindowPosition.y + 15 + iChildDoubleSizeY + 15;

	ImGui::PopFont();

	ImGui::SetNextWindowPos(ImVec2(iChildPosFirstX, iChildPosFirstY));
	ImGui::BeginChild(_S("Movement"), ImVec2(iChildDoubleSizeX, iChildDoubleSizeY));

	ImGui::Spacing( );
	ImGui::Spacing( );

	ImGui::Checkbox(_S("Auto jump"), g_Settings->m_bBunnyHop);
	ImGui::Checkbox(_S("Auto strafe"), g_Settings->m_bAutoStrafe);
	ImGui::Checkbox(_S("WASD strafe"), g_Settings->m_bWASDStrafe);
	ImGui::Checkbox(_S("Start speed"), g_Settings->m_bSpeedBoost);
	ImGui::Checkbox(_S("Quick stop"), g_Settings->m_bFastStop);
	ImGui::Checkbox(_S("Edge jump"), g_Settings->m_bEdgeJump);
	ImGui::Checkbox(_S("Infinity duck"), g_Settings->m_bInfinityDuck);
	ImGui::EndChild();

	ImGui::SetNextWindowPos(ImVec2(iChildPosSecondX, iChildPosFirstY));
	ImGui::BeginChild(_S("Other"), ImVec2(iChildDoubleSizeX, iChildDoubleSizeY));

	ImGui::Spacing( );
	ImGui::Spacing( );

	ImGui::Checkbox(_S("Anti Untrusted"), g_Settings->m_bAntiUntrusted);

	std::vector < std::string > aLogItems = { _S( "Hurt" ), _S( "Harm" ), _S( "Purchase" ), _S( "Bomb" ), _S( "Miss") };
	if (ImGui::BeginCombo(_S("Event logs"), _S("Select"), ImGuiComboFlags_HeightSmall, aLogItems.size()))
	{
		ImGui::Selectable(aLogItems[0].c_str(), g_Settings->m_bLogHurts.GetPtr(), ImGuiSelectableFlags_DontClosePopups);
		ImGui::Selectable(aLogItems[1].c_str(), g_Settings->m_bLogHarms.GetPtr(), ImGuiSelectableFlags_DontClosePopups);
		ImGui::Selectable(aLogItems[2].c_str(), g_Settings->m_bLogPurchases.GetPtr(), ImGuiSelectableFlags_DontClosePopups);
		ImGui::Selectable(aLogItems[3].c_str(), g_Settings->m_bLogBomb.GetPtr(), ImGuiSelectableFlags_DontClosePopups);
		ImGui::Selectable(aLogItems[4].c_str(), g_Settings->m_bLogMisses.GetPtr(), ImGuiSelectableFlags_DontClosePopups);

		ImGui::EndCombo();
	}

	ImGui::Checkbox( _S( "Filter server ads" ), g_Settings->m_bAdBlock );
	ImGui::Checkbox( _S( "Filter console" ), g_Settings->m_bFilterConsole );
	ImGui::Checkbox( _S( "Unlock convars" ), g_Settings->m_bUnhideConvars );
	ImGui::Checkbox( _S( "Reveal ranks" ), g_Settings->m_bRevealRanks );
	ImGui::Checkbox( _S( "Unlock inventory" ), g_Settings->m_bUnlockInventoryAccess );

	ImGui::EndChild();

	ImGui::SetNextWindowPos(ImVec2(iChildPosSecondX, iChildPosSecondY));
	ImGui::BeginChild(_S("BuyBot"), ImVec2(iChildDoubleSizeX, iChildDoubleSizeY));
	
	ImGui::Spacing( );
	ImGui::Spacing( );

	ImGui::Checkbox(_S("Enable BuyBot"), g_Settings->m_bBuyBotEnabled);

	std::vector < std::string > aEquipment
		=
	{
	_S("Fire grenade/Molotov"),
	_S("Smoke grenade"),
	_S("Flash grenade"),
	_S("Explosive grenade"),
	_S("Taser"),
	_S("Heavy armor"),
	_S("Helmet"),
	_S("Defuser")
	};

	ImGui::SingleSelect
	(
		_S("Primary"),
		g_Settings->m_BuyBotPrimaryWeapon,
		{
			_S("None"),
			_S("SCAR20/G3SG1"),
			_S("Scout"),
			_S("AWP"),
			_S("M4A1/AK47")
		}
	);
	ImGui::SingleSelect(
		_S("Secondary"),
		g_Settings->m_BuyBotSecondaryWeapon,
		{
			_S("None"),
			_S("FN57/TEC9"),
			_S("Dual elites"),
			_S("Deagle/Revolver"),
			_S("P2000/Glock-18"),
			_S("P250")
		}
	);

	if (ImGui::BeginCombo(_S("Equipment"), _S("Select"), 0, 8))
	{
		for (int i = 0; i < aEquipment.size(); i++)
			ImGui::Selectable(aEquipment[i].c_str(), &g_Settings->m_aEquipment[i], ImGuiSelectableFlags_DontClosePopups);

		ImGui::EndCombo();
	}

	ImGui::Checkbox(_S("Don't override AWP"), g_Settings->m_bBuyBotKeepAWP);

	ImGui::EndChild();
}

void C_Menu::DrawWorldTab( )
{
	ImVec2 vecWindowPosition = ImGui::GetWindowPos();

	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
	int iMainTextSize = ImGui::CalcTextSize(this->GetMenuName()).x;
	int iChildDoubleSizeX = (800 - ImGui::CalcTextSize(this->GetMenuName()).x - 85) / 2;
	int iChildDoubleSizeY = (565 - 45) / 2;

	int iChildSizeX = (800 - ImGui::CalcTextSize(this->GetMenuName()).x - 40 - 30);
	int iChildSizeY = (565 - 30);

	int iChildPosFirstX = vecWindowPosition.x + iMainTextSize + 15 + 40;
	int iChildPosSecondX = vecWindowPosition.x + iMainTextSize + 30 + iChildDoubleSizeX + 40;

	int iChildPosFirstY = vecWindowPosition.y + 15;
	int iChildPosSecondY = vecWindowPosition.y + 15 + iChildDoubleSizeY + 15;
	ImGui::PopFont();

	ImGui::SetNextWindowPos(ImVec2(iChildPosFirstX, iChildPosFirstY));
	ImGui::BeginChild(_S("View settings"), ImVec2(iChildDoubleSizeX, iChildDoubleSizeY));
	ImGui::Spacing( );
	ImGui::Spacing( );
	ImGui::Text( _S( "Thirdperson" ) );
	ImGui::Keybind( _S( "ThirdPerson Bind" ), &g_Settings->m_aThirdPerson->m_iKeySelected, &g_Settings->m_aThirdPerson->m_iModeSelected );
	ImGui::SliderInt( _S( "Distance" ), g_Settings->m_iThirdPersonDistance, 50, 300 );
	ImGui::SliderInt( _S( "Camera distance" ), g_Settings->m_iCameraDistance, 90, 140 );
	ImGui::Checkbox( _S( "Force distance while scoped" ), g_Settings->m_bOverrideFOVWhileScoped );
	ImGui::SliderFloat( _S( "Aspect ratio" ), g_Settings->m_flAspectRatio, 0.01f, 3.0f );

	ImGui::EndChild();

	ImGui::SetNextWindowPos(ImVec2(iChildPosFirstX, iChildPosSecondY ) );
	ImGui::BeginChild (_S( "Viewmodel" ), ImVec2( iChildDoubleSizeX, iChildDoubleSizeY ) );
	ImGui::Spacing( );
	ImGui::Spacing( );
	ImGui::SliderInt( _S( "Viewmodel distance" ), g_Settings->m_iViewmodelDistance, 60, 140 );
	ImGui::SliderInt( _S( "Viewmodel X axis" ), g_Settings->m_iViewmodelX, -10, 10 );
	ImGui::SliderInt( _S( "Viewmodel Y axis" ), g_Settings->m_iViewmodelY, -10, 10 );
	ImGui::SliderInt( _S( "Viewmodel Z axis" ), g_Settings->m_iViewmodelZ, -10, 10 );
	ImGui::SliderInt( _S( "Viewmodel roll" ), g_Settings->m_iViewmodelRoll, -90, 90 );

	ImGui::EndChild();

	ImGui::SetNextWindowPos(ImVec2(iChildPosSecondX, iChildPosFirstY ) );
	ImGui::BeginChild (_S( "World modulation" ), ImVec2( iChildDoubleSizeX, iChildDoubleSizeY ) );
	ImGui::Spacing( );
	ImGui::Spacing( );

	std::vector < const char* > aSkyboxList =
	{
		_S( "None" ),
		_S( "Tibet" ),
		_S( "Baggage" ),
		_S( "Italy" ),
		_S( "Aztec" ),
		_S( "Vertigo" ),
		_S( "Daylight" ),
		_S( "Daylight 2" ),
		_S( "Clouds" ),
		_S( "Clouds 2" ),
		_S( "Gray" ),
		_S( "Clear" ),
		_S( "Canals" ),
		_S( "Cobblestone" ),
		_S( "Assault" ),
		_S( "Clouds dark" ),
		_S( "Night" ),
		_S( "Night 2" ),
		_S( "Night flat" ),
		_S( "Dusty" ),
		_S( "Rainy" ),
		_S( "Custom" )
	};

	ImGui::Checkbox( _S( "Hold fire animation" ), g_Settings->m_bHoldFireAnimation );
	ImGui::Checkbox( _S( "Preserve killfeed" ), g_Settings->m_bPreserveKillfeed );
	ImGui::SingleSelect( _S( "Skybox Changer" ), g_Settings->m_iSkybox.GetPtr( ), aSkyboxList );
	if ( g_Settings->m_iSkybox == aSkyboxList.size( ) - 1 )
	{
		static char aSkyBox[ 32 ];
		if ( !g_Settings->m_szCustomSkybox->empty( ) )
			strcpy( aSkyBox, g_Settings->m_szCustomSkybox.Get( ).c_str( ) );

		if ( ImGui::InputText( _S( "##324234124" ), aSkyBox, 32 ) )
			g_Settings->m_szCustomSkybox.Get( ) = aSkyBox;
	}

	ImGui::Text( _S( "World color" ) );
	this->DrawColorEdit4( _S( "##123123" ), g_Settings->m_WorldModulation );

	ImGui::Text( _S( "Props color" ) );
	this->DrawColorEdit4( _S( "##11233" ), g_Settings->m_PropModulation );

	ImGui::Text( _S( "Skybox color" ) );
	this->DrawColorEdit4( _S( "##51223" ), g_Settings->m_SkyModulation );
	
	ImGui::Spacing( );

	ImGui::Checkbox( _S( "Force crosshair" ), g_Settings->m_bForceCrosshair );
	ImGui::Checkbox( _S( "Hitmarker" ), g_Settings->m_bHitMarker );
	ImGui::Checkbox( _S( "Hitsound" ), g_Settings->m_bHitSound );
	ImGui::Checkbox( _S( "Damage marker" ), g_Settings->m_bDamageMarker);

	ImGui::Checkbox( _S( "Client bullet impacts" ), g_Settings->m_bDrawClientImpacts );
	this->DrawColorEdit4( _S( "##41242354" ), g_Settings->m_ClientImpacts );
	ImGui::Checkbox( _S( "Server bullet impacts" ), g_Settings->m_bDrawServerImpacts );
	this->DrawColorEdit4( _S( "##412423154" ), g_Settings->m_ServerImpacts );

	ImGui::Checkbox( _S( "Local bullet tracers" ), g_Settings->m_bDrawLocalTracers );
	this->DrawColorEdit4( _S( "##43242354" ), g_Settings->m_LocalTracers );
	ImGui::Checkbox( _S( "Enemy bullet tracers" ), g_Settings->m_bDrawEnemyTracers );
	this->DrawColorEdit4( _S( "##432423154" ), g_Settings->m_EnemyTracers );

	std::vector < std::string > aWorldRemovals =
	{
		_S( "Visual punch" ),
		_S( "Visual kick" ),
		_S( "Scope" ),
		_S( "Smoke" ),
		_S( "Flash" ),
		_S( "Post process" ),
		_S( "World FOG" ),
		_S( "Shadows" ),
		_S( "Landing bob" ),
		_S( "Hand shaking" )
	};

	ImGui::Checkbox( _S( "Grenade prediction" ), g_Settings->m_bPredictGrenades );
	this->DrawColorEdit4( _S( "##1234142124" ), g_Settings->m_GrenadeWarning );

	ImGui::Checkbox( _S( "Grenade timers" ), g_Settings->m_GrenadeTimers );
	this->DrawColorEdit4( _S( "##1234145151" ), g_Settings->m_GrenadeWarningTimer );

	if (ImGui::BeginCombo(_S("World removals"), _S("Select"), 0, 8))
	{
		for (int i = 0; i < aWorldRemovals.size(); i++)
			ImGui::Selectable(aWorldRemovals[i].c_str(), &g_Settings->m_aWorldRemovals[i], ImGuiSelectableFlags_DontClosePopups);

		ImGui::EndCombo();
	}

	ImGui::EndChild( );

	ImGui::SetNextWindowPos(ImVec2(iChildPosSecondX, iChildPosSecondY ) );
	ImGui::BeginChild( _S( "Weather" ), ImVec2( iChildDoubleSizeX, iChildDoubleSizeY ) );

	ImGui::Spacing( );
	ImGui::Spacing( );

	ImGui::Checkbox( _S( "Enable weather" ), g_Settings->m_CustomWeather );

	ImGui::SliderInt( _S( "Rain length" ), g_Settings->m_RainLength, 1, 100 );
	ImGui::SliderInt( _S( "Rain width" ), g_Settings->m_RainWidth, 1, 100 );
	ImGui::SliderInt( _S( "Rain speed" ), g_Settings->m_RainSpeed, 1, 2000 );

	ImGui::EndChild( );
}

void C_Menu::DrawConfigTab( )
{	
	ImVec2 vecWindowPosition = ImGui::GetWindowPos();

	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
	int iMainTextSize = ImGui::CalcTextSize(this->GetMenuName()).x;
	int iChildDoubleSizeX = (800 - ImGui::CalcTextSize(this->GetMenuName()).x - 85) / 2;
	int iChildDoubleSizeY = (565 - 45) / 2;

	int iChildPosFirstX = vecWindowPosition.x + iMainTextSize + 15 + 40;
	int iChildPosSecondX = vecWindowPosition.x + iMainTextSize + 30 + iChildDoubleSizeX + 40;

	int iChildPosFirstY = vecWindowPosition.y + 15;
	int iChildPosSecondY = vecWindowPosition.y + 15 + iChildDoubleSizeY + 15;

	ImGui::PopFont();

	ImGui::SetNextWindowPos(ImVec2(iChildPosFirstX, iChildPosFirstY));
	ImGui::BeginChild("Main", ImVec2(iChildDoubleSizeX, iChildDoubleSizeY));

	if ( ImGui::Button( _S( "Save config" ) ) )
		g_ConfigSystem->SaveConfig( );

	if ( ImGui::Button( _S( "Load config" ) ) )
		g_ConfigSystem->LoadConfig( );
	
	ImGui::EndChild( );
}

void C_Menu::DrawColorEdit4( const char* strLabel, Color* aColor )
{
	 float aColour[ 4 ] =
		{
            aColor->r( ) / 255.0f,
			aColor->g( ) / 255.0f,
			aColor->b( ) / 255.0f,
			aColor->a( ) / 255.0f
        };

	 if ( ImGui::ColorEdit4( strLabel, aColour, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_RGB ) )
		 aColor->SetColor( aColour[ 0 ], aColour[ 1 ], aColour[ 2 ], aColour[ 3 ] );
}

void C_Menu::Initialize()
{
	ImGui::GetStyle().Colors[ImGuiCol_ScrollbarBg] = ImVec4(45 / 255.f, 45 / 255.f, 45 / 255.f, 1.f);
	ImGui::GetStyle().Colors[ImGuiCol_ScrollbarGrab] = ImVec4(65 / 255.f, 65 / 255.f, 65 / 255.f, 1.f);
	ImGui::GetStyle().AntiAliasedFill = true;
	ImGui::GetStyle().AntiAliasedLines = true;
	ImGui::GetStyle().ScrollbarSize = 6;

	D3DXCreateTextureFromFileInMemoryEx(g_Globals.m_Interfaces.m_DirectDevice, &avatarka, sizeof(avatarka), 512, 512, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &g_Menu->m_dTexture);
}

void C_Menu::WaterMark( )
{

}

#include "Tools/Tools.hpp"
#define PUSH_BIND( m_Variable, Name )\
if ( g_Tools->IsBindActive( m_Variable ) )\
{\
	if ( m_BindList[ FNV32( #m_Variable ) ].m_flAlphaPercent == 0.0f )\
		m_BindList[ FNV32( #m_Variable ) ].m_szName = _S( Name );\
	m_BindList[ FNV32( #m_Variable ) ].m_flAlphaPercent = std::clamp( m_BindList[ FNV32( #m_Variable ) ].m_flAlphaPercent + ImGui::GetIO( ).DeltaTime * 10.0f, 0.0f, 1.0f );\
}\
else\
{\
	if ( m_BindList[ FNV32( #m_Variable ) ].m_flAlphaPercent == 0.0f )\
		m_BindList[ FNV32( #m_Variable ) ].m_szName = "";\
	m_BindList[ FNV32( #m_Variable ) ].m_flAlphaPercent = std::clamp( m_BindList[ FNV32( #m_Variable ) ].m_flAlphaPercent - ImGui::GetIO( ).DeltaTime * 10.0f, 0.0f, 1.0f );\
}\

void C_Menu::DrawKeybindList( )
{
	int m_Last = 0;
	PUSH_BIND( g_Settings->m_aFakeDuck, "Fake Duck" );
	PUSH_BIND( g_Settings->m_aDoubleTap, "Double tap" );
	PUSH_BIND( g_Settings->m_aSlowwalk, "Slow walk" );
	PUSH_BIND( g_Settings->m_aHideShots, "Hide shots" );
	PUSH_BIND( g_Settings->m_aSafePoint, "Safe points" );
	PUSH_BIND( g_Settings->m_aInverter, "Invert side" ); 
	PUSH_BIND( g_Settings->m_aAutoPeek, "Auto peek" );
	PUSH_BIND( g_Settings->m_aMinDamage, "Damage override" );

	int32_t iCount = 0;
	for ( auto& Bind : m_BindList )
	{
		if ( Bind.second.m_szName.length( ) )
			iCount++;
	}

	if ( iCount <= 0 )
		return;

	int nAdvancedFlag = 0;
	if ( !m_bIsMenuOpened )
		nAdvancedFlag = ImGuiWindowFlags_NoMove;
	
	ImGui::SetNextWindowSize( ImVec2( 190, m_BindList.empty( ) ? 0 : 35 + ( 21.5f * iCount ) ) );
	ImGui::DefaultBegin( _S( "Keybind List" ), g_Settings->m_bDrawKeyBindList, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | nAdvancedFlag );
	{
		int32_t x = ImGui::GetCurrentWindow( )->Pos.x + 4.5f;
		int32_t y = ImGui::GetCurrentWindow( )->Pos.y;

		ImGui::GetOverlayDrawList( )->AddRectFilled( ImVec2( x, y ), ImVec2( x + 181, y + 22 ), ImColor( 24.0f / 255.0f, 31.0f / 255.0f, 44.0f / 255.0f, 1.0f ) );
		ImGui::GetOverlayDrawList( )->AddRectFilled( ImVec2( x, y + 22 ), ImVec2( x + 181, y + 22.27f ), ImColor( 0.0f, 115.0f / 255.0f, 222.0f / 255.0f, 1.0f ) );
		ImGui::GetOverlayDrawList( )->AddRectFilled( ImVec2( x, y + 22.27f ), ImVec2( x + 181, y + 22.30f ), ImColor( 0.0f, 115.0f / 255.0f, 222.0f / 255.0f, 0.65f ) );

		ImGui::PushFont( g_Globals.m_Fonts.m_MenuIcons );
		ImGui::GetOverlayDrawList( )->AddText( ImVec2( x + 5, y + 2 ), ImColor( 71, 163, 255 ), _S( "a" ) );
		ImGui::PopFont( );

		ImGui::PushFont( g_Globals.m_Fonts.m_LogFont );
		ImGui::GetOverlayDrawList( )->AddText( ImVec2( x + 26, y + 2 ), ImColor( 255, 255, 255 ), _S( "Keybind list" ) );
		ImGui::PopFont( );

		for ( auto& Bind : m_BindList )
		{
			if ( !Bind.second.m_szName.length( ) )
				continue;
		
			ImGui::PushFont( g_Globals.m_Fonts.m_LogFont );
			ImGui::GetOverlayDrawList( )->AddText( ImVec2( x + 2, 27 + ( y + 16 * m_Last ) ), ImColor( 255, 255, 255, static_cast < int >( Bind.second.m_flAlphaPercent * 255.0f ) ), Bind.second.m_szName.c_str( ) );
			ImGui::GetOverlayDrawList( )->AddText( ImVec2( x + 160, 27 + ( y + 16 * m_Last ) ), ImColor( 255, 255, 255, static_cast < int >( Bind.second.m_flAlphaPercent * 255.0f ) ), _S( "On" ) );
			ImGui::PopFont( );

			m_Last++;
		}
		
	}
	ImGui::DefaultEnd( );
}