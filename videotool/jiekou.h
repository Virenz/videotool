#pragma once
#include <map>
#include <string>
#include "json\json\json.h"

class JieKou
{
public:
	JieKou();
	~JieKou();

	int readandparseJsonFromFile();
	void setJieKou(std::string& key, std::string& value);
	std::multimap<std::string, std::string>& getJieKou();
private:
	std::multimap<std::string, std::string> jiekou;
};