/*
 * this file contains the UI thread's logic. it could be used with any
 * underlying IO device, as long as that has an interface that conforms
 * to the one given by IODevice.
 */
#include <sstream>
#include "inc/ClientUiTask.h"


namespace msg_store {

#define ANY_CAST(CHUNK) \
    std::any_cast<framework::StreamDataChunk>(CHUNK)


const std::string ClientUiTask::WELCOME = "\nWelcome to the Message Store. Your options are: \n";
const std::string ClientUiTask::ADD =  "1. Add a new user to Message Store";
const std::string ClientUiTask::SEND = "2. Send a message to a user";
const std::string ClientUiTask::RECV = "3. Receive all messages bound to a user";
const std::string ClientUiTask::EXIT = "4. Exit Message Store";
const std::string ClientUiTask::GOOD_BYE = "Thank you for using Message Store. Drop by any time!";
const std::string ClientUiTask::BAD_OPT = "Bad option selected. Please try again";
const std::string ClientUiTask::USR_ADD = "Please enter your first and last name, separated by a space ";
const std::string ClientUiTask::SEND_USR_FROM = "Please enter the first and last name of the user "
                                                "__FROM__ which the message is sent, separated by space." ;
const std::string ClientUiTask::SEND_USR_TO = "Please enter the first and last name of the user "
                                              "__TO__ which the message is sent, separated by space." ;
const std::string ClientUiTask::NO_SUCH_USER = "The user you have requested does not exist in the system. Please try again.";
const std::string ClientUiTask::USER_MSG = "Please enter the message you wish to send [128 chars max].";
const std::string ClientUiTask::RCV_USR = "Please enter the first and last name of the user for which you "
                                          "would like to receive messages, separated by space";
const std::string ClientUiTask::USR_EXIST =  "The user you chose to add exists in the system.";
const uint8_t ClientUiTask::MAX_USR_NAME = 20;
const uint8_t ClientUiTask::MAX_USR_LAST = 20;
const uint8_t ClientUiTask::MAX_MSG_SIZE = 128;


// this ui thread works with the std streams. but
// you can plugin whatever you like here ...
// cant make it a template, as the init way is not enforced
// by the interface, and therefor is not unified for all impls
ClientUiTask::ClientUiTask() : m_thread(nullptr) {

    m_ioDevice =
            std::make_unique<framework::StdStreamIODevice>(std::cin,
                                                           std::cout);
}

ClientUiTask::~ClientUiTask() {
    // has to be here otherwise the program might std::terminate
    m_keepRunning = false;
    if(m_thread->joinable())
        m_thread->join();
}

void ClientUiTask::startThread()
{
    m_thread = std::make_unique<std::thread>(&ClientUiTask::runUiLoop, this);
}

void ClientUiTask::waitForUiThread()
{
    if(m_thread->joinable())
        m_thread->join();
}


void ClientUiTask::runUiLoop() {
    do {
        // MAIN UI LOOP
        UserOptions opt = displayUserOptions();
        handleUserRequest(opt);
    } while (m_keepRunning);
}

ClientUiTask::UserOptions ClientUiTask::displayUserOptions()
{
    UserOptions chosenOpt = UserOptions::BAD_OPT;

    displaySingleMessage(WELCOME);
    displaySingleMessage(ADD);
    displaySingleMessage(SEND);
    displaySingleMessage(RECV);
    displaySingleMessage(EXIT);

    chosenOpt = readUserRequest();

    return chosenOpt;
}


ClientUiTask::UserOptions ClientUiTask::readUserRequest()
{
    framework::StreamDataChunk chunk = readLineFromStream(1);

    switch (chunk[0])
    {
        case '1': return UserOptions::ADD_USER;
        case '2': return UserOptions::SEND_MSG;
        case '3': return UserOptions::RECV_ALL_MSGS;
        case '4': return UserOptions::EXIT;
        default:  return UserOptions::BAD_OPT;
    }
}

void ClientUiTask::handleUserRequest(UserOptions opt)
{
    switch (opt)
    {

        case UserOptions::ADD_USER:
        {
            handleAddUser();
            break;
        }
        case UserOptions::SEND_MSG:
        {
            handleSendToUser();
            break;
        }
        case UserOptions::RECV_ALL_MSGS:
        {
            handleRecvForUser();
            break;
        }

        case UserOptions::BAD_OPT:
        {
            displaySingleMessage(BAD_OPT);
            break;
        }
        case UserOptions::EXIT:
        {
            displaySingleMessage(GOOD_BYE);
            m_keepRunning = false;
            break;
        }

        default: throw;
    }
}

void ClientUiTask::handleAddUser()
{
    displaySingleMessage(USR_ADD);

    UserManager::User user =
            parseUserFromInput(MAX_USR_NAME + MAX_USR_LAST);

    if( UserManager::ReturnCodes::EXISTS ==
            m_userManager.addUser(std::move(user)))
        displaySingleMessage(USR_EXIST);
}

void ClientUiTask::handleSendToUser()
{
    displaySingleMessage(SEND_USR_FROM);

    UserManager::User userFrom = parseUserFromInput(MAX_USR_NAME + MAX_USR_LAST);

    if(false == m_userManager.doesUserExist(userFrom)) {
        displaySingleMessage(NO_SUCH_USER);
        return;
    }

    displaySingleMessage(SEND_USR_TO);
    UserManager::User userTo = parseUserFromInput(MAX_USR_NAME + MAX_USR_LAST);

    if(false == m_userManager.doesUserExist(userTo))  {
        displaySingleMessage(NO_SUCH_USER);
        return;
    }


    displaySingleMessage(USER_MSG);
    framework::StreamDataChunk chunkMsg = readLineFromStream(MAX_MSG_SIZE);

    UserManager::UserMsg msg {userFrom,
                              userTo,
                              std::string(chunkMsg.data(), chunkMsg.size())};

    m_userManager.sendToUser(msg);
}

void ClientUiTask::handleRecvForUser()
{
    displaySingleMessage(RCV_USR);

    UserManager::User user =
            parseUserFromInput(MAX_USR_NAME + MAX_USR_LAST);

    if(false == m_userManager.doesUserExist(user))
    {
        displaySingleMessage(NO_SUCH_USER);
        return;
    }

    UserManager::Messages msgs = m_userManager.getUserMessages(user);
    for(auto& msg : msgs)
    {
        displaySingleMessage(msg.m_msg);
    }

}

////////////////////////// UTILITIES ///////////////////////////////////
void ClientUiTask::displaySingleMessage(std::string prompt)
{
    framework::StreamDataChunk chunk {prompt.begin(), prompt.end()} ;
    m_ioDevice->
            writeData(ANY_CAST(chunk), chunk.size());
}

framework::StreamDataChunk
ClientUiTask::readLineFromStream(std::uint64_t len)
{
    framework::StreamDataChunk chunk =
            ANY_CAST(m_ioDevice->readData(len));

    // discard newline if exists
    if(chunk[chunk.size()-1] != '\n') {
        m_ioDevice->ignoreTillChar('\n');
    }
    else
        chunk.resize(chunk.size()-1);

    return chunk;
}

UserManager::User
ClientUiTask::parseUserFromInput(std::uint64_t  lenToRead)
{
    framework::StreamDataChunk chunkFirstLast =
            readLineFromStream(lenToRead);

    // very fragile. no sanity checks!!!
    std::string first, last;
    std::stringstream ss{ std::string(chunkFirstLast.data(), chunkFirstLast.size())};
    std::getline(ss, first, ' ');
    std::getline(ss, last, '\0');

    UserManager::User user{.m_firstName = first,
            .m_lastName = last};
    return user;
}

} // namespace msg_store



