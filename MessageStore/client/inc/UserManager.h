#pragma once

#ifndef MESSAGESTORE_USERMANAGER_H
#define MESSAGESTORE_USERMANAGER_H

#include <string>
#include <memory>
#include <map>

namespace msg_store{


/*
* this class represents a database of users in the msg-store system.
* this class provides apis for accessing, and manipulating the database.
* the db manages its own memory.
* this database assumes that a user's combination of first and last names
* are unique.
* a good design decision here would be to place this logic in a separate thread
* possibly the server thread. this thread could handle stuff like:
* persistence of the user's data.
*/
class UserManager
{

public:

    enum class ReturnCodes {SUCCESS, NOT_EXISTS, EXISTS, BAD_OPT};
    struct UserMsg;
    using Messages = std::vector<UserMsg>;

    struct User
    {
        User(std::string f, std::string l) :
            m_firstName(std::move(f)), m_lastName(std::move(l)){}
        std::string m_firstName;
        std::string m_lastName;
    };


    struct UserMsg
    {
        User m_from;
        User m_to;
        std::string m_msg;
    };

    // arg1 - user first name
    // arg2 - user last name
    ReturnCodes addUser(User);

    ReturnCodes recvForUser(User);

    void sendToUser(UserMsg);

    bool doesUserExist(User);

    Messages getUserMessages(User);




private:


    class MsgStoreUserBox
    {
    public:
        void addMessage(UserMsg msg){m_msgs.push_back(msg);}
        Messages getUserMessages() { return m_msgs;}
        void clearUsersBox(){m_msgs.clear();}

    private:
        Messages m_msgs;

        // .. other satellite data you want to keep on a user
    };

    using MsgStoreUserBoxPtr = std::unique_ptr<MsgStoreUserBox>;
    using UserUniqueHash = size_t;
    using UserHashToUsrBoxPtrMap =  std::map<UserUniqueHash, MsgStoreUserBoxPtr>;
    using UserHashToUsrBoxPtrItr = UserHashToUsrBoxPtrMap::iterator;
    const UserHashToUsrBoxPtrItr END_OF_MAP = m_usersByHash.end();

    UserHashToUsrBoxPtrItr findUserBox(User);

    UserHashToUsrBoxPtrMap m_usersByHash;
};

}


#endif //MESSAGESTORE_USERMANAGER_H
