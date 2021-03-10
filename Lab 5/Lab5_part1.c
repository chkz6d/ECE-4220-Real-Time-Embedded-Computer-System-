#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdbool.h>
#include <time.h>



int main(void)
{
	// socket () creates an endpoint for a network connection
	// int socket(int domain, int type,int protocol);
	// domain -> PF_INET (IPv4 communication), AF_INET?
	// type -> SOCK_STREAM (TCP), SOCK_DGRAM (UDP)
	// protocol = 0
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	int boolval = 1;
	//This x will determine who is the Master on second time asking WHOIS	
	int x = 0;
	//length 	
	socklen_t length;

	// I AM THE MASTER MESSAGE
	char buffer_MASTER[40];

	// Hard coded my own IP address on buffer_IP
	char buffer_IP[40] = "# 128.206.19.28_";

	// bind () to attach itself to a specific port and IP address
	// int bind(int sockfd, struct sockaddr *my_addr, socklen_t addrlen)
	// sockfd -> socket descriptor returned by socket()
	// my_addr -> pointer to a valid sockaddr_in structure cast as a sockaddr *pointer
	// addrlen -> length of the sockaddr_in structure

	//configuration
	struct sockaddr_in servA;
	servA.sin_family = AF_INET;
	servA.sin_port = htons(3000);
	servA.sin_addr.s_addr = INADDR_ANY;
	
	int size_A = sizeof(servA);
	bind(sock, (struct sockaddr*)&servA, size_A);

	//f
	setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &boolval, sizeof(boolval));

	//communication
	//(struct sockaddr *) is a structure to get addr of socket, cliAdd will be val not pointer
	// buffers can be "MyMsg" etc.

	//initializing client (IP address)
	struct sockaddr_in cliAdd;
	cliAdd.sin_addr.s_addr = inet_addr("128.206.19.255");

	//initializing buffer
	char buffer[40];

	//initializing length, size of structure 
	length = sizeof(struct sockaddr_in);

	//generating random number and assign it to buffer_IP
	srand(time(0));	
	int num = rand() % 10;
	//creating strings of array for integer random number
	char str3[1];
	//sends formatted output (num) to a string pointed to, by str3
	sprintf(str3, "%d",num);	

	//putting two string arrays into one, string2 is an array that it's stored random number	
	strcat(buffer_IP,str3);

	while (1)
	{
		// Receive "WHOIS" || "strings" in buffer , message size
		//1st argument: socket
		//2nd argument: the IP-address from other user
		//3rd argument: the buffer size
		//4th argument: 
		recvfrom(sock, buffer, 40, 0, (struct sockaddr*)&cliAdd, &length);

		// Send the message "VOTE" if buffer receives WHOIS
		if (strncmp(buffer, "WHOIS",5) == 0)
		{
			
			//When I am the Master 
			if (x == 1)
			{
				// sending a message to the client that I am the Master, buffer_Master string size
				sendto(sock, buffer_MASTER, 41, 0, (struct sockaddr*)&cliAdd, length);

			}
			//When no one knows the MASTER
			else if (x == 0)
			{
				
				// bzero: clean up the buffer (seeting all values to zero)
				bzero(buffer, 40);
				// initalizing buffer with a message "....." 
				strncpy(buffer, ".....", 5);
				// sending a message buffer back to the client
				sendto(sock, buffer, 5, 0, (struct sockaddr*)&cliAdd, length);

			}
			//send a message I am not the MASTER, back to the client after being asked WHOIS twice 		
			else
			{
				sendto(sock, buffer, 25, 0, (struct sockaddr*)&cliAdd, length);
			}
		}

		// Receive a message from the client "VOTE"
		else if (strncmp(buffer, "VOTE", 4) == 0)
		{
	
			// sending a message buffer_IP back to the client
			sendto(sock, buffer_IP, 17, 0, (struct sockaddr*)&cliAdd, length);

		}
		else
		{		
			//My own vote is bigger than the client vote
			// comparing its own vote with the other votes
			if (strcmp(buffer_IP, buffer) > 0)
			{

				//I WON! I'm a master!, using this for if (statement) when the client asks "WHOIS" twice
				x = 1;
				//replace(copy) the string buffer with "Chan on board....."
				strncpy(buffer_MASTER, "Chan on board 128.206.19.28 is master", 37);
				sendto(sock, buffer_MASTER, 37, 0, (struct sockaddr*)&cliAdd, length);


			}
			// WHEN the votes TIE, the highest IP wins the vote
			else if (strcmp(buffer_IP, buffer) == 0)
			{	
				//intialize return value for strncmp
				int ret;

				//compare first 14 strings in array 
				ret = strncmp(buffer_IP, buffer, 14);
				// My IP_address (buffer_IP) is SMALLER than the client's IP_address (buffer)
				if (ret < 0)
				{
					//this will be used for if statement when client asks WHOIS twice and I am not the master
					x = 2;
					// bzero: clean up the buffer (seeting all values to zero)
					bzero(buffer, 40);
					strncpy(buffer, "I LOST, I am not a master", 25);
	
					// sending a message buffer back to the client
					sendto(sock, buffer, 25, 0, (struct sockaddr*)&cliAdd, length);
				}
				// My IP_address is BIGGER than the client's IP_address				
				else if (ret > 0)
				{
				
					//I won a master!, using this for if (statement) when the client asks second time "WHOIS"
					x = 1;
					
					strncpy(buffer_MASTER, "Chan on board 128.206.19.28 is the master", 37);
					sendto(sock, buffer_MASTER, 37, 0, (struct sockaddr*)&cliAdd, length);
				}
			}

			// My vote is SMALLER than the client's vote 
			else if(strcmp(buffer_IP, buffer) < 0)
			{
				//this will be used for if statement when client asks WHOIS twice and I am not the master
				x = 2;
				// bzero: clean up the buffer (seeting all values to zero)
				bzero(buffer, 40);
				strncpy(buffer, "I LOST, I am not a master", 25);

				// sending a message buffer back to the client
				sendto(sock, buffer, 25, 0, (struct sockaddr*)&cliAdd, length);
			}
		}
	}
	return 0;
}
