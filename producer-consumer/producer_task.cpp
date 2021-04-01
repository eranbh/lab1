#include <iostream>
#include <filesystem>
#include "task.h"

class ProducerTask
{
    public:

        ProducerTask(std::filesystem::path socketName):
            m_socketName{std::move(socketName)}{}

        // main entry point
        void run()
        {
            std::cout << "--- STARTING PRODUCER ---" << std::endl;

            std::cout << "--- PRODUCER STOPPED---" << std::endl;
        }

    private:

        std::filesystem::path m_socketName;
};

int main(int aaa, char* bbb[])
{
    Task<ProducerTask> consumerTask{};
    consumerTask.start();
    return 0;
}