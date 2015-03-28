#include "Misc_InternalPch.h"

#if defined(PLATFORM_WINDOWS)
#include <windows.h>
#elif defined(PLATFORM_MAC)
#include <CoreServices/CoreServices.h>
#include <unistd.h>
#endif // PLATFORM_WINDOWS

#include <stdio.h>
#include <assert.h>
#include "Map.h"
#include "AnsiString.h"
#include <time.h>

namespace Misc {

bool gInAssert = false;

DWORD GetMilliseconds()
{
#if defined(PLATFORM_WINDOWS)
	return ::GetTickCount();
#endif
#if defined(PLATFORM_MAC)
	Duration duration = AbsoluteToDuration(UpTime());
	return duration < 0 ? duration / -1000 : duration;
#endif // PLATFORM_WINDOWS
}


void SleepMilliseconds(unsigned int milliseconds)
{
#if defined(PLATFORM_WINDOWS)
	::Sleep(milliseconds);
#elif defined(PLATFORM_MAC)
	usleep(milliseconds * 1000);
#endif
}

} // namespace Misc
