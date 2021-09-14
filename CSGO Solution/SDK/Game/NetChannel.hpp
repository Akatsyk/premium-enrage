#pragma once

class C_NetChannel
{
public:
	char pad_0000[20];           //0x0000
	bool m_bProcessingMessages;  //0x0014
	bool m_bShouldDelete;        //0x0015
	char pad_0016[2];            //0x0016
	int32_t m_iOutSequenceNr;    //0x0018 last send outgoing sequence number
	int32_t m_iInSequenceNr;     //0x001C last received incoming sequnec number
	int32_t m_iOutSequenceNrAck; //0x0020 last received acknowledge outgoing sequnce number
	int32_t m_iOutReliableState; //0x0024 state of outgoing reliable data (0/1) flip flop used for loss detection
	int32_t m_iInReliableState;  //0x0028 state of incoming reliable data
	int32_t m_iChokedCommands;   //0x002C number of choked packets
	char pad_0030[1044];         //0x0030

	PushVirtual( SendDatagram( LPVOID Data ), 46, int( __thiscall* )( LPVOID, LPVOID ), Data );
}; //Size: 0x0444