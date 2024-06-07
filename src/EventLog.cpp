#include <string>
#include <Windows.h>

#include "EventLog.h"

bool logSync(HANDLE hdl, DWORD eventId, WORD type, const std::string &message)
{
    WORD category = 0;     // no category
    PSID user = NULL;      // use process' user
    DWORD binDataSize = 0; // no additional binary data provided
    LPVOID binData = NULL;
    const WORD numStrings = 1; // we just log the provided message
    LPCTSTR strings[numStrings];
    std::wstring wideMessage(message.begin(), message.end());
    strings[0] = wideMessage.c_str();
    return ReportEvent(hdl, type, category, eventId, user, numStrings, binDataSize, strings, binData);
}