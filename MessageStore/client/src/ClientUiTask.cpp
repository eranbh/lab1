//
// Created by Eran Ben Horin on 29/03/2018.
//
#include "inc/StdStreamIODevice.h"
#include "inc/ClientUiTask.h"


namespace msg_store {


ClientUiTask::ClientUiTask() : m_thread(nullptr) {
    // this ui thread works with the std streams. but
    // you can plugin whatever you like here ...
    m_ioDevice =
            std::make_unique<framework::StdStreamIODevice>(std::cin,
                                                           std::cout);
}

ClientUiTask::~ClientUiTask() {
    m_keepRunning = false;
    if(m_thread)
        m_thread->join();
}

void ClientUiTask::startThread()
{
    m_thread = std::make_unique<std::thread>(&ClientUiTask::runUiLoop, this);
}


void ClientUiTask::runUiLoop() {
    do {

        framework::StreamDataChunk chunk =
                std::any_cast<framework::StreamDataChunk>(m_ioDevice->readData(4));
        m_ioDevice->
                writeData(std::any_cast<framework::StreamDataChunk>(chunk), chunk.size());
    } while (m_keepRunning);
}

} // namespace msg_store



