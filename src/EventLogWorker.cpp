#include <napi.h>

#include <string>
#include <Windows.h>

#include "EventLog.h"
#include "EventLogWorker.h"

EventLogWorker::EventLogWorker(Napi::Function &callback, HANDLE &hdl, DWORD &eventId, WORD &type, std::string &message) : 
    Napi::AsyncWorker(callback), hdl(hdl), eventId(eventId), type(type), message(message) {}

EventLogWorker::~EventLogWorker() {}

void EventLogWorker::Execute()
{
    logSync(hdl, eventId, type, message);
}

void EventLogWorker::OnOK()
{
    Napi::HandleScope scope(Env());
    Callback().Call({Env().Null(), Napi::Boolean::New(Env(), true)});
}

void EventLogWorker::OnError(const Napi::Error& e)
{
    Napi::HandleScope scope(Env());
    Callback().Call({e.Value(), Napi::Boolean::New(Env(), false)});
}