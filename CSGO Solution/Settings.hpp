#pragma once
#include <array>
#include "Tools/Obfuscation/XorStr.hpp"
#include "Config.hpp"

struct C_KeyBind
{
	int32_t m_iModeSelected = 0;
	int32_t m_iKeySelected = 0;
};

enum REMOVALS
{
	REMOVALS_VISUAL_PUNCH = 0,
	REMOVALS_VISUAL_KICK = 1,
	REMOVALS_VISUAL_SCOPE = 2,
	REMOVALS_VISUAL_SMOKE = 3,
	REMOVALS_VISUAL_FLASH = 4,
	REMOVALS_VISUAL_POSTPROCESS = 5,
	REMOVALS_VISUAL_FOG = 6,
	REMOVALS_VISUAL_SHADOWS = 7,
	REMOVALS_VISUAL_LANDING_BOB = 8,
	REMOVALS_VISUAL_HAND_SHAKING = 9
};

enum AUTOSTOP
{
	AUTOSTOP_ACCURACY,
	AUTOSTOP_EARLY
};

enum AIM_POINTS
{
	PREFER_SAFE,
	FORCE_SAFE,
	PREFER_BODY,
	FORCE_BODY
};

enum DOUBLETAP_OPTIONS
{
	MOVE_BETWEEN_SHOTS,
	FULL_STOP
};

enum FAKELAG_TRIGGERS
{
	FAKELAG_MOVE,
	FAKELAG_AIR,
	FAKELAG_PEEK
};

// thanks @es3n1n
template < typename T >
class C_CheatVar 
{
private:
	uint32_t m_VarValue;
public:
	C_CheatVar( std::string v1, std::string v2, T v3 ) 
	{
		m_VarValue = g_ConfigSystem->PushItem< T >( v1, v2, v3 );
	}

	T& Get( )
	{
		return *g_ConfigSystem->GetConfigValue< T >( m_VarValue );
	}

	T* GetPtr( )
	{
		return g_ConfigSystem->GetConfigValue< T >( m_VarValue );
	}
	
	operator T&( )
	{
		return Get( );
	}
	operator T* ( )
	{
		return GetPtr();
	}
	T* operator->( )
	{
		return GetPtr();
	}
	operator T&( ) const
	{
		return Get();
	}
};

struct C_ChamsSettings
{
	int32_t m_iMainMaterial = 0;

	bool m_bRenderChams = false;
	bool m_aModifiers[ 4 ] = { false, false, false };

	Color m_aModifiersColors[ 4 ] = { { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 } };
	Color m_Color = Color( 255, 255, 255, 255 );
};

struct C_PlayerSettings
{
	bool m_BoundaryBox = false;
	bool m_RenderName = false;
	bool m_RenderHealthBar = false;
	bool m_RenderHealthText = false;
	bool m_RenderAmmoBar = false;
	bool m_RenderAmmoBarText = false;
	bool m_RenderWeaponText = false;
	bool m_RenderWeaponIcon = false;

	bool m_bRenderGlow = false;
	int m_iGlowStyle = 0;
	Color m_aGlow = Color( 255, 255, 255, 255 );

	Color m_aBoundaryBox = Color( 255, 255, 255, 255 );
	Color m_aNameColor = Color( 255, 255, 255, 255 );
	Color m_aHealthBar = Color( 255, 255, 255, 255 );
	Color m_aHealthText = Color( 255, 255, 255, 255 );
	Color m_aAmmoBar = Color( 255, 255, 255, 255 );
	Color m_aAmmoBarText = Color( 255, 255, 255, 255 );
	Color m_aArmorBar = Color( 255, 255, 255, 255 );
	Color m_aArmorBarText = Color( 255, 255, 255, 255 );
	Color m_aWeaponText = Color( 255, 255, 255, 255 );
	Color m_aWeaponIcon = Color( 255, 255, 255, 255 );
};

struct C_RageSettings
{
	int32_t m_iFOV = 180;
	int32_t m_iHitChance = 0;
	int32_t m_iDoubleTapHitChance = 0;
	
	int32_t m_iMinDamage = 0;
	int32_t m_iMinDamageOverride = 0;
	int32_t m_iAccuracyBoost = 0;

	int32_t m_iHeadScale = 50;
	int32_t m_iBodyScale = 50;

	bool m_bEnabled = false;
	bool m_bLethalSafety = false;

	bool m_AutoStopOptions[ 2 ] = { false, false }; // Force Accuracy, Early
	bool m_DoubleTapOptions[ 2 ] = { false, false }; // Move between shots, stop
	bool m_Hitboxes[ 6 ] = { true, true, true, true, true, true }; // head, chest, arms, pelvis, stomach, legs
	bool m_SafeHitboxes[ 6 ] = { }; // head, chest, arms, pelvis, stomach, legs
	bool m_Multipoints[ 6 ] = { }; // head, chest, arms, pelvis, stomach, legs

