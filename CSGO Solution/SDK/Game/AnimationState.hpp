#pragma once
#include <cstdint>
#include "AnimationLayer.hpp"

#pragma pack(push, 1)
struct C_AimLayer
{
	float m_flUnknown0;
	float m_flTotalTime;
	float m_flUnknown1;
	float m_flUnknown2;
	float m_flWeight;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct C_AimLayers
{
	C_AimLayer layers[3];
};
#pragma pack(pop)

struct AnimstatePose_t
{
	bool		m_bInitialized;
	int			m_nIndex;
	const char* m_szName;

	AnimstatePose_t()
	{
		m_bInitialized = false;
		m_nIndex = -1;
		m_szName = "";
	}
};

struct procedural_foot_t
{
	Vector m_vecPosAnim;
	Vector m_vecPosAnimLast;
	Vector m_vecPosPlant;
	Vector m_vecPlantVel;
	float m_flLockAmount;
	float m_flLastPlantTime;
};

class CUtlSymbolTable;
class CUtlSymbolTableMT;


//-----------------------------------------------------------------------------
// This is a symbol, which is a easier way of dealing with strings.
//-----------------------------------------------------------------------------
typedef unsigned short UtlSymId_t;

#define UTL_INVAL_SYMBOL  ((UtlSymId_t)~0)

class CUtlSymbol
{
public:
	// constructor, destructor
	CUtlSymbol() : m_Id(UTL_INVAL_SYMBOL) {}
	CUtlSymbol(UtlSymId_t id) : m_Id(id) {}
	CUtlSymbol(const char* pStr);
	CUtlSymbol(CUtlSymbol const& sym) : m_Id(sym.m_Id) {}

	// operator=
	CUtlSymbol& operator=(CUtlSymbol const& src) { m_Id = src.m_Id; return *this; }

	// operator==
	bool operator==(CUtlSymbol const& src) const { return m_Id == src.m_Id; }
	bool operator==(const char* pStr) const;

	// Is valid?
	bool IsValid() const { return m_Id != UTL_INVAL_SYMBOL; }

	// Gets at the symbol
	operator UtlSymId_t () const { return m_Id; }

	// Gets the string associated with the symbol
	const char* String() const;

	// Modules can choose to disable the static symbol table so to prevent accidental use of them.
	static void DisableStaticSymbolTable();

	// Methods with explicit locking mechanism. Only use for optimization reasons.
	static void LockTableForRead();
	static void UnlockTableForRead();
	const char* StringNoLock() const;

protected:
	UtlSymId_t   m_Id;

	// Initializes the symbol table
	static void Initialize();

	// returns the current symbol table
	static CUtlSymbolTableMT* CurrTable();

	// The standard global symbol table
	static CUtlSymbolTableMT* s_pSymbolTable;

	static bool s_bAllowStaticSymbolTable;

