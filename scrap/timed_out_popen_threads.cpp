#include<stdio.h> // popen
#include <signal.h> // for kill (2)
#include<iostream>
#include <future>
#include <thread>
#include <chrono>
#include <unistd.h> // fork (2), getpid(2)



static void
executeSubProcess(std::string cmd){
	char buff[128];
	FILE *fp = popen(cmd.c_str(), "r");
    if( !fp ){
       // handle the issue with popen.
       //this might be cause because of fork or malloc failur
    }

	char* res = fgets(buff, 128, fp);
	if(res)
	   std::cout << " sub-process with pid: " << getpid() << 
              " got result: " << buff;
}


int
main(int argc, char* argv[]){

	std::cout << " starting parent process. pid: " << getpid() << std::endl;

    std::promise<int> cmdPromise;
    std::future<int> cmdFuture = cmdPromise.get_future();
    
    std::thread( [argv] ( std::promise<int> cmdPromise )
                  {
	                std::cout << " spawned thread. pid:  " << std::this_thread::get_id << std::endl;
                    executeSubProcess(argv[1]);
                    cmdPromise.set_value_at_thread_exit(1); 
                  }, std::move(cmdPromise) ).detach();
    std::chrono::system_clock::time_point two_seconds_passed
            = std::chrono::system_clock::now() + std::chrono::seconds(2);
    if(std::future_status::ready == cmdFuture.wait_until(two_seconds_passed))
        { std::cout << "f_completes: " << cmdFuture.get() << "\n"; }
    else
        { std::cout << "f_completes did not complete!\n"; }
}