	bool m_bAutoStop = false;
	bool m_bAutoScope = false;

	bool m_bPreferSafe = false;
	bool m_bPreferBody = false;
};

#define SETTING( type, var, val ) C_CheatVar< type > var = C_CheatVar< type >( _S( #var ), _S( #type ), val )
class C_Settings
{
public:
	SETTING( bool, m_bBunnyHop, false );
	SETTING( bool, m_bAntiUntrusted, true );
	SETTING( bool, m_bAutoStrafe, false );
	SETTING( bool, m_bWASDStrafe, false );
	SETTING( bool, m_bSpeedBoost, false );
	SETTING( bool, m_bEdgeJump, false );
	SETTING( bool, m_bFilterConsole, false );
	SETTING( bool, m_bUnhideConvars, false );
	SETTING( bool, m_bRevealRanks, false );
	SETTING( bool, m_bAdBlock, false );
	SETTING( bool, m_bInfinityDuck, false );
	SETTING( bool, m_bFastStop, false );
	SETTING( int, m_AdvancedExploit, 0 );
	SETTING( bool, m_bHitSound, false );
	SETTING( bool, m_bDrawKeyBindList, false );
	SETTING( bool, m_bUnlockInventoryAccess, false );
	SETTING( bool, m_bOutOfViewArrows, false );
	SETTING( float, m_flAspectRatio, 1.77f );

	SETTING( bool, m_CustomWeather, false );
	SETTING( int, m_WeatherType, 0 );
	SETTING( int, m_RainDensity, 100 );
	SETTING( int, m_RainLength, 50 );
	SETTING( int, m_RainWidth, 50 );
	SETTING( int, m_RainSpeed, 600 );

	SETTING( bool, m_bForceCrosshair, false );
	SETTING( bool, m_bHitMarker, false );
	SETTING( bool, m_bDamageMarker, false );

	SETTING( bool, m_bPredictGrenades, false );
	SETTING( bool, m_GrenadeTimers, false );

	SETTING( bool, m_bHoldFireAnimation, false );
	SETTING( bool, m_bPreserveKillfeed, false );

	SETTING( bool, m_bFakeLagEnabled, false );
	SETTING( int32_t, m_iLagLimit, 0 );
	SETTING( int32_t, m_iTriggerLimit, 0 );
	
	SETTING( bool, m_bAntiAim, false );
	SETTING( bool, m_bAntiAimAtTargets, false );
	SETTING( int, m_iLeftFakeLimit, 0 );
	SETTING( int, m_iRightFakeLimit, 0 );
	SETTING( int, m_iJitterAmount, 0 );
	SETTING( int, m_iYawAddOffset, 0 );
	SETTING( int, m_iPitchMode, 1 );
	SETTING( int, m_iLegMovement, 0 );
	SETTING( bool, m_bAutoDirection, false );
	SETTING( bool, m_bJitterMove, false );

	SETTING( bool, m_bDrawServerImpacts, false );
	SETTING( bool, m_bDrawClientImpacts, false );
	SETTING( bool, m_bDrawLocalTracers, false );
	SETTING( bool, m_bDrawEnemyTracers, false );
	SETTING( bool, m_bDrawRagdolls, false );

	SETTING( bool, m_bOverrideFOVWhileScoped, false );
	SETTING( int32_t, m_iThirdPersonDistance, 90 );
	SETTING( int32_t, m_iCameraDistance, 90 );
	SETTING( int32_t, m_iViewmodelDistance, 60 );
	SETTING( int32_t, m_iViewmodelX, 1 ); 
	SETTING( int32_t, m_iViewmodelY, 1 );
	SETTING( int32_t, m_iViewmodelZ, -1 );
	SETTING( int32_t, m_iViewmodelRoll, 0 );
	SETTING( int32_t, m_iSkybox, 0 );
	SETTING( float_t, m_flRechargeTime, 0.5f );
	SETTING( int32_t, m_VisualInterpolation, 14 );
	SETTING( std::string, m_szCustomSkybox, "" );
	SETTING( std::string, m_szCustomFont, "" );

	SETTING( bool, m_bBuyBotEnabled, false );
	SETTING( bool, m_bBuyBotKeepAWP, false );
	SETTING( int, m_BuyBotPrimaryWeapon, 0 );
	SETTING( int, m_BuyBotSecondaryWeapon, 0 );

	SETTING( bool, m_bLogPurchases, false );
	SETTING( bool, m_bLogBomb, false );
	SETTING( bool, m_bLogHurts, false );
	SETTING( bool, m_bLogHarms, false );
	SETTING( bool, m_bLogMisses, false );

