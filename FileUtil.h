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

Description: FileUtil class
*/

#pragma once

using namespace std;

class FileUtil {
public:
	static wstring GetDatabaseFolder();
	static bool FolderExist(const wstring& path);
	static bool CreateFolder(const wstring& path);
	static bool FileExist(const wstring& fileName);
	static wstring GetFileNameFromPath(const wstring& path);
	static wstring GetFolderNameFromPath(const wstring& path);	
};