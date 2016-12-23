#include <winsock2.h>  
#include <ws2tcpip.h>
#include<windows.h>
#pragma comment(lib,"Ws2_32.lib")
struct addrinfo hints;

struct addrinfo *result = NULL;

struct addrinfo *ptr = NULL;

struct sockaddr_in  *sockaddr_ipv4;

LPSOCKADDR sockaddr_ip;

char ipstringbuffer[46];
DWORD ipbufferlength = 46;
int ival;
DWORD dwval;
char valid_ip[50];
char* gethost(char *echoBuffer)
{
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	dwval = getaddrinfo(echoBuffer, "7", &hints, &result);
	if (dwval != 0) {
		printf("getaddrinfo failed with error: %d\n", dwval);
		WSACleanup();
		return 1;
	}

	// Retrieve each address and print out 
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
		switch (ptr->ai_family) {
		case AF_UNSPEC:
			printf("Unspecified\n");
			break;
		case AF_INET:
			sockaddr_ipv4 = (struct sockaddr_in *) ptr->ai_addr;
			printf("IP address %s\n",
				inet_ntoa(sockaddr_ipv4->sin_addr));
			strcpy(valid_ip, inet_ntoa(sockaddr_ipv4->sin_addr));
			break;
		case AF_INET6:

			sockaddr_ip = (LPSOCKADDR)ptr->ai_addr;
			ipbufferlength = 46;
			ival = WSAAddressToString(sockaddr_ip, (DWORD)ptr->ai_addrlen, NULL,
				ipstringbuffer, &ipbufferlength);
			if (ival)
				printf("WSAAddressToString failed with %u\n", WSAGetLastError());
			else
			{
				printf("IP address %s\n", ipstringbuffer);
				strcpy(valid_ip, ipstringbuffer);
			}
			break;
		case AF_NETBIOS:
			printf("AF_NETBIOS (NetBIOS)\n");
			break;
		default:
			printf("Other %ld\n", ptr->ai_family);
			break;
		}

	}


	freeaddrinfo(result);
	return(valid_ip);
}