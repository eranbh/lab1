//
// Created by Eran Ben Horin on 30/03/2018.
//

#ifndef MESSAGESTORE_USERMANAGER_H
#define MESSAGESTORE_USERMANAGER_H

#include <string>
#include <memory>
#include <map>

namespace msg_store{


/*
* this class represents a database of users in the msg-store system.
* this class provides apis for accessing, and manipultating the database.
* the db manages its own memory.
* this database assumes that a user's combination of first and last names
* are unique.
*/
class UserManager
{

public:

    enum class ReturnCodes {SUCCESS, NOT_EXISTS, EXISTS, BAD_OPT};

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


private:


    class MsgStoreUserBox
    {
    public:
        void addMessage(UserMsg msg){m_msgs.push_back(msg);}

    private:
        std::vector<UserMsg> m_msgs;

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
