#include "DJson.h"

DJson::DJson(string str)
{
	strJson = str;

	Json::Reader reader;
	Json::Value value;
	if(reader.parse(strJson, value))
	{
		strMethod = value["method"].asString();
		cout << "DJson::strMethod = " << strMethod << endl;

		Json::Value info = value["info"];
		strInfo = value["info"].toStyledString();
		if(info.isNull())
		{
			cout << "info is NULL " << strInfo << endl;
			return;
		}

		cout << "DJson::strInfo = " << strInfo << endl;
	}
}

string DJson::makeDefaultJson()
{
	Json::Value root;

	root["ret"] = -2;

	return root.toStyledString();
}

string DJson::makeItemList(int retValue)
{
	Json::Value root;
	Json::Value arrayObj;
	Json::Value item;

	root["ret"] = retValue;

	return root.toStyledString();
}

string DJson::makeLoginJson(int retValue)
{
	Json::Value root;
	Json::Value arrayObj;
	Json::Value item;

	root["ret"] = retValue;

	return root.toStyledString();
}

int DJson::parseItemId()
{
	cout << strInfo << endl;
	Json::Reader reader;
	Json::Value value;
	if(reader.parse(strInfo, value))
	{
		return value["id"].asInt();
	}
}

void DJson::parseLoginJson()
{
	//string test ="{\"account\":"double", \"passwd\":\"***\"}";
	cout << strInfo << endl;
	Json::Reader reader;
	Json::Value value;
	if(reader.parse(strInfo, value))
	{
		strAccount = value["account"].asString();
		strPasswd = value["passwd"].asString();
		/*
		if(!value["account"].isNull())
		{
			cout << value["account"].asString() << endl;
			cout << value["passwd"].asString() << endl;
		}
		*/
	}
}

string DJson::getMethod()
{
	return strMethod;
}

string DJson::getInfo()
{
	return strInfo;
}

string DJson::getAccount()
{
	return strAccount;
}

string DJson::getPasswd()
{
  return strPasswd;
}

/*
string DJson::getItemList()
{
}

string DJson::getItemList()
{
}
*/
