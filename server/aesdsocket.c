#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <syslog.h>
#include <arpa/inet.h>
#define BUFFSIZE 1000

int sockfd, connfd;
void handle_signals(int sgno)
{
	if (sgno == SIGTERM || sgno == SIGINT)
	{
		close(connfd);
		close(sockfd);
		chdir("/var/tmp");
		remove("aesdsocketdata");
		syslog(LOG_ALERT,"Caught signal exiting");
		exit(0);
	}
}

int send_data(int connfd, FILE *fp)
{

	int size = ftell(fp);
	char *data = calloc(sizeof(char), size);
	fseek(fp, 0L, SEEK_SET);
	fread(data, sizeof(char), size, fp);
	write(connfd, data, size);
	free(data);
	return 0;
}
void handle_connection(int connfd)
{
	char *buff;
	char tmp_buff[BUFFSIZE];

	for (;;)
	{


		buff = calloc(BUFFSIZE + 1, sizeof(char));
		int msg_len;
		int read_count = 1;
		while ((msg_len = read(connfd, tmp_buff, BUFFSIZE)) >= BUFFSIZE)
		{
			strcat(buff, tmp_buff);
			buff = realloc(buff, ++read_count * BUFFSIZE + 1);
			syslog(LOG_ALERT, "REALLOC SIZE: %d",read_count * BUFFSIZE + 1 );
		}

		if (msg_len == 0)
		{
			break;
		}

		tmp_buff[msg_len] = '\0';
		strcat(buff, tmp_buff);

		FILE *output_file = fopen("/var/tmp/aesdsocketdata", "at+");

		fprintf(output_file, "%s", buff);
		fflush(output_file);
		send_data(connfd, output_file);

		free(buff);
		fclose(output_file);
	}
	close(connfd);
	free(buff);
}

int main(int c, char **argv)
{
	int len;
	struct sockaddr_in servaddr, cli;

	signal(SIGINT, handle_signals);
	signal(SIGTERM, handle_signals);

	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		perror("Can't create socket");
		exit(-1);
	}
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(9000);

	// Binding newly created socket to given IP and verification
	if ((bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) != 0)
	{
		perror("BIND Failed");
		exit(-1);
	}
	

	if (c == 2 && strcmp(argv[1], "-d") == 0)
	{
		pid_t pid;
		/* create new process */
		pid = fork();
		if (pid == -1)
			return -1;
		else if (pid != 0)
			exit(EXIT_SUCCESS);

		if (setsid() == -1)
			return -1;
		if (chdir("/tmp") == -1)
			return -1;

		/* redirect fd's 0,1,2 to /dev/null */
		open("/dev/null", O_RDWR);
		/* stdin */
		dup(0);
		/* stdout */
		dup(0);
		/* stderror */
	}

	// Now server is ready to listen and verification
	if ((listen(sockfd, 5)) != 0)
	{
		perror("listen failed");
		exit(-1);
	}
	else
	len = sizeof(cli);

	while (1)
	{
		char ip[INET_ADDRSTRLEN];
		// Accept the data packet from client and verification
		connfd = accept(sockfd, (struct sockaddr *)&cli, (socklen_t *)&len);
		if (connfd < 0)
		{
			perror("Accept failed");
			exit(-1);
		}
		else{

			inet_ntop(AF_INET,&(cli.sin_addr),ip,INET_ADDRSTRLEN);
			syslog(LOG_INFO,"Accepted connection from %s",ip);
		}

		// Function for chatting between client and server
		handle_connection(connfd);
		syslog(LOG_INFO,"Closed connection from %s",ip);
	}
}
