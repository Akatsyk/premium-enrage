#pragma once
#include "IClientNetworkable.hpp"
#include "IClientRenderable.hpp"
#include "IClientUnknown.hpp"
#include "IClientThinkable.hpp"

class Vector;
class QAngle;
struct SpatializationInfo_t;

class C_ClientEntity : public IClientUnknown, public IClientRenderable, public IClientNetworkable, public IClientThinkable
{
public:
	virtual void			Release( void ) = 0;
	virtual const Vector&	GetAbsOrigin( void ) const = 0;
	virtual const QAngle&	GetAbsAngles( void ) const = 0;
};