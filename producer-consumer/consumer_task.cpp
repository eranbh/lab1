#include <iostream>
#include "task.h"

class ConsumerTask
{
    public:

        ConsumerTask(std::string socketPath)
        {}

        // main entry point
        void run()
        {
            std::cout << "--- STARTING CONSUMER ---" << std::endl;

            std::cout << "--- CONSUMER STOPPED---" << std::endl;
        }
};

int main(int aaa, char* bbb[])
{
    Task<ConsumerTask> consumerTask{};
    consumerTask.start();
    return 0;
}