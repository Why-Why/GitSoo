//server2.cpp

#include<iostream>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<cstdlib>
#include<fstream>
#include<iomanip>
#include<cstring>

#define MAXSIZE 256
using namespace std;

int tcp_server()
{
        int socket_fd;
        sockaddr_in server;

        if(( socket_fd = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP)) < 0)
			cerr << "socket() failed.\n";

        memset(&server, 0, sizeof(server));
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = htonl(INADDR_ANY);
        server.sin_port = htons(22222);
        if( bind(socket_fd, (sockaddr*)&server, sizeof(server)) < 0 )
			cerr << "bind() failed.\n";

        if( listen(socket_fd, 10) < 0 )
			cerr << "listen() failed.\n";

        return socket_fd;
}

void solve(int sf)
{
	int accept_fd;
	sockaddr_in client;

	while(1) {
		socklen_t sin_size = sizeof(sockaddr_in);

		if(( accept_fd = accept(sf, (sockaddr*)&client, &sin_size)) == -1)
		{
			cerr << "Accept error!\n";
			continue;
		}

		if(!fork()) {
			while(1)
			{
				char buffer[MAXSIZE];
				int i;
				
				buffer[recv(accept_fd,buffer,MAXSIZE,0)] = 0;
				fstream file;
               	const char* filename = "test.txt";
				file.open(filename,ios::in |
				ios::app | ios::out | ios::binary);
					
				if( strcmp(buffer, "1") == 0)
				{
					buffer[recv(accept_fd,buffer,MAXSIZE,0)] = 0;
					file.write(buffer,256);	
					buffer[recv(accept_fd,buffer,MAXSIZE,0)] = 0;
					file.write(buffer,256);
					char finish[256] = "Successful!\n";
					send(accept_fd, finish, 256, 0);
					file.close();
					continue;
				}
				else if( strcmp(buffer, "0") == 0)
				{
					file.seekg(0, ios::beg);
					buffer[recv(accept_fd, buffer, MAXSIZE, 0)] = 0;
					char pw[MAXSIZE];
					pw[recv(accept_fd, pw, MAXSIZE, 0)] = 0;
					char account[MAXSIZE] = "";
					char password[MAXSIZE] = "";
					bool flag = false;
					while(!file.eof())
					{
						file.read(account, 256);
						file.read(password, 256);
						if(strcmp(buffer, account) == 0)
						{
							flag = true;
							if(strcmp(pw, password) == 0)
							{	
								char win[256] = "log in!\n";
								send(accept_fd, win, 256, 0);
								break;
							}
							else
							{
								char fail[256] = "password is wrong.\n";
								send(accept_fd,fail,256, 0);
								break;
							}
						}
				    }
					file.close();
					cout << "After break,where are you." << endl;
					if(flag == false)
					{
						char text[256] = "cannot find account.\n";
						send(accept_fd, text, 256,0);
						continue;
					}
					cout << "What is it!" << endl;
				}
			}
		}
		close(accept_fd);
	}
}
int main()
{
	int sf = tcp_server();
	solve(sf);
}
