#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#include <iostream>
#include <cstring>
#include <cstdlib>

using namespace std;

int connect() {
	sockaddr_in pin;
	hostent *nlp_host;
	int sd;
	//char host_name[]="115.28.136.33";
	char host_name[]="192.168.1.101";
	int port=22222;

	while(!(nlp_host=gethostbyname(host_name))) cerr<<"Resolve Error!\n";

	bzero(&pin,sizeof(pin));
	pin.sin_family=AF_INET;
	pin.sin_addr.s_addr=htonl(INADDR_ANY);
	pin.sin_addr.s_addr=((in_addr*)(nlp_host->h_addr))->s_addr;
	pin.sin_port=htons(port);

	sd=socket(AF_INET,SOCK_STREAM,0);
	while(connect(sd,(sockaddr*)&pin,sizeof(pin))==-1) {
		cerr<<"Connect Error!\n";
		sleep(1.2);
	}

	return sd;
}

void solve(int sd) {
	cerr<<"Connected!\n";

	char temp[256];

	while(1) {
		do {
			cout<<"What to do ?\n(0 for sign in, 1 for sign up, 2 for quit)\n";
			cin>>temp;
		}while(strcmp(temp,"0") && strcmp(temp,"1") && strcmp(temp,"2"));

		send(sd,temp,strlen(temp),0);
		if(strcmp(temp,"2")==0) break;

		cout<<"Enter in your account:\n";
		cin>>temp;
		send(sd,temp,strlen(temp),0);

		cout<<"Enter in your passward:\n";
		cin>>temp;
		send(sd,temp,strlen(temp),0);

		temp[recv(sd,temp,256,0)]=0;
		cerr<<temp<<endl;
		cerr<<endl;
	}
}

int main() {
	int sd=connect();
	solve(sd);

	cerr<<"Close Connect.\n";
	close(sd);

	return 0;
}
