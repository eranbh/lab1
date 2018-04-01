#pragma once

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

    // friendship for testing.
    // this technique has the advantage of not requiring
    // inclusion of the testing code, or special set methods
    // for setting the impl details [ e.g. the IODevice impl ]
    friend class ClientUITest;

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
    static const std::string USR_ADD;
    static const std::string SEND_USR_FROM;
    static const std::string SEND_USR_TO;
    static const std::string NO_SUCH_USER;
    static const std::string USER_MSG;
    static const std::string RCV_USR;
    static const std::string USR_EXIST;
    static const uint8_t MAX_USR_NAME;
    static const uint8_t MAX_USR_LAST;
    static const uint8_t MAX_MSG_SIZE;


    void runUiLoop();

    UserOptions displayUserOptions();

    void handleUserRequest(UserOptions);

    void displaySingleMessage(std::string);
    // reads till new line [ discarding it ]
    framework::StreamDataChunk readLineFromStream(std::uint64_t);

    UserManager::User parseUserFromInput(std::uint64_t  lenToRead);

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
