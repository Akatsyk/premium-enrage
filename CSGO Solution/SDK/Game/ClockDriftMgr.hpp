#pragma once
#include <cstdint>

class C_ClockDriftMgr
{
public:
	float m_ClockOffsets[16];   //0x0000
	int32_t m_iCurClockOffset; //0x0044
	int32_t m_nServerTick;     //0x0048
	int32_t m_iClientTick;     //0x004C
}; //Size: 0x0050