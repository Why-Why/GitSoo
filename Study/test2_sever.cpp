// complie by : g++ test2_sever.cpp -o test2_sever -l mysqlclient

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#include <mysql/mysql.h>

#include <iostream>
#include <cstring>
#include <cstdlib>

using namespace std;

class MyMySql {
	private:
		MYSQL *connect;

		
	public:
		MYSQL_RES *res;
		int process(const char[]);
		int error;
		void solve_error();

		MyMySql();
		~MyMySql();

		int insert(const char [],const char []);
		int find(const char [],const char []);
};

class MySocket {
	private:
		int severSocket;
		int newSocket;
		sockaddr_in severAddr;
		sockaddr_in clientAddr;
		int port;
		int error;

		MyMySql severSql;

		void solve_error();
		void connect();
		void process();
		void close_client();

		void sign_in(const char[],const char[]);
		void sign_up(const char[],const char[]);

	public:
		MySocket(int=22222);
		~MySocket();
		
		void solve();
};

//////////////////////////////////////

int main() {
	MySocket a;
	a.solve();
	return 0;
}

//////////////////////////////////////

MySocket::MySocket(int _port) {
	error=0;
	if(severSql.error) { error=3; return; }
	port=_port;
	severSocket=socket(AF_INET,SOCK_STREAM,0);

	int val=1;
	setsockopt(severSocket,SOL_SOCKET,SO_REUSEADDR,&val,sizeof(val));

	severAddr.sin_family=AF_INET;
	severAddr.sin_port=htons(port);
	severAddr.sin_addr.s_addr=htonl(INADDR_ANY);
	memset(&(severAddr.sin_zero),0,8);

	int rc=bind(severSocket,(sockaddr*)&severAddr,sizeof(sockaddr));
	if(rc==-1) { error=1; return; }

	rc=listen(severSocket,5);
	if(rc==-1) { error=2; return; }
}

MySocket::~MySocket() {
	if(error) return;
	close(severSocket);
}

void MySocket::solve() {
	if(error) {
		solve_error();
		return;
	}

	while(1) {
		connect();
		process();
		close_client();
	}
}

void MySocket::solve_error() {
	switch(error) {
		case 0:
			cerr<<"No error!\n";
			break;
		case 1:
			cerr<<"Bad bind!\n";
			break;
		case 2:
			cerr<<"Bad listen!\n";
			break;
		case 3:
			cerr<<"Database Error!\n";
			severSql.solve_error();
			break;
		default:
			cerr<<"Unknown error!\n";
			break;
	}
}

void MySocket::connect() {
	cerr<<"OK, Wait for connect.\n";
	
	int size=sizeof(sockaddr_in);
	newSocket=accept(severSocket,(sockaddr*)&clientAddr,(socklen_t*)&size);
}

void MySocket::close_client() {
	cerr<<"Close.\n";
	close(newSocket);
}

void MySocket::process() {
	cerr<<"OK, connected.\n";
	
	int t;
	char temp[256];
	char account[256],passward[256];

	while(1) {
		t=recv(newSocket,temp,256,0);
		if(t<=0) break;
		temp[t]=0;
		if(strcmp(temp,"2")==0) break;

		t=recv(newSocket,account,256,0);
		if(t<=0) break;
		account[t]=0;
		t=recv(newSocket,passward,256,0);
		if(t<=0) break;
		passward[t]=0;

		if(strcmp(temp,"0")==0) sign_in(account,passward);
		else if(strcmp(temp,"1")==0) sign_up(account,passward);
	}
}

void MySocket::sign_in(const char acc[],const char pas[]) {
	char ret[256];
	int t;

	t=severSql.find(acc,pas);
	if(t==0) strcpy(ret,"Yes!");
	else if(t==1) strcpy(ret,"No such user!");
	else if(t==2) strcpy(ret,"Password is't right!");

	send(newSocket,ret,strlen(ret),0);
}

void MySocket::sign_up(const char acc[],const char pas[]) {
	char ret[256];
	int t;

	t=severSql.find(acc,pas);
	if(t!=1) strcpy(ret,"The user has existed!");
	else {
		if(severSql.insert(acc,pas)==0) strcpy(ret,"Successful sign up!");
		else strcpy(ret,"Unkonw sign up error!");
	}

	send(newSocket,ret,strlen(ret),0);
}

//////////////////////////////////////

MyMySql::MyMySql() {
	error=0;
	connect=NULL;

	connect=mysql_init(NULL);
	if(connect==NULL) { error=1; return; }
	
	connect=mysql_real_connect(connect,"localhost","root","zhang123","test1",0,NULL,0);
	if(connect==NULL) { error=2; return; }
}

MyMySql::~MyMySql() {
	if(error) return;
	if(!connect) mysql_close(connect);
}

void MyMySql::solve_error() {
	switch(error) {
		case 0:
			cerr<<"No error!\n";
			break;
		case 1:
			cerr<<"Init Error!\n";
			break;
		case 2:
			cerr<<"Connect Error!\n";
			break;
		default:
			cerr<<"Unkonw Error!\n";
			break;
	};
}

int MyMySql::process(const char comm[]) {
	if(mysql_query(connect,comm)) return 1;

	res=mysql_store_result(connect);
	if(res==NULL) return 3;
	if(mysql_num_rows(res)==0) return 4;

	return 0;
}

int MyMySql::find(const char acc[],const char pas[]) {
	char comm[256]="SELECT password FROM user WHERE account='";
	strcat(comm,acc);
	strcat(comm,"'");

	int t=process(comm);
	if(t) return 1;

	int ok=2;

	MYSQL_ROW row;
	while(1) {
		row=mysql_fetch_row(res);
		if(!row) break;
		if(strcmp(row[0],pas)==0) ok=0;
	}

	if(res) mysql_free_result(res);

	return ok;
}

int MyMySql::insert(const char acc[],const char pas[]) {
	char comm[256]="INSERT INTO user VALUES(\"";
	strcat(comm,acc);
	strcat(comm,"\",\"");
	strcat(comm,pas);
	strcat(comm,"\")");

	int t=process(comm);

	if(res) mysql_free_result(res);

	if(t && t!=4 && t!=3) return 1;
	return 0;
}
