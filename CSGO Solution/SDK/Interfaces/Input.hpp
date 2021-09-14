#pragma once
#include "../Game/UserCmd.hpp"

#define MULTIPLAYER_BACKUP 150

class bf_write;
class bf_read;

class C_Input
{
public:
	char                pad_0x00[0x0C];  
	bool                m_bTrackirAvailable;  
	bool                m_bMouseInitiated;  
	bool                m_bMouseActive;  
	bool                m_bJoystickAdvancedInit;       
	char                pad_0x08[0x2C];                
	void*               m_aKeys;                       
	char                pad_0x38[0x6C];                
	bool                m_bCameraInterceptingMouse;    
	bool                m_bCameraInThirdPerson;        
	bool                m_bCameraMovingWithMouse;      
	Vector				m_vecCameraOffset;             
	bool                m_bCameraDistanceMove;         
	int                 m_iCameraOldX;                 
	int                 m_iCameraOldY;                 
	int                 m_iCameraX;                    
	int                 m_iCameraY;                    
	bool                m_bCameraIsOrthographic;        
	Vector				m_vecPreviousViewAngles;       
	Vector				m_vecPreviousViewAnglesTilt;   
	float               m_flLastForwardMove;           
	int                 m_iClearInputState;            
	char                pad_0xE4[0x8];                 
	C_UserCmd*          m_aCommands;                   
	C_VerifiedUserCmd*  m_aVerifiedCommands;           

	C_UserCmd* C_Input::GetUserCmd( int iSequenceNumber)
	{
		return GetVirtual < C_UserCmd*( __thiscall* )( void*, int, int ) > ( this, 8 )( this, 0, iSequenceNumber );
	}

	C_UserCmd *C_Input::GetUserCmd( int iSlot, int iSequenceNumber)
	{
		return GetVirtual < C_UserCmd*( __thiscall* )( void*, int, int ) > ( this, 8 )( this, iSlot, iSequenceNumber );
	}

	C_VerifiedUserCmd* C_Input::GetVerifiedCmd( int iSequenceNumber )
	{
		auto verifiedCommands = *(C_VerifiedUserCmd**)(reinterpret_cast<uint32_t>(this) + 0xF8);
		return &verifiedCommands[iSequenceNumber % MULTIPLAYER_BACKUP];
	}
};