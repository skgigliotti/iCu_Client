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
 */
int connect2v4stream(){
	struct sockaddr_in server;
	int sockd, ret;
	char *message = "GET /?i=tleslie&uptime=60 HTTP/1.1\r\n"  //make sure to remove the hard-coded stuff
			"Host: pilot.westmont.edu:28900\r\n\r\n";

	printf("%s\n", message);

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

	ret = write(sockd, message, strlen(message));
	if(ret == -1){
		printf("%s\n", strerror(errno));
		exit(errno);
	}

	return sockd;
}

/*
 * receives the messages from the server and prints them on the screen
 */
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
			printf("%s\n", strerror(errno));
			exit(errno);
		}
	} else if(ret == 0){
		endwin();
		exit(0);
	} else{
		clearscr(win, maxy);
		wprintw(win, "%s", buffer);
		wrefresh(win);
	}

	return ret;
}


char * getuser(int fd){
	char * name;
	name = malloc(NAMESIZE);
	printf("Please enter a user-name:\n");
	fgets(name, NAMESIZE, stdin);
	send(fd, name, NAMESIZE, 0);
	free(name);
	return name;
}

int main(){

	struct timeval t1, t2;
	double elapsed;
	gettimeofday(&t1,NULL);
	t1.tv_sec; // seconds
	gettimeofday(&t2, NULL);
	elapsed = t2.tv_sec - t1.tv_sec;
	if( elapsed%60 == 0){
		//send update to server
	}




	int fd, sret, counter, maxy, maxx;
	char c, *name, buffer[BUFSIZE], sendbuf[BUFSIZE];
	struct timeval timev;
	fd_set readfds;

	memset(sendbuf, 0, BUFSIZE);


	//connects to server
	fd = connect2v4stream();

	//prepares select
	setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timev, sizeof(timev));

	//gets a username
	name = getuser(fd);


	while(1){
		FD_ZERO(&readfds);
		FD_SET(fd, &readfds);
		FD_SET(STDIN_FILENO, &readfds);
		timev.tv_sec = 0;
		timev.tv_usec = 1000 * 100;

		sret = select(fd+1, &readfds, NULL, NULL, &timev);
		if(sret == -1){
			endwin();
			printf("%s\n", strerror(errno));
			exit(errno);
		}

		else if(sret == 0){
			/*Do nothing*/
		}

		else if(sret > 0){
			if(FD_ISSET(fd, &readfds)){ //if something is ready to be received
				recvandprint(fd, recvwin, buffer, maxy);
			}

			if(FD_ISSET(STDIN_FILENO, &readfds)){ //if something is ready to send
				c = wgetch(sendwin);
				counter = buildmsg(c, sendwin, fd, sendbuf, counter);
				if(counter == -1){
					break;
				}
			}
		}
	}

	//ends the connection
	printf("Logging Off. Goodbye.\n");
	close(fd);
	return 0;
}

