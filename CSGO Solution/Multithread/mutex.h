#define MUTEX_H

#include <windows.h>

class CMutex 
{
public:
	CMutex();
	~CMutex();
	void Lock();
	bool TryLock();
	void Unlock();
private:
	CRITICAL_SECTION m_Lock;
};
