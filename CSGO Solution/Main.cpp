#include "Setup.hpp"

BOOL WINAPI DllMain( HINSTANCE hInstance, DWORD dwCallReason, LPVOID pReserved )
{
	DisableThreadLibraryCalls( hInstance );
	if ( dwCallReason != DLL_PROCESS_ATTACH )
		return FALSE;

	HANDLE hThread = CreateThread( NULL, NULL, ( LPTHREAD_START_ROUTINE )( C_SetUp::Instance ), hInstance, NULL, NULL );
	if ( hThread )
		CloseHandle( hThread );

	return TRUE;
}