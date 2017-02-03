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

Description: QueryFileInformation and related structures
*/

#pragma once

#include <string>

#pragma warning(disable:4996)

using namespace std;

// Struct
/// <summary>
/// Struct FileInformation
/// </summary>
struct FileInformation {
	wstring Name;
	wstring Location;
	wstring Description;
	wstring ErrorMessage;

	/// <summary>
	/// Initializes a new instance of the <see cref="FileInformation"/> struct.
	/// </summary>
	FileInformation() {

	}

	/// <summary>
	/// Initializes a new instance of the <see cref="FileInformation"/> struct.
	/// </summary>
	/// <param name="errMessage">The error message.</param>
	FileInformation(const wchar_t* errMessage) : ErrorMessage(wstring(errMessage)) {

	}

	/// <summary>
	/// Determines whether this instance is empty.
	/// </summary>
	/// <returns>bool.</returns>
	bool IsEmpty() const {
		return (Name.empty() && Location.empty() && Description.empty());
	}
};

/// <summary>
/// Class QueryFileInformation.
/// </summary>
class QueryFileInformation {
private:
	FileInformation GetFileInformationHelper(const wstring& targetFile);

public:
	/// <summary>
	/// Gets the file information.
	/// </summary>
	/// <param name="targetFile">The target file.</param>
	/// <returns>FileInformation.</returns>
	FileInformation GetFileInformation(const wstring& targetFile);
};

