#ifndef __HTTP_TOOL_H__
#define __HTTP_TOOL_H__

#include "curl/curl.h"
#pragma comment(lib, "libcurl.lib") 

typedef struct MemoryStruct {
	char *memory;
	size_t size;
}_MEMORYSTRUNCT;

class HttpTool
{
public:
	HttpTool();
	~HttpTool();

	bool httpGet(const char* sourses);
	bool httpPost(const char* url, const char* strpost);
	char* getReponseHTML();
	int getReponseHTMLSize();
	void cleanChunk();

private:
	_MEMORYSTRUNCT chunk;
};
#endif  // __HTTP_TOOL_H__