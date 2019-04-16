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

/*
 * replies to sent messages with where we are
 */
char * getuser(int fd){
	name = malloc(NAMESIZE);
	printf("Please enter a user-name:\n");
	fgets(name, NAMESIZE, stdin);
	send(fd, name, NAMESIZE, 0);
	
	return name;
}

char * returnUserInfo(){
    //"rodkey vl-1a-wap3\n" format
    return ("%s %s", name, accesspoint);
}

char * getAP(){

}

int recvandprint(int fd, WINDOW * win, char * buffer, int maxy){
	int ret;

	memset(buffer, 0, BUFSIZE);
	ret = recv(fd, buffer, BUFSIZE, 0);

	if(ret == -1){
		if(errno == EAGAIN){
			wprintw(win, "%s", buffer);
			wrefresh(win);
			recvandprint(fd, win, buffer, maxy);
		} else{
			endwin();
			printf("%s %s\n", name, accesspoint);
			exit(errno);
		}
	} else if(ret == 0){
		endwin();
		exit(0);
	} else{
		clearscr(win, maxy);
		send(win, "%s %s", name, accesspoint);
		wrefresh(win);
	}

	return ret;
}
 

/*
 * connects to the server and prints errors if any is encountered
 */
int connect2v4stream(){
	struct sockaddr_in server;
	int sockd, ret;
	char *message = "GET http://pilot.westmont.edu:28900/?i=tleslie&uptime=60 HTTP/1.0\r\n\r\n";

	sockd = socket(AF_INET,SOCK_STREAM,0);
	if(sockd == -1){
		printf("%s\n", strerror(errno));
		exit(errno);
	}
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT); //converts port number

	ret = inet_pton(AF_INET, SERVER, &server.sin_addr);
 	if(ret == -1){
 		printf("%s\n", strerror(errno));
 		exit(errno);
 	}

 	ret = connect(sockd, (struct sockaddr *)&server, sizeof(server));
	if(ret == -1){
		printf("%s\n", strerror(errno));
		exit(errno);
	}

	ret = write(sockd, (void *) message, sizeof(message));
	if(ret == -1){
		printf("%s\n", strerror(errno));
		exit(errno);
	}

	close(sockd);

	return sockd;
}

int main(){

	//ends the connection
	printf("Logging Off. Goodbye.\n");
	close(fd);
    free(name);
	return 0;
}

