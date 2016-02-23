#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <mysql/mysql.h>

using namespace std;

MYSQL *connect=NULL;

void init() {
	connect=mysql_init(NULL);
	if(connect==NULL) {
		cerr<<"ERROR1!\n";
		exit(1);
	}

	connect=mysql_real_connect(connect,"localhost","root","zhang123","test1",0,NULL,0);

	if(connect==NULL) {
		cerr<<"ERROR2!\n";
		exit(1);
	}
}

void close() {
	if(!connect) mysql_close(connect);
}

void process(const char comm[]) {
	cerr<<"OK\n";
	if(mysql_query(connect,comm)) {
		cerr<<"ERROR3!\n";
		return;
	}
	
	MYSQL_RES *res=mysql_use_result(connect);
	MYSQL_ROW row;
	int len;

	if(res==NULL) {
		cerr<<"EMPTY!\n";
		return;
	}

	while(1) {
		row=mysql_fetch_row(res);
		if(!row) break;
		len=mysql_num_fields(res);
		for(int i=0;i<len;++i) cerr<<row[i]<<' ';
		cerr<<endl;
	}

	mysql_free_result(res);
}

int main() {
	init();
	process("SELECT * FROM haha");
	//process("INSERT INTO haha VALUES(\"123\",23,\"qq\")");
	close();
	return 0;
}
