#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include "Config.h"
#include "Debug.h"
#include "FileMan.h"
#include "LibraryDataBase.h"
#include "MemMan.h"
#include "WCheck.h"

#ifdef _WIN32
#	include <direct.h>
#	include <shlobj.h>
#	include <winerror.h>

#	define mkdir(path, mode) _mkdir(path)

#else
#	include <glob.h>
#	include <pwd.h>
#	include <sys/stat.h>
#	include <sys/types.h>
#	include "Stubs.h" // XXX

#	if defined __APPLE__ && defined __MACH__
#		include <CoreFoundation/CoreFoundation.h>
#		include <sys/param.h>
#	endif
#endif


//The FileDatabaseHeader
DatabaseManagerHeaderStruct gFileDataBase;


#ifdef _WIN32

static HANDLE hFindInfoHandle[20];

#else

struct Glob
{
	glob_t Glob;
	UINT32 Index;
};
typedef struct Glob Glob;

static Glob Win32FindInfo[20];

#endif


static BOOLEAN fFindInfoInUse[20];


static char LocalPath[512];
static config_entry* BinDataDir;

static void TellAboutDataDir(const char* ConfigFile)
{
	FILE* const IniFile = fopen(ConfigFile, "a");
	if (IniFile != NULL)
	{
		fprintf(IniFile, "#Tells ja2-stracciatella where the binary datafiles are located\ndata_dir = /some/place/where/the/data/is");
		fclose(IniFile);
		fprintf(stderr, "Please edit \"%s\" to point to the binary data.\n", ConfigFile);
	}
}


#if defined __APPLE__  && defined __MACH__

void SetBinDataDirFromBundle(void)
{
	CFBundleRef const app_bundle = CFBundleGetMainBundle();
	if (app_bundle == NULL)
	{
		fputs("WARNING: Failed to get main bundle.\n", stderr);
		return;
	}

	CFURLRef const app_url = CFBundleCopyBundleURL(app_bundle);
	if (app_url == NULL)
	{
		fputs("WARNING: Failed to get URL of bundle.\n", stderr);
		return;
	}

#define RESOURCE_PATH "/Contents/Resources/ja2"
	char app_path[PATH_MAX + lengthof(RESOURCE_PATH)];
	if (!CFURLGetFileSystemRepresentation(app_url, TRUE, (UInt8*)app_path, PATH_MAX))
	{
		fputs("WARNING: Failed to get application path.\n", stderr);
		return;
	}

	strcat(app_path, RESOURCE_PATH);
	ConfigSetValue(BinDataDir, app_path);
#undef RESOURCE_PATH
}

#endif


BOOLEAN InitializeFileManager(void)
{
#ifdef _WIN32
	_fmode = O_BINARY;

	for (UINT i = 0; i < lengthof(hFindInfoHandle); ++i)
	{
		hFindInfoHandle[i] = INVALID_HANDLE_VALUE;
	}

	char home[MAX_PATH];
	if (FAILED(SHGetFolderPath(NULL, CSIDL_PERSONAL | CSIDL_FLAG_CREATE, NULL, 0, home)))
	{
		fprintf(stderr, "Unable to locate home directory\n");
		return FALSE;
	}

#	define LOCALDIR "JA2"

#else
	const char* home = getenv("HOME");
	if (home == NULL)
	{
		const struct passwd* const passwd = getpwuid(getuid());
		if (passwd == NULL || passwd->pw_dir == NULL)
		{
			fprintf(stderr, "Unable to locate home directory\n");
			return FALSE;
		}

		home = passwd->pw_dir;
	}

#	define LOCALDIR ".ja2"

#endif

	snprintf(LocalPath, lengthof(LocalPath), "%s/" LOCALDIR, home);
	if (mkdir(LocalPath, 0700) != 0 && errno != EEXIST)
	{
		fprintf(stderr, "Unable to create directory \"%s\"\n", LocalPath);
		return FALSE;
	}

	char DataPath[512];
	snprintf(DataPath, lengthof(DataPath), "%s/Data", LocalPath);
	if (mkdir(DataPath, 0700) != 0 && errno != EEXIST)
	{
		fprintf(stderr, "Unable to create directory \"%s\"\n", DataPath);
		return FALSE;
	}
	BinDataDir = ConfigRegisterKey("data_dir");

#if defined __APPLE__  && defined __MACH__
	SetBinDataDirFromBundle();
#endif

	char ConfigFile[512];
	snprintf(ConfigFile, lengthof(ConfigFile), "%s/ja2.ini", LocalPath);
	if (ConfigParseFile(ConfigFile))
	{
		fprintf(stderr, "WARNING: Could not open configuration file (\"%s\").\n", ConfigFile);
	}

	if (GetBinDataPath() == NULL)
	{
		fputs("ERROR: Path to binary data is not set.\n", stderr);
		TellAboutDataDir(ConfigFile);
		return FALSE;
	}
	return TRUE;
}


BOOLEAN FileExists(const char* const filename)
{
	BOOLEAN fExists = FileExistsNoDB(filename);
	if (!fExists)
	{
		fExists = CheckIfFileExistInLibrary(filename);
	}
	return fExists;
}


BOOLEAN FileExistsNoDB(const char* const filename)
{
	FILE* file = fopen(filename, "rb");
	if (file == NULL)
	{
		char Path[512];
		snprintf(Path, lengthof(Path), "%s/Data/%s", GetBinDataPath(), filename);
		file = fopen(Path, "rb");
		if (file == NULL) return FALSE;
	}

	fclose(file);
	return TRUE;
}


BOOLEAN FileDelete(const char* const path)
{
	return unlink(path) == 0 || errno == ENOENT;
}


HWFILE FileOpen(const char* const filename, const UINT32 flags)
{
	const char* fmode;
	int         mode;
	switch (flags & (FILE_ACCESS_READWRITE | FILE_ACCESS_APPEND))
	{
		case FILE_ACCESS_READ:      fmode = "rb";  mode = O_RDONLY;            break;
		case FILE_ACCESS_WRITE:     fmode = "wb";  mode = O_WRONLY;            break;
		case FILE_ACCESS_READWRITE: fmode = "r+b"; mode = O_RDWR;              break;
		case FILE_ACCESS_APPEND:    fmode = "ab";  mode = O_WRONLY | O_APPEND; break;

		default: abort();
	}

	int d;
	if (flags & FILE_CREATE_ALWAYS)
	{
		d = open(filename, mode | O_CREAT | O_TRUNC, 0600);
		if (d < 0) return 0;
	}
	else if (flags & (FILE_ACCESS_WRITE | FILE_ACCESS_APPEND))
	{
		if (flags & FILE_OPEN_ALWAYS) mode |= O_CREAT;
		d = open(filename, mode, 0600);
		if (d < 0) return 0;
	}
	else
	{
		d = open(filename, mode);
		if (d < 0)
		{
			char path[512];
			snprintf(path, lengthof(path), "%s/Data/%s", GetBinDataPath(), filename);
			d = open(path, mode);
			if (d < 0)
			{
				const HWFILE h = OpenFileFromLibrary(filename);
				if (h != 0 || !(flags & FILE_OPEN_ALWAYS)) return h;

				d = open(filename, mode | O_CREAT, 0600);
				if (d < 0) return 0;
			}
		}
	}

	FILE* const f = fdopen(d, fmode);
	if (f == NULL)
	{
		close(d);
		return 0;
	}
	return CreateRealFileHandle(f);
}


void FileClose(const HWFILE hFile)
{
	CloseLibraryFile(hFile);
}


#ifdef JA2TESTVERSION
#	include "Timer_Control.h"
extern UINT32 uiTotalFileReadTime;
extern UINT32 uiTotalFileReadCalls;
#endif

BOOLEAN FileRead(const HWFILE hFile, void* const pDest, const UINT32 uiBytesToRead)
{
#ifdef JA2TESTVERSION
	const UINT32 uiStartTime = GetJA2Clock();
#endif
	const BOOLEAN ret = LoadDataFromLibrary(hFile, pDest, uiBytesToRead);
#ifdef JA2TESTVERSION
	//Add the time that we spent in this function to the total.
	uiTotalFileReadTime += GetJA2Clock() - uiStartTime;
	uiTotalFileReadCalls++;
#endif
	return ret;
}


BOOLEAN FileWrite(HWFILE hFile, const void* pDest, UINT32 uiBytesToWrite)
{
	INT16 sLibraryID;
	UINT32 uiFileNum;
	GetLibraryAndFileIDFromLibraryFileHandle(hFile, &sLibraryID, &uiFileNum);

	// we cannot write to a library file
	if (sLibraryID != REAL_FILE_LIBRARY_ID) return FALSE;

	FILE* const hRealFile = gFileDataBase.RealFiles.pRealFilesOpen[uiFileNum];
	return fwrite(pDest, uiBytesToWrite, 1, hRealFile) == 1;
}


