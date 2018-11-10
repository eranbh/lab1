#include<stdio.h> // popen
#include <unistd.h> // fork (2), getpid(2)
#include <sys/wait.h> // waitpid
#include <signal.h> // for kill (2)
#include<iostream>


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
	
	int forkRes = fork();
	switch(forkRes){
		case -1:
		{
			std::cout << "fork failed. exiting" << std::endl;
			exit(1);
		}
		case 0:
		{
			
			std::cout << " sub-process. pid: " << getpid() << " spawned"<< std::endl;
			executeSubProcess(argv[1]);	
			exit(errno);
		}
		default:
		{
				int secs = 0, wpid = 0;
			do{
				int status=0;
				wpid = waitpid(forkRes, &status, WNOHANG);
				if(wpid == 0 && secs > 3){
					std::cout << "killing sub-process. pid: " << forkRes << std::endl;
					kill(forkRes, SIGKILL);
					break;
				}else{
					sleep(1);
					secs++;
				}
				
				
			}while(0 == wpid);
		}
	}
	return 0;
}
