// Tests all network interfaces
// Focuses on the client side

#include <string.h>
#include <stdio.h> // For printf
#include <stdlib.h> // For exit(3)
#include "network_interface.h"

static int send_create();
static int send_select();
static int send_insert();
static void prepareEnv(int argc, char* argv[]);
static void usage();
static unsigned long s_port;

int main(int argc, char* argv[])
{
  prepareEnv(argc, argv);
  if(0 == strcmp(argv[2],"cr"))
    send_create();
  if(0 == strcmp(argv[2],"sl"))
     send_select();
   if(0 == strcmp(argv[2],"in"))
     send_insert();
  
  return 0;
}


int send_create()
{
  BlockingTcpTransmitor transmitor(s_port);
  char buff[sizeof(mem_db::NetworkMsg)];
  (*(reinterpret_cast<mem_db::NetworkMsg*>(buff))).qTyp=mem_db::CR;
  (*(reinterpret_cast<mem_db::NetworkMsg*>(buff))).paramSz=0;
  (*(reinterpret_cast<mem_db::NetworkMsg*>(buff))).layNmSz=4;
  unsigned int pnt=0;
  memcpy(buff, "ERAN", 4);
  pnt+=4;
  *(reinterpret_cast<unsigned int*>(buff+pnt))=3;
  pnt+=sizeof(unsigned int);
  memcpy(buff+pnt, "age", 3);
  pnt+=3;
  *(reinterpret_cast<unsigned int*>(buff+pnt))=3;
  pnt+=sizeof(unsigned int);
  memcpy(buff+pnt, "int", 3);
  pnt+=3;
  *(reinterpret_cast<unsigned int*>(buff+pnt))=6;
  pnt+=sizeof(unsigned int);
  memcpy(buff+pnt, "height", 6);
  pnt+=6;
  *(reinterpret_cast<unsigned int*>(buff+pnt))=5;
  pnt+=sizeof(unsigned int);
  memcpy(buff+pnt, "float", 5);
  pnt+=5;
  *(reinterpret_cast<unsigned int*>(buff+pnt))=5;
  pnt+=sizeof(unsigned int);
  memcpy(buff+pnt, "color", 5);
  pnt+=5;
  *(reinterpret_cast<unsigned int*>(buff+pnt))=4;
  pnt+=sizeof(unsigned int);
  memcpy(buff+pnt, "varc", 4);
  //pnt+=4; // not taking into account the laynm

   (*(reinterpret_cast<mem_db::NetworkMsg*>(buff))).paramSz=pnt;
    
  return transmitor.writeData(sizeof(mem_db::NetworkMsg), buff);
}

void prepareEnv(int argc, char* argv[])
{
  if(3 > argc)
    usage();
  char buff[16];
  char* pbuff = buff;
  s_port = strtol(argv[1], &pbuff, 10);

  if(*pbuff != '\0')
  {
    printf("port not a valid numbrt %s\n", argv[1]);
    usage();
  }
}



int send_select()
{
   BlockingTcpTransmitor transmitor(s_port);
  char buff[sizeof(mem_db::NetworkMsg)];
  (*(reinterpret_cast<mem_db::NetworkMsg*>(buff))).qTyp=mem_db::SEL;
  (*(reinterpret_cast<mem_db::NetworkMsg*>(buff))).paramSz=0;
  (*(reinterpret_cast<mem_db::NetworkMsg*>(buff))).layNmSz=4;
  unsigned int pnt=0;
  memcpy(buff, "ERAN", 4);
  pnt+=4;
  *(reinterpret_cast<unsigned int*>(buff+pnt))=3;
  pnt+=sizeof(unsigned int);
  memcpy(buff+pnt, "age", 3);
  pnt+=3;
  *(reinterpret_cast<unsigned int*>(buff+pnt))=3;
  pnt+=sizeof(unsigned int);
  (*(reinterpret_cast<mem_db::NetworkMsg*>(buff))).paramSz=pnt-4;
  return transmitor.writeData(sizeof(mem_db::NetworkMsg), buff);
}

int send_insert()
{
  BlockingTcpTransmitor transmitor(s_port);
  char buff[sizeof(mem_db::NetworkMsg)];
  (*(reinterpret_cast<mem_db::NetworkMsg*>(buff))).qTyp=mem_db::INST;
  (*(reinterpret_cast<mem_db::NetworkMsg*>(buff))).paramSz=0;
  (*(reinterpret_cast<mem_db::NetworkMsg*>(buff))).layNmSz=4;
  unsigned int pnt=0;
  memcpy(buff, "ERAN", 4);
  pnt+=4;
  
  memcpy(buff+pnt, "age", 3);
  pnt+=3;
  *(reinterpret_cast<int*>(buff+pnt)) = 36;
  pnt+=sizeof(int);


  memcpy(buff+pnt, "height", 3);
  pnt+=3;
  *(reinterpret_cast<float*>(buff+pnt)) = 1.84;
  pnt+=sizeof(float);


  memcpy(buff+pnt, "color", 3);
  pnt+=3;
  memcpy(buff+pnt, "blue", 4);
  pnt+=4;

  return transmitor.writeData(sizeof(mem_db::NetworkMsg), buff);

}

void usage()
{
  printf("USAGE: COM_SEND_TEST <port number> [host name[localhost]] ACT\n");
  exit(1);
}
