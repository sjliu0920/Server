#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include "json/json.h"
#include "mysql.h"
#include <string>
#include <iostream>

#include "DJson.h"
using namespace std;

#define SELECT_QUERY "select * from user_table where account = \'%s\' && passwd = \'%s\'"
#define SELECT_ITEM_LIST_QUERY "select item_name, price, star from item_table where type_id = \'%d\'"

typedef void Sigfunc(int);
Sigfunc *signal(int signo, Sigfunc *func);
void sig_chld(int signo);

void Echo(int connfd);
// string GetLoginResult(char* buf);
string DoClientRequest(char* buf);
int CheckForShouldLogin(string account, string passwd);
string getItemList (int id);

int main()
{
	pid_t childpid;
	struct sockaddr_in s_in; //server address structure
	struct sockaddr_in c_in; //client address structure
	int l_fd, c_fd;
	socklen_t len;
	char addr_p[16];
	int port = 8009;
	memset((void *)&s_in, 0, sizeof(s_in));    
	s_in.sin_family = AF_INET; //IPV4 communication domain
	s_in.sin_addr.s_addr = INADDR_ANY; //accept any address
	s_in.sin_port = htons(port); //change port to netchar

	l_fd = socket(AF_INET, SOCK_STREAM, 0); //socket(int domain,  int type,  int protocol)

	bool bReuseaddr = true;
	setsockopt(l_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&bReuseaddr, sizeof(bool));

	linger lig;
	lig.l_onoff = 1;
	lig.l_linger = 0;
	int ilen=sizeof(linger);
	setsockopt(l_fd, SOL_SOCKET,SO_LINGER, (char*)&lig, ilen);

	bind(l_fd, (struct sockaddr *)&s_in, sizeof(s_in));

	listen(l_fd, 10000); //lisiening start
	signal(SIGCHLD, sig_chld);

	cout << "begin" << endl;
	while(1){
		len = sizeof(c_in);
		if((c_fd = accept(l_fd, (struct sockaddr *)&c_in, &len)) < 0 )
		{
			if(errno == EINTR)
				continue;
			else
				cout << "appept error" << endl;
			cout << "accept one client, c_fd = " << c_fd << endl;
		}

		if( (childpid = fork()) == 0 ) {  // child process 
			close(l_fd);
			Echo(c_fd);
			exit(0);
		}

		close(c_fd);

	}
}

void Echo(int connfd)
{
	ssize_t n;
	char buf[1024]; //content buff area
	memset(buf, 0, 1024*sizeof(char));

again:
	while( (n = read(connfd, buf, 1023)) > 0) //read the message send by client
	{
		buf[n] = 0;
		cout << "1. receive str from client: " << buf << endl;

		string str = DoClientRequest(buf);
		cout << "2. send str to client: " << str.c_str() << endl;
		write(connfd, str.c_str(), str.length()); //sent message back to client
	}

	if(n < 0 && errno == EINTR)
		goto again;
	else if(n < 0)
		cout << "read erro !" << endl;
}

string DoClientRequest(char* buf)
{
	cout << "begin = " << buf << endl;
	// { "method":"getItemList" , "info":{"name":"lsj", "id":1 }}
	// { "method":"checkLogin" , "info":{"account":"doe", "passwd":"xxx" }}
	DJson* json = new DJson(buf);
	string method = json->getMethod();
	cout << "begin 2= " << method.c_str() << endl;
	string info = json->getInfo();

	cout << "method = " << method.c_str() << endl;

	string echoToClient = "";
	if(method == "CheckLogin")
	{
		cout << "get CheckLogin" << endl;
		json->parseLoginJson();
		int ret = CheckForShouldLogin(json->getAccount(), json->getPasswd());
		echoToClient = json->makeLoginJson(ret);
	}
	else if(method == "getItemList")
	{
		cout << "getItemList" << endl;
		int id = json->parseItemId();
		cout << "getItemList id = " << id << endl;
		echoToClient = getItemList(id);

	}
	else
	{
		echoToClient = json->makeDefaultJson();
	}

	if(json) {
		delete json;
	}

	cout << echoToClient.c_str() << endl;
	return echoToClient;
}

string getItemList (int id)
{

	MYSQL mysql, *sock;    
	MYSQL_RES *res;       
	MYSQL_FIELD *fd ;    
	MYSQL_ROW row ;     
	char querySQL[160] = {0};

	mysql_init(&mysql);
	if (!(sock = mysql_real_connect(&mysql, "localhost", "root", "lsj", "costco", 0, NULL, 0))) {
		fprintf(stderr, "Couldn't connect to engine!\n%s\n\n", mysql_error(&mysql));
		return "";
	}

	if (!mysql_set_character_set(&mysql, "utf8"))
	{
		printf("New client character set: %s\n", mysql_character_set_name(&mysql));
	}

	sprintf(querySQL, SELECT_ITEM_LIST_QUERY, id);
	printf("querySQL = %s\n", querySQL);
	if(mysql_query(sock, querySQL)) {
		fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
		return "";
	}

	if (!(res = mysql_store_result(sock))) {
		fprintf(stderr,"Couldn't get result from %s\n", mysql_error(sock));
		return "";
	}

	int num = mysql_num_fields(res);
	printf("number of fields returned: %d\n", num);


	Json::Value root;
	Json::Value arrayObj;

	num = 0;
	while (row = mysql_fetch_row(res)) {
		num++;
		printf("get one item: [%s:%s,%s]\n", row[0], row[1], row[2]);
		puts( "query ok !\n" ) ; 

		Json::Value item;
		item["type_name"] = row[0];
		item["price"] = row[1];
		item["star"] = row[2];
		arrayObj.append(item);
	} 

	root["num"] = num;
	root["array"] = arrayObj;


	mysql_free_result(res);
	mysql_close(sock);

	cout << root.toStyledString() << endl;

	return root.toStyledString();
}

int CheckForShouldLogin(string account, string passwd)
{

	MYSQL mysql, *sock;    
	MYSQL_RES *res;       
	MYSQL_FIELD *fd ;    
	MYSQL_ROW row ;     
	char querySQL[160] = {0};

	mysql_init(&mysql);
	if (!(sock = mysql_real_connect(&mysql, "localhost", "root", "lsj", "costco", 0, NULL, 0))) {
		fprintf(stderr, "Couldn't connect to engine!\n%s\n\n", mysql_error(&mysql));
		return -1;
	}

	sprintf(querySQL, SELECT_QUERY, account.c_str(), passwd.c_str());
	printf("querySQL = %s\n", querySQL);
	if(mysql_query(sock, querySQL)) {
		fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
		return -1;
	}

	if (!(res=mysql_store_result(sock))) {
		fprintf(stderr,"Couldn't get result from %s\n", mysql_error(sock));
		return -1;
	}

	printf("number of fields returned: %d\n",mysql_num_fields(res));

	while (row = mysql_fetch_row(res)) {
		printf("get one user: [%s:%s:%s]\n", row[0], row[1], row[2]);
		puts( "query ok !\n" ) ; 

		return 0;
	} 

	mysql_free_result(res);
	mysql_close(sock);

	return -1;
}

Sigfunc *signal(int signo, Sigfunc *func)
{
	struct sigaction act, oact;

	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	if(signo == SIGALRM) {
#ifdef SA_INTERRUPT
		act.sa_flags |= SA_INTERRUPT;
#endif
	} else {
#ifdef SA_RESTART
		act.sa_flags |= SA_RESTART;
#endif
	}

	if(sigaction(signo, &act, &oact) < 0)
		return (SIG_ERR);

	return (oact.sa_handler);
}

void sig_chld(int signo)
{
	pid_t pid;
	int stat;

	while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
			cout << "child pid:" << pid << " terminated" << endl;

	return;
}
