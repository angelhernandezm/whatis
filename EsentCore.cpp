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

Description: Implementation of EsentCore class
*/

#include "stdafx.h"
#include "EsentCore.h"
#include "FileUtil.h"
#include <strstream>
#include <typeinfo>

#pragma warning(disable:4996 4267)

using namespace std;

/// <summary>
/// Initializes a new instance of the <see cref="EsentCore"/> class.
/// </summary>
EsentCore::EsentCore() {
	Initialize();
}

/// <summary>
/// Finalizes an instance of the <see cref="EsentCore"/> class.
/// </summary>
EsentCore::~EsentCore() {
	if (m_Initialized) {
		JetCloseDatabase(m_sessionId, m_databaseId, NULL);
		JetEndSession(m_sessionId, NULL);
		JetTerm2(m_Instance, NULL);
	}
}

/// <summary>
/// Columns the definition_get.
/// </summary>
/// <returns>std.vector&lt;_Ty, _Alloc&gt;.</returns>
vector<ColumnInfo> EsentCore::ColumnDefinition_get() const {
	return m_columns;
}

/// <summary>
/// Creates the database.
/// </summary>
/// <returns>bool.</returns>
bool EsentCore::CreateOrOpenDatabase(const wstring& databaseName) {
	wstring folderName;
	auto retval = false;
	char dbName[MAX_PATH];

	if (m_Initialized && !databaseName.empty() && wcstombs(dbName, databaseName.c_str(), MAX_PATH) > 0) {
		if (!FileUtil::FileExist(databaseName)) {
			if (!FileUtil::FolderExist(folderName = FileUtil::GetFolderNameFromPath(databaseName))) {
				if (FileUtil::CreateFolder(folderName) && SUCCEEDED(JetCreateDatabase(m_sessionId, dbName,
					NULL, &m_databaseId, JET_bitDbShadowingOff)))
					retval = CreateTable();
			}
		} else {
			retval = SUCCEEDED(JetAttachDatabase(m_sessionId, dbName, NULL)) &&
				SUCCEEDED(JetOpenDatabase(m_sessionId, dbName, nullptr, &m_databaseId, NULL));

			if (retval) {
				retval = SUCCEEDED(JetOpenTable(m_sessionId, m_databaseId, TableName, nullptr, NULL,
					JET_bitTableUpdatable | JET_bitTableSequential, &m_tableId));
			}
		}
	}

	return retval;
}


/// <summary>
/// Creates the or retrieve record.
/// </summary>
/// <param name="columns">The columns.</param>
/// <returns>WhatIsRecord.</returns>
WhatIsRecord EsentCore::CreateOrRetrieveRecord(const vector<ColumnType>& columns) {
	JET_ERR result;
	WhatIsRecord retval;
	char buffer[MaxNameColumnLength];
	retval.Result = ExecutionResult::Failure;

	if (m_Initialized && m_tableId != NULL && columns.size() > 0 && wcstombs(buffer, Name_Column, MaxNameColumnLength) > 0) {
		if (SUCCEEDED(result = JetSetCurrentIndex(m_sessionId, m_tableId, buffer))) {
			auto searchVal = find_if(columns.begin(), columns.end(), [&](ColumnType search) -> bool {
				return (search.ColumnName == Name_Column ? true : false);
			});

			if (searchVal != columns.end() && searchVal->ColumnData.pvData != nullptr) {
				auto name = reinterpret_cast<wchar_t*>(const_cast<void*>(searchVal->ColumnData.pvData));
				if (wcstombs(buffer, name, MaxNameColumnLength) > 0 &&
					SUCCEEDED(JetMakeKey(m_sessionId, m_tableId, buffer, MaxNameColumnLength, JET_bitNewKey | JET_bitPartialColumnEndLimit))) {
					for (result = JetSetIndexRange(m_sessionId, m_tableId, JET_bitRangeInclusive | JET_bitRangeUpperLimit); SUCCEEDED(result);
					result = JetMove(m_sessionId, m_tableId, JET_MoveNext, NULL)) {
						JET_RETRIEVECOLUMN fields[fieldCollectionSize];
						ZeroMemory(fields, sizeof(JET_RETRIEVECOLUMN) * fieldCollectionSize);
						PrepareFields(&fields[0], fieldCollectionSize);
						auto tmpRecord = GetRecord(&fields[0]);

						for (auto n = 0; n < fieldCollectionSize; n++)
							free(fields[n].pvData);

						// Is it the valid record?
						if (!tmpRecord.IsEmpty() && tmpRecord.Matches(name)) {
							retval.Name = tmpRecord.Name;
							retval.Location = tmpRecord.Location;
							retval.State = RecordState::Existing;
							retval.Description = tmpRecord.Description;
							retval.Result = ExecutionResult::Success;
							result = JET_errSuccess;
							break;
						}
					}

					if (result == JET_errNoCurrentRecord && InsertRecord(columns)) {
						retval = GetRecord(&columns);
						retval.State = RecordState::New;
						retval.Result = ExecutionResult::Success;
					}
				}
			}
		}
	}

	return retval;
}

/// <summary>
/// Trims the specified text.
/// </summary>
/// <param name="text">The text.</param>
/// <returns>std.string.</returns>
string EsentCore::Trim(strstream& text) {
	string retval;
	string newString(text.str());
	
	for (auto index = 0; isprint((unsigned char) newString[index]); index++) 
		retval.append(1, newString[index]);

	retval.erase(retval.find_last_not_of(' ') + 1);

	return retval;
}

/// <summary>
/// Creates the index.
/// </summary>
/// <param name="column">The column.</param>
/// <returns>bool.</returns>
bool EsentCore::CreateIndex(const ColumnInfo& column) {
	auto retval = false;
	char buffer[MAX_PATH];

	if (m_Initialized && m_tableId != NULL  && wcstombs(buffer, column.ColumnName.c_str(), MAX_PATH) > 0) {
		retval = SUCCEEDED(JetCreateIndex(m_sessionId, m_tableId, buffer, (column.ColumnName == Pk_Id_Column ? JET_bitIndexPrimary
			: JET_bitIndexDisallowNull), (column.ColumnName == Pk_Id_Column ? pkIndex : nameIndex),
			strlen((column.ColumnName == Pk_Id_Column ? pkIndex : nameIndex)) + 2, 80)) ? true : false;
	}

	return retval;
}

/// <summary>
/// Gets the column definition.
/// </summary>
/// <returns>std.vector&lt;_Ty, _Alloc&gt;.</returns>
vector<ColumnInfo> EsentCore::GetColumnDefinition() {
	vector<ColumnInfo> retval;

	retval.push_back(ColumnInfo{0, wstring(Pk_Id_Column), JET_COLUMNDEF{
		sizeof(JET_COLUMNDEF), NULL, JET_coltypLong, NULL, NULL, NULL, NULL,
		GetLengthInBytes(4), JET_bitColumnAutoincrement}});

	retval.push_back(ColumnInfo{0, wstring(Name_Column), JET_COLUMNDEF{
		sizeof(JET_COLUMNDEF), NULL, JET_coltypText, NULL, NULL, NULL, NULL,
		GetLengthInBytes(50), JET_bitColumnFixed | JET_bitColumnNotNULL}});

	retval.push_back(ColumnInfo{0, wstring(Location_Column), JET_COLUMNDEF{
		sizeof(JET_COLUMNDEF), NULL, JET_coltypText, NULL, NULL, NULL, NULL,
		GetLengthInBytes(MAX_PATH), JET_bitColumnFixed | JET_bitColumnNotNULL}});

	retval.push_back(ColumnInfo{0, wstring(Description_Column), JET_COLUMNDEF{
		sizeof(JET_COLUMNDEF), NULL, JET_coltypLongText , NULL, NULL, NULL, NULL,
		GetLengthInBytes(MAX_PATH * 5), JET_bitColumnMaybeNull}});

	return retval;
}

/// <summary>
/// Gets the record.
/// </summary>
/// <param name="cols">The cols.</param>
/// <returns>WhatIsRecord.</returns>
template <typename T>
WhatIsRecord EsentCore::GetRecord(const T* cols) {
	WhatIsRecord retval;
	vector<void*> values;
	JET_RETRIEVECOLUMN* colValues = nullptr;
	vector<ColumnType>* colDef = nullptr;
	strstream name, location, description;
	auto colType = typeid(T) == typeid(JET_RETRIEVECOLUMN);

	if (colType)
		colValues = (JET_RETRIEVECOLUMN*)cols;
	else {
		colDef = (vector<ColumnType>*) cols;

		for (auto index = 0; index < 3; index++) {
			auto colData = colDef->at(index + 1).ColumnData;
			auto ptrData = make_unique<char[]>(MAX_PATH * 5);
			auto wstr = reinterpret_cast<wchar_t*>(const_cast<void*>(colData.pvData));
			auto size = wcslen(wstr);
			wcstombs(ptrData.get(), wstr, size);
			values.push_back(malloc(colData.cbData));
			ZeroMemory(values.at(index), colData.cbData);
			memcpy(values.at(index), ptrData.get(), size);
		}
	}

	name << reinterpret_cast<char*>(const_cast<void*>(colType ? colValues[1].pvData : values.at(0))) << endl;
	retval.Name = Trim(name);
	location << reinterpret_cast<char*>(const_cast<void*>(colType ? colValues[2].pvData : values.at(1))) << endl;
	retval.Location = Trim(location);
	description << reinterpret_cast<char*>(const_cast<void*>(colType ? colValues[3].pvData : values.at(2))) << endl;
	retval.Description = Trim(description);

	for_each(values.begin(), values.end(), [&](void* block) {free(block); });

	return retval;
}

/// <summary>
/// Gets the column ids.
/// </summary>
/// <returns>std.unordered_map&lt;_Kty, _Ty, _Hasher, _Keyeq, _Alloc&gt;.</returns>
unordered_map<wstring, JET_COLUMNDEF> EsentCore::GetColumnIds() {
	unordered_map<wstring, JET_COLUMNDEF> retval;

	for_each(m_columns.begin(), m_columns.end(), [&](const ColumnInfo& column) {
		char colName[MAX_PATH] = {0};
		auto buffer = make_unique<JET_COLUMNDEF>();

		if (wcstombs(colName, column.ColumnName.c_str(), column.ColumnName.size()) > 0 &&
			SUCCEEDED(JetGetTableColumnInfo(m_sessionId, m_tableId, colName, buffer.get(), sizeof(JET_COLUMNDEF), JET_ColInfo))) {
			retval.insert_or_assign(column.ColumnName, JET_COLUMNDEF(*buffer.get()));
		}
	});


	return retval;
}

/// <summary>
/// Inserts the record.
/// </summary>
/// <param name="columns">The columns.</param>
/// <returns>bool.</returns>
bool EsentCore::InsertRecord(const vector<ColumnType>& columns) {
	auto colIndex = 0;
	auto retval = false;
	auto columnInfo = GetColumnIds();
	auto newColumns = make_unique<JET_SETCOLUMN[]>(modifiableColumnCount);

	if (columns.size() > 0) {
		if (SUCCEEDED(JetBeginTransaction(m_sessionId)) && SUCCEEDED(JetPrepareUpdate(m_sessionId, m_tableId, JET_prepInsert))) {
			ZeroMemory(newColumns.get(), sizeof(JET_SETCOLUMN) * modifiableColumnCount);
			for_each(columns.begin(), columns.end(), [&](const ColumnType& column) {
				if (column.ColumnName != Pk_Id_Column && column.ColumnName != CreatedOn_Column) {
					auto colInfo = columnInfo.at(column.ColumnName);
					newColumns[colIndex] = JET_SETCOLUMN{0};
					newColumns[colIndex].columnid = colInfo.columnid;
					auto ptrData = make_unique<char[]>(MAX_PATH * 5);

					if (colInfo.coltyp == JET_coltypText || colInfo.coltyp == JET_coltypLongText) {
						auto wstr = reinterpret_cast<wchar_t*>(const_cast<void*>(column.ColumnData.pvData));
						auto size = wcslen(wstr);
						wcstombs(ptrData.get(), wstr, size);
						newColumns[colIndex].pvData = malloc(size);
						memcpy(const_cast<void*>(newColumns[colIndex].pvData), ptrData.get(), size);
						newColumns[colIndex].cbData = size;
					}

					newColumns[colIndex].err = JET_errSuccess;
					colIndex++;
				}
			});

			retval = SUCCEEDED(JetSetColumns(m_sessionId, m_tableId, newColumns.get(), modifiableColumnCount)) &&
				SUCCEEDED(JetUpdate(m_sessionId, m_tableId, nullptr, NULL, NULL));

			// Free memory
			for (auto nIndex = 0; nIndex < colIndex; nIndex++)
				free(const_cast<void*>(newColumns[nIndex].pvData));

			// Commit or rollback transaction
			if (retval)
				JetCommitTransaction(m_sessionId, NULL);
			else  JetRollback(m_sessionId, NULL);
		}
	}

	return retval;
}

/// <summary>
/// Prepares the fields.
/// </summary>
/// <param name="cols">The cols.</param>
/// <param name="fieldCount">The field count.</param>
void EsentCore::PrepareFields(JET_RETRIEVECOLUMN * cols, int fieldCount) {
	auto index = 0;
	auto colInfo = GetColumnIds();

	if (colInfo.size() == fieldCount) {
		for_each(colInfo.begin(), colInfo.end(), [&](const pair<wstring, JET_COLUMNDEF>& column) {
			unsigned long bytesRead = 0;
			cols[index].columnid = column.second.columnid;
			cols[index].pvData = malloc(column.second.cbMax);
			JetRetrieveColumn(m_sessionId, m_tableId, column.second.columnid, cols[index].pvData, column.second.cbMax, &bytesRead, NULL, nullptr);
			cols[index].cbData = bytesRead;
			index++;
		});
	}
}

/// <summary>
/// Initializes this instance.
/// </summary>
void EsentCore::Initialize() {
	if (SUCCEEDED(JetCreateInstance2(&m_Instance, InstanceName, InstanceDisplayName, NULL)) &&
		SUCCEEDED(JetInit3(&m_Instance, nullptr, NULL)) &&
		SUCCEEDED(JetBeginSession(m_Instance, &m_sessionId, NULL, NULL))) {
		SetParameters();
		m_columns = GetColumnDefinition();
		m_Initialized = true;
	}

}

/// <summary>
/// Creates the table.
/// </summary>
/// <returns>bool.</returns>
bool EsentCore::CreateTable() {
	auto retval = false;

	if (m_Initialized && m_databaseId != NULL) {
		char buffer[MAX_PATH];

		if (SUCCEEDED(JetCreateTable(m_sessionId, m_databaseId, TableName, PageInitialCount, NULL, &m_tableId))) {
			retval = all_of(m_columns.begin(), m_columns.end(), [&](ColumnInfo& col) {
				if (wcstombs(buffer, col.ColumnName.c_str(), MAX_PATH) > 0 &&
					SUCCEEDED(JetAddColumn(m_sessionId, m_tableId, buffer, &col.ColumnDef, nullptr, NULL, &col.ColumnId))) {
					return (col.ColumnName == Pk_Id_Column || col.ColumnName == Name_Column ? CreateIndex(col) : true);
				} else return false;
			});
		}
	}

	return retval;
}

/// <summary>
/// Sets the parameters.
/// </summary>
void EsentCore::SetParameters() {
	//TODO: JET Engine parameters to be set here
}