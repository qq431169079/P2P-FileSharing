#include <stdio.h>
#include <stdlib.h>	/* needed for os x*/
#include <string.h>	/* for strlen */
#include <netdb.h>      /* for gethostbyname() */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <errno.h>
int def_port1=23456;
int def_port2=12345;
int consult_server(int input_port,char host[],char f_args[]);
int main()
{
	int input_port1,input_port2;
	
	char host1[1000],host2[1000];
    printf("Server 1 information:\n");
	printf("Enter host IP(127.0.0.1 for local host)\n");
	scanf("%s",host1);
	printf("Enter the port number between 1024 and 65535:(-1 to use default port)\n");
	scanf("%d",&input_port1);
	if (input_port1==-1)
	{
		input_port1=def_port1;
	}
	else if (input_port1< 1024 || input_port1 > 65535)
	{
		printf("invalid port number for the server 1\n");
		return 0;
	}

	printf("Server 2 information:\n");
	printf("Enter host IP(127.0.0.1 for local host)\n");
	scanf("%s",host2);
	printf("Enter the port number between 1024 and 65535:(-1 to use default port)\n");
	scanf("%d",&input_port2);
	if (input_port2==-1)
	{
		input_port2=def_port2;
	}
	else if (input_port2< 1024 || input_port2 > 65535)
	{
		printf("invalid port number for the server 1\n");
		return 0;
	}
   char file1[1000];
   printf("Please enter the file name to request to server:\n");
   scanf("%s",file1);
	int res=consult_server(input_port1,host1,file1);
	if (res==1)
	{
		printf("File transfer successful from server 1\n");
	}
	else if (res==-1 || res==0 )
	{
		int res1=consult_server(input_port2,host2,file1);
		if (res1==1)
		{
			printf("File transfer successful from server 2\n");
		}
		else 
		{
			if (res==0)
			{
				printf("Connection broke with server 1\n");
			}
			if (res==-1)
			{
				printf("File not found in server 1\n");
			}
			if (res1==0)
			{
				printf("Connection broke with server 2\n");
			}
			if (res1==-1)
			{
				printf("File not found in server 2\n");
			}
			
		}
	}
	return 0;
}

int consult_server(int input_port,char host[],char f_args[])
{
	
	struct sockaddr_in * server_addr;
	char msg_buffer1[1000]="",msg_buffer2[1000]=""; 
	struct hostent *server_info;	/* host information */
	struct sockaddr_in * client_addr;
	int len_addr,curr_socket_desc;
	curr_socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (curr_socket_desc < 0) {
		printf("Socket Creation failed\n");
		return 0;
	}

	client_addr = (struct sockaddr_in *) calloc (1,sizeof(struct sockaddr_in));
	client_addr->sin_family = AF_INET;
	client_addr->sin_addr.s_addr = htonl(INADDR_ANY);
	client_addr->sin_port = htons(0);
	
	if (bind(curr_socket_desc, (struct sockaddr *)client_addr, sizeof(*client_addr)) < 0) 
	{
		printf("could not bind the socket");
		return 0;
	}

	server_addr = (struct sockaddr_in *) calloc (1,sizeof(struct sockaddr_in));
	server_addr->sin_family = AF_INET;
	server_addr->sin_port = htons(input_port);
	server_addr->sin_addr.s_addr = inet_addr(host);

	if (connect(curr_socket_desc, (struct sockaddr *)server_addr, sizeof(*server_addr)) < 0) 
	{
		printf("connection failed");
		return 0;
	}
    int  n;
    char filename[1000];
    strcpy (filename,"client_files/");
    strcat (filename,f_args);
    n = write(curr_socket_desc,f_args,strlen(f_args));
    if (n < 0) 
    {
         return 0;
    }
    
    n = read(curr_socket_desc,msg_buffer2,255);
    if (n < 0) 
    {
         return 0;
    }
    int i=atoi(msg_buffer2);
    if (i==1)
    {
    	printf("File found!\n");
    	char file_buffer[100000];
    	FILE *fp;
    	//fp=fopen(filename,"w");
    	int fd = open(filename, O_WRONLY|O_CREAT,S_IRWXU);
    	n = read(curr_socket_desc,msg_buffer2,255);
    	if (n < 0) 
    	{
        	return 0;
        }
	   int sd;
	   for (sd=0;sd<100000;sd++)
	          file_buffer[sd]='\0';
	printf("File Size: %d\n",atoi(msg_buffer2));
	long long int len=atoi(msg_buffer2);

	printf("File Transfer Started!\n");

	int chunk_size=256;
	long long int x=len/chunk_size;
	int y=len%chunk_size;
	long long int i,prev=0;
	for (i=0;i<x;i++)
	{
		n=read(curr_socket_desc,file_buffer,chunk_size);
		 if (n < 0) 
		    {
		         return 0;
		    }

	   n=write(fd,file_buffer,chunk_size);
	   prev=prev+n;
	   printf("Transferred bytes %lld\n",prev );

		n=write(curr_socket_desc,"1",1);
		 if (n < 0) 
		    {
		         return 0;
		    }	

	}
	n=read(curr_socket_desc,file_buffer,y);
		 if (n < 0) 
		    {
		         return 0;
		    }

	n=write(fd,file_buffer,y);
	prev=prev+n;
    printf("Transferred bytes %lld\n",prev );


				if (prev!=len)
					{
						printf("Incomplete File Transfer,Problem with the Network:\n");

					}
					else
					{
						printf("File Transfer Successful!\n");
					}
	if (len>90000)
	{
		printf("File may not be transferred properly due to fragmentation\n");
	}
	

	return 1;
}
else
 {
    	return -1;
  }
    return 0;

}
