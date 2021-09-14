#pragma once
#include "../SDK/Includes.hpp"
#include "../Tools/Tools.hpp"

struct Weather_t
{
	bool m_bHasBeenCreated = false;

	int32_t m_WeatherID = 0;

	IClientNetworkable* m_Networkable = nullptr;
};

enum PrecipitationType_t : int
{
	PRECIPITATION_TYPE_RAIN = 0,
	PRECIPITATION_TYPE_SNOW,
	PRECIPITATION_TYPE_ASH,
	PRECIPITATION_TYPE_SNOWFALL,
	PRECIPITATION_TYPE_PARTICLERAIN,
	PRECIPITATION_TYPE_PARTICLEASH,
	PRECIPITATION_TYPE_PARTICLERAINSTORM,
	PRECIPITATION_TYPE_PARTICLESNOW,
	NUM_PRECIPITATION_TYPES
};

class C_WeatherSystem
{
public:
	virtual void Instance( ClientFrameStage_t Stage );
	virtual void ResetData( );
	virtual IClientNetworkable* CreateWeatherEntity( );
private:
	Weather_t m_WeatherData = Weather_t( );
};

inline C_WeatherSystem* g_WeatherSystem = new C_WeatherSystem( );