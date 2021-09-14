#pragma once
#include <cstdint>

class C_CSWeaponData
{
public:
	void* pVTable;                       // 0x0000
	char m_strConsoleName;                   // 0x0004
	char pad_0008[12];                   // 0x0008
	int m_iMaxClip1;                       // 0x0014
	int m_iMaxClip2;                       // 0x0018
	int m_iDefaultClip1;                   // 0x001C
	int m_iDefaultClip2;                   // 0x0020
	char pad_0024[8];                    // 0x0024
	char* m_szWorldModel;                  // 0x002C
	char* m_szViewModel;                   // 0x0030
	char* m_szDroppedMode;                 // 0x0034
	char pad_0038[4];                    // 0x0038
	char* m_szShotSound;                   // 0x003C
	char pad_0040[56];                   // 0x0040
	char* m_szEmptySound;                  // 0x0078
	char pad_007C[4];                    // 0x007C
	char* m_szBulletType;                  // 0x0080
	char pad_0084[4];                    // 0x0084
	char* m_szHudName;                     // 0x0088
	char* m_szWeaponName;                  // 0x008C
	char pad_0090[56];                   // 0x0090
	int m_iWeaponType;            // 0x00C8
	char pad_00CC[4];                    // 0x00CC
	int m_iWeaponPrice;                    // 0x00D0
	int m_iKillAward;                      // 0x00D4
	char* m_szAnimationPrefix;             // 0x00D8
	float m_flCycleTime;                   // 0x00DC
	float m_flCycleTimeAlt;                // 0x00E0
	float m_flTimeToIdle;                  // 0x00E4
	float m_flIdleInterval;                // 0x00E8
	bool m_bFullAuto;                      // 0x00EC
	char pad_00ED[3];                    // 0x00ED
	int m_iDamage;                         // 0x00F0
	float m_flArmorRatio;                  // 0x00F4
	int m_iBullets;                        // 0x00F8
	float m_flPenetration;                 // 0x00FC
	float m_flFlinchVelocityModifierLarge; // 0x0100
	float m_flFlinchVelocityModifierSmall; // 0x0104
	float m_flRange;                       // 0x0108
	float m_flRangeModifier;               // 0x010C
	float m_flThrowVelocity;               // 0x0110
	char pad_0114[12];                   // 0x0114
	bool m_bHasSilencer;                   // 0x0120
	char pad_0121[3];                    // 0x0121
	char* m_pSilencerModel;                // 0x0124
	int m_iCrosshairMinDistance;           // 0x0128
	int m_iCrosshairDeltaDistance;         // 0x012C
	float m_flMaxPlayerSpeed;              // 0x0130
	float m_flMaxPlayerSpeedAlt;           // 0x0134
	float m_flMaxPlayerSpeedMod;           // 0x0138
	float m_flSpread;                      // 0x013C
	float m_flSpreadAlt;                   // 0x0140
	float m_flInaccuracyCrouch;            // 0x0144
	float m_flInaccuracyCrouchAlt;         // 0x0148
	float m_flInaccuracyStand;             // 0x014C
	float m_flInaccuracyStandAlt;          // 0x0150
	float m_flInaccuracyJumpInitial;       // 0x0154
	float m_flInaccuracyJump;              // 0x0158
	float m_flInaccuracyJumpAlt;           // 0x015C
	float m_flInaccuracyLand;              // 0x0160
	float m_flInaccuracyLandAlt;           // 0x0164
	float m_flInaccuracyLadder;            // 0x0168
	float m_flInaccuracyLadderAlt;         // 0x016C
	float m_flInaccuracyFire;              // 0x0170
	float m_flInaccuracyFireAlt;           // 0x0174
	float m_flInaccuracyMove;              // 0x0178
	float m_flInaccuracyMoveAlt;           // 0x017C
	float m_flInaccuracyReload;            // 0x0180
	int m_iRecoilSeed;                     // 0x0184
	float m_flRecoilAngle;                 // 0x0188
	float m_flRecoilAngleAlt;              // 0x018C
	float m_flRecoilAngleVariance;         // 0x0190
	float m_flRecoilAngleVarianceAlt;      // 0x0194
	float m_flRecoilMagnitude;             // 0x0198
	float m_flRecoilMagnitudeAlt;          // 0x019C
	float m_flRecoilMagnitudeVariance;     // 0x01A0
	float m_flRecoilMagnitudeVarianceAlt;  // 0x01A4
	float m_flRecoveryTimeCrouch;          // 0x01A8
	float m_flRecoveryTimeStand;           // 0x01AC
	float m_flRecoveryTimeCrouchFinal;     // 0x01B0
	float m_flRecoveryTimeStandFinal;      // 0x01B4
	char pad_01B8[40];                   // 0x01B8
	char* m_szWeaponClass;                 // 0x01E0
	char pad_01E4[8];                    // 0x01E4
	char* m_szEjectBrassEffect;            // 0x01EC
	char* m_szTracerEffect;                // 0x01F0
	int m_iTracerFrequency;                // 0x01F4
	int m_iTracerFrequencyAlt;             // 0x01F8
	char* m_szMuzzleFlashEffect_1stPerson; // 0x01FC
	char pad_0200[4];                    // 0x0200
	char* m_szMuzzleFlashEffect_3rdPerson; // 0x0204
	char pad_0208[4];                    // 0x0208
	char* m_szMuzzleSmokeEffect;           // 0x020C
	float m_flHeatPerShot;                 // 0x0210
	char* m_szZoomInSound;                 // 0x0214
	char* m_szZoomOutSound;                // 0x0218
	float m_flInaccuracyPitchShift;        // 0x021C
	float m_flInaccuracySoundThreshold;    // 0x0220
	float m_flBotAudibleRange;             // 0x0224
	char pad_0228[12];                   // 0x0228
	bool m_bHasBurstMode;                  // 0x0234
	bool m_bIsRevolver;                    // 0x0235
};

