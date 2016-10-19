/*
* a poc program for making sure using this mecnanism
* on a cellular device is even possible.
* there will be 2 run modes for the tester:
* 1. no argumets
     as this tester will run in a syntetic env, its first
*    order of business will be to set up an fs on which it
*    will work.
* 2. absolute path to a directory
*    the user sends the watch-point as an argument.
*/
#include <unistd.h> // for read(2)
#include <stdlib.h> // for exit(3)
#include <string.h> // for strerror(3)
#include <errno.h> // well ... you know what this is for
#include <sys/inotify.h> // for inotify_init(2)
#include <sys/stat.h> // for stat(2)
#include <functional> // for std::hash
#include<iostream> // for terminal related io


static const char* const USAGE="USAGE: inotify_test <watch-point absolute path>"; 

// dear god !!! make this a member
static std::string s_watchPointPath;


using DirNameHash2FdMap = std::map<size_t, int>;
static DirNameHash2FdMap s_dirNmHash2FdMap;

static void 
logFatalAndExit(const char* const msg)
{
    std::cerr << "Fatal error occured. Reason is: " << msg << std::endl;
    exit(1);
}

static int
startInotifyWatchList()
{
    int wlFd = 0;
    if(0 > (wlFd=inotify_init()))
      logFatalAndExit(strerror(errno));
    return wlFd;
}

// keep in mind that the type of the other
// argument should be kept - its inotify(7)
static int
addPath2WatchList(int         wl, 
                  const char* path, 
                  uint32_t    mask)
{
   int wp = 0;
   if(0 > (wp=inotify_add_watch(wl, path, mask)))
       logFatalAndExit(strerror(errno));

   return wp; 
}

// use this for debugging
static void 
printEvent(inotify_event* pevent)
{
    std::cout << "new event just in for descriptor " << 
                           pevent->wd << ":" << std::endl;
    std::cout << "mask: " << pevent->mask << std::endl;
    std::cout << "cookie: " << pevent->cookie << std::endl;
    std::cout << "len: " << pevent->len << std::endl;
    std::cout << "name: " << pevent->name << std::endl;
}


// TODO this should be a handler function
// for now, we just check if this file is
// setuid. if it is ... we meed to decide
// what to do? maybe only alert if we're talking
// about a process owned by root?
static void 
probeEvent(inotify_event*)
{
    
}

void 
listen2Events(int wlFd)
{
   // one buffer assumes a max path of 256 chars
   // we attempt to read 1024 each time.
   const int EVENT_SIZE =sizeof(struct inotify_event);
   const int EVENT_BUFFER_SZ = 1024*(EVENT_SIZE+256);
   char eventBuffer[EVENT_BUFFER_SZ];
   struct inotify_event* pevent;

   // read loop. this will stop once there is a
   // "hangup" by the party writing to the watch list 
   // TODO kill the server loop with a signal

   while(1) // may you live forever
   {
       // read as much data as possible
       int bytesRead=0/*, bytes2Read=EVENT_BUFFER_SZ*/;
       if(0 > (bytesRead=read(wlFd, &eventBuffer, EVENT_BUFFER_SZ)))
           logFatalAndExit(strerror(errno));

       // lets see what the cat dragged in
       // we must have - at least - an event
       if(bytesRead < EVENT_SIZE) logFatalAndExit(strerror(errno));
      
       pevent = 
            reinterpret_cast<struct inotify_event*>(eventBuffer);

       // if this is a dir, add its path
       if((pevent->mask & IN_ISDIR) &&
          (pevent->mask & IN_CREATE))
       {
           std::string newDir (s_watchPointPath + ("/") + pevent->name);
           std::cout << "directory name: " << newDir << std::endl;
           // hash the path
           std::size_t dirNameHash = std::hash<std::string>{}(newDir);
           std::cout << "hash value is: " << dirNameHash << std::endl;
           // add the key-value pair to our map
           addPath2WatchList(wlFd, 
                             newDir.c_str(), 
                             IN_CREATE | IN_DELETE | IN_DELETE_SELF | IN_ATTRIB);
       }

       while(0 < bytesRead)
       {
            probeEvent(pevent);
            printEvent(pevent);
            int fullEventSz = EVENT_SIZE+pevent->len;
            bytesRead-=fullEventSz;
            pevent = 
               reinterpret_cast<struct inotify_event*>(eventBuffer+bytesRead);
       }
      
   }
}

int
main(int argc, char* argv[])
{
   if(argc > 1)
   {
     // a user provided watchpoint
  
     if(0 == argv[1]) logFatalAndExit(USAGE);
     // validate the fact that this is a dir
     struct stat fileMetaData={0};
     if(0 !=  stat(argv[1], &fileMetaData))
        logFatalAndExit(strerror(errno));

     if(0 == S_ISDIR(fileMetaData.st_mode)) 
        logFatalAndExit("Path does not lead to directory");
   
     // get the real path out of the watch point
     // this is needed if we want accurate paths
     s_watchPointPath = realpath(argv[1], 0);
   }
   else
   {
      // we create our own watchpoint
      logFatalAndExit("USAGE [for now]");
   }

    // START INOTIFY
   int wlFd = startInotifyWatchList();


   int wpFd = 
     addPath2WatchList(wlFd, 
                       s_watchPointPath.c_str(), 
                       IN_CREATE | IN_DELETE | IN_DELETE_SELF | IN_ATTRIB);

   (void)wpFd; // we dont need this here. only one nesting level
  
   // Listening to events

   // this will block untill all events were read.
   listen2Events(wlFd);
   return 0;    
}
