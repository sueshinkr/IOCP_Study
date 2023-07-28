#pragma once

#include <cstdint>
#include <iostream>

void ErrorHandling(const char* message, int64_t errorcode = -1)
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