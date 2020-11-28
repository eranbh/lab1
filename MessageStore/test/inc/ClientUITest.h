#pragma once
/*
 *  this file contain the unit-test level testing for the ClientUI task.
 *  this probably would be better off implemented over some unit testing
 *  framework like cpptest, or even better - gtest.
 *  this is just an example of how through our design, we can plus in
 *  some other IO device, and get better control over the results.
 */
#ifndef MESSAGESTORE_CLIENTUITEST_H
#define MESSAGESTORE_CLIENTUITEST_H
#include "inc/ClientUITask.h"

namespace msg_store {


class ClientUITest {

// the visibility is usually protected, because the way this works is
// you inherit from a framework object that registers your class to a suite
protected:

    /* class level initializations */
    ClientUITest();

    // test case level setup / tear downs
    void setUp(){}

    void tearDown(){}

    // individual flow test cases
    void test_handleAddUser();

    void test_handleSendToUser();

    void test_handleRecvAllForUser();

    // .....


private:

    msg_store::ClientUiTask m_clientUITask;

};

}

#endif //MESSAGESTORE_CLIENTUITEST_H
