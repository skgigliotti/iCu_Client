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
int connect2v4stream(char *IP_adr)
{
	struct sockaddr_in server;
	int sockd, ret;

	sockd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockd == -1)
	{
		printf("%s\n", strerror(errno));
	}
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT); //converts port number

	ret = inet_pton(AF_INET, IP_adr, &server.sin_addr);
	if (ret == -1)
	{
		printf("%s\n", strerror(errno));
	}

	ret = connect(sockd, (struct sockaddr *)&server, sizeof(server));
	if (ret == -1)
	{
		printf("%s\n", strerror(errno));
	}

	return sockd;
}

/*
 * replies to sent messages with where we are
 */
char *getuser(int fd)
{
	name = malloc(BUFSIZ + 1);
	printf("Please enter a user-name:\n");
	fgets(name, BUFSIZ, stdin);
	send(fd, name, BUFSIZ, 0);

	return name;
}

/*
 * get access point
 */
char *getap()
{
	//accesspoint = malloc(BUFSIZ + 1);
	accesspoint = "netsh wlan show interfaces  | grep BSSID | awk  '{print $3}'"; //"vl-1a-wap3\n";
  FILE *mycmd = popen(accesspoint, "r");
  char buf[256];
  fgets(buf, sizeof(buf), mycmd);

pclose(mycmd);
	return accesspoint;
}

int recvandtell(int fd)
{
	int ret;
	buffer = malloc(BUFSIZ + 1);
	memset(buffer, 0, BUFSIZ);
	ret = recv(fd, buffer, BUFSIZ + 1, 0);

	if (ret == -1)
	{
		if (errno == EAGAIN)
		{
		}
	}
	else if (ret == 0)
	{

	}
	else
	{
		char *combo = malloc(BUFSIZ + 1);
		int c = 0;
		strcat(combo, " ");
		strcat(combo, accesspoint);
		send(fd, combo, sizeof(combo + 1), 0);
		printf("%s ", combo);
		free(combo);
	}

	return ret;
}

void update_time(int sockd)
{
	int ret;
	char *message = "GET /?i=tleslie&uptime=60 HTTP/1.1\r\n" //make sure to remove the hard-coded stuff
					"Host: pilot.westmont.edu:28900\r\n\r\n";

	ret = write(sockd, message, strlen(message));
	if (ret == -1)
	{
		printf("%s\n", strerror(errno));
		exit(errno);
	}
}

int main()
{
	int sockd;
	int s;

	sockd = connect2v4stream(SERVER);

	getuser(sockd);
	getap();

	update_time(sockd);

	fd_set write, activeWrite;
	struct timeval time;
	int quit = 0;
  printf(getap());
	//set time for select to switch after 1 second
	//so that messages can be received in real time
	//Set up recv timeout for .5 sec
  	time.tv_sec = 0;
  	time.tv_usec = 1000 * 1000;
  	setsockopt(sockd, SOL_SOCKET, SO_RCVTIMEO, &time, sizeof(time));

	while (!quit)
	{

		//reset the fd select variable each iteration through the loop
		FD_ZERO(&write);
		FD_SET(0, &write);
		FD_SET(sockd, &write);

		s = select(sockd + 1, &write, NULL, NULL, &time);

		//if the fd is set, receive the message
		if (FD_ISSET(sockd, &write))
		{
			int sentMsg;
			printf("Inside \n");
			recvandtell(sockd);

			sleep(9);
			printf("Outside \n");
			buffer = malloc(BUFSIZ + 1);
			strcat(buffer,"Who are you?\n");
			sentMsg = send(sockd, buffer, strlen(buffer), 0);
			printf("Outside2 \n");
			free(buffer);

		}

	}

	FD_CLR(0, &activeWrite);


	/*
	 * still need to set up select so it works to listen for attacks and send out who are you requests to
	 * the range of IP numbers (64.136.178.1 - 64.136.178.254)
	 */

	printf("boo \n");
	close(sockd);
	printf("boo2 \n");
}