BOOLEAN FileSeek(const HWFILE hFile, INT32 distance, const INT how)
{
	return LibraryFileSeek(hFile, distance, how);
}


INT32 FileGetPos(const HWFILE hFile)
{
	INT16 sLibraryID;
	UINT32 uiFileNum;
	GetLibraryAndFileIDFromLibraryFileHandle(hFile, &sLibraryID, &uiFileNum);

	if (sLibraryID == REAL_FILE_LIBRARY_ID)
	{
		FILE* const hRealFile = gFileDataBase.RealFiles.pRealFilesOpen[uiFileNum];
		return ftell(hRealFile);
	}
	else
	{
		//if the library is open
		if (IsLibraryOpened(sLibraryID))
		{
			const FileOpenStruct* const fo = &gFileDataBase.pLibraries[sLibraryID].pOpenFiles[uiFileNum];
			if (fo->pFileHeader != NULL) // if the file is open
			{
				const UINT32 uiPositionInFile = fo->uiFilePosInFile;
				return uiPositionInFile;
			}
		}
	}

	return BAD_INDEX;
}


UINT32 FileGetSize(const HWFILE hFile)
{
	INT16 sLibraryID;
	UINT32 uiFileNum;
	GetLibraryAndFileIDFromLibraryFileHandle(hFile, &sLibraryID, &uiFileNum);

	UINT32 uiFileSize = 0xFFFFFFFF;
	if (sLibraryID == REAL_FILE_LIBRARY_ID)
	{
		FILE* const hRealHandle = gFileDataBase.RealFiles.pRealFilesOpen[uiFileNum];
		const long here = ftell(hRealHandle);
		fseek(hRealHandle, 0, SEEK_END);
		uiFileSize = ftell(hRealHandle);
		fseek(hRealHandle, here, SEEK_SET);
	}
	else
	{
		if (IsLibraryOpened(sLibraryID))
		{
			uiFileSize = gFileDataBase.pLibraries[sLibraryID].pOpenFiles[uiFileNum].pFileHeader->uiFileLength;
		}
	}

	return uiFileSize == 0xFFFFFFFF ? 0 : uiFileSize;
}


BOOLEAN SetFileManCurrentDirectory(const char* const pcDirectory)
{
#if 1 // XXX TODO
	return chdir(pcDirectory) == 0;
#else
	return SetCurrentDirectory(pcDirectory);
#endif
}


static BOOLEAN GetFileManCurrentDirectory(STRING512 pcDirectory)
{
#if 1 // XXX TODO
	return getcwd(pcDirectory, sizeof(STRING512)) != NULL;
#else
	return GetCurrentDirectory(512, pcDirectory) != 0;
#endif
}


BOOLEAN MakeFileManDirectory(const char* const pcDirectory)
{
	return mkdir(pcDirectory, 0755) == 0;
}


// Removes ALL FILES in the specified directory but leaves the directory alone.  Does not affect any subdirectories!
BOOLEAN EraseDirectory(const char *pcDirectory)
{
#if 1 // XXX TODO
	UNIMPLEMENTED
#else
	WIN32_FIND_DATA sFindData;
	HANDLE		SearchHandle;
	const CHAR8	*pFileSpec = "*.*";
	BOOLEAN	fDone = FALSE;
	CHAR8		zOldDir[512];

	GetFileManCurrentDirectory( zOldDir );

	if( !SetFileManCurrentDirectory( pcDirectory ) )
	{
		FastDebugMsg(String("EraseDirectory: ERROR - SetFileManCurrentDirectory on %s failed, error %d", pcDirectory, GetLastError()));
		return( FALSE );		//Error going into directory
	}

	//If there are files in the directory, DELETE THEM
	SearchHandle = FindFirstFile( pFileSpec, &sFindData);
	if( SearchHandle !=  INVALID_HANDLE_VALUE )
	{

		fDone = FALSE;
		do
		{
			// if it's a file, not a directory
			if( GetFileAttributes( sFindData.cFileName ) != FILE_ATTRIBUTES_DIRECTORY )
			{
				FileDelete( sFindData.cFileName );
			}

			//find the next file in the directory
			if ( !FindNextFile( SearchHandle, &sFindData ))
			{
				fDone = TRUE;
			}
		} while(!fDone);

		// very important: close the find handle, or subsequent RemoveDirectory() calls will fail
		FindClose( SearchHandle );
	}

	if( !SetFileManCurrentDirectory( zOldDir ) )
	{
		FastDebugMsg(String("EraseDirectory: ERROR - SetFileManCurrentDirectory on %s failed, error %d", zOldDir, GetLastError()));
		return( FALSE );		//Error returning from directory
	}

	return( TRUE );
#endif
}


const char* GetExecutableDirectory(void)
{
	return LocalPath;
}


#ifdef _WIN32
static void W32toSGPFileFind(GETFILESTRUCT* pGFStruct, WIN32_FIND_DATA* pW32Struct);
#else
static void W32toSGPFileFind(GETFILESTRUCT* pGFStruct, Glob* pW32Struct);
#endif


BOOLEAN GetFileFirst(const char* const pSpec, GETFILESTRUCT* const pGFStruct)
{
	CHECKF(pSpec     != NULL);
	CHECKF(pGFStruct != NULL);

	INT32   iWhich = 0;
	BOOLEAN fFound = FALSE;
	for (INT32 x = 0; x < 20; ++x)
	{
		if (!fFindInfoInUse[x])
		{
			iWhich = x;
			fFound = TRUE;
			break;
		}
	}

	if (!fFound) return FALSE;

	pGFStruct->iFindHandle = iWhich;

#ifdef _WIN32
	WIN32_FIND_DATA Win32FindInfo;
	hFindInfoHandle[iWhich] = FindFirstFile(pSpec, &Win32FindInfo);
	if (hFindInfoHandle[iWhich] == INVALID_HANDLE_VALUE) return FALSE;
	W32toSGPFileFind(pGFStruct, &Win32FindInfo);
#else
	if (glob(pSpec, 0, NULL, &Win32FindInfo[iWhich].Glob) != 0)
	{
		globfree(&Win32FindInfo[iWhich].Glob);
		return FALSE;
	}
	Win32FindInfo[iWhich].Index = 0;
	W32toSGPFileFind(pGFStruct, &Win32FindInfo[iWhich]);
#endif

	fFindInfoInUse[iWhich] = TRUE;
	return TRUE;
}


BOOLEAN GetFileNext(GETFILESTRUCT* const pGFStruct)
{
	CHECKF(pGFStruct != NULL);

#ifdef _WIN32
	WIN32_FIND_DATA Win32FindInfo;
	if (!FindNextFile(hFindInfoHandle[pGFStruct->iFindHandle], &Win32FindInfo))
	{
		return FALSE;
	}
	W32toSGPFileFind(pGFStruct, &Win32FindInfo);
#else
	Glob* const g = &Win32FindInfo[pGFStruct->iFindHandle];
	if (g->Index >= g->Glob.gl_pathc) return FALSE;
	W32toSGPFileFind(pGFStruct, g);
#endif
	return TRUE;
}


void GetFileClose(GETFILESTRUCT* const pGFStruct)
{
	if (pGFStruct == NULL) return;

#ifdef _WIN32
	FindClose(hFindInfoHandle[pGFStruct->iFindHandle]);
	hFindInfoHandle[pGFStruct->iFindHandle] = INVALID_HANDLE_VALUE;
#else
	globfree(&Win32FindInfo[pGFStruct->iFindHandle].Glob);
#endif
	fFindInfoInUse[pGFStruct->iFindHandle] = FALSE;
}


#ifdef _WIN32
static void W32toSGPFileFind(GETFILESTRUCT* pGFStruct, WIN32_FIND_DATA* pW32Struct)
#else
static void W32toSGPFileFind(GETFILESTRUCT* pGFStruct, Glob* pW32Struct)
#endif
{
	UINT32 uiAttribMask;

	// Copy the filename
#ifdef _WIN32
	strcpy(pGFStruct->zFileName, pW32Struct->cFileName);
#else
	const char* start = strrchr(pW32Struct->Glob.gl_pathv[pW32Struct->Index], '/');
	start = (start != NULL ? start + 1 : pW32Struct->Glob.gl_pathv[pW32Struct->Index]);
	strcpy(pGFStruct->zFileName, start);
#endif

	// Copy the file attributes
#ifdef _WIN32
	pGFStruct->uiFileAttribs = 0;

	for (uiAttribMask = 0x80000000; uiAttribMask > 0; uiAttribMask >>= 1)
	{
		switch (pW32Struct->dwFileAttributes & uiAttribMask)
		{
			case FILE_ATTRIBUTE_ARCHIVE:    pGFStruct->uiFileAttribs |= FILE_IS_ARCHIVE;    break;
			case FILE_ATTRIBUTE_DIRECTORY:  pGFStruct->uiFileAttribs |= FILE_IS_DIRECTORY;  break;
			case FILE_ATTRIBUTE_HIDDEN:     pGFStruct->uiFileAttribs |= FILE_IS_HIDDEN;     break;
			case FILE_ATTRIBUTE_NORMAL:     pGFStruct->uiFileAttribs |= FILE_IS_NORMAL;     break;
			case FILE_ATTRIBUTE_READONLY:   pGFStruct->uiFileAttribs |= FILE_IS_READONLY;   break;
			case FILE_ATTRIBUTE_SYSTEM:     pGFStruct->uiFileAttribs |= FILE_IS_SYSTEM;     break;
			case FILE_ATTRIBUTE_TEMPORARY:  pGFStruct->uiFileAttribs |= FILE_IS_TEMPORARY;  break;
			case FILE_ATTRIBUTE_COMPRESSED: pGFStruct->uiFileAttribs |= FILE_IS_COMPRESSED; break;
			case FILE_ATTRIBUTE_OFFLINE:    pGFStruct->uiFileAttribs |= FILE_IS_OFFLINE;    break;
		}
	}
#else
	pGFStruct->uiFileAttribs = FILE_IS_NORMAL; // XXX TODO
	++pW32Struct->Index;
#endif
}


UINT32 FileGetAttributes(const char* const strFilename)
{
#ifndef _WIN32 // XXX TODO
	FIXME
	struct stat sb;
	if (stat(strFilename, &sb) != 0) return 0xFFFFFFFF;

	UINT32 uiFileAttrib = 0;
	if (S_ISDIR(sb.st_mode)) uiFileAttrib |= FILE_ATTRIBUTES_DIRECTORY;
	return uiFileAttrib;
#else
	const UINT32 uiAttribs = GetFileAttributes(strFilename);

	if (uiAttribs == 0xFFFFFFFF) return uiAttribs;

	UINT32 uiFileAttrib = 0;
	if (uiAttribs & FILE_ATTRIBUTE_ARCHIVE)   uiFileAttrib |= FILE_ATTRIBUTES_ARCHIVE;
	if (uiAttribs & FILE_ATTRIBUTE_HIDDEN)    uiFileAttrib |= FILE_ATTRIBUTES_HIDDEN;
	if (uiAttribs & FILE_ATTRIBUTE_NORMAL)    uiFileAttrib |= FILE_ATTRIBUTES_NORMAL;
	if (uiAttribs & FILE_ATTRIBUTE_OFFLINE)   uiFileAttrib |= FILE_ATTRIBUTES_OFFLINE;
	if (uiAttribs & FILE_ATTRIBUTE_READONLY)  uiFileAttrib |= FILE_ATTRIBUTES_READONLY;
	if (uiAttribs & FILE_ATTRIBUTE_SYSTEM)    uiFileAttrib |= FILE_ATTRIBUTES_SYSTEM;
	if (uiAttribs & FILE_ATTRIBUTE_TEMPORARY) uiFileAttrib |= FILE_ATTRIBUTES_TEMPORARY;
	if (uiAttribs & FILE_ATTRIBUTE_DIRECTORY) uiFileAttrib |= FILE_ATTRIBUTES_DIRECTORY;
	return uiFileAttrib;
#endif
}


BOOLEAN FileClearAttributes(const char* const filename)
{
#if 1 // XXX TODO
#	if defined WITH_FIXMES
	fprintf(stderr, "===> %s:%d: IGNORING %s(\"%s\")\n", __FILE__, __LINE__, __func__, filename);
#	endif
	return FALSE;
	UNIMPLEMENTED
#else
	return SetFileAttributes(filename, FILE_ATTRIBUTE_NORMAL);
#endif
}


