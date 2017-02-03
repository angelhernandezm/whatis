// Copyright (C) 2015 Angel Hernandez Matos / Bonafide Ideas.
// You can redistribute this software and/or modify it under the terms of the 
// Microsoft Reciprocal License (Ms-RL).  This program is distributed in the hope 
// that it will be useful, but WITHOUT ANY WARRANTY; without even the implied 
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
// See License.txt for more details. 

/* C++ compiler   : Microsoft (R) C/C++ Optimizing Compiler Version 19.00.23026 for x64
Creation date     : 04/11/2015
Developer         : Angel Hernandez Matos
e-m@il            : angel@bonafideideas.com
Website           : http://www.bonafideideas.com

Description: Entry point for console application
*/

#include "stdafx.h"
#include <iostream>
#include "WhatIsCore.h"

#pragma warning(disable:4996 4267)

using namespace std;

/// <summary>
/// Mains this instance.
/// </summary>
/// <returns>int.</returns>
int main(int argc, char *argv[]) {
	char* targetFile;
	WhatIsCore whatis;
	wchar_t buffer[MAX_PATH];
	ZeroMemory(buffer, MAX_PATH);

	if (argc == 2 && (targetFile = argv[1]) != nullptr) {
		if (mbstowcs(buffer, targetFile, strlen(targetFile)) > 0)
			whatis.Execute(wstring(buffer));
	} else wcout << InvalidNumberArgs << endl;

	return 0;
}