	friend class CCleanupUtlSymbolTable;
};

class C_BaseCombatWeapon;
class C_CSGOPlayerAnimationState
{
public:
	void*					m_pThis;									
	bool					m_bIsReset;									
	bool					m_bUnknownClientBoolean;					
	char					m_aSomePad[ 2 ];							
	int32_t					m_nTick;									
	float_t					m_flFlashedStartTime;						
	float_t					m_flFlashedEndTime;							
	C_AimLayers				m_AimLayers;								
	int32_t					m_iModelIndex;								
	int32_t					m_iUnknownArray[ 3 ];						
	C_BasePlayer*			m_pBasePlayer;
	C_BaseCombatWeapon*		m_pWeapon;
	C_BaseCombatWeapon*		m_pWeaponLast;
	float					m_flLastUpdateTime;
	int						m_nLastUpdateFrame;
	float					m_flLastUpdateIncrement;
	float					m_flEyeYaw;
	float					m_flEyePitch;
	float					m_flFootYaw;
	float					m_flFootYawLast;
	float					m_flMoveYaw;
	float					m_flMoveYawIdeal;
	float					m_flMoveYawCurrentToIdeal;
	float					m_flTimeToAlignLowerBody;
	float					m_flPrimaryCycle;
	float					m_flMoveWeight;
	float					m_flMoveWeightSmoothed;
	float					m_flAnimDuckAmount;
	float					m_flDuckAdditional;
	float					m_flRecrouchWeight;
	Vector					m_vecPositionCurrent;
	Vector					m_vecPositionLast;
	Vector					m_vecVelocity;
	Vector					m_vecVelocityNormalized;
	Vector					m_vecVelocityNormalizedNonZero;
	float					m_flVelocityLengthXY;
	float					m_flVelocityLengthZ;
	float					m_flSpeedAsPortionOfRunTopSpeed;
	float					m_flSpeedAsPortionOfWalkTopSpeed;
	float					m_flSpeedAsPortionOfCrouchTopSpeed;
	float					m_flDurationMoving;
	float					m_flDurationStill;
	bool					m_bOnGround;
	bool					m_bLanding;
	char					m_pad[2];
	float					m_flJumpToFall;
	float					m_flDurationInAir;
	float					m_flLeftGroundHeight;
	float					m_flLandAnimMultiplier;
	float					m_flWalkToRunTransition;
	bool					m_bLandedOnGroundThisFrame;
	bool					m_bLeftTheGroundThisFrame;
	float					m_flInAirSmoothValue;
	bool					m_bOnLadder;
	float					m_flLadderWeight;
	float					m_flLadderSpeed;
	bool					m_bWalkToRunTransitionState;
	bool					m_bDefuseStarted;
	bool					m_bPlantAnimStarted;
	bool					m_bTwitchAnimStarted;
	bool					m_bAdjustStarted;
	char					m_ActivityModifiers[ 20 ];
	float					m_flNextTwitchTime;
	float					m_flTimeOfLastKnownInjury;
	float					m_flLastVelocityTestTime;
	Vector					m_vecVelocityLast;
	Vector					m_vecTargetAcceleration;
	Vector					m_vecAcceleration;
	float					m_flAccelerationWeight;
	float					m_flAimMatrixTransition;
	float					m_flAimMatrixTransitionDelay;
	bool					m_bFlashed;
	float					m_flStrafeChangeWeight;
	float					m_flStrafeChangeTargetWeight;
	float					m_flStrafeChangeCycle;
	int						m_nStrafeSequence;
	bool					m_bStrafeChanging;
	float					m_flDurationStrafing;
	float					m_flFootLerp;
	bool					m_bFeetCrossed;
	bool					m_bPlayerIsAccelerating;
	AnimstatePose_t			m_tPoseParamMappings[ 20 ];
	float					m_flDurationMoveWeightIsTooHigh;
	float					m_flStaticApproachSpeed;
	int						m_nPreviousMoveState;
	float					m_flStutterStep;
	float					m_flActionWeightBiasRemainder;
	procedural_foot_t		m_footLeft;
	procedural_foot_t		m_footRight;
	float					m_flCameraSmoothHeight;
	bool					m_bSmoothHeightValid;
	float					m_flLastTimeVelocityOverTen;
	float					m_flAimYawMin;
	float					m_flAimYawMax;
	float					m_flAimPitchMin;
	float					m_flAimPitchMax;
	int						m_nAnimstateModelVersion;

	int32_t SelectSequenceFromActivityModifier( int32_t iActivity );

	bool IsLayerSequenceFinished( C_AnimationLayer* pAnimationLayer, float_t flTime );
	void SetLayerSequence( C_AnimationLayer* pAnimationLayer, int32_t iActivity );
	void SetLayerCycle( C_AnimationLayer* pAnimationLayer, float_t flCycle );
	void SetLayerRate( C_AnimationLayer* pAnimationLayer, float_t flRate );
	void SetLayerWeight( C_AnimationLayer* pAnimationLayer, float_t flWeight );
	void SetLayerWeightRate( C_AnimationLayer* pAnimationLayer, float_t flWeightRate );
	void IncrementLayerCycleWeightRateGeneric( C_AnimationLayer* pAnimationLayer );
	void IncrementLayerCycle( C_AnimationLayer* pAnimationLayer, bool bIsLoop );
	float_t GetLayerIdealWeightFromSeqCycle( C_AnimationLayer* );
}; // 836