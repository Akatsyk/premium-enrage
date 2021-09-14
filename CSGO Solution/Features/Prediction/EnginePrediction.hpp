#pragma once
#include "../SDK/Includes.hpp"

struct C_NetvarData
{
	CHandle< C_BaseEntity > m_hGroundEntity;

	float_t m_flPostponeFireReadyTime = 0.f;
	float_t m_flRecoilIndex = 0.f;
	float_t m_flAccuracyPenalty = 0.f;
	float_t m_flDuckAmount = 0.f;
	float_t m_flDuckSpeed = 0.f;
	float_t m_flFallVelocity = 0.f;

	int32_t m_nRenderMode = 0;
	int32_t m_nTickbase = 0;
	int32_t m_fFlags = 0;

	Vector m_vecOrigin = Vector(0.f, 0.f, 0.f);
	Vector m_vecVelocity = Vector(0.f, 0.f, 0.f);
	Vector m_vecBaseVelocity = Vector(0.f, 0.f, 0.f);
	Vector m_vecViewOffset = Vector(0.f, 0.f, 0.f);
	Vector m_vecAimPunchAngleVel = Vector(0.f, 0.f, 0.f);

	QAngle m_angAimPunchAngle = QAngle(0.f, 0.f, 0.f);
	QAngle m_angViewPunchAngle = QAngle(0.f, 0.f, 0.f);
};

class C_PredictionSystem
{
public:
	virtual void Instance( );
	virtual void Repredict( );
	virtual void UpdatePacket( );
	virtual void ResetPacket( );
	virtual void SaveNetvars( int32_t nCommand );
	virtual void RestoreNetvars( int32_t nCommand );
	virtual void SaveVelocityModifier( );
	virtual void SaveCommand( int32_t nCommand );
	virtual float_t GetVelocityModifier( int32_t nCommand );
	virtual void ResetData( );
	virtual C_NetvarData GetNetvars( int32_t nCommand ) { return this->m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ]; };

	virtual void AdjustViewmodelData( );
	virtual void SaveViewmodelData( );

	int32_t m_nTickBase = 0;
	C_MoveData m_MoveData = C_MoveData( );
	C_MoveData m_FreeMoveData = C_MoveData( );
private:
	std::array < C_NetvarData, MULTIPLAYER_BACKUP > m_aNetvarData = { };

	float_t m_flVelocityModifier = 0.0f;
	float_t m_flOldCurtime = 0.0f;
	float_t m_flOldFrametime = 0.0f;

	int32_t m_iLastCommand = 0;
	bool m_bInPrediction_Backup = false;
	bool m_bIsFirstTimePredicted_Backup = false;
	
	float_t m_flCycle = 0.f;
	float_t m_flAnimTime = 0.f;

	int32_t m_iSequence = 0;
	int32_t m_iAnimationParity = 0;

	C_UserCmd* m_LastPredictedCmd = nullptr;
};

inline C_PredictionSystem* g_PredictionSystem = new C_PredictionSystem( );