#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#include <cstdio>
#include <iostream>
#include <cstring>
#include <cstdlib>

using namespace std;

int Connect() {
	sockaddr_in pin;
	hostent *nlp_host;
	int sd;
	char host_name[256];
	int port;

	strcpy(host_name,"115.28.136.33");
	port=12345;

	while((nlp_host=gethostbyname(host_name))==0)
		cerr<<"Resolve Error!\n";

	bzero(&pin,sizeof(pin));
	pin.sin_family=AF_INET;
	pin.sin_addr.s_addr=htonl(INADDR_ANY);
	pin.sin_addr.s_addr=((in_addr*)(nlp_host->h_addr))->s_addr;
	pin.sin_port=htons(port);

	sd=socket(AF_INET,SOCK_STREAM,0);
	while(connect(sd,(sockaddr*)&pin,sizeof(pin))==-1) {
		cerr<<"Connect Error!\n";
		sleep(1.4);
	}

	return sd;
}

char temp[100000];

void Process(int sd) {
	cerr<<"Ok,connected\n";

	int t;
	while(1) {
		cin>>temp;
		send(sd,temp,strlen(temp),0);
		if(strcmp(temp,"quit")==0) break;

		t=recv(sd,temp,100000,0);
		temp[t]=0;
		cerr<<temp<<endl;
	}
	cerr<<"Close Connect.\n";
	close(sd);
}

int main() {
	int sd=Connect();
	Process(sd);

	return 0;
}
