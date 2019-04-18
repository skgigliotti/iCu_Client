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
#define DEFAULT_IP "10.124.5.88"

char * name, * accesspoint;


int connect2v4stream(char *IP_adr){
	struct sockaddr_in server;
	int sockd, ret, sret;
	struct timeval timev;
	fd_set readfds;

	sockd = socket(AF_INET,SOCK_STREAM | SOCK_NONBLOCK,0);  //SOCK_NONBLOCK
	if(sockd == -1){
		printf("Socket Error: %s\n", strerror(errno));
		close(sockd);
		return sockd;
	}

	setsockopt(sockd, SOL_SOCKET, SO_RCVTIMEO, &timev, sizeof(timev));

	server.sin_family = AF_INET;
	server.sin_port = htons(PORT); //converts port number

	ret = inet_pton(AF_INET, IP_adr, &server.sin_addr);
 	if(ret == -1){
 		printf("Inet_pton Error: %s\n", strerror(errno));
 		close(sockd);
 		return ret;
 	}

 	ret = connect(sockd, (struct sockaddr *)&server, sizeof(server));
	if(ret == -1){
//		printf("Connect Error: %s\n", strerror(errno));
		FD_ZERO(&readfds);
		FD_SET(sockd, &readfds);
		timev.tv_sec = 0;
		timev.tv_usec = 500000; //.5 seconds

		sret = select(sockd+1, &readfds, NULL, NULL, &timev);
		if(sret > 0){
			if(FD_ISSET(sockd, &readfds)){
				printf("Attacking %s\n", IP_adr);
				return sockd;
			}
		}
		close(sockd);
		return ret;
	}

	close(sockd);
	return -1;
}

char * increase_IP(char *IP_adr){
	char str[20];
	int ret;
	int last;
	strcpy(str,IP_adr);
	char *first = strtok(str, ".");
	char *ipnum = strtok(NULL, ".");
	int second = atoi(ipnum);
	ipnum = strtok(NULL, ".");
	ret = atoi(ipnum);
	ipnum = strtok(NULL, ".");
	last = atoi(ipnum);
	if (ret >= 0 && ret < 202){
  		if (last >= 1 && last <= 252){
			last ++;
		}
		else if (last == 253){
        		ret ++;
        		last = 1;
        	}
	}
	else if (ret == 202){
		second = 124;
		ret = 0;
		last = 1;
	}
	else if (second == 124 && ret == 202 && last == 253){
		second = 90;
		ret = 0;
		last = 1;
	}


	char *scan;
	scan = malloc(20);
	char two[10];
	char three[10];
	char four[10];
	snprintf(two, 10, "%d", second);
	snprintf(three, 10, "%d", ret);
	snprintf(four, 10, "%d", last);
	scan = strcat(scan,first);
	scan = strcat(scan,".");
	scan = strcat(scan,two);
	scan = strcat(scan,".");
	scan = strcat(scan,three);
	scan = strcat(scan,".");
	scan = strcat(scan,four);

	return scan;
}

void attack(char *IP_adr){
	int ret, foo;
	char *message;
	char *rdmsg;

	rdmsg = malloc(500 * sizeof(char));

	message = "Who are you?\n";
	ret = connect2v4stream(IP_adr);
	if(ret >= 0){
		foo = write(ret, message, strlen(message));
		if( foo == -1 ){
			printf("Write Error: %s\n", strerror(errno));
			free(rdmsg);
			return;
		}
		foo = read(ret, rdmsg, 500);
		if( foo == -1 ){
			printf("Read Error: %s\n", strerror(errno));
			free(rdmsg);
			return;
		}

//		printf("%s\n", rdmsg);

		char *victim = strtok(message, " ");
		char *location = strtok(NULL, " ");
		sprintf(message,"GET /?i=%s&u=%s&where=%s\r\n"
				        "Host: pilot.westmont.edu:28900\r\n\r\n", name, victim, location );
		int sockd = connect2v4stream(SERVER);
		write(sockd, message, strlen(message));
		close(ret);
		close(sockd);
	}

	free(rdmsg);
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
	char *IP_addr;

	name = getuser();

	IP_addr = DEFAULT_IP;



//	printf("Before: %s\n", IP_addr);
//	IP_addr = increase_IP(IP_addr);
//	printf("After: %s", IP_addr);

	while(1){
		attack(IP_addr);
		IP_addr = increase_IP(IP_addr);
	}

	return 0;
}

