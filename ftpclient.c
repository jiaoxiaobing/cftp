/*
 * ftpclient.c
 *
 *  Created on: 2017年11月6日
 *      Author: jiaobing
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 9990
#define SIZE 1024
#define host "192.168.0.103"


int main()
{
	int sockfd,ret,confd;
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

	 printf("%s\n","success to connect the host");
    char buffer[1024];
while(1){
		 printf("input the value: ");
		 scanf("%s",buffer);
		 write(sockfd,buffer,strlen(buffer));
		 memset(buffer,0,sizeof(buffer));
		 ret = read(sockfd,buffer,strlen(buffer));
		 printf("%s\n",buffer);
	   close(sockfd);

       sleep(10);


}
}

