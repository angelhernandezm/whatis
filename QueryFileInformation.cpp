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

Description: Implementation of QueryFileInformation class
*/

#include "stdafx.h"
#include "FileUtil.h"
#include "QueryFileInformation.h"

/// <summary>
/// Gets the file information helper.
/// </summary>
/// <param name="targetFile">The target file.</param>
/// <returns>FileInformation.</returns>
FileInformation QueryFileInformation::GetFileInformationHelper(const wstring& targetFile) {
	UINT cbSize = 0;
	DWORD hHandle = 0, nSize = 0;
	FileInformation retval(FileNotFoundErrorMsg) ;

	if ((nSize = GetFileVersionInfoSize(targetFile.c_str(), &hHandle)) != NULL) {
		auto lpData = malloc(nSize);
		auto lpBuffer = malloc(MaxDescriptionLength);
		ZeroMemory(lpData, nSize);
		ZeroMemory(lpBuffer, MaxDescriptionLength);

		if (GetFileVersionInfo(targetFile.c_str(), hHandle, nSize, lpData) != NULL) {
			if (VerQueryValue(lpData, DefaultWindowsFileDescriptionKey, &lpBuffer, &cbSize)) {
				retval.Location = targetFile;
				retval.ErrorMessage.clear();
				retval.Name = FileUtil::GetFileNameFromPath(targetFile);
				retval.Description = wstring(reinterpret_cast<wchar_t*>(lpBuffer));
				free(lpData);
			} else retval.ErrorMessage = UnableToRetrieveFileDescription;
		}
	}

	return retval;
}

/// <summary>
/// Gets the file information.
/// </summary>
/// <param name="targetFile">The target file.</param>
/// <returns>FileInformation.</returns>
FileInformation QueryFileInformation::GetFileInformation(const wstring& targetFile) {
	wchar_t fullpath[MAX_PATH] = {0};
	FileInformation retval(FileNotFoundErrorMsg);

	if (!targetFile.empty()) {
		if (wcsncpy(fullpath, targetFile.c_str(), wcslen(targetFile.c_str())) != nullptr) {
			if (FileUtil::FileExist(targetFile))
				retval = GetFileInformationHelper(targetFile);
			else if (PathFindOnPath(fullpath, nullptr))
				retval = GetFileInformationHelper(wstring(fullpath));
		}
	}

	return retval;
}