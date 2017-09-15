#include "jiekou.h"
#include <fstream>

JieKou::JieKou()
{
	readandparseJsonFromFile();
}

JieKou::~JieKou()
{
	std::multimap<std::string, std::string>::iterator iter = jiekou.begin();
	while (iter != jiekou.end()) //#1
	{
		//删除map元素
		jiekou.erase(iter++); //#1
	}
}

int JieKou::readandparseJsonFromFile()
{
	Json::Reader reader;// 解析json用Json::Reader   
	Json::Value root; // Json::Value是一种很重要的类型，可以代表任意类型。如int, string, object, array         

	std::filebuf in;
	if (!in.open("jiekou", std::ios::in)) {
		std::cout << "fail to open file" << std::endl;
		return -1;
	}
	std::istream iss(&in);

	if (reader.parse(iss, root, 0))
	{
		Json::Value::Members arrayMember = root.getMemberNames(); //ayyayMember是成员名称的集合，即name,age;
		for (Json::Value::Members::iterator iter = arrayMember.begin(); iter != arrayMember.end(); ++iter) //遍历json成员
		{
			Json::Value valueList = root.get(*iter, "null");
			setJieKou(*iter, valueList.asString());
		}
	}
	return 0;
}

std::multimap<std::string, std::string>& JieKou::getJieKou()
{
	// TODO: 在此处插入 return 语句
	return jiekou;
}

void JieKou::setJieKou(std::string & key, std::string & value)
{
	jiekou.insert(std::pair<std::string, std::string>(key, value));
}