class C_BaseAttributableItem : public C_BaseEntity
{
public:
	NETVAR( m_OriginalOwnerXuid, uint64_t, FNV32( "DT_BaseAttributableItem" ), FNV32( "m_OriginalOwnerXuidLow" ) );
	NETVAR( m_OriginalOwnerXuidLow, int32_t, FNV32( "DT_BaseAttributableItem" ), FNV32( "m_OriginalOwnerXuidLow" ) );
	NETVAR( m_OriginalOwnerXuidHigh, int32_t, FNV32( "DT_BaseAttributableItem" ), FNV32( "m_OriginalOwnerXuidHigh" ) );
	NETVAR( m_nFallbackStatTrak, int32_t, FNV32( "DT_BaseAttributableItem" ), FNV32( "m_nFallbackStatTrak" ) );
	NETVAR( m_nFallbackPaintKit, int32_t, FNV32( "DT_BaseAttributableItem" ), FNV32( "m_nFallbackPaintKit" ) );
	NETVAR( m_nFallbackSeed, int32_t, FNV32( "DT_BaseAttributableItem" ), FNV32( "m_nFallbackSeed" ) );
	NETVAR( m_flFallbackWear, float_t, FNV32( "DT_BaseAttributableItem" ), FNV32( "m_flFallbackWear" ) );
	NETVAR( m_bInitialized, bool, FNV32( "DT_BaseAttributableItem" ), FNV32( "m_bInitialized" ) );
	NETVAR( m_iEntityLevel, int32_t, FNV32( "DT_BaseAttributableItem" ), FNV32( "m_iEntityLevel" ) );
	NETVAR( m_iAccountID, int32_t, FNV32( "DT_BaseAttributableItem" ), FNV32( "m_iAccountID" ) );
	NETVAR( m_iItemIDLow, int32_t, FNV32( "DT_BaseAttributableItem" ), FNV32( "m_iItemIDLow" ) );
	NETVAR( m_iItemIDHigh, int32_t, FNV32( "DT_BaseAttributableItem" ), FNV32( "m_iItemIDHigh" ) );
	NETVAR( m_iEntityQuality, int32_t, FNV32( "DT_BaseAttributableItem" ), FNV32( "m_iEntityQuality" ) );
	NETVAR( m_iCustomName, char, FNV32( "DT_BaseAttributableItem" ), FNV32( "m_szCustomName" ) );
};

