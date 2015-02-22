#include <stdio.h>
#include <stdlib.h>	
#include <sys/socket.h>
#include <arpa/inet.h>	
#include <netinet/in.h>
#include <sys/wait.h>    
#include <fcntl.h> 
#include <string.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
void *file_handler(void *s1);
int def_port=23456;
int main()
{
	 
	int curr_socket_desc,request,input_port;
	socklen_t len_str;       						/* length of address structure */
	struct sockaddr_in * curr_socket_addr;    		/* address of this service */
	struct sockaddr_in client_addr;  				/* client's address */
	printf("Enter port number between 1024 and 65535:(Enter -1 to use default port)\n");
	scanf("%d",&input_port);
	if (input_port==-1)
	{
		input_port=def_port;
	}
	else if (input_port< 1024 || input_port > 65535)
	{
		printf("invalid port number for the service\n");
		return 0;
	}
	curr_socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (curr_socket_desc < 0) {
		printf("Socket Creation failed\n");
		return 0;
	}
	curr_socket_addr = (struct sockaddr_in *) calloc (1,sizeof(struct sockaddr_in));
	curr_socket_addr->sin_family = AF_INET;   /* address family */
	curr_socket_addr->sin_port = htons(input_port);
	curr_socket_addr->sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(curr_socket_desc, (struct sockaddr *)curr_socket_addr, sizeof(*curr_socket_addr)) < 0) 
	{
		printf("could not bind the socket");
		return (0);
	}

	if (listen(curr_socket_desc, 3) < 0) {
		printf("could not listen to connections");
		return (0);
	}

	printf("server is running on input_port %d\n",input_port);

	len_str = sizeof(client_addr);                   /* length of address */

    while( (request = accept(curr_socket_desc,(struct sockaddr *)&client_addr, &len_str)) )
    {
        printf("Connection accepted\n");
        pthread_t new_thread;
        int * args = calloc(1,sizeof(int));
        *args = request;
         
        if( pthread_create( &new_thread, NULL ,file_handler,(void*) args) < 0)
        {
            printf("error on thread creation");
        return 0;
        }
        printf("Thread handler attached\n");
    }
  if (request < 0) 
    {
        printf("error on accept");
        return 0;
    }
     
    return 0;
}

void *file_handler(void *s1)
{

   int request = *(int*)s1;
   char buf[1000]="";
   int n;
   n = read(  request ,buf,255 );
  if (n < 0)
  {
        printf("error on read");
        return 0;
  }
  printf("Requested file name: %s\n",buf);
	FILE *f1;
	char filename[1000];
	strcpy (filename,"server1_files/");
	strcat (filename,buf);
	if (n < 0)
	 {
	       printf("error on write");
           return 0;
	 }
	char file_buffer[1000];
  char msg_buffer2[1024];
  int fd,sd,addrlen;                    
  struct stat stat_buf;     
  off_t offset = 0;          
    fd = open(filename, O_RDONLY);
    if (fd == -1) 
    {
      
      printf("unable to open '%s': %s\n", filename, strerror(errno));
      n = write( request ,"0",1);
      if (n < 0)
      {
           printf("error on write");
             return 0;
      }
      return 0;
    }

    printf("File found!\n");
    n = write( request ,"1",1);
    fstat(fd, &stat_buf);
    printf(" %9jd", (intmax_t)stat_buf.st_size);
    char buffer[100000];
    for (sd=0;sd<100000;sd++)
          buffer[sd]='\0';
    sprintf(buffer, " %9jd", (intmax_t)stat_buf.st_size );
     n = write( request ,buffer,strlen(buffer));    // write the length of the file
     printf("File transfer starts \n");
     FILE * fpIn = fopen(filename, "r");
    if (fpIn)
    {
            char buf[256];
	   char buf1[2];
            while(1)
            {
               ssize_t bytesRead = fread(buf, 1, sizeof(buf), fpIn);
               if (bytesRead <= 0) break;  // EOF

               printf("Read %i bytes from file, sending them to network...\n", (int)bytesRead);
               if (send(request, buf, bytesRead, 0) != bytesRead)
               {
                  printf("Error,sending ");
                  break;
               }
		n = read( request,buf1,1);
              if (n < 0)
              {
                     printf("error on write");
                     return 0;
              }
			

            }
         }
         else {
          printf("Error, couldn't open file [%s] to send!\n", filename);
         }
          
      close(fd);
      printf("the file transfer was successful\n");
      free(s1);


}
