#include "../Hooks.hpp"

bool __fastcall C_Hooks::hkSendNetMessage( LPVOID pEcx, uint32_t, C_NetMessage& Message, bool bReliable, bool bVoice )
{
	return g_Globals.m_Hooks.m_Originals.m_SendNetMessage( pEcx, Message, bReliable, bVoice || Message.GetType( ) == 9 );
}