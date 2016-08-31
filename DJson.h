#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include "json/json.h"
#include "mysql.h"
#include <string>
#include <iostream>
using namespace std;

class DJson
{
	public:
		DJson(string str);
		string makeLoginJson(int);
		void parseLoginJson();
		string getAccount();
		string getPasswd();
		string getMethod();
		string getInfo();

		string makeDefaultJson();
		int parseItemId();
		string makeItemList(int retValue);

	private:
		string strJson;
		string strAccount;
		string strPasswd;
		string strMethod;
		string strInfo;

};
