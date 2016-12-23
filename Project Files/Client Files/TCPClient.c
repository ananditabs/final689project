/* CHANGES FROM UNIX VERSION                                                   */
/*                                                                             */
/* 1.  Changed header files.                                                   */
/* 2.  Added WSAStartUP() and WSACleanUp().                                    */
/* 3.  Used closesocket() instead of close().                                  */

#include <stdio.h>      /* for printf(), fprintf() */
#include <winsock.h>    /* for socket(),... */
#include <stdlib.h>     /* for exit() */

#define RCVBUFSIZE 32   /* Size of receive buffer */

void DieWithError(char *errorMessage);  /* Error handling function */

/* Validate IP adrress  */
int  validate(char ipAddr[])
{
	                         										              //Stores IP address
	int a, b, c, d;

	int l = strlen(ipAddr);            										        		     //Computing length of IP Address

	if (l<0 || l>15)                        										 		    //If IP Address string is too small or too long it can be detected as invalid and program exits
	{
		//printf("\n IP Address is invalid");
		return 0;
	}

	for (int i = 0; i<l; i++)                									               //If program contains any alphabets and other characters other than the '.'
	{
		if (isalpha(ipAddr[i]) && (ipAddr[i] != '.'))
		{
			//printf("\n IP address is invalid");
			return 0;
		}
	}

	sscanf(ipAddr, "%d.%d.%d.%d", &a, &b, &c, &d);       //Using the sscanf function we can parse the IP Address to split the individual octets

	if ((0 <= a && a <= 255) && (0 <= b && b <= 255) && (0 <= c && c <= 255) && (1 <= d && d <= 255)) // The individual octets can then be used to compare whether they are within the limits
	{
		//printf("IP address is valid\n"); 
		return 1;
	}
	else if ((a == 0) && (b == 0) && (c == 0) && (d == 0)) return 0;
	else
		//printf("IP address is invalid \n");


		return 0;
}


void main(int argc, char *argv[])
{
	int sock;                        /* Socket descriptor */
	struct sockaddr_in echoServAddr; /* Echo server address */
	unsigned short echoServPort;     /* Echo server port */
	char *servIP;                    /* Server IP address (dotted quad) */
	char *echoString;                /* String to send to echo server */
	char *command_code;
	char *domainIP;
	char echoBuffer[RCVBUFSIZE];     /* Buffer for echo string */
	int echoStringLen;               /* Length of string to echo */
	int bytesRcvd, totalBytesRcvd;   /* Bytes read in single recv() and total bytes read */
	WSADATA wsaData;                 /* Structure for WinSock setup communication */
	int command_len;
	int domainIP_len;
	printf("ARGC is %d", argc);
    
	if ((argc < 5) || (argc > 7))    /* Test for correct number of arguments */
	{
		fprintf(stderr, "Usage: %s <Server IP> <Echo Word> [<Echo Port>]\n", argv[0]);
		
		exit(1);
	}
	if (argc == 5)
	{
		servIP = argv[2];             /* First arg: server IP address (dotted quad) */
		echoServPort = atoi(argv[3]);
		command_code = argv[4];
		echoString = "";
	}
	
	if (argc == 6)
	{
		servIP = argv[2];             /* First arg: server IP address (dotted quad) */
		echoString = argv[5];         /* Second arg: string to echo */
		echoServPort = atoi(argv[3]);
		command_code = argv[4];
	}
	if (argc == 7)
	{
		servIP = argv[2];             /* First arg: server IP address (dotted quad) */
		echoString = argv[5];/* Second arg: string to echo */
		echoServPort = atoi(argv[3]);
		command_code = argv[4];
		domainIP = argv[6];
	}

	int tempo = validate(servIP);
	if (tempo == 0) { DieWithError("Invalid IP Address"); }
	printf("servIP %s\n", servIP);
	printf("echoServPort %d \n", echoServPort);
	printf("echoString %s \n", echoString);
	printf("command_code %s \n", command_code);
	

	if (atoi(command_code) < 1 || atoi(command_code) >6)
	{
		printf("Invalid command code. Please choose between 1 to 6 .\n");
	}

	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) /* Load Winsock 2.0 DLL */
	{
		fprintf(stderr, "WSAStartup() failed");
		exit(1);
	}

	/* Create a reliable, stream socket using TCP */
	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		DieWithError("socket() failed");
	
	/* Construct the server address structure */
	memset(&echoServAddr, 0, sizeof(echoServAddr));     /* Zero out structure */
	echoServAddr.sin_family = AF_INET;             /* Internet address family */
	echoServAddr.sin_addr.s_addr = inet_addr(servIP);   /* Server IP address */
	echoServAddr.sin_port = htons(echoServPort); /* Server port */
												 /* Establish the connection to the echo server */
	if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
		DieWithError("connect() failed");


	if (argc == 5)
	{
		echoStringLen = strlen(command_code);

		if (send(sock, command_code, echoStringLen, 0) != echoStringLen)
			DieWithError("send() sent a different number of bytes than expected");
		printf("Command sent");
		
	}
	if (argc == 6)
	{
		command_len = strlen(command_code);
		if (send(sock, command_code, command_len, 0) != command_len)
			DieWithError("send() sent a different number of bytes than expected");
		printf("Command sent \n ");
		
		echoStringLen = strlen(echoString);
		if (send(sock, echoString, echoStringLen, 0) != echoStringLen)
			DieWithError("send() sent a different number of bytes than expected");
		printf("Echostring \n");
		
		echoStringLen = echoStringLen + command_len;
	}
	if (argc == 7)
	{
		command_len = strlen(command_code);
		if (send(sock, command_code, command_len, 0) != command_len)
			DieWithError("send() sent a different number of bytes than expected");
		printf("command \n");
		echoStringLen = strlen(echoString);
		if (send(sock, echoString, echoStringLen, 0) != echoStringLen)
			DieWithError("send() sent a different number of bytes than expected");
		printf("Echostring \n");
		int tempov = validate(domainIP);
		if (tempov == 0){DieWithError("wrong format of IP");}
		domainIP_len = strlen(domainIP);
		if (send(sock, domainIP, domainIP_len, 0) != domainIP_len)
			DieWithError("send() sent a different number of bytes than expected");
		printf("DomainIP  %s\n",domainIP);

		echoStringLen = echoStringLen + command_len + domainIP_len;
	}
	
	
	
	/* Receive the same string back from the server */
	totalBytesRcvd = 0;
	printf("Received: ");                /* Setup to print the echoed string */
	while (totalBytesRcvd < echoStringLen)
	{
		/* Receive up to the buffer size (minus 1 to leave space for
		a null terminator) bytes from the sender */
		if ((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0)) <0)
		{

			DieWithError("recv() failed or connection closed prematurely");
		}
		totalBytesRcvd += bytesRcvd;   /* Keep tally of total bytes */
		echoBuffer[bytesRcvd] = '\0';  /* Add \0 so printf knows where to stop */
		printf("%s", echoBuffer);    /* Print the echo buffer */

	}

	printf("\n");    /* Print a final linefeed */
	for (;;) {}
	closesocket(sock);
	WSACleanup();  /* Cleanup Winsock */

	exit(0);
}