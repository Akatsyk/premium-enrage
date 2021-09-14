#pragma once

class C_EventInfo
{
public:
	enum
	{
		EVENT_INDEX_BITS = 8,
		EVENT_DATA_LEN_BITS = 11,
		MAX_EVENT_DATA = 192,
	};

	inline C_EventInfo( )
	{
		m_iClassID = 0;
		m_flFireDelay = 0.0f;
		m_nFlags = 0;
		m_pSendTable = 0;
		m_pClientClass = 0;
		m_iPacked = 0;
	}

	short m_iClassID;
	short pad;
	float m_flFireDelay;
	const void* m_pSendTable;
	const void* m_pClientClass;
	int m_iPacked;
	int m_nFlags;
	int m_aFilters[ 8 ];
	C_EventInfo* m_pNext;
};