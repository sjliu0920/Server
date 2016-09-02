#include "common.h"
#include "json/json.h"

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
