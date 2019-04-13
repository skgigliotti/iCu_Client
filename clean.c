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

char * increase_IP(char *IP_adr){


	return IP_adr;
}

void attack(char *IP_adr, char *user, int sockd){
	int ret, foo;
	char *message;
	char *rdmsg;

	rdmsg = malloc(500 * sizeof(char));

	message = "Who are you?\n";
	ret = connect2v4stream(IP_adr);
	if(ret >= 0){
		write(ret, message, strlen(message));
		foo = read(ret, rdmsg, 500);
		if(foo == -1){
			printf("Error: %s\n", strerror(errno));
			exit(errno);
		}

		printf("%s\n", rdmsg);

//		char *victim = strtok(message, " ");
//		char *location = strtok(NULL, " ");
//		message = "GET /?i=%s&u=%s&where=%s\r\n"
//				  "Host: pilot.westmont.edu:28900\r\n\r\n", user, victim, location;
//		write(sockd, message, strlen(message));
	}
}

int main(){
	int sockd;
	struct timeval timev;
	fd_set readfds;
	char *user;


	user = "tleslie";  //change this to be an argument from the command line

	sockd = connect2v4stream(SERVER);

	//FOR TESTING
	char *vostro_IP;
	vostro_IP = "10.20.43.234";
	attack(vostro_IP, user, sockd);
	//FOR TESTING

	//update_time(sockd);

	//prepares select
	setsockopt(sockd, SOL_SOCKET, SO_RCVTIMEO, &timev, sizeof(timev));



	/*
	 * still need to set up select so it works to listen for attacks and send out who are you requests to
	 * the range of IP numbers (64.136.178.1 - 64.136.178.254)
	 */


	close(sockd);
}

