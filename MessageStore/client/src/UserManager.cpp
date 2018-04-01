#include <functional>
#include "inc/UserManager.h"

namespace msg_store {

UserManager::ReturnCodes
UserManager::addUser(User user)
{

    // find is cheap with maps. so run it before allocating stuff
    if(doesUserExist(user))
        return ReturnCodes::EXISTS;

    std::string fullName{user.m_firstName + user.m_lastName};
    UserUniqueHash userHash = std::hash<std::string>{}(fullName);

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
    return (findUserBox(user) != END_OF_MAP);
}


UserManager::UserHashToUsrBoxPtrItr
UserManager::findUserBox(User user)
{
    std::string fullName{user.m_firstName + user.m_lastName};
    UserUniqueHash userHash = std::hash<std::string>{}(fullName);

    return m_usersByHash.find(userHash);
}

UserManager::Messages UserManager::getUserMessages(User user)
{
    UserHashToUsrBoxPtrItr iter = findUserBox(user);
    if (END_OF_MAP == iter)
        return Messages{};
    Messages msgs = iter->second->getUserMessages();
    iter->second->clearUsersBox(); // clear the user's box
    return msgs;
}

} // namespace msg_store
