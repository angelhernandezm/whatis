// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <Shlobj.h>
#include <Objbase.h>
#include <memory>
#include <string>
#include <Shlwapi.h>
#include <esent.h>
#include <atlbase.h>
#include <stdlib.h>

// TODO: reference additional headers your program requires here

#define pkIndex "+Pk_Id\0"
#define nameIndex "+Name\0"
#define Name_Column L"Name"
#define Pk_Id_Column L"Pk_Id"
#define PageInitialCount 512
#define fieldCollectionSize 4
#define MaxNameColumnLength 50
#define modifiableColumnCount 3
#define MaxDescriptionLength 512
#define HomeFolder  L"\\whatis\\"
#define DatabaseName L"whatis.db"
#define Location_Column L"Location"
#define TableName "WhatIsCatalogue"
#define CreatedOn_Column L"Created_On"
#define Description_Column L"Description"
#define InstanceDisplayName "WhatIsDbInstance"
#define NoInputFileWasSpecified L"No input file was specified."
#define InstanceName "WhatIsDbInstance-D9B709F1-55C7-4A9D-8979-1B3AB1626702"
#define FileNotFoundErrorMsg L"File not found (try specifying a search path)."
#define DefaultWindowsFileDescriptionKey L"\\StringFileInfo\\040904b0\\FileDescription"
#define InvalidNumberArgs L"Usage: Whatis <targetfile> - <targetfile> with or without full path."
#define UnableToRetrieveFileDescription L"Unable to retrieve file description from specified file."
#define UnableToOpenOrCreateDatabase L"Unable to open or create database - Contact your system administrator."

#define GetLengthInBytes(factor) (sizeof(int) * factor)
#define Array_Size(array) (sizeof(array) / sizeof(array[0]))