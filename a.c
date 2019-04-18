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

char * name, * accesspoint;

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
//	char *foo = malloc(1000);
//	read(new_socket, foo, 1000);
	write(new_socket, found_msg, strlen(found_msg));
	close(serverfd);
}

char *getap() //server side
{
	accesspoint = "vl-1a-wap3"; //"vl-1a-wap3\n";

	return accesspoint;
}

char *getuser() //both
{
	name = malloc(BUFSIZ + 1);
	printf("Please enter a user-name:\n");
	fgets(name, BUFSIZ, stdin);
	name = strtok(name, "\n");
	return name;
}

int main(){

	getuser();
	getap();

	while(1){
		make_socket();
	}

	return 0;
}
