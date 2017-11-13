/*
 * ftpclient.c
 *
 *  Created on: 2017年11月6日
 *      Author: jiaobing
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <limits.h>
#include <dirent.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#define PORT 21
struct restore gethost();
struct restore
{
   char *a;
   char b[100];
}x,y;

int main()
{
    struct restore x = gethost();
	 printf("%s\n",x.a);
	  printf("%s\n","ffeweff");
	  printf("%s\n",x.b);
      sleep(10);


	/*int sockfd,ret,confd;
	struct sockaddr_in addr;
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd == -1)
	{
		perror("socket");
         return -1;

	}
    printf("%d\n",sockfd);

	addr.sin_family = AF_INET;
	addr.sin_port =htons(PORT);
	addr.sin_addr.s_addr= inet_addr(host);

   confd = connect(sockfd,(struct sockaddr *)&addr, sizeof(addr));
   if(confd == -1)
   	{
   		perror("connect");
            return -1;

   	}

	 printf("%s\n","success to connect the host");*/
}

struct restore gethost()
 {
		char ch;
		char fread[1024],ip[10];
		int i=0;
		char *ptr;
		char fullpath[200];
		struct dirent *dirp;
		DIR *dp;
	 FILE *pfread = fopen("D:/dabao/ftpdir.txt","r+");
	 while(ch != '\n')
	 {
		 ch = fgetc(pfread);
		 if(ch != '\n')
		      {
			     fread[i]=ch;
		      }
		 i++;
	  }
   printf("%s\n",fread);
   ptr =strtok(fread," ");
   strcpy(ip,ptr);
   printf("%s\n",ip);
    ptr =strtok(NULL,"\r");
     printf("%s\n",ptr);
   if((dp = opendir(ptr)) == NULL)
	{
           perror("DP");

		   }

   while ((dirp = readdir(dp)) != NULL)
   {
	  sprintf(fullpath,"%s/%s",ptr,dirp->d_name);
   }
   x.a=ip;
   strcpy(x.b,fullpath);
   printf("%s\n",x.b);
   return x;
 }














