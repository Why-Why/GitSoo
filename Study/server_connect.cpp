//server2.cpp

#include<iostream>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<cstdlib>
#include<iomanip>
#include<cstring>
#include<mysql/mysql.h>
#include<string>

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



                socklen_t sin_size = sizeof(sockaddr_in);
                while(( accept_fd =
                accept(sf, (sockaddr*)&client, &sin_size)) == -1)
                {
                        cerr << "Accept error!\n";
                        continue;
                }


        while(1)
        {
                
                {
                        char buffer[MAXSIZE];
                        int i;
                        buffer[recv(accept_fd,buffer,MAXSIZE,0)] = 0;
                        MYSQL mysql;
			mysql_init(&mysql);
			mysql_real_connect(&mysql, "localhost", "root", "toushirou", "hijikata", 3306, NULL, 0);
			MYSQL_RES *result = NULL;
                        if( strcmp(buffer, "1") == 0)
                        {
				char account[MAXSIZE],password[MAXSIZE];
				account[recv(accept_fd, account, MAXSIZE, 0)] = 0;
				password[recv(accept_fd, password, MAXSIZE, 0)] = 0;
				string sql = "select * from connect where account = '" + string(account) + "';";
				mysql_query(&mysql, sql.c_str());
				result = mysql_store_result(&mysql);
				int rowcount = mysql_num_rows(result);
				if(rowcount)
				{
					char exist[256] =  "The account is already exist.\n";
					send(accept_fd, exist, 256, 0);
					mysql_close(&mysql);
					continue;
				}
				sql = "insert into connect(account, password) values ('" 
						+ string(account) + "', '" + string(password) + "');";
				mysql_query(&mysql, sql.c_str());
				mysql_close(&mysql);
				char finish[256] = "Successful!\n";
                                send(accept_fd, finish, 256, 0);
                                continue;
                        }

                        else if( strcmp(buffer, "0") == 0)
                        {
                                
 				buffer[recv(accept_fd, buffer, MAXSIZE, 0)] = 0;
			 	char pw[MAXSIZE];
                                pw[recv(accept_fd, pw, MAXSIZE, 0)] = 0;
                                string sql = "select * from connect where account = '" + string(buffer) + 
						"' and password = '" + string(pw) + "';";
				mysql_query(&mysql, sql.c_str());
				result = mysql_store_result(&mysql);
				int rowcount = mysql_num_rows(result);
				if(rowcount)
				{	
					char win[256] = "log in!\n";
                                	send(accept_fd, win, 256, 0);
                                }
				else
				{
					char fail[256] = "Failed.\n";
                                	send(accept_fd,fail,256, 0);
                                }
                	}
                }
        }
        close(accept_fd);

}
int main()
{
        int sf = tcp_server();
        solve(sf);
}

