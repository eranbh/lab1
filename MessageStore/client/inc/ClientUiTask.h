//
// Created by Eran Ben Horin on 29/03/2018.
//

#ifndef MESSAGESTORE_CLIENT_UI_TASK_H
#define MESSAGESTORE_CLIENT_UI_TASK_H
#include <atomic>
#include <memory>
#include <thread>


namespace framework{
    class IODevice;
}

namespace msg_store {

class ClientUiTask {

public:

    ClientUiTask();

    ~ClientUiTask();

    void startThread();
    void stopUiThread(){m_keepRunning = false;}

private:

    void runUiLoop();

    std::unique_ptr<framework::IODevice> m_ioDevice;

    std::atomic<bool> m_keepRunning = true;

    std::unique_ptr<std::thread> m_thread;
};

} // namespace msg_store


#endif //MESSAGESTORE_CLIENT_UI_TASK_H