class C_BaseCombatWeapon : public C_BaseAttributableItem
{
public:
	NETVAR( m_iClip1, int32_t, FNV32( "DT_BaseCombatWeapon" ), FNV32( "m_iClip1" ) );
	NETVAR( m_iClip2, int32_t, FNV32( "DT_BaseCombatWeapon" ), FNV32( "m_iClip2" ) );
	NETVAR( m_flRecoilIndex, float_t, FNV32( "DT_WeaponCSBase" ), FNV32( "m_flRecoilIndex" ) );
	NETVAR( m_fAccuracyPenalty, float_t, FNV32( "DT_WeaponCSBase" ), FNV32( "m_fAccuracyPenalty" ) );
	NETVAR( m_flLastShotTime, float_t, FNV32( "DT_WeaponCSBase" ), FNV32( "m_fLastShotTime" ) );
	NETVAR( m_iViewModelIndex, int32_t, FNV32( "DT_BaseCombatWeapon" ), FNV32( "m_iViewModelIndex" ) );
	NETVAR( m_iWorldModelIndex, int32_t, FNV32( "DT_BaseCombatWeapon" ), FNV32( "m_iWorldModelIndex" ) );
	NETVAR( m_iWorldDroppedModelIndex, int32_t, FNV32( "DT_BaseCombatWeapon" ), FNV32( "m_iWorldDroppedModelIndex" ) );
	NETVAR( m_bPinPulled, bool, FNV32( "DT_BaseCSGrenade" ), FNV32( "m_bPinPulled" ) );
	NETVAR( m_fThrowTime, float_t, FNV32( "DT_BaseCSGrenade" ),  FNV32( "m_fThrowTime" ) );
	NETVAR( m_flThrowStrength, float_t, FNV32( "DT_BaseCSGrenade" ),  FNV32( "m_flThrowStrength" ) );
	NETVAR( m_iIronSightMode, int32_t, FNV32( "DT_WeaponCSBase" ), FNV32( "m_iIronSightMode" ) );
	NETVAR( m_nZoomLevel, int32_t, FNV32( "DT_WeaponCSBase" ), FNV32( "m_zoomLevel" ) );
	NETVAR( m_hWeaponWorldModel, CBaseHandle, FNV32("DT_BaseCombatWeapon"), FNV32("m_hWeaponWorldModel"));

	PushVirtual( UpdateAccuracyPenalty( ), 483, void( __thiscall* )( void* ) );
	PushVirtual( GetWeaponData( ), 460, C_CSWeaponData*( __thiscall* )( void* ) );
	PushVirtual( GetInaccuracy( ), 482, float( __thiscall* )( void* ) );
	PushVirtual( GetSpread( ), 452, float( __thiscall* )( void* ) );

	DATAMAP( int32_t, m_Activity );

	float_t& m_flNextPrimaryAttack( )
	{
		return *( float_t* )( ( uintptr_t )( this ) + 12856 );
	}

	float_t& m_flPostponeFireReadyTime( )
	{
		return *( float_t* )( ( uintptr_t )( this ) + 13132 );
	}

	float_t& m_flNextSecondaryAttack( )
	{
		return *( float_t* )( ( uintptr_t )( this ) + 12860 );
	}

