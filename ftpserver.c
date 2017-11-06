
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
	int sockfd,ret,newsockfd;
	struct sockaddr_in addr, newaddr;
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

	ret = bind(sockfd, (struct sockaddr *)&addr,sizeof(addr));
	if(ret == -1)
	{

	  perror("bind");
	  return -1;

	}


	ret = listen(sockfd,5);
	if(ret == -1)
	{

	  perror("listen");
	  return -1;

	}

	while(1)
	{
	 printf("%s\n","please wait the connect");
  	int newaddrlen = sizeof(newaddr);
    	newsockfd = accept(sockfd,(struct sockaddr *)&newaddr,&newaddrlen);
	 printf("%d\n",newsockfd);
	 char buf[1024];

	      memset(buf,0,sizeof(buf));
		ret = read(newsockfd,buf,sizeof(buf-1));
		 printf("%s\n",buf);
		 if(ret !=-1)
		 {
			 printf("%s\n","I have receive the data");

		 }
		 buf[1024]="1234567890";
		 write(newsockfd,buf,sizeof(buf));


	}

	 close(sockfd);
	 close(newsockfd);
	 sleep(10);

}

