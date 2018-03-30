//
// Created by Eran Ben Horin on 30/03/2018.
//
#include <functional>
#include "inc/UserManager.h"

namespace msg_store {

UserManager::ReturnCodes
UserManager::addUser(User user)
{

    // find is cheap with maps. so run it before allocating stuff
    if( END_OF_MAP != findUserBox(User(user.m_firstName, user.m_lastName)))
        return ReturnCodes::EXISTS;

    UserUniqueHash userHash = std::hash<std::string>{}(user.m_firstName + user.m_lastName);

    MsgStoreUserBoxPtr userPtr = std::make_unique<MsgStoreUserBox>();

    // this is OK, cause I ran the find first, and there will be no collisions
    m_usersByHash[userHash] = std::move(userPtr);

    return ReturnCodes::SUCCESS;
}


void UserManager::sendToUser(UserMsg msg)
{
    User user{msg.m_to.m_firstName, msg.m_to.m_lastName};
    UserHashToUsrBoxPtrItr iter = findUserBox(user);

    // put msg in box
    iter->second->addMessage(msg);
}


bool UserManager::doesUserExist(User user)
{
    return (findUserBox(User(user.m_firstName, user.m_lastName)) == END_OF_MAP);
}


UserManager::UserHashToUsrBoxPtrItr
UserManager::findUserBox(User user)
{
    UserUniqueHash userHash = std::hash<std::string>{}(user.m_firstName + user.m_lastName);

    return m_usersByHash.find(userHash);
}

} // namespace msg_store
