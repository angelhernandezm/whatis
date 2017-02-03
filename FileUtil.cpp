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

Description: Implementation of FileUtil class
*/

#include "stdafx.h"
#include "FileUtil.h"
#include <sys/stat.h>

#pragma warning(disable:4996)

/// <summary>
/// Gets the database folder.
/// </summary>
/// <returns>wstring.</returns>
wstring FileUtil::GetDatabaseFolder() {
	wstring retval;
	auto buffer = reinterpret_cast<wchar_t*>(CoTaskMemAlloc(MAX_PATH));
	buffer[0] = '\0';

	if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, NULL, NULL, &buffer)))
		retval.append(buffer) + HomeFolder;

	CoTaskMemFree(buffer);

	return retval;
}

/// <summary>
/// Files the exist.
/// </summary>
/// <param name="fileName">Name of the file.</param>
/// <returns>bool.</returns>
bool FileUtil::FileExist(const wstring& fileName) {
	struct _stat buffer;
	char fileToCheck[MAX_PATH];
	return (wcstombs(fileToCheck, fileName.c_str(), MAX_PATH) > 0 && _stat(fileToCheck, &buffer) == 0);
}

/// <summary>
/// Gets the folder name from path.
/// </summary>
/// <param name="path">The path.</param>
/// <returns>wstring.</returns>
wstring FileUtil::GetFolderNameFromPath(const wstring& path) {
	wstring retval;
	wchar_t szDir[_MAX_DIR];
	wchar_t szExt[_MAX_EXT];
	wchar_t szFName[_MAX_FNAME];
	wchar_t szDrive[_MAX_DRIVE];

	if (!path.empty()) {
		if (_wsplitpath_s(path.c_str(), szDrive, Array_Size(szDrive), szDir, Array_Size(szDir),
			szFName, Array_Size(szFName), szExt, Array_Size(szExt)) == 0) {
			retval = wstring(szDrive) + wstring(szDir);
		}
	}

	return retval;
}

/// <summary>
/// Gets the file name from full path.
/// </summary>
/// <param name="path">The path.</param>
/// <returns>wstring.</returns>
wstring FileUtil::GetFileNameFromPath(const wstring& path) {
	wstring retval;
	wchar_t szDir[_MAX_DIR];
	wchar_t szExt[_MAX_EXT];
	wchar_t szFName[_MAX_FNAME];
	wchar_t szDrive[_MAX_DRIVE];

	if (!path.empty()) {
		if (_wsplitpath_s(path.c_str(), szDrive, Array_Size(szDrive), szDir, Array_Size(szDir),
			szFName, Array_Size(szFName), szExt, Array_Size(szExt)) == 0) {
			retval = wstring(szFName) + wstring(szExt);
		}
	}

	return retval;
}

/// <summary>
/// Folders the exist.
/// </summary>
/// <param name="path">The path.</param>
/// <returns>bool.</returns>
bool FileUtil::FolderExist(const wstring& path) {
	auto retval = false;

	if (!path.empty())
		retval = PathFileExists(path.c_str()) ? true : false;

	return retval;
}

/// <summary>
/// Creates the folder.
/// </summary>
/// <param name="path">The path.</param>
/// <returns>bool.</returns>
bool FileUtil::CreateFolder(const wstring& path) {
	auto retval = false;

	if (!path.empty())
		retval = CreateDirectory(path.c_str(), nullptr) != NULL ? true : false;

	return retval;
}