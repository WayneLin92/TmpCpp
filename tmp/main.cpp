// tmp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "mymath.h"
#include <iostream>

int main()
{
	Poly s1 = { {1, 2}, {0, 2, 3}, {3, 2, 1} };
	Poly s2 = { {3, 2, 1}, {0, 1}, {0, 2} };
	std::sort(s1.begin(), s1.end());
	std::sort(s2.begin(), s2.end());
	std::cout << (s1 ^ s2) << std::endl;
}