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

Description: EsentCore class and related structures
*/

#pragma once

#include <esent.h>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <memory>
#include <string>

using namespace std;

#pragma warning(disable:4996)

namespace std{
	class strstream;
}

// Enum
enum RecordState {
	New, Existing
};

enum ExecutionResult {
	Success, Failure
};

// Structs
/// <summary>
/// Struct ColumnInfo
/// </summary>
struct ColumnInfo {
	JET_COLUMNID ColumnId;
	wstring ColumnName;
	JET_COLUMNDEF ColumnDef;
};

/// <summary>
/// Struct WhatIsRecord
/// </summary>
struct WhatIsRecord {
	string Name;
	string Location;
	string Description;
	RecordState State;
	ExecutionResult Result;

	/// <summary>
	/// Determines whether this instance is empty.
	/// </summary>
	/// <returns>bool.</returns>
	bool IsEmpty() const {
		return (Name.empty() && Location.empty() && Description.empty());
	}

	/// <summary>
	/// Matcheses the specified value to check.
	/// </summary>
	/// <param name="valueToCheck">The value to check.</param>
	/// <returns>bool.</returns>
	bool Matches(const wstring& valueToCheck) const {
		auto retval = false;

		if (!valueToCheck.empty() && !Name.empty()) {
			auto strLen = wcslen(valueToCheck.c_str());
			auto tmpBuf = make_unique<char[]>(MAX_PATH * 5);
			wcstombs(tmpBuf.get(), valueToCheck.c_str(), strLen);
			retval = strcmp(tmpBuf.get(), Name.c_str()) == 0;
		}

		return retval;
	}

};

/// <summary>
/// Struct ColumnType
/// </summary>
struct ColumnType {
	wstring ColumnName;
	JET_SETCOLUMN ColumnData = {0};

	/// <summary>
	/// Initializes a new instance of the <see cref="ColumnType"/> struct.
	/// </summary>
	/// <param name="columnName">Name of the column.</param>
	/// <param name="columns">The columns.</param>
	/// <param name="data">The data.</param>
	/// <param name="sizeData">The size data.</param>
	ColumnType(const wstring& columnName, const vector<ColumnInfo>& columns, const void* data, int sizeData) {
		auto found = find_if(columns.begin(), columns.end(), [&](const ColumnInfo& selected) -> bool {
			auto retval = false;

			if (columnName == selected.ColumnName) {
				ColumnName = selected.ColumnName;

				if (data != nullptr) {
					ColumnData.pvData = data;
					ColumnData.cbData = sizeData;
				}

				retval = true;
			}
			return retval;
		});
	}
};


// Class
class EsentCore {
public:
	EsentCore();
	virtual ~EsentCore();
	vector<ColumnInfo> ColumnDefinition_get() const;
	bool CreateOrOpenDatabase(const wstring& databaseName);
	WhatIsRecord CreateOrRetrieveRecord(const vector<ColumnType>& columns);

private:
	JET_DBID m_databaseId;
	JET_TABLEID m_tableId;
	JET_SESID m_sessionId;
	JET_INSTANCE m_Instance;
	bool m_Initialized = false;
	vector<ColumnInfo> m_columns;
	string Trim(strstream& text);
	vector<ColumnInfo> GetColumnDefinition();
	bool CreateIndex(const ColumnInfo& column);
	bool InsertRecord(const vector<ColumnType>& columns);
	unordered_map<wstring, JET_COLUMNDEF> GetColumnIds();
	template <typename T> WhatIsRecord GetRecord(const T* cols);
	void PrepareFields(JET_RETRIEVECOLUMN* cols, int fieldCount);
	

protected:
	void Initialize();
	virtual bool CreateTable();
	virtual void SetParameters();
};

