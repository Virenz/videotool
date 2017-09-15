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
		//ɾ��mapԪ��
		jiekou.erase(iter++); //#1
	}
}

int JieKou::readandparseJsonFromFile()
{
	Json::Reader reader;// ����json��Json::Reader   
	Json::Value root; // Json::Value��һ�ֺ���Ҫ�����ͣ����Դ����������͡���int, string, object, array         

	std::filebuf in;
	if (!in.open("jiekou", std::ios::in)) {
		std::cout << "fail to open file" << std::endl;
		return -1;
	}
	std::istream iss(&in);

	if (reader.parse(iss, root, 0))
	{
		Json::Value::Members arrayMember = root.getMemberNames(); //ayyayMember�ǳ�Ա���Ƶļ��ϣ���name,age;
		for (Json::Value::Members::iterator iter = arrayMember.begin(); iter != arrayMember.end(); ++iter) //����json��Ա
		{
			Json::Value valueList = root.get(*iter, "null");
			setJieKou(*iter, valueList.asString());
		}
	}
	return 0;
}

std::multimap<std::string, std::string>& JieKou::getJieKou()
{
	// TODO: �ڴ˴����� return ���
	return jiekou;
}

void JieKou::setJieKou(std::string & key, std::string & value)
{
	jiekou.insert(std::pair<std::string, std::string>(key, value));
}


