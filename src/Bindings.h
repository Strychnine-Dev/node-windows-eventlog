#include <string>
#include <Windows.h>

class EventLog : public Napi::ObjectWrap<EventLog>
{
public:
    EventLog(const Napi::CallbackInfo &info);
    ~EventLog();
    static Napi::Function EventLog::GetClass(Napi::Env env);

private:
    Napi::Value LogSync(const Napi::CallbackInfo &info);
    Napi::Value LogAsync(const Napi::CallbackInfo &info);
    bool EventLog::parseSeverity(const std::string &severity, WORD *type);
    std::string EventLog::getLastErrorAsString();
    HANDLE eventLogHandle_;
};