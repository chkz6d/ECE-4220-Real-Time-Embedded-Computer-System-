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
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <linux/if_link.h>

int main(void)
{
		char buffer_IP[40];
	
	//The header for linked list (The pointer ifaddr)
	struct ifaddrs *ifaddr;
        int family, s;
        char host[NI_MAXHOST];
	//getifaddrs(&ifaddr) is the fist linked list, checking if it failed to create a linked list        
	if (getifaddrs(&ifaddr) == -1) 
	{
		perror("getifaddrs");
                exit(EXIT_FAILURE);
        }

           /* Walk through linked list, maintaining head pointer so we
              can free list later */

        for (struct ifaddrs *ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) 
	{
               if (ifa->ifa_addr == NULL)
		{
                   continue;
		}
               	   //stores all the information into family
		   family = ifa->ifa_addr->sa_family;
                   /* For an AF_INET* interface address, display the address */

                   if (family == AF_INET) 
		   {
			//getnameinfo converts ip address into string and stores into the host                   	
			s = getnameinfo(ifa->ifa_addr,
                        (family == AF_INET) ? sizeof(struct sockaddr_in) :
                                                 sizeof(struct sockaddr_in6),
                           host, NI_MAXHOST,
                           NULL, 0, NI_NUMERICHOST);
                   if (s != 0) 
	  	   {
                       printf("getnameinfo() failed: %s\n", gai_strerror(s));
                       exit(EXIT_FAILURE);
                   }
               	   //string compare returns true 1
		   if(!strncmp(host, "128.2", 5))
		   {
                  	  printf("\t\taddress: <%s>\n", host);
			    strcpy(buffer_IP, host);             
		   }
               }
           }

           freeifaddrs(ifaddr);
	return 0;
}
