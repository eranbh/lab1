#include<stdio.h> // popen
#include<string.h> // strerror
#include <unistd.h> // fork (2), getpid(2)
#include <sys/wait.h> // waitpid
#include <signal.h> // for kill (2)
#include<iostream>


int
main(int argc, char* argv[]){

	std::cout << " starting parent process. pid: " << getpid() << std::endl;
    int pipeFds[2];

    if( -1 == pipe(pipeFds)){
			std::cout << "pipe failed. exiting" << std::endl;
    }
        
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
		        close(pipeFds[0]);
            		dup2(pipeFds[1],1);
            		close(pipeFds[1]);
                    close(3); // dont care about errors
			execlp("/Users/eranb/work/lab/scrap/test_script.sh", "test_script.sh", "10", (char*)0);
			std::cout << "execl failed " << strerror(errno) << std::endl; 
			exit(errno);
		}
		default:
		{
	                close(pipeFds[1]);
        	        dup2(pipeFds[0],0); 
                	close(pipeFds[0]);
			int secs = 0, wpid = 0;
			do{
				int status=0;
				wpid = waitpid(forkRes, &status, WNOHANG);
				if(wpid == 0 && secs > 3){
					std::cout << "killing sub-process. pid: " << forkRes << std::endl;
					kill(forkRes, SIGKILL);
					exit(1);
				}else{
					sleep(1);
					secs++;
				}
				
				
			}while(0 == wpid);
	        char buff[128];
            //char* res = fgets(buff, 128, 0);
            int res = read(0, buff, sizeof(buff));
            if(res)
	            //std::cout << " sub-process with pid: " << getpid() << 
                //    " got result: " << buff;
                write(1, buff, res);

		}
	}
	return 0;
}
