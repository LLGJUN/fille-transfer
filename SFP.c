#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/wait.h>
#include<sys/types.h>
#define MAXSIZE 1024

int sendfile(char *serverip,char *port,char *filename)
{
  struct sockaddr_in sockaddr;
  int sockfd;
  int netport;
  int sta0,sta1;
  int filelen;
  FILE *file;
  char *buf;
  file=fopen(filename,"rb");
  if(file<0)
    {
      perror("file-open failed:");
      return -1;
    }
  if(file>0)
    {
      //printf("file-open ok!\n");
    }
  fseek(file,0,SEEK_END);
  filelen=ftell(file);
  //printf("the length of file %s is %d\n",filename,filelen);
  fseek(file,0,SEEK_SET);
  netport=atoi(port);
  bzero(&sockaddr,sizeof(sockaddr));
  sockaddr.sin_family=AF_INET;
  sockaddr.sin_port=htons(netport);
  inet_aton(serverip,&sockaddr.sin_addr);
  sockfd=socket(AF_INET,SOCK_STREAM,0);
  if(sockfd<0)
    {
      perror("init sock failed!");
      return -1;
    }
  else
    {
      //printf("init sock ok!\n");
    }
  sta0=connect(sockfd,(struct sockaddr*)&sockaddr,sizeof(sockaddr));
  if(sta0<0)
    {
      printf("=====");
      printf("server(%s)",serverip);
      perror("connect failed");
      return -1;
    }
  else
    {
      //printf("sock connect ok!\n");
    }
  //printf("the file is sending....\n");
  //it would be exist a mistake if the read-length more than file length.
  //send(sockfd,&filelen,sizeof(int),0);
  //while(recv(socket,))
  while((sta1=fread(buf,1,MAXSIZE,file))>0)
    {
      // printf("the read bytes is %d\n",sta1);
      //printf("the buf string is %s\n",buf);
      /*
      if(feof(file))
	{
	  printf("the pointer is to the end of file\n");
	  }*/
      send(sockfd,buf,sta1,0);
    }   
  return 0;
}

int main(int argc,char **argv)
{
  if(argc<2)
    {
      printf("please input recvive ip address!\n");
      exit(1);
    }
  if(argc<3)
    {
      printf("please input recvive port number!\n");
      exit(1);
    }
  if(argc<4)
    {
      printf("please input filename  address!\n");
      exit(1);
    }
  pid_t child[argc-3];
  int i=0;
  for(i=0;i<argc-3;i++)
    {
      if((child[i]=fork())==-1)
	{
	  perror("fork create error:");
	  exit(1);
	}
      else if(child[i]==0)
	{
	  //printf("in child %d \n",i+1);
	  // printf("   child pid :%d\n",getpid());
	  // printf("   child ppid :%d\n",getppid());
         
	  if(sendfile(argv[i+1],argv[argc-2],argv[argc-1])==0)
	    {
	      printf("server(%s):File has been sent completely\n",argv[i+1]);
	    }
	  else
	    {
	      printf("server(%s):File transmission has been breakdown \n",argv[i+1]);
	    }
	  exit(EXIT_SUCCESS);
	}
    }
  int status[argc-3];
  //printf("in parent parent pid %d\n",getpid());
  for(i=0;i<argc-3;i++)
    {
      waitpid(child[i],&status[i],0);
      //printf("child %d eixt with status %d\n",i+1,status[i]);
    }
  return 0;
}