BOOLEAN GetFileManFileTime(const HWFILE hFile, SGP_FILETIME* const pCreationTime, SGP_FILETIME* const pLastAccessedTime, SGP_FILETIME* const pLastWriteTime)
{
#if 1 // XXX TODO
	UNIMPLEMENTED
#else
	//Initialize the passed in variables
	memset(pCreationTime,     0, sizeof(*pCreationTime));
	memset(pLastAccessedTime, 0, sizeof(*pLastAccessedTime));
	memset(pLastWriteTime,    0, sizeof(*pLastWriteTime));

	INT16  sLibraryID;
	UINT32 uiFileNum;
	GetLibraryAndFileIDFromLibraryFileHandle(hFile, &sLibraryID, &uiFileNum);

	if (sLibraryID == REAL_FILE_LIBRARY_ID)
	{
		const HANDLE hRealFile = gFileDataBase.RealFiles.pRealFilesOpen[uiFileNum].hRealFileHandle;

		//Gets the UTC file time for the 'real' file
		SGP_FILETIME sCreationUtcFileTime;
		SGP_FILETIME sLastAccessedUtcFileTime;
		SGP_FILETIME sLastWriteUtcFileTime;
		GetFileTime(hRealFile, &sCreationUtcFileTime, &sLastAccessedUtcFileTime, &sLastWriteUtcFileTime);

		//converts the creation UTC file time to the current time used for the file
		FileTimeToLocalFileTime(&sCreationUtcFileTime, pCreationTime);

		//converts the accessed UTC file time to the current time used for the file
		FileTimeToLocalFileTime(&sLastAccessedUtcFileTime, pLastAccessedTime);

		//converts the write UTC file time to the current time used for the file
		FileTimeToLocalFileTime(&sLastWriteUtcFileTime, pLastWriteTime);
		return TRUE;
	}
	else
	{
		return GetLibraryFileTime(sLibraryID, uiFileNum, pLastWriteTime);
	}
#endif
}


INT32	CompareSGPFileTimes(const SGP_FILETIME* const pFirstFileTime, const SGP_FILETIME* const pSecondFileTime)
{
#if 1 // XXX TODO
	UNIMPLEMENTED
#else
	return CompareFileTime(pFirstFileTime, pSecondFileTime);
#endif
}


FILE* GetRealFileHandleFromFileManFileHandle(const HWFILE hFile)
{
	INT16  sLibraryID;
	UINT32 uiFileNum;
	GetLibraryAndFileIDFromLibraryFileHandle(hFile, &sLibraryID, &uiFileNum);

	if (sLibraryID == REAL_FILE_LIBRARY_ID)
	{
		return gFileDataBase.RealFiles.pRealFilesOpen[uiFileNum];
	}
	else
	{
		const LibraryHeaderStruct* const lh = &gFileDataBase.pLibraries[sLibraryID];
		if (lh->pOpenFiles[uiFileNum].pFileHeader != NULL) return lh->hLibraryHandle;
	}
	return NULL;
}


static UINT32 GetFreeSpaceOnHardDrive(const char* pzDriveLetter);


UINT32 GetFreeSpaceOnHardDriveWhereGameIsRunningFrom(void)
{
#if 1 // XXX TODO
	FIXME
	return 1024 * 1024 * 1024; // XXX TODO return an arbitrary number for now
#else
	//get the drive letter from the exec dir
  STRING512 zDrive;
	_splitpath(GetExecutableDirectory(), zDrive, NULL, NULL, NULL);

	sprintf(zDrive, "%s\\", zDrive);
	return GetFreeSpaceOnHardDrive(zDrive);
#endif
}


static UINT32 GetFreeSpaceOnHardDrive(const char* const pzDriveLetter)
{
#if 1 // XXX TODO
	UNIMPLEMENTED
#else
	UINT32 uiSectorsPerCluster     = 0;
	UINT32 uiBytesPerSector        = 0;
	UINT32 uiNumberOfFreeClusters  = 0;
	UINT32 uiTotalNumberOfClusters = 0;
	if (!GetDiskFreeSpace(pzDriveLetter, &uiSectorsPerCluster, &uiBytesPerSector, &uiNumberOfFreeClusters, &uiTotalNumberOfClusters))
	{
		const UINT32 uiLastError = GetLastError();
		char zString[1024];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, uiLastError, 0, zString, 1024, NULL);
		return TRUE;
	}

	return uiBytesPerSector * uiNumberOfFreeClusters * uiSectorsPerCluster;
#endif
}


const char* GetBinDataPath(void)
{
	return ConfigGetValue(BinDataDir);
}