	SETTING( bool, m_bRenderC4Glow, false );
	SETTING( int, m_iC4GlowStyle, 0 );

	SETTING( bool, m_bRenderDroppedWeaponGlow, false );
	SETTING( int, m_iDroppedWeaponGlowStyle, 0 );

	SETTING( bool, m_bRenderProjectileGlow, false );
	SETTING( int, m_iProjectileGlowStyle, 0 );

	SETTING( Color, m_aProjectileGlow, Color( 255, 255, 255, 255 ) );
	SETTING( Color, m_WorldModulation, Color( 255, 255, 255, 255 ) );
	SETTING( Color, m_PropModulation, Color( 255, 255, 255, 255 ) );
	SETTING( Color, m_SkyModulation, Color( 255, 255, 255, 255 ) );
	SETTING( Color, m_ClientImpacts, Color( 255, 255, 255, 255 ) );
	SETTING( Color, m_ServerImpacts, Color( 255, 255, 255, 255 ) );
	SETTING( Color, m_LocalTracers, Color( 255, 255, 255, 255 ) );
	SETTING( Color, m_EnemyTracers, Color( 255, 255, 255, 255 ) );
	SETTING( Color, m_GrenadeWarning, Color( 255, 255, 255, 255 ) );
	SETTING( Color, m_GrenadeWarningTimer, Color( 255, 255, 255, 255 ) );
	SETTING( Color, m_aC4Glow, Color( 255, 255, 255, 255 ) );
	SETTING( Color, m_aDroppedWeaponGlow, Color( 255, 255, 255, 255 ) );
	SETTING( Color, m_aOutOfViewArrows, Color( 255, 255, 255, 255 ) );

	SETTING( C_KeyBind, m_aSlowwalk, C_KeyBind( ) );
	SETTING( C_KeyBind, m_aThirdPerson, C_KeyBind( ) );
	SETTING( C_KeyBind, m_aAutoPeek, C_KeyBind( ) );
	SETTING( C_KeyBind, m_aFakeDuck, C_KeyBind( ) );
	SETTING( C_KeyBind, m_aInverter, C_KeyBind( ) );
	SETTING( C_KeyBind, m_aMinDamage, C_KeyBind( ) );
	SETTING( C_KeyBind, m_aSafePoint, C_KeyBind( ) );

	SETTING( C_KeyBind, m_aDoubleTap, C_KeyBind( ) );
	SETTING( C_KeyBind, m_aHideShots, C_KeyBind( ) );

	SETTING( C_KeyBind, m_aManualLeft, C_KeyBind( ) );
	SETTING( C_KeyBind, m_aManualBack, C_KeyBind( ) );
	SETTING( C_KeyBind, m_aManualRight, C_KeyBind( ) );

	// 0 - enemy
	// 1 - teammate
	// 2 - localplayer
	SETTING( C_PlayerSettings, m_LocalPlayer, C_PlayerSettings( ) );
	SETTING( C_PlayerSettings, m_Teammates, C_PlayerSettings( ) );
	SETTING( C_PlayerSettings, m_Enemies, C_PlayerSettings( ) );

	// 0 - enemy
	// 1 - enemy invisible
	// 2 - backtrack 
	// 3 - shot chams
	// 4 - team
	// 5 - team invis
	// 6 - local
	// 7 - desync
	// 8 - lag
	// 9 - hands
	std::array < C_ChamsSettings, 10 > m_aChamsSettings = { };

	// 0 - punch
	// 1 - kick
	// 2 - scope
	// 3 - smoke
	// 4 - flash
	// 5 - postprocessing
	// 6 - fog
	// 7 - shadows
	// 8 - landing bob
	// 9 - hand shaking
	std::array < bool, 10 > m_aWorldRemovals = { };

	// inverter conditions
	// 0 - Always
	// 1 - Air
	// 2 - Move
	std::array < bool, 3 > m_aInverterConditions = { };

	// fakelag triggers
	// 0 - Move
	// 1 - Air
	// 2 - Peek
	std::array < bool, 3 > m_aFakelagTriggers = { };

	// rage settings
	// 0 - Autosniper
	// 1 - Scout
	// 2 - AWP
	// 3 - Deagle
	// 4 - Revolver
	// 5 - Pistols
	// 6 - Rifles
	std::array < C_RageSettings, 7 > m_aRageSettings = { };

	// equipment
	// 0 - Fire/Molotov
	// 1 - Smoke grenade
	// 2 - Flash grenade
	// 3 - HE grenade
	// 4 - Taser
	// 5 - Heavy armor
	// 6 - Helmet
	// 7 - Defuser
	std::array < bool, 7 > m_aEquipment = { };
};

inline C_Settings* g_Settings = new C_Settings( );