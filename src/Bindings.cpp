#pragma comment(lib, "node")

#include <napi.h>

#include "Bindings.h"
#include "EventLog.h"
#include "EventLogWorker.h"

EventLog::EventLog(const Napi::CallbackInfo &info) : Napi::ObjectWrap<EventLog>(info)
{
    Napi::Env env = info.Env();

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "First argument must be the name of the event log source").ThrowAsJavaScriptException();
        return;
    }

    const std::string &source = info[0].As<Napi::String>().Utf8Value();
    std::wstring wideSource(source.begin(), source.end());

    eventLogHandle_ = RegisterEventSource(NULL, wideSource.c_str());
    if (!eventLogHandle_)
    {
        Napi::TypeError::New(env, "Unable to register event source: " + getLastErrorAsString()).ThrowAsJavaScriptException();
        return;
    }
}

EventLog::~EventLog()
{
    DeregisterEventSource(this->eventLogHandle_);
}

Napi::Value EventLog::LogSync(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (!(info[0].IsString() && info[1].IsString()) &&
        !(info[0].IsString() && info[1].IsUndefined()) &&
        !(info[0].IsString() && info[1].IsString() && info[2].IsNumber()) &&
        !(info[0].IsString() && info[1].IsNumber() && info[2].IsUndefined()))
    {
        Napi::TypeError::New(env, "A message and an optional severity must be provided.").ThrowAsJavaScriptException();
        return Napi::Boolean::Boolean(env, false);
    }

    bool severityProvided = info[1].IsString();
    bool eventIdProvided = !info[1].IsUndefined() && info[1].IsNumber() || !info[2].IsUndefined() && info[2].IsNumber();
    std::string severity = severityProvided ? info[0].As<Napi::String>().Utf8Value() : "info";
    std::string message = info[severityProvided ? 1 : 0].As<Napi::String>().Utf8Value();
    DWORD eventId = eventIdProvided ? (info[1].IsNumber() ? info[1].As<Napi::Number>().Uint32Value() : info[2].As<Napi::Number>().Uint32Value()) : 1000;

    WORD type;
    if (!parseSeverity(severity, &type))
    {
        Napi::TypeError::New(env, "Unsupported severity " + severity).ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }

    if (!logSync(this->eventLogHandle_, eventId, type, message))
    {
        Napi::TypeError::New(env, "Error while logging " + getLastErrorAsString()).ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }

    return Napi::Boolean::New(env, true);
}

Napi::Value EventLog::LogAsync(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (!(info[0].IsString() && info[1].IsFunction()) &&
        !(info[0].IsString() && info[1].IsString() && info[2].IsFunction()) &&
        !(info[0].IsString() && info[1].IsNumber() && info[2].IsFunction()) &&
        !(info[0].IsString() && info[1].IsString() && info[2].IsNumber() && info[3].IsFunction())) {
        Napi::TypeError::New(env, "A message and callback must be provided.").ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }

    bool severityProvided = info[1].IsString();
    bool eventIdProvided = info[1].IsNumber() || info[2].IsNumber();
    std::string severity = severityProvided ? info[0].As<Napi::String>().Utf8Value() : "info";

    std::string message = info[severityProvided ? 1 : 0].As<Napi::String>().Utf8Value();
    DWORD eventId = eventIdProvided ? (info[1].IsNumber() ? info[1].As<Napi::Number>().Uint32Value() : info[2].As<Napi::Number>().Uint32Value()) : 1000;
    Napi::Function callback = info[severityProvided ? 2 : 1].As<Napi::Function>();
    
    WORD type;
    if (!parseSeverity(severity, &type)) {
        Napi::TypeError::New(env, "Unsupported severity " + severity).ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }

    EventLogWorker* wk = new EventLogWorker(callback, this->eventLogHandle_, eventId, type, message);
    wk->Queue();
    return info.Env().Undefined();
}

bool EventLog::parseSeverity(const std::string &severity, WORD *type)
{
    if (severity == "info")
    {
        *type = EVENTLOG_INFORMATION_TYPE;
        return true;
    }
    else if (severity == "warn" || severity == "warning")
    {
        *type = EVENTLOG_WARNING_TYPE;
        return true;
    }
    else if (severity == "error")
    {
        *type = EVENTLOG_ERROR_TYPE;
        return true;
    }
    else
    {
        return false;
    }
}

std::string EventLog::getLastErrorAsString()
{
    DWORD errorMessageID = ::GetLastError();
    if (errorMessageID == 0)
    {
        return std::string();
    }

    DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
    DWORD languageId = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);
    LPSTR messageBuffer = NULL;
    size_t size = FormatMessageA(flags, NULL, errorMessageID, languageId, (LPSTR)&messageBuffer, 0, NULL);

    std::string message(messageBuffer, size);
    LocalFree(messageBuffer);
    return message;
}

Napi::Function EventLog::GetClass(Napi::Env env)
{
    return DefineClass(env, "EventLog", {
                                            EventLog::InstanceMethod("logSync", &EventLog::LogSync),
                                            EventLog::InstanceMethod("log", &EventLog::LogAsync),
                                        });
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    Napi::String name = Napi::String::New(env, "EventLog");
    exports.Set(name, EventLog::GetClass(env));
    return exports;
}

// Register and initialize native add-on
NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)