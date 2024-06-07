#include <napi.h>

#include <string>
#include <Windows.h>

class EventLogWorker : public Napi::AsyncWorker
{
public:
    EventLogWorker(Napi::Function &callback, HANDLE &hdl, DWORD &eventId, WORD &type, std::string &message);
    ~EventLogWorker();

    void EventLogWorker::Execute() override;
    void EventLogWorker::OnOK() override;
    void EventLogWorker::OnError(const Napi::Error &e) override;

private:
    HANDLE hdl;
    DWORD eventId;
    WORD type;
    std::string message;
};