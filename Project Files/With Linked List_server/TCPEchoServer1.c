// Anandita Bose : 114233158 
//Arvind Vijayanand: 114252104
#include <stdio.h>      /* for printf(), fprintf() */
#include <winsock.h>    /* for socket(),... */
#include <stdlib.h>     /* for exit() */
#include <time.h>
#include<string.h>
#define MAXPENDING 5    /* Maximum outstanding connection requests */
#define SECURITYCODE "1000"

void DieWithError(char *errorMessage);  /* Error handling function */
//void HandleTCPClient(int clntSocket);   /* TCP client handling function */
char * gethost(char *EchoBuffer);/* Get host by address info */
/* Node that stores the domain name, ipaddress and number of times it has been requested */
typedef struct node {
	char dns[20];
	char ip[128];
	char num[5];
	struct node* next;
}Node;
// root node stores the pointer to the head of the linked list and the number of nodes in the linked list
typedef struct root {
	Node *head;
	int number;

}Root;
// Linked list to store the current timestamp of the request made by the client
typedef struct clqueue {
	char clientip[30];
	long int clk;
	struct clqueue *next;

}clq;
// rootnode to keep a track of the head poitner to clque linked list 
typedef struct Roottime {
	clq *head;
	int count_time;

}roottime;
//typedef roottime *rt;

int searchtime(char clientip[], long int t,roottime *rt, int thresh)/*Search by time Function to check if the request mafe is within thresh seconds by the same client  */
{
	clq *temp = (clq *)malloc(sizeof(clq));
	temp = rt->head;
	while (temp != NULL)
	{
		if (strncmp(clientip,temp->clientip,strlen(clientip)) == 0)/* If the same client makes multiple requests */
		{
			
			long int diff = (t - temp->clk);
			if((diff)>thresh){
				temp->clk = t;// update to current time period
				
				return 1;
			}
			else if ((t - temp->clk) < thresh) {
				printf("\n multiple requests within %d s \n",thresh);
				DieWithError("\n cannot make multiple requests withn the threshold time  \n");
				//exit(0);
				return 1;

			}
		}
		else { temp = temp->next; }
	}
	
	return (0);

}

void insert(Root *r, char dns[], char num[], char ip[])/* Insert function to insert a new record into linked list  */
{
	Node *newptr = malloc(sizeof(Node));
	if (newptr != NULL)
	{
		strcpy(newptr->dns, dns);
		strcpy(newptr->num, num);
		//printf("%s \n",ip[i]);
		strcpy(newptr->ip, ip);
		//printf("%s \n ", newptr->ip);
	}

	if (r->number == 0) {

		newptr->next = r->head;
		r->head = newptr;
		r->head->next = NULL;
		r->number = 1;
	}
	else {
		newptr->next = r->head; // inserting at head 
		r->head = newptr;
		r->number = r->number + 1;
	}

}
/* Delete  function to delete  a record from linked list using DNS name */
void delete(Root *r, char dns[]) {
	Node* current = r->head;
	Node* previous = NULL;

	while (current != NULL) {
		if (strcmp(current->dns, dns) == 0) {
			if (previous == NULL) {
				current = current->next;
				free(r->head);
			}
			else {
				previous->next = current->next;
				free(current);
				current = previous->next;
			}
		}
		else {
			previous = current;
			current = current->next;
		}
	}
}
/* Print all the nodes of the linked list  */
void traverse(Root *r)
{ //printf("trav");
	Node *temp = (Node *)malloc(sizeof(Node));
	temp = r->head;
	while (temp != NULL)
	{
		printf("%s %s %s \n", temp->dns, temp->num, temp->ip);
		temp = temp->next;

	}
}
/* Search function to search for a DNS name in a linked list  */
Node * search(Root *r,char dns[])
{
Node *temp = (Node *)malloc(sizeof(Node));
temp = r->head;
while (temp != NULL)
{
if (strncmp(temp->dns, dns, strlen(dns)) == 0)
{
return temp;
}
else { temp = temp->next; }
}
printf("No record found");
return (0);
}
/* Maximum function to find the maximum number of requests in the linked list  */
Node * maximum(Root *r) {

	Node *temp = (Node*) malloc(sizeof(Node));
	int max = 0;
	int temp1;
	temp = r->head;
	while(temp!=NULL)
	{
		temp1 = atoi(temp->num);
		
		//exit(0);
		if (temp1 > max) { max = temp1;}
		temp = temp->next;
		
	}
	
	//printf("the maximum is %d", max);
	return(max);
}
/* Minimum function to find the minimum number of requests in the linked list  */
Node * minimum(Root *r,int *max) {

	Node *temp = (Node*)malloc(sizeof(Node));
	int min=*max;
	int temp1;
	temp = r->head;
	while (temp != NULL)
	{
		temp1 = atoi(temp->num);

		//exit(0);
		if (temp1 < min) { min = temp1; }
		temp = temp->next;

	}

	//printf("the maximum is %d", max);
	return(min);
}
// to obtaing timestamp for the logfile
char * timestamp()
{
	char *timest=malloc(sizeof(char)*30);
	time_t ltime; /* calendar time */
	ltime = time(NULL); /* get current cal time */
	//printf("%s", asctime(localtime(&ltime)));
	strcpy(timest, asctime(localtime(&ltime)));
	return timest;
}


void main(int argc, char *argv[])
{
	int servSock;                    /* Socket descriptor for server */
	int clntSock;                    /* Socket descriptor for client */
	struct sockaddr_in echoServAddr; /* Local address */
	struct sockaddr_in echoClntAddr; /* Client address */
	unsigned short echoServPort;     /* Server port */
	unsigned int clntLen;            /* Length of client address data structure */
	WSADATA wsaData;                 /* Structure for WinSock setup communication */
	
	roottime *rt = calloc(1, sizeof(roottime));
	clq *timea = malloc(sizeof(clq));
	rt->head = NULL;
	//struct tm *tm;
	//time_t now;
	if (argc <4)     /* Test for correct number of arguments */
	{
		fprintf(stderr, "Usage:  %s <Server Port>\n", argv[0]);
		exit(1);
	}
	
	echoServPort = atoi(argv[1]);  /* first arg:  Local port */
	
	char *filename;
	filename = (argv[2]);/* second arg:  Path for the text file  */
	int thresh = (argv[3]);/* third arg:  threshhold value for multiple requests */
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) /* Load Winsock 2.0 DLL */
	{
		fprintf(stderr, "WSAStartup() failed");
		exit(1);
	}

	/* Create socket for incoming connections */
	if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		DieWithError("socket() failed");

	/* Construct local address structure */
	memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
	echoServAddr.sin_family = AF_INET;                /* Internet address family */
	echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
	echoServAddr.sin_port = htons(echoServPort);      /* Local port */

													  /* Bind to the local address */
	if (bind(servSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
		DieWithError("bind() failed");

	/* Mark the socket so it will listen for incoming connections */
	if (listen(servSock, MAXPENDING) < 0)
		DieWithError("listen() failed");
	FILE *fp, *f1;
	fp = fopen(filename, "r");
	//f1 = fopen("C:\\Users\\Viraj Deshpande\\Documents\\Visual Studio 2015\\Projects\\linkedlist\\ipaddress.txt", "r");
	f1= fopen(filename, "r");
	Root *r = (Root *)calloc(1, sizeof(Root));
	//r->head->next = NULL;
	r->number = 0;
	if (fp == NULL) {
		printf("Error: file pointer is null.");
		exit(1);
	}
	int maximumLineLength = 128;
	char *lineBuffer = (char *)malloc(sizeof(char) * maximumLineLength);
	if (lineBuffer == NULL) {
		printf("Error allocating memory for line buffer.");
		exit(1);
	}
	char ch;
	int count = 0;
	while (fgets(lineBuffer, maximumLineLength, fp) != NULL)/* Reading the text file and storing into linked list */
	{
		char *line = (char *)malloc(sizeof(char) * maximumLineLength);
		char ch = getc(f1);
		int count = 0;

		while ((ch != '\n') && (ch != EOF))
		{
			if (count == maximumLineLength)
			{
				maximumLineLength += 128;
				line = realloc(line, maximumLineLength);
				if (line == NULL) {
					printf("Error reallocating space for line buffer.");
					exit(1);
				}
			}
			line[count] = ch;
			//printf("%c",ch);
			count++;
			ch = getc(f1);
		}
		line[count] = '\0';
		char dns[30] = "";
		char num[5] = "";
		char(*ip) = calloc(128, sizeof(char));

		int j = 0; int k = 0, cnt = 0, i = 0, b = 0;
		while (i < strlen(line)) {

			//for(i=0;i<strlen(line);i++){
			if (line[i] == ' ') {

				cnt++;
				i++;
			}
			if (line[i] != ' ' && cnt == 0) {
				dns[j] = line[i];
				//printf("%c",dns[j]);
				j++;
				i++;
				continue;
			}
			else if (line[i] != ' ' && cnt == 1) {

				num[k] = line[i];
				k++;
				i++;
				continue;
			}
			else if (line[i] != '/0' && cnt >= 2)
			{
				int n;
				int a = 0;
				while (line[i] != '\0')
				{
					ip[a] = line[i];
					a++;
					i++;
				}

				//printf(" %s \n",ip);

				//b++;
			}
			//addnode(dns,num,ip)
		}
		//printf("%s \n",num);
		//printf("value of b is %d ",b);
		insert(r, dns, num, ip);

		free(line);
	}
	traverse(r);
	//Run in an infinite loop-accept  client socket	
	time_t clk = NULL;

	for (;;) /* Run forever */
	{
		/* Set the size of the in-out parameter */
		clntLen = sizeof(echoClntAddr);

		/* Wait for a client to connect */
		if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, &clntLen)) < 0)
			DieWithError("accept() failed");
		/*now = time(0);
		if ((tm = localtime(&now)) == NULL) {
			printf("Error extracting time stuff\n");
			return 1;
		}*/
		
		
		if (rt->head==NULL)
		{
			strcpy(timea->clientip, inet_ntoa(echoClntAddr.sin_addr));
			timea->clk = (unsigned long)time(NULL);
			
			//timea->next = NULL;
			timea->next = rt->head;
			rt->head = timea;
			//printf(" this is the new time at the header  %20d", rt->head->clk);
			rt->head->next = NULL;
			rt->count_time = 1;
			
		}
		else 
		{
			clq *timeb = malloc(sizeof(clq));
			strcpy(timeb->clientip, inet_ntoa(echoClntAddr.sin_addr));
			timeb->clk = (unsigned long)time(NULL);
			//printf(" this is the new time %20d", timeb->clk);
			//printf("The root node time %ld", rt->head->clk);
			int temptime=searchtime(timeb->clientip,timeb->clk,rt,thresh);
			if (temptime==0)
			{ timeb->next = rt->head;
			rt->head = timeb;
			rt->count_time = rt->count_time + 1;
			}
		}

		/* clntSock is connected to a client! */

		printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));
		char echoBuffer[32] = "";        /* Buffer for echo string */
		int recvMsgSize;                    /* Size of received message */
													/* Receive message from client */
		if ((recvMsgSize = recv(clntSock, echoBuffer, 32, 0)) < 0)
			DieWithError("recv() failed");

		//printf("this is the echo buffer %s \n", echoBuffer);

		// extracting from the echo buffer
		char *token;
		char *next_token1 = NULL;
		//get the first token
		token = strtok(echoBuffer, " ");
		int com = atoi(token);
		Node * temp1 = malloc(sizeof(Node));
		Node *temp2 = malloc(sizeof(Node));
		char dns_search[30] = "";
		char dns_add[30] = "";
		char ip_add[30] = "";
		char dns_del[30] = "";
		int max = 0;
		int min = 0;
		//FILE *logfile = fopen("C:\\Users\\Viraj Deshpande\\Documents\\Visual Studio 2015\\Projects\\linkedlist\\logfile.txt", "a");
		FILE *logfile = fopen("logfile.txt", "a");
		switch (com)   /* Perform different functions depending on te command code */
		{
		case 1:   /* serach for IP address for a given domain name */
			token = strtok(NULL, "\0");
			int d = 0;
			//int len = strlen(token);
			//token[len] = " ";
			while (token[d])
			{
				token[d]=putchar(tolower(token[d]));
				d++;
			}
			
			strcpy(dns_search, token);
			printf("this is the domain name %s", dns_search);
			temp1 = search(r, dns_search);
			char *ip=malloc(sizeof(char)*25);
			//strcpy(echoBuffer, temp1->ip);
			if (temp1==NULL)/* If tthe domain name is not in the linked list */
			{
				char *valid_ip=gethost(dns_search);
				
				printf("%s", valid_ip);
				if (send(clntSock, valid_ip, strlen(valid_ip), 0) != strlen(valid_ip))
					DieWithError("send() failed");
				strcpy(ip, valid_ip);
			
			}
			else {
				int num = atoi(temp1->num);// incrementing the number of times the domain has been requested by 1 
				num = num + 1;
				itoa(num, temp1->num, 10);
				if (send(clntSock, temp1->ip, strlen(temp1->ip), 0) != strlen(temp1->ip)) /* found the ip using gethost function */
					DieWithError("send() failed");
				strcpy(ip, temp1->ip);
				//printf("%s", temp1->ip);
			}
			if (ip!=NULL)
				/* Storing the contents into a logfile along with time stamp */
			fprintf(logfile, "%s  the ip adress for %s is  %s requested by %s \n",timestamp() , dns_search,ip, inet_ntoa(echoClntAddr.sin_addr));
			fclose(logfile);
			break;
		case 2:   /* insert the domain name and ip address into the linked list  */
			token = strtok(NULL, " ");
			 d = 0;
			while (token[d])
			{
				putchar(tolower(token[d])); // convert to lower case
				d++;
			}
			strcpy(dns_add, token);
			token = strtok(NULL, "\0");
			strcpy(ip_add, token);
			temp1 = search(r, dns_add);
			if (temp1 != NULL) { printf("Already exists"); break; }
			else {
				insert(r, dns_add, "0", ip_add);
			}
			/* Storing the contents into a logfile along with time stamp */
			fprintf(logfile, "%s %s  %s record added by client %s  \n", timestamp(), dns_add, ip_add, inet_ntoa(echoClntAddr.sin_addr));
			fclose(logfile);
			printf("\n \n");
			traverse(r);
			break;
		case 3:/* deleting the record if it exists  */
			token = strtok(NULL, "\0");
			d = 0;
			while (token[d])
			{
				putchar(tolower(token[d]));
				d++;
			}
			strcpy(dns_del, token);
			printf("the DNS to be deleted %s \n", dns_del);
			temp1 = search(r, dns_del);
			if (temp1 != NULL) { delete(r, dns_del); fprintf(logfile, "%s  the record deleted  is  %s requested by %s \n", timestamp(), dns_del, inet_ntoa(echoClntAddr.sin_addr));
			fclose(logfile); traverse(r); }
			else { printf("Record does not exist"); }
			break;

		case 4:/* Returning record with maximum number of requests  */
			max = maximum(r);
			char maxstr[15];
			sprintf(maxstr, "%d", max);
			printf("size of max is--->%d", strlen(maxstr));
			if (send(clntSock, maxstr, strlen(maxstr), 0) <= strlen(maxstr))
				DieWithError("send() failed");
			/* Storing the contents into a logfile along with time stamp */
			fprintf(logfile, "%s  the maxnumber of times query called %d requested by %s \n", timestamp(), max,inet_ntoa(echoClntAddr.sin_addr));
			fclose(logfile);
			printf(" the maximum number of times a query is called is %d", max);
			break;
		case 5: /* Returning record with minimum number of requests  */
			max = maximum(r);
			min = minimum(r, &max);
			char minstr[15];
			sprintf(minstr, "%d", min);
			printf("size of max is--->%d", strlen(minstr));
			if (send(clntSock, minstr, strlen(minstr), 0) <= strlen(minstr))
				DieWithError("send() failed");
			/* Storing the contents into a logfile along with time stamp */
			fprintf(logfile, "%s  the minimum  of times query called %d requested by %s \n", timestamp(), min, inet_ntoa(echoClntAddr.sin_addr));
			fclose(logfile);
			printf("the minimum number of times a query is called is %d", min);
			break;
		case 6:/* Shutdown after storing the linked list in an outpt file for a given security code */
			token = strtok(NULL, "\0");
			if (strcmp(token, "1000") == 0)
			{
				temp2 = r->head;
				//FILE *fout = fopen("C:\\Users\\Viraj Deshpande\\Documents\\Visual Studio 2015\\Projects\\linkedlist\\outputfile.txt", "w");
				FILE *fout = fopen("outputfile.txt", "w");
				while (temp2 != NULL)
				{
					printf("%s %s %s \n", temp2->dns, temp2->num, temp2->ip);
					/* Storing the contents into a logfile along with time stamp */
					fprintf(fout, "%s %s %s \n", temp2->dns, temp2->num, temp2->ip);
					
					
					temp2 = temp2->next;
				}
				fprintf(logfile, "%s  shutdown requested by %s \n", timestamp(), inet_ntoa(echoClntAddr.sin_addr));
				fclose(fout);
				fclose(logfile);
				closesocket(clntSock);
				break;
			}

		}

	}

}