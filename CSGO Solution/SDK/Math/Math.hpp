#pragma once
#include <DirectXMath.h>
#include <algorithm>

#include "VMatrix.hpp"
#include "QAngle.hpp"
#include "Vector.hpp"

#include "m128.h"
#include "../../Tools/Obfuscation/XorStr.hpp"

#define RAD2DEG(x) DirectX::XMConvertToDegrees( x ) 
#define DEG2RAD(x) DirectX::XMConvertToRadians( x )
#define M_PI 3.14159265358979323846
#define PI_F ( ( float )( M_PI ) ) 
#define M_RADPI 57.295779513082f

static float lastAmt = -1;
static float lastExponent = 0;
namespace Math
{
	void FixMovement( void* pCmd );


	__forceinline float Bias(float x, float biasAmt)
	{
		if (lastAmt != biasAmt)
			lastExponent = log(biasAmt) * -1.4427f;

		return pow(x, lastExponent);
	}

	__forceinline float Approach(float target, float value, float speed)
	{
		float delta = target - value;

		if (delta > speed)
			value += speed;
		else if (delta < -speed)
			value -= speed;
		else
			value = target;

		return value;
	}
	
	template <class T>
	inline T Lerp(float flPercent, T const& A, T const& B)
	{
		return A + (B - A) * flPercent;
	}

	inline float SmoothstepBounds(float edge0, float edge1, float x)
	{
		x = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
		return x * x * (3 - 2 * x);
	}

	__forceinline float flAngleMod(float flAngle)
	{
		return((360.0f / 65536.0f) * ((int32_t)(flAngle * (65536.0f / 360.0f)) & 65535));
	}
	__forceinline float ApproachAngle(float target, float value, float speed)
	{
		target = flAngleMod(target);
		value = flAngleMod(value);

		float delta = target - value;

		// Speed is assumed to be positive
		if (speed < 0)
			speed = -speed;

		if (delta < -180)
			delta += 360;
		else if (delta > 180)
			delta -= 360;

		if (delta > speed)
			value += speed;
		else if (delta < -speed)
			value -= speed;
		else
			value = target;

		return value;
	}


	// BUGBUG: Why do we need both of these?
	__forceinline float AngleDiff(float destAngle, float srcAngle)
	{
		float delta = fmodf(destAngle - srcAngle, 360.0f);
		if (destAngle > srcAngle)
		{
			if (delta >= 180)
				delta -= 360;
		}
		else
		{
			if (delta <= -180)
				delta += 360;
		}
		return delta;
	}


	__forceinline float AngleDistance(float next, float cur)
	{
		float delta = next - cur;

		if (delta < -180)
			delta += 360;
		else if (delta > 180)
			delta -= 360;

		return delta;
	}


	__forceinline float AngleNormalize(float angle)
	{
		angle = fmodf(angle, 360.0f);
		if (angle > 180)
		{
			angle -= 360;
		}
		if (angle < -180)
		{
			angle += 360;
		}
		return angle;
	}

	__forceinline float RemapVal(float val, float A, float B, float C, float D)
	{
		if (A == B)
			return val >= B ? D : C;
		return C + (D - C) * (val - A) / (B - A);
	}

	__forceinline float RemapValClamped(float val, float A, float B, float C, float D)
	{
		if (A == B)
			return val >= B ? D : C;
		float cVal = (val - A) / (B - A);
		cVal = std::clamp(cVal, 0.0f, 1.0f);

		return C + (D - C) * cVal;
	}

	__forceinline void AngleMatrix(const QAngle& angView, matrix3x4_t& matOutput, const Vector& vecOrigin)
	{
		float sp, sy, sr, cp, cy, cr;

		DirectX::XMScalarSinCos(&sp, &cp, DEG2RAD(angView.pitch));
		DirectX::XMScalarSinCos(&sy, &cy, DEG2RAD(angView.yaw));
		DirectX::XMScalarSinCos(&sr, &cr, DEG2RAD(angView.roll));

		matOutput.SetForward(Vector(cp * cy, cp * sy, -sp));

		const float crcy = cr * cy;
		const float crsy = cr * sy;
		const float srcy = sr * cy;
		const float srsy = sr * sy;

		matOutput.SetLeft(Vector(sp * srcy - crsy, sp * srsy + crcy, sr * cp));
		matOutput.SetUp(Vector(sp * crcy + srsy, sp * crsy - srcy, cr * cp));
		matOutput.SetOrigin(vecOrigin);
	}

	__forceinline Vector CrossProduct(const Vector& a, const Vector& b)
	{
		return Vector(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
	}

	__forceinline float NormalizeAngle( float flAngle )
	{
		flAngle = fmod( flAngle, 360.0f );
		if ( flAngle > 180.0f ) 
			flAngle -= 360.0f;
		if ( flAngle < -180.0f )
			flAngle += 360.0f;

		return flAngle;
	}

	template< class T >
	__forceinline void Normalize3( T& vecVector )
	{
		vecVector[ 0 ] = std::clamp( vecVector[ 0 ], -89.0f, 89.0f );
		vecVector[ 1 ] = std::clamp( NormalizeAngle( vecVector[ 1 ] ), -180.0f, 180.0f );
		vecVector[ 2 ] = 0.0f;
	}


	__forceinline float VectorNormalize( Vector& vecVector )
	{
		float_t flLength = vecVector.Length( );
		if ( flLength )
			vecVector /= flLength;
		else
			vecVector = Vector( 0.0f, 0.0f, 1.0f );
		
		return flLength;
	}

	__forceinline void VectorAngles(const Vector& forward, Vector& up, QAngle& angles)
	{
		Vector left = CrossProduct(up, forward);
		left.NormalizeInPlace();

		float forwardDist = forward.Length2D();

		if (forwardDist > 0.001f)
		{
			angles.pitch = atan2f(-forward.z, forwardDist) * 180.0f / PI_F;
			angles.yaw = atan2f(forward.y, forward.x) * 180.0f / PI_F;

			float upZ = (left.y * forward.x) - (left.x * forward.y);
			angles.roll = atan2f(left.z, upZ) * 180.0f / PI_F;
		}
		else
		{
			angles.pitch = atan2f(-forward.z, forwardDist) * 180.0f / PI_F;
			angles.yaw = atan2f(-left.x, left.y) * 180.0f / PI_F;
			angles.roll = 0;
		}
	}
	__forceinline void AngleVectors( const QAngle& angles, Vector& forward, Vector& right, Vector& up )
	{
		float sr, sp, sy, cr, cp, cy;

		DirectX::XMScalarSinCos( &sp, &cp, DEG2RAD( angles[ 0 ] ) );
		DirectX::XMScalarSinCos( &sy, &cy, DEG2RAD( angles[ 1 ] ) );
		DirectX::XMScalarSinCos( &sr, &cr, DEG2RAD( angles[ 2 ] ) );

		forward.x = (cp * cy);
		forward.y = (cp * sy);
		forward.z = (-sp);
		right.x = (-1 * sr * sp * cy + -1 * cr * -sy);
		right.y = (-1 * sr * sp * sy + -1 * cr * cy);
		right.z = (-1 * sr * cp);
		up.x = (cr * sp * cy + -sr * -sy);
		up.y = (cr * sp * sy + -sr * cy);
		up.z = (cr * cp);
	}

	__forceinline float DistanceToRay( const Vector& pos, const Vector& rayStart, const Vector& rayEnd, float* along = nullptr, Vector* pointOnRay = nullptr )
	{
		Vector to = pos - rayStart;
		Vector dir = rayEnd - rayStart;
		float length = dir.Normalize();

		float rangeAlong = dir.Dot(to);
		if (along)
			*along = rangeAlong;

		float range;

		if (rangeAlong < 0.0f)
		{
			range = -(pos - rayStart).Length();

			if (pointOnRay)
				*pointOnRay = rayStart;
		}
		else if (rangeAlong > length)
		{
			range = -(pos - rayEnd).Length();

			if (pointOnRay)
				*pointOnRay = rayEnd;
		}
		else
		{
			Vector onRay = rayStart + dir * rangeAlong;

			range = (pos - onRay).Length();

			if (pointOnRay)
				*pointOnRay = onRay;
		}

		return range;
	}

	__forceinline void ClampAngles( QAngle& angAngles )
	{
		if ( angAngles.pitch > 89.0f )
			angAngles.pitch = 89.0f;
		else if ( angAngles.pitch < -89.0f )
			angAngles.pitch = -89.0f;

		if ( angAngles.yaw > 180.0f )
			angAngles.yaw = 180.0f;
		else if ( angAngles.yaw < -180.0f )
			angAngles.yaw = -180.0f;

		angAngles.roll = 0.0f;
	}

	__forceinline float DotProduct( Vector v1, Vector v2 )
	{
		return ( v1.x * v2.x + v1.y * v2.y + v1.z * v2.z );
	};

	template < typename T>
	__forceinline T Interpolate( const T& T1, const T& T2, float flProgress )
	{
		return T1 * flProgress + T2 * ( 1.f - flProgress );
	}

	__forceinline void VectorTransform(const Vector& in1, const matrix3x4_t& in2, Vector& out)
	{
		out[ 0 ] = in1.Dot( in2[ 0 ] ) + in2[ 0 ][ 3 ];
		out[ 1 ] = in1.Dot( in2[ 1 ] ) + in2[ 1 ][ 3 ];
		out[ 2 ] = in1.Dot( in2[ 2 ] ) + in2[ 2 ][ 3 ];
	}

	__forceinline void AngleVectors( const QAngle& vecAngles, Vector& vecForward )
	{
		float sp, sy, cp, cy;

		DirectX::XMScalarSinCos(&sp, &cp, DEG2RAD( vecAngles[ 0 ] ) );
		DirectX::XMScalarSinCos(&sy, &cy, DEG2RAD( vecAngles[ 1 ] ) );

		vecForward.x = cp * cy;
		vecForward.y = cp * sy;
		vecForward.z = -sp;
	}

	__forceinline void VectorRotate( const Vector& in1, const matrix3x4_t& in2, Vector& out )
	{
		out.x = in1.x * in2[0][0] + in1.y * in2[1][0] + in1.z * in2[2][0];
		out.y = in1.x * in2[0][1] + in1.y * in2[1][1] + in1.z * in2[2][1];
		out.z = in1.x * in2[0][2] + in1.y * in2[1][2] + in1.z * in2[2][2];
	}

	__forceinline QAngle CalcAngle(const Vector& src, const Vector& dst)
	{
		QAngle vAngle;
		Vector delta((src.x - dst.x), (src.y - dst.y), (src.z - dst.z));
		double hyp = sqrt(delta.x * delta.x + delta.y * delta.y);

		vAngle.pitch = float(atanf(float(delta.z / hyp)) * 57.295779513082f);
		vAngle.yaw = float(atanf(float(delta.y / delta.x)) * 57.295779513082f);
		vAngle.roll = 0.0f;

		if (delta.x >= 0.0)
			vAngle.yaw += 180.0f;

		return vAngle;
	}

	__forceinline void VectorAngles( const Vector vecForward, QAngle& vecAngles )
	{
		float flTemp, flYaw, flPitch;

		if ( vecForward.y == 0 && vecForward.x == 0)
		{
			flYaw = 0;

			flPitch = 90;
			if ( vecForward.z > 0 )
				flPitch = 270;       
		}
		else 
		{
			flYaw = ( atan2( vecForward.y, vecForward.x ) * 180 / DirectX::XM_PI );
			if ( flYaw < 0 )
				flYaw += 360;

			flTemp = sqrt( vecForward.x * vecForward.x + vecForward.y * vecForward.y );
			flPitch = (atan2( -vecForward.z, flTemp ) * 180 / DirectX::XM_PI );
			if ( flPitch < 0 )
				flPitch += 360;
		}

		vecAngles.pitch = flPitch;
		vecAngles.yaw = flYaw;
		vecAngles.roll = 0;
	}

	bool WorldToScreen(const Vector& in, Vector& out);
}