#include <string>
#include <Windows.h>

bool logSync(HANDLE hdl, DWORD eventId, WORD type, const std::string& message);