	int16_t& m_iItemDefinitionIndex( )
	{
		return *( int16_t* )( ( uintptr_t )( this ) + 0x2FAA );
	}

	__forceinline bool IsSMG( )
	{
		int32_t ID = m_iItemDefinitionIndex( );
		
		if ( ID == WEAPON_M249 || ID == WEAPON_NEGEV || ID == WEAPON_MP5SD || ID == WEAPON_MP7 || ID == WEAPON_MP9 || ID == WEAPON_P90 || ID == WEAPON_UMP45 || ID == WEAPON_BIZON )
			return true;

		return false;
	}

	bool IsGrenade( )
	{
		return GetWeaponData()->m_iWeaponType == WEAPONTYPE_GRENADE;
	}

	__forceinline bool IsGun( )
	{
		switch ( GetWeaponData( )->m_iWeaponType )
		{
			case WEAPONTYPE_C4:
				return false;
			case WEAPONTYPE_GRENADE:
				return false;
			case WEAPONTYPE_KNIFE:
				return false;
			case WEAPONTYPE_UNKNOWN:
				return false;
			default:
				return true;
		}
	}

	__forceinline bool CanShift( )
	{
		switch ( ( int32_t )( this->m_iItemDefinitionIndex( ) ) )
		{
			case 43:
			case 44:
			case 45:
			case 46:
			case 47:
			case 48:
			case 64:
			case 84:
				return false;
				break;
			default:
				break;
		}

		return this->IsGun( );
	}

	__forceinline bool IsKnife()
	{
		int32_t idx = this->m_iItemDefinitionIndex( );
		return idx == WEAPON_KNIFE || idx == WEAPON_KNIFE_BAYONET || idx == WEAPON_KNIFE_BUTTERFLY || idx == WEAPON_KNIFE_FALCHION
			|| idx == WEAPON_KNIFE_FLIP || idx == WEAPON_KNIFE_GUT || idx == WEAPON_KNIFE_KARAMBIT || idx == WEAPON_KNIFE_M9_BAYONET
			|| idx == WEAPON_KNIFE_PUSH || idx == WEAPON_KNIFE_SURVIVAL_BOWIE || idx == WEAPON_KNIFE_T || idx == WEAPON_KNIFE_TACTICAL
			|| idx == WEAPON_KNIFEGG || idx == WEAPON_KNIFE_GHOST || idx == WEAPON_KNIFE_GYPSY_JACKKNIFE || idx == WEAPON_KNIFE_STILETTO
			|| idx == WEAPON_KNIFE_URSUS || idx == WEAPON_KNIFE_WIDOWMAKER || idx == WEAPON_KNIFE_CSS || idx == WEAPON_KNIFE_CANIS
			|| idx == WEAPON_KNIFE_CORD || idx == WEAPON_KNIFE_OUTDOOR || idx == WEAPON_KNIFE_SKELETON;
	}

	__forceinline bool IsRifle( )
	{
		switch ( GetWeaponData( )->m_iWeaponType )
		{
		case WEAPONTYPE_RIFLE:
			return true;
		case WEAPONTYPE_SUBMACHINEGUN:
			return true;
		case WEAPONTYPE_SHOTGUN:
			return true;
		case WEAPONTYPE_MACHINEGUN:
			return true;
		default:
			return false;
		}
	}

	__forceinline bool IsSniper( )
	{
		switch ( GetWeaponData( )->m_iWeaponType )
		{
		case WEAPONTYPE_SNIPER_RIFLE:
			return true;
		default:
			return false;
		}
	}

	CUSTOM_OFFSET( m_hThrower, CHandle < C_BasePlayer >, FNV32( "m_hThrower" ), 10656 );
	CUSTOM_OFFSET( m_flDamage, float_t, FNV32( "m_flDamage" ), 10648 );
	CUSTOM_OFFSET( m_flDamageRadius, float_t, FNV32( "m_flDamageRadius" ), 10628 );
};