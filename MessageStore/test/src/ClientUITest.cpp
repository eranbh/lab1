#include "inc/StubIODevice.h"
#include "inc/ClientUITest.h"

namespace msg_store {

ClientUITest::ClientUITest()
{
    m_clientUITask.m_ioDevice = std::make_unique<framework::StubIODevice>();
}

void ClientUITest::test_handleAddUser()
{
    m_clientUITask.handleAddUser();
    //ASSERT(stuff == other_stuff)
    //ASSERT(stuff == other_stuff)
    //ASSERT(stuff == other_stuff)
}

void ClientUITest::test_handleSendToUser()
{
    m_clientUITask.handleSendToUser();
}

void ClientUITest::test_handleRecvAllForUser()
{
    m_clientUITask.handleRecvForUser();
}

} // namespace msg_store

