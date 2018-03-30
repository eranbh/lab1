//
// Created by Eran Ben Horin on 29/03/2018.
//

#ifndef MESSAGESTORE_CLIENT_UI_TASK_H
#define MESSAGESTORE_CLIENT_UI_TASK_H
#include <atomic>
#include <memory>
#include <thread>
#include "inc/StdStreamIODevice.h"
#include "inc/UserManager.h"


namespace framework{
    class IODevice;
}

namespace msg_store {

class ClientUiTask {

public:

    enum class UserOptions : short { ADD_USER, SEND_MSG, RECV_ALL_MSGS, EXIT, BAD_OPT};

    ClientUiTask();

    ~ClientUiTask();

    void startThread();
    void stopUiThread(){m_keepRunning = false;}
    void waitForUiThread();

private:

    static const std::string WELCOME;
    static const std::string ADD;
    static const std::string SEND;
    static const std::string RECV;
    static const std::string EXIT;
    static const std::string GOOD_BYE;
    static const std::string BAD_OPT;
    static const std::string USR_NAME;
    static const std::string USR_LAST;
    static const std::string SEND_USR_FROM;
    static const std::string SEND_USR_TO;
    static const std::string NO_SUCH_USER;
    static const std::string USER_MSG;
    static const uint8_t MAX_USR_NAME;
    static const uint8_t MAX_USR_LAST;
    static const uint8_t MAX_MSG_SIZE;


    void runUiLoop();

    UserOptions displayUserOptions();

    void handleUserRequest(UserOptions);

    void displaySingleOption(std::string);
    // reads till new line [ discarding it ]
    framework::StreamDataChunk readLineFromStream(std::uint64_t);

    void handleAddUser();
    void handleSendToUser();
    void handleRecvForUser();

    UserOptions readUserRequest();

    std::unique_ptr<framework::IODevice> m_ioDevice;

    std::atomic<bool> m_keepRunning = true;

    std::unique_ptr<std::thread> m_thread;

    UserManager m_userManager;
};

} // namespace msg_store


#endif //MESSAGESTORE_CLIENT_UI_TASK_H
