#pragma once
#include "../SDK/Includes.hpp"

class C_LocalAnimations
{
public:
	virtual void Instance( );
	virtual void SetupShootPosition( );
	virtual void DoAnimationEvent( int type );
	virtual bool GetCachedMatrix( matrix3x4_t* aMatrix );
	virtual void OnUpdateClientSideAnimation( );

	virtual void ResetData( );

	virtual std::array < matrix3x4_t, 128 > GetDesyncMatrix( );
	virtual std::array < matrix3x4_t, 128 > GetLagMatrix( );
	virtual std::array < C_AnimationLayer, 13 > GetAnimationLayers( );
	virtual std::array < C_AnimationLayer, 13 > GetFakeAnimationLayers( );
};

inline C_LocalAnimations* g_LocalAnimations = new C_LocalAnimations( );