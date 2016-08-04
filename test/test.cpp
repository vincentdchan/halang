// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include "../halang/string.h"

using namespace std;

int main()
{
	halang::IString s("abc");
	cout << s << endl;
	string ch;
	cin >> ch;
    return 0;
}

