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

Description: Implementation of WhatIsCore class
*/

#include "stdafx.h"
#include "WhatIsCore.h"
#include "FileUtil.h"
#include "EsentCore.h"
#include "QueryFileInformation.h"
#include <iostream>

/// <summary>
/// Executes the specified target file.
/// </summary>
/// <param name="targetFile">The target file.</param>
void WhatIsCore::Execute(const wstring& targetFile) {
	if (!targetFile.empty()) {
		QueryFileInformation qf;
		vector<ColumnType> columns;
		auto fi = qf.GetFileInformation(targetFile);

		if (!fi.IsEmpty()) {
			EsentCore jetInstance;
			auto dbname = FileUtil::GetDatabaseFolder() + HomeFolder + DatabaseName;

			if (jetInstance.CreateOrOpenDatabase(dbname)) {
				columns.push_back(ColumnType(wstring(Pk_Id_Column), jetInstance.ColumnDefinition_get(), nullptr, GetLengthInBytes(4)));
				columns.push_back(ColumnType(wstring(Name_Column), jetInstance.ColumnDefinition_get(), fi.Name.c_str(), GetLengthInBytes(50)));
				columns.push_back(ColumnType(wstring(Location_Column), jetInstance.ColumnDefinition_get(), fi.Location.c_str(), GetLengthInBytes(MAX_PATH)));
				columns.push_back(ColumnType(wstring(Description_Column), jetInstance.ColumnDefinition_get(), fi.Description.c_str(), GetLengthInBytes(MAX_PATH * 5)));
				auto result = jetInstance.CreateOrRetrieveRecord(columns);

				if (result.Result == ExecutionResult::Success)
					cout << result.Name.c_str() << " - " << result.Description.c_str() << endl;
				else wcout << UnableToRetrieveFileDescription << endl;
			} else wcout << UnableToOpenOrCreateDatabase << endl;
		} else wcout << FileNotFoundErrorMsg;
	} else wcout << NoInputFileWasSpecified << endl;
}
