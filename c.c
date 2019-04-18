#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/time.h>
#include <stdlib.h>
#include <ncurses.h>

#define SERVER "10.115.20.250"
#define PORT 28900

char * name;

int connect2v4stream(char *IP_adr){ //both
	struct sockaddr_in server;
	int sockd, ret;

	sockd = socket(AF_INET,SOCK_STREAM,0);
	if(sockd == -1){
		printf("%s\n", strerror(errno));
		return sockd;
	}
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT); //converts port number

	ret = inet_pton(AF_INET, IP_adr, &server.sin_addr);
 	if(ret == -1){
 		printf("%s\n", strerror(errno));
 		return sockd;
 	}

 	ret = connect(sockd, (struct sockaddr *)&server, sizeof(server));
	if(ret == -1){
		printf("%s\n", strerror(errno));
		return sockd;
	}

	return sockd;
}

void update_time(){ //either
	int ret, sockd;
	char *message;

	sockd = connect2v4stream(SERVER);

	message = malloc(100);
	sprintf(message, "GET /?i=%s&uptime=60 HTTP/1.1\r\nHost: pilot.westmont.edu:28900\r\n\r\n", name);

	ret = write(sockd, message, strlen(message));
	if(ret == -1){
		printf("%s\n", strerror(errno));
		exit(errno);
	}
	free(message);
	close(sockd);
	return;
}

char *getuser()
{
	name = malloc(BUFSIZ + 1);
	printf("Please enter a user-name:\n");
	fgets(name, BUFSIZ, stdin);
	name = strtok(name, "\n");
	return name;
}

int main(){
	int sockd;
	struct timeval start, now;
	double elapsed;



	getuser();

	gettimeofday(&start,NULL);

	while(1){
		gettimeofday(&now, NULL);
		elapsed = now.tv_sec - start.tv_sec;
		if( ((int) elapsed) % 60 == 0 ){
			update_time();
		}
	}


	return 0;
}

