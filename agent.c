#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <sys/ioctl.h>
#include <net/if.h>

#define PORT_NUMBER_1 53953
#define PORT_NUMBER_2 35935

int read_one_byte(int client_socket, char *buffer);
int receiveFully(int client_socket, char *buffer, int length);
void printBinaryArray(char *buffer, int length);
int toInteger32(char *bytes);
void convertUpperCase(char *buffer, int length);

void err_sys(char *msg)
{
  perror(msg);
  exit(0);
}

/* Threads */
void* BeaconSender(void *host);
void* CmdAgent();

/* Helper Functions */
void* toBytes(uint32_t num, char *bytes);

/* Functions */
void GetLocalOS(char OS[16], int *valid);
void GetLocalTime(int *time, int *valid);

struct BEACON
{
  int ID;          // randomly generated during startup
  int StartUpTime; // the time when the client starts
  char IP[4];	 // the IP address of this client
  int CmdPort;     // the client listens to this port for cmd
}; 
  
int main(int argc, char *argv[])
{
  char *host;
  pthread_t workers[2];
  pthread_attr_t attr;
  
  pthread_attr_init(&attr); /* Get the default attributes */

  /* Store command line arugment. */
  host = argv[1];
  
  /* Create the threads for each operation */
  pthread_create(&workers[0], &attr, BeaconSender, host);
  pthread_create(&workers[1], &attr, CmdAgent, NULL);
  
  /* Wait for all threads to terminate */
  pthread_join(workers[0], NULL);
  pthread_join(workers[1], NULL);
  
  return 0;
}

void* BeaconSender(void *host) //As client
{
  struct BEACON beacon;
  struct hostent *host_entry;
  int client_socket, i, n, length;
  char *host_IP, name_buffer[256];
  // char *buff, *token, *temp, *q = "quit";
  int serverlen;
  struct sockaddr_in serv_addr;
  struct hostent *server; // For the definition of the server on the internet.                                                    

  if ((client_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    err_sys("socket error");
  }
  printf("client_socket = %d\n", client_socket);
  
  /* Initialize server struct to zero. */
  memset((char *) &serv_addr, 0, sizeof(struct sockaddr_in));

  /* Get server name from the command line. */
  server = gethostbyname(host);

  /* Specify the address family. */
  serv_addr.sin_family = AF_INET;

  /* Get server address size. */
  serverlen = sizeof(serv_addr);
  
  /* Specify and convert the port number to network byte order. */
  serv_addr.sin_port = htons(PORT_NUMBER_1);

  /* Specify the IP address of the server using the struct hostent type defined above. */
  memcpy((char* )&serv_addr.sin_addr.s_addr,
         (char* )server->h_addr,
         server->h_length);
 
  /* Get host IP. */
  gethostname(name_buffer, sizeof(name_buffer));
  host_entry = gethostbyname(name_buffer);
  host_IP = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0]));
  printf("host_IP = %s\n", host_IP);

  /* Convert IP string to integer. */
  uint32_t ip = inet_addr(host_IP);
  
  /* Instantiate BEACON. */
  char bytes1[4] = {0, 0, 0, 0};
  uint32_t id;
  
  srand(time(NULL));
  uint32_t ran = (rand() % 1000) + 1;
  id = htonl(ran);
  beacon.ID = (int) id;
  printf("ID: %d\n", ran);

  int t = time(NULL);
  beacon.StartUpTime = htonl(t);
  printf("StartUpTime: %d\n", t);

  char bytes2[4] = {0, 0, 0, 0};
  toBytes(ip, bytes2); // Convert int IP to byte order.
  
  memcpy(&beacon.IP, bytes2, 4);
  printf("Host IP in Hex: %hhx %hhx %hhx %hhx\n", beacon.IP[0],
	 beacon.IP[1], beacon.IP[2], beacon.IP[3]);

  beacon.CmdPort = htonl(PORT_NUMBER_1);
  printf("CmdPort: %d\n", PORT_NUMBER_1); 

  /* Testing GetLocalTime Function. */
  int *time, *valid;
  GetLocalTime(time, valid);  
  
  /* Send BEACON to manager every 1 minute. */
  while(1) {
    sendto(client_socket, (struct BEACON *) &beacon, sizeof(beacon), 0, (struct sockaddr *) &serv_addr, serverlen);
    sleep(60); //sleep for 60 seconds
  }
}

void* CmdAgent()
{
}

void GetLocalOS(char OS[16], int *valid)
{
}

void GetLocalTime(int *t, int *valid)
{
  time_t rawtime;
  struct tm * timeinfo;

  time (&rawtime);
  timeinfo = localtime (&rawtime);
  printf ("Current local time and date: %s", asctime(timeinfo));
}

void* toBytes(uint32_t num, char* bytes)
{
  bytes[0] = (num >> 24) & 0xFF;
  bytes[1] = (num >> 16) & 0xFF;
  bytes[2] = (num >> 8) & 0xFF;
  bytes[3] = num & 0xFF;
}

/* 
int send_packets(int cSock, void *buffer, int buffer_len)
{
  int send_bytes = send(cSock, buffer, buffer_len, 0);

  if (send_bytes < 0)
    {
      fprintf(stderr, "failed to send. \n");
    }

  return 0;
} 
*/

  
void convertUpperCase(char *buffer, int length)
{
	int i = 0;
	while (i < length)
	{
		if (buffer[i] >= 'a' && buffer[i] <= 'z')
		{
			buffer[i] = buffer[i] - 'a' + 'A'; 
		}
		i++;
	}
}

int receive_one_byte(int client_socket, char *cur_char)
{
    ssize_t bytes_received = 0;
	while (bytes_received != 1)
	{
		bytes_received = recv(client_socket, cur_char, 1, 0);
	} 
	
	return 1;
}

int receiveFully(int client_socket, char *buffer, int length)
{
	char *cur_char = buffer;
	ssize_t bytes_received = 0;
	while (bytes_received != length)
	{
	    receive_one_byte(client_socket, cur_char);
	    cur_char++;
	    bytes_received++;
	}
	
	return 1;
}

void printBinaryArray(char *buffer, int length)
{
    int i=0;
    while (i<length)
    {
		printf("%d ", buffer[i]);
		i++;
	}
	
	printf("\n");
}

int toInteger32(char *bytes)
{
	int tmp = (bytes[0] << 24) + 
	          (bytes[1] << 16) + 
	          (bytes[2] << 8) + 
	           bytes[3];
	
	return tmp;
}

/* On an OS with a different endian from java, use this one
int toInteger32(char *bytes)
{
	int tmp = bytes[0] + 
	          (bytes[1] << 8) + 
	          (bytes[2] << 16) + 
	          (bytes[3] << 24);
	             
	return tmp;
}
*/
