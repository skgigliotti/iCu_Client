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
#define BUFSIZE = 1000;
#define NAMESIZE = 1000;
char *name;
char *accesspoint;
char *buffer;

/*
 * connects to the server and prints errors if any is encountered
 *
 * the error handling needs to be changed because we will be expecting many errors when trying to connect
 */
int connect2v4stream(char *IP_adr){
	struct sockaddr_in server;
	int sockd, ret;

	sockd = socket(AF_INET,SOCK_STREAM,0);
	if(sockd == -1){
		printf("%s\n", strerror(errno));
	}
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT); //converts port number

	ret = inet_pton(AF_INET, IP_adr, &server.sin_addr);
 	if(ret == -1){
 		printf("%s\n", strerror(errno));
 	}

 	ret = connect(sockd, (struct sockaddr *)&server, sizeof(server));
	if(ret == -1){
		printf("%s\n", strerror(errno));
	}

	return sockd;
}

/*
 * replies to sent messages with where we are
 */
char * getuser(int fd){
	name = malloc(BUFSIZ+1);
	printf("Please enter a user-name:\n");
	fgets(name, BUFSIZ, stdin);
	send(fd, name, BUFSIZ, 0);
	
	return name;
}

/*
 * get access point
 */
char * getap(){
	accesspoint =  malloc(BUFSIZ+1);
	accesspoint = "vl-1a-wap3\n";

	return accesspoint;
}

int recvandtell(int fd){
	int ret;
	buffer = malloc(BUFSIZ+1);
	memset(buffer, 0, BUFSIZ);
	ret = recv(fd, buffer, BUFSIZ+1, 0);

	if(ret == -1){
		if(errno == EAGAIN){
		} 
	} else if(ret == 0){
		exit(0);
	} else{
		char * combo = malloc(BUFSIZ+1);
		int c = 0;
		strcat(combo, " ");
		strcat(combo, accesspoint);
		send(fd, combo, sizeof(combo+1),0);
		printf("%s ",combo);
		free(combo);
	}

	return ret;
}

void update_time(int sockd){
	int ret;
	char *message = "GET /?i=tleslie&uptime=60 HTTP/1.1\r\n"  //make sure to remove the hard-coded stuff
				    "Host: pilot.westmont.edu:28900\r\n\r\n";

	ret = write(sockd, message, strlen(message));
	if(ret == -1){
		printf("%s\n", strerror(errno));
		exit(errno);
	}
}

int main(){
	int sockd;
	struct timeval timev;
	fd_set readfds;

	sockd = connect2v4stream(SERVER);

	getuser(sockd);
	getap();

	update_time(sockd);

	//prepares select
	setsockopt(sockd, SOL_SOCKET, SO_RCVTIMEO, &timev, sizeof(timev));

	recvandtell(sockd);

	/*
	 * still need to set up select so it works to listen for attacks and send out who are you requests to
	 * the range of IP numbers (64.136.178.1 - 64.136.178.254)
	 */


	close(sockd);
}
