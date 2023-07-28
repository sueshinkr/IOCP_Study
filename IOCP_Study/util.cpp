#include "util.h"

void ErrorHandling(const char* message, int64_t errorcode)
{
	std::cout << message << std::endl;

	if (errorcode != -1) {
		std::cout << "errorcode : " << errorcode << std::endl;
	}
}

void PrintMessage(const char* message)
{
	std::cout << message << std::endl;
}