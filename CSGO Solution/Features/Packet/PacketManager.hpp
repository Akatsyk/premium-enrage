#pragma once
#include <deque>
#include "../../SDK/Includes.hpp"

class C_PacketManager
{
public:
	virtual bool SetupPacket( int32_t iSequence, bool* pbSendPacket );
	virtual void FinishPacket( int32_t iSequence );
	virtual void FinishNetwork( );
	virtual void SetLastPacket( bool bLastPacket ) { m_bLastSendPacket = bLastPacket; };
	virtual bool ShouldProcessPacketStart( int32_t iCommand );
	virtual bool& GetModifablePacket( );
	virtual C_UserCmd* GetModifableCommand( );
	virtual void PushCommand( int nCommand ) { m_aCommandList.emplace_back( nCommand ); };
	virtual QAngle GetFakeAngles( ) { return this->m_angFakeAngle; };
	virtual void SetFakeAngles( QAngle angle ) { this->m_angFakeAngle = angle; };
private:
	C_UserCmd* m_pUserCmd = nullptr;
	bool* m_pbSendPacket = nullptr;
	bool m_bLastSendPacket = false;

	QAngle m_angFakeAngle = QAngle( 0, 0, 0 );

	std::deque < int32_t > m_aCommandList;
};

inline C_PacketManager* g_PacketManager = new C_PacketManager( );