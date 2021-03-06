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
#define DEFAULT_IP "10.90.1.1"

char * name, * accesspoint;

/*
 * Help from this site: https://www.geeksforgeeks.org/socket-programming-cc/
 */
void make_socket(){ //server side
    struct sockaddr_in address;
    socklen_t address_len;
    int ret;

    address_len = sizeof(struct sockaddr_in);


	int serverfd = socket(AF_INET, SOCK_STREAM, 0);



    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    ret = bind(serverfd, (struct sockaddr *)&address, sizeof(address));
    if(ret == -1){
   		printf("%s\n", strerror(errno));
   	}
	ret = listen(serverfd, 1);
	if(ret == -1){
 		printf("%s\n", strerror(errno));
 	}
	int new_socket = accept(serverfd, (struct sockaddr *)&address, &address_len);
	char *found_msg = name;
	strcat(found_msg, " ");
	strcat(found_msg, accesspoint);
	strcat(found_msg, "\n");
	char *foo = malloc(1000);
	read(new_socket, foo, 1000);
	write(new_socket, found_msg, strlen(found_msg));
	close(serverfd);
}

/*
 * connects to the server and prints errors if any is encountered
 */
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

void update_time(int sockd){ //either
	int ret;
	char *message;
	sprintf(message, "GET /?i=%s&uptime=60 HTTP/1.1\r\nHost: pilot.westmont.edu:28900\r\n\r\n", name);

	ret = write(sockd, message, strlen(message));
	if(ret == -1){
		printf("%s\n", strerror(errno));
		exit(errno);
	}
	return;
}

char * increase_IP(char *IP_adr){ //client side
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
    	char scan[20];
    	char two[10];
    	char three[10];
    	char four[10];
    	snprintf(two, 10, "%d", second);
    	snprintf(three, 10, "%d", ret);
    	snprintf(four, 10, "%d", last);
    	strcat(scan,first);
    	strcat(scan,".");
    	strcat(scan,two);
    	strcat(scan,".");
    	strcat(scan,three);
    	strcat(scan,".");
    	strcat(scan,four);

	return scan;
}

void attack(char *IP_adr, int sockd){ //client side
	int ret, foo;
	char *message;
	char *rdmsg;

	rdmsg = malloc(500 * sizeof(char));

	message = "Who are you?\n";
	ret = connect2v4stream(IP_adr);
	if(ret >= 0){
		write(ret, message, strlen(message));
		foo = read(ret, rdmsg, 500);
		if( foo == -1 ){
			printf("Error: %s\n", strerror(errno));
		}

		printf("%s\n", rdmsg);

		char *victim = strtok(message, " ");
		char *location = strtok(NULL, " ");
		sprintf(message,"GET /?i=%s&u=%s&where=%s\r\n"
				        "Host: pilot.westmont.edu:28900\r\n\r\n", name, victim, location );
		write(sockd, message, strlen(message));
	}

	return;
}

char *getap() //server side
{
	accesspoint = "34:85:84:0e:6e:e5"; //"vl-1a-wap3\n";

	return accesspoint;
}

char *getuser() //both
{
	name = malloc(BUFSIZ + 1);
	printf("Please enter a user-name:\n");
	fgets(name, BUFSIZ, stdin);

	return name;
}


int main(){
	int sockd;
	char *IP_addr;
	struct timeval start, now;
	double elapsed;

	sockd = connect2v4stream(SERVER);

	getuser();  //change this to be an argument from the command line

	getap();

	IP_addr = DEFAULT_IP;

	gettimeofday(&start,NULL);

	if( fork() == 0 ){ //server side
		while(1){
			make_socket();
		}
	}else{ //client side
		while(1){
			attack(IP_addr,sockd);       //try to attack the IP
			IP_addr = increase_IP(IP_addr);     //increase the IP
			gettimeofday(&now, NULL);           //update time if needed
			elapsed = now.tv_sec - start.tv_sec;
			if( ((int) elapsed) % 60 == 0 ){
				update_time(sockd);
			}
		}
	}


	close(sockd);
	return 0;
}

