#include <windows.h>
#include "locale.h"
#include "string"
#include <iostream>
#include <sstream>
#pragma warning(disable : 4996)
using namespace std;

int MyGetLogicalDrivers();
int DriveName(char* Name);
int Menu(void);
int MyGetDriveType(char* Name);
int MyGetDiskFreeSpace(char* Name);
void MyGetVolumeInformation(char* Name);
void MyCreateFile();
int MyCopyFile();
int MyMoveFile();
bool FileExists(LPCSTR fname);
int MyFileInfo();
void changeFileAttributes();
void changeCreationTime();




int main()
{
	setlocale(LC_ALL, "rus");
	int choice;
	char drive[4] = { ' ',':','\\','\0' };
	string directory;
	int ch;


	do
	{
		choice = Menu();
		system("cls");
		switch (choice)
		{
		case 1:
			MyGetLogicalDrivers();
			break;
		case 2:
			DriveName(drive);
			MyGetDriveType(drive);
			MyGetVolumeInformation(drive);
			MyGetDiskFreeSpace(drive);

			break;
		case 3:
			int ch1;

			cout << "Введите путь хранения директории" << endl;
			cin >> directory;

			cout << "1. Создать" << endl << "2. Удалить" << endl;
			cin >> ch1;
			if (ch1 == 1) {
				if (CreateDirectoryA(directory.c_str(), NULL))
					cout << "directory create" << endl;
				else
					cout << "error create directory" << endl;
			}
			if (ch1 == 2) {
				if (RemoveDirectoryA(directory.c_str()))
					cout << "directory remove" << endl;
				else
					cout << "error remove directory" << endl;

			}
			system("pause");
			break;
		case 4:
			cout << "1. Создание файла" << endl;
			cout << "2. Скопировать файл" << endl;
			cout << "3. Переместить файл" << endl;
			cout << "4. Анализ атрибутов файла" << endl;
			cout << "5. Изменение атрибутов файла" << endl;
			cout << "6. Установка времени файла" << endl;
			cout << "7. Выход в меню" << endl;
			cin >> ch;
			system("cls");
			switch (ch) {
			case 1:
				MyCreateFile();
				break;
			case 2:
				MyCopyFile();
				break;
			case 3:
				MyMoveFile();
				break;
			case 4:
				MyFileInfo();
				break;
			case 5:
				changeFileAttributes();
				break;
			case 6:
				changeCreationTime();
				break;
			}
			break;
		}
	} while (choice != 7);
	return 1;


}

int DriveName(char* Name)
{
	cout << "Введите название диска: ";
	cin >> Name[0];
	cout << endl;
	while (Name[0] < 'A' || Name[0]>'Z')
	{
		cout <<"Неправильное название диска.Введите символ(A - Z) : ";
		cin >> Name[0];
		Name[0] = char(Name[0]);
		cout << endl;
	}
	return 1;
}

int MyGetLogicalDrivers() {

	// Инифиализируемм массив дисков
	char myDrives[] = "A";

	//получаем битовую маску  
	DWORD myDrivesBitMask = GetLogicalDrives();


	// Verifying the returned drive mask  
	if (myDrivesBitMask == 0)
		cout << "GetLogicalDrives() failed" << endl;
	else {
		cout << "This machine has the following logical drives:" << endl;
		while (myDrivesBitMask) {

			if (myDrivesBitMask & 1) {
				cout << "drive " << string(myDrives) << endl;
			}

			myDrives[0]++;
			myDrivesBitMask >>= 1;
		}
	}
	system("pause");
	return 0;
}

int MyGetDriveType(char* Name)
{
	UINT Type;

	Type = GetDriveTypeA(Name);

	switch (Type)
	{
	case 0: cout << "Тип не определен" << endl;
		break;
	case 1: cout << "Корневой директории не существует" << endl;
		break;
	case DRIVE_REMOVABLE: cout << "Накопитель может удаляться" << endl;
		break;
	case DRIVE_FIXED: cout << "Фиксированный диск, не может быть удален" << endl;
		break;
	case DRIVE_REMOTE: cout << "Удаленный накопитель (сетевой диск)" << endl;
		break;
	case DRIVE_CDROM: cout << "CD-ROM" << endl;
		break;
	case DRIVE_RAMDISK: cout << "Накопитель явлется виртуальным RAM DISK'ом" << endl;
		break;
	}
	return 1;
}

int Menu()
{
	int number;
	do
	{
		system("cls");
		cout << "1. Вывод списка дисков\n";
		cout << "2. Вывод информации о дисках\n";
		cout << "3. Создание/удаление директории\n";
		cout << "4. Работа с файлом\n";
		cout << "5. Выход\n";

		cout << "Ваш выбор:  ";
		cin >> number;
	} while (number > 5 || number < 1);
	return number;
}

void MyGetVolumeInformation(char* Name) {
	TCHAR VolumeNameBuffer[MAX_PATH + 1];
	DWORD VolumeSerialNumber = 0;
	DWORD MaximumComponentLength = 0;
	DWORD FileSystemFlags = 0;
	TCHAR FileSystemNameBuffer[MAX_PATH + 1];
	BOOL b = GetVolumeInformation(LPWSTR(Name), VolumeNameBuffer, MAX_PATH + 1,
		&VolumeSerialNumber, &MaximumComponentLength, &FileSystemFlags,
		FileSystemNameBuffer, MAX_PATH + 1);
	if (!b) {
		cout << "Volume serial number:  " << VolumeSerialNumber << endl;
		cout << "Volume Name: " << VolumeNameBuffer << endl;
		cout << "FileSystem: " << FileSystemNameBuffer << endl;
	}
}

int MyGetDiskFreeSpace(char* Name) {
	DWORD 	SectorsPerCluster, BytesPerSector, NumberOfFreeClusters, TotalNumberOfClusters;

	GetDiskFreeSpaceA(Name, (LPDWORD)&SectorsPerCluster, (LPDWORD)&BytesPerSector, (LPDWORD)&NumberOfFreeClusters, (LPDWORD)&TotalNumberOfClusters);
	cout << SectorsPerCluster << " - SectorsPerCluster" << endl;
	cout << BytesPerSector << " - BytesPerSector" << endl;
	cout << NumberOfFreeClusters << " - NumberOfFreeClusters" << endl;
	cout << TotalNumberOfClusters << " - TotalNumberOfClusters" << endl;
	cout << "Free  space: " << SectorsPerCluster * BytesPerSector * NumberOfFreeClusters << " bytes" << endl;
	cout << "Total space: " << SectorsPerCluster * BytesPerSector * TotalNumberOfClusters << " bytes" << endl;
	system("pause");
	return 1;
}

void MyCreateFile() {
	string Name;
	cout << "Введите имя файла: ";
	cin >> Name;
	HANDLE hFile = CreateFileA(Name.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
		cout << "Файл был создан\n";
	else
		cout << "Ошибка,файл не был создан! (CODE ERROR: " << GetLastError() << ") \n";
	CloseHandle(hFile);

	system("pause");

}


int MyMoveFile() {

	string Name1;
	string Name2;
	int ans;
	cout << "Введите полный путь и название файла: ";
	cin >> Name1;
	cout << "Введите новый путь и название: ";
	cin >> Name2;

	if (FileExists(Name2.c_str())) {

		cout << "Данный файл уже существует. Перезаписать его? 1 - да, 0 - нет" << endl;
		cin >> ans;
		if (ans) {
			int a = MoveFileExA(Name1.c_str(), Name2.c_str(), MOVEFILE_REPLACE_EXISTING);

			if (a) {
				cout << "Файл был перемещен\n";
			}
			else {
				cout << "Файл не был перемещен\n";

			}
			system("pause");
			return a;
		}

		return 0;
	}
	else {

		int a = MoveFileExA(Name1.c_str(), Name2.c_str(), MOVEFILE_COPY_ALLOWED);

		if (a) {
			cout << "Файл был перемещен\n";
		}
		else {
			cout << "Файл не был перемещен\n";
		}
		system("pause");

		return a;
	}

}



int MyCopyFile() {

	string Name1;
	string Name2;
	int ans;
	cout << "Введите полный путь и название файла: ";
	cin >> Name1;
	cout << "Введите новый путь и название: ";
	cin >> Name2;

	if (FileExists(Name2.c_str())) {

		cout << "Данный файл уже существует. Перезаписать его? 1 - да, 0 - нет" << endl;
		cin >> ans;
		if (ans) {
			int a = CopyFileA(Name1.c_str(), Name2.c_str(), false);

			if (a) {
				cout << "Файл был скопирован\n";
			}
			else {
				cout << "Файл не был скопирован\n";

			}
			system("pause");
			return a;
		}

		return 0;
	}
	else {

		int a = CopyFileA(Name1.c_str(), Name2.c_str(), true);

		if (a) {
			cout << "Файл был скопирован\n";
		}
		else {
			cout << "Файл не был скопирован\n";
		}
		system("pause");

		return a;
	}

}


bool FileExists(LPCSTR fname)
{
	return::GetFileAttributesA(fname) != INVALID_FILE_ATTRIBUTES;
}



int MyFileInfo() {
	wchar_t fileName[250];
	char fileCharName[250];
	cout << "Введите имя файла: ";
	cin >> fileCharName;
	mbstowcs(fileName, fileCharName, 250);
	ostringstream tempStringStream;

	DWORD fileAttributes;
	fileAttributes = GetFileAttributes(fileName);
	tempStringStream << "0x";
	tempStringStream << hex << fileAttributes << "\n";
	cout << "Аттрибуты: " << tempStringStream.str() << endl;
	if (fileAttributes & FILE_ATTRIBUTE_ARCHIVE)
		cout << "FILE_ATTRIBUTE_ARCHIVE:\nA file or directory that is an archive file or directory. Applications typically use this attribute to mark files for backup or removal.\n--\n";
	if (fileAttributes & FILE_ATTRIBUTE_COMPRESSED)
		cout << "FILE_ATTRIBUTE_COMPRESSED:\nA file or directory that is compressed. For a file, all of the data in the file is compressed. For a directory, compression is the default for newly created files and subdirectories.\n--\n";
	if (fileAttributes & FILE_ATTRIBUTE_DEVICE)
		cout << "FILE_ATTRIBUTE_DEVICE:\nThis value is reserved for system use.\n--\n";
	if (fileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		cout << "FILE_ATTRIBUTE_DIRECTORY:\nThe handle that identifies a directory.\n--\n";
	if (fileAttributes & FILE_ATTRIBUTE_ENCRYPTED)
		cout << "FILE_ATTRIBUTE_ENCRYPTED:\nA file or directory that is encrypted. For a file, all data streams in the file are encrypted. For a directory, encryption is the default for newly created files and subdirectories.\n--\n";
	if (fileAttributes & FILE_ATTRIBUTE_HIDDEN)
		cout << "FILE_ATTRIBUTE_HIDDEN:\nThe file or directory is hidden. It is not included in an ordinary directory listing.\n--\n";
	if (fileAttributes & FILE_ATTRIBUTE_INTEGRITY_STREAM)
		cout << "FILE_ATTRIBUTE_INTEGRITY_STREAM:\nThe directory or user data stream is configured with integrity (only supported on ReFS volumes). It is not included in an ordinary directory listing. The integrity setting persists \
			 with the file if it's renamed. If a file is copied the destination file will have integrity set if either the source file or destination directory have integrity set.\n--\n";
	if (fileAttributes & FILE_ATTRIBUTE_NORMAL)
		cout << "FILE_ATTRIBUTE_NORMAL:\nA file that does not have other attributes set. This attribute is valid only when used alone.\n";
	if (fileAttributes & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED)
		cout << "FILE_ATTRIBUTE_NOT_CONTENT_INDEXED:\nThe file or directory is not to be indexed by the content indexing service.\n";
	if (fileAttributes & FILE_ATTRIBUTE_NO_SCRUB_DATA)
		cout << "FILE_ATTRIBUTE_NO_SCRUB_DATA:\nThe user data stream not to be read by the background data integrity scanner (AKA scrubber). When set on a directory it only provides inheritance. This flag is only supported on\
				Storage Spaces and ReFS volumes. It is not included in an ordinary directory listing.\n--\n";
	if (fileAttributes & FILE_ATTRIBUTE_OFFLINE)
		cout << "FILE_ATTRIBUTE_OFFLINE:\nThe data of a file is not available immediately. This attribute indicates that the file data is physically moved to offline storage. This attribute is used by Remote Storage,\
				 which is the hierarchical storage management software. Applications should not arbitrarily change this attribute.\n--\n";
	if (fileAttributes & FILE_ATTRIBUTE_READONLY)
		cout << "FILE_ATTRIBUTE_READONLY:\nA file that is read-only. Applications can read the file, but cannot write to it or delete it. This attribute is not honored on directories.\n--\n";
	if (fileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
		cout << "FILE_ATTRIBUTE_REPARSE_POINT:\nA file or directory that has an associated reparse point, or a file that is a symbolic link.\n--\n";
	if (fileAttributes & FILE_ATTRIBUTE_SPARSE_FILE)
		cout << "FILE_ATTRIBUTE_SPARSE_FILE:\nA file that is a sparse file.\n--\n";
	if (fileAttributes & FILE_ATTRIBUTE_SYSTEM)
		cout << "FILE_ATTRIBUTE_SYSTEM:\nA file or directory that the operating system uses a part of, or uses exclusively.\n--\n";
	if (fileAttributes & FILE_ATTRIBUTE_TEMPORARY)
		cout << "FILE_ATTRIBUTE_TEMPORARY:\nA file that is being used for temporary storage. File systems avoid writing data back to mass storage if sufficient cache memory is available, because typically, an application\
				 deletes a temporary file after the handle is closed. In that scenario, the system can entirely avoid writing the data. Otherwise, the data is written after the handle is closed.\n--\n";
	if (fileAttributes & FILE_ATTRIBUTE_VIRTUAL)
		cout << "FILE_ATTRIBUTE_VIRTUAL:\nThis value is reserved for system use.\n--\n";

	HANDLE hFile = CreateFile(fileName, 0x00, 0x00, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		cout << "Невозможно получить обработчик файла!\n";
		system("pause");
		return 0;
	}

	if (hFile == NULL) {
		cout << "Невозможно получить обработчик файла!\n";
		system("pause");
		return 0;
	}

	FILETIME fileCreatedTime;
	SYSTEMTIME fileCreatedSystemTime;
	wchar_t createdLocalDate[255];
	wchar_t createdLocalTime[255];
	FILETIME fileAccessedTime;
	SYSTEMTIME fileAccessedSystemTime;
	wchar_t accessedLocalDate[255];
	wchar_t accessedLocalTime[255];
	FILETIME fileWritedTime;
	SYSTEMTIME fileWritedSystemTime;
	wchar_t writedLocalDate[255];
	wchar_t writedLocalTime[255];

	if (GetFileTime(hFile, &fileCreatedTime, &fileAccessedTime, &fileWritedTime) != 0) {
		FileTimeToLocalFileTime(&fileCreatedTime, &fileCreatedTime);
		FileTimeToLocalFileTime(&fileAccessedTime, &fileAccessedTime);
		FileTimeToLocalFileTime(&fileWritedTime, &fileWritedTime);

		FileTimeToSystemTime(&fileCreatedTime, &fileCreatedSystemTime);
		FileTimeToSystemTime(&fileAccessedTime, &fileAccessedSystemTime);
		FileTimeToSystemTime(&fileWritedTime, &fileWritedSystemTime);

		GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, &fileCreatedSystemTime, NULL, createdLocalDate, 255);
		GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, &fileAccessedSystemTime, NULL, accessedLocalDate, 255);
		GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, &fileWritedSystemTime, NULL, writedLocalDate, 255);

		GetTimeFormat(LOCALE_USER_DEFAULT, 0, &fileCreatedSystemTime, NULL, createdLocalTime, 255);
		GetTimeFormat(LOCALE_USER_DEFAULT, 0, &fileAccessedSystemTime, NULL, accessedLocalTime, 255);
		GetTimeFormat(LOCALE_USER_DEFAULT, 0, &fileWritedSystemTime, NULL, writedLocalTime, 255);

		cout << "Время создания: " << fileCreatedSystemTime.wDay << "." << fileCreatedSystemTime.wMonth << "."
			<< fileCreatedSystemTime.wYear << " " << fileCreatedSystemTime.wHour << ":"
			<< fileCreatedSystemTime.wMinute << "\n";
		cout << "Последнее обращение: " << fileAccessedSystemTime.wDay << "." << fileAccessedSystemTime.wMonth << "."
			<< fileAccessedSystemTime.wYear << " " << fileAccessedSystemTime.wHour << ":"
			<< fileAccessedSystemTime.wMinute << "\n";
		cout << "Последнее изменение: " << fileWritedSystemTime.wDay << "." << fileWritedSystemTime.wMonth << "."
			<< fileWritedSystemTime.wYear << " " << fileWritedSystemTime.wHour << ":"
			<< fileWritedSystemTime.wMinute << "\n";
	}

	BY_HANDLE_FILE_INFORMATION fileinfo;
	if (GetFileInformationByHandle(hFile, &fileinfo)) {
		cout << "\nСерийный номер тома: " << fileinfo.dwVolumeSerialNumber << endl
			<< "Количество ссылок: " << fileinfo.nNumberOfLinks << endl;
	}

	CloseHandle(hFile);
	system("pause");
}

void changeFileAttributes() {
	char fileName[250];
	cout << "Введите имя файла: ";
	cin >> fileName;
	DWORD attrs = GetFileAttributesA(fileName);

	char answer;

	cout << "Сделать архивным? (1/0):";
	cin >> answer;
	if (answer == '1')
		attrs |= FILE_ATTRIBUTE_ARCHIVE;
	else
		attrs &= ~FILE_ATTRIBUTE_ARCHIVE;
	cout << "Сделать невидимым? ";
	cin >> answer;
	if (answer == '1')
		attrs |= FILE_ATTRIBUTE_HIDDEN;
	else
		attrs &= ~FILE_ATTRIBUTE_HIDDEN;
	cout << "Сделать обычным? ";
	cin >> answer;
	if (answer == '1')
		attrs |= FILE_ATTRIBUTE_NORMAL;
	else
		attrs &= ~FILE_ATTRIBUTE_NORMAL;
	cout << "Индексировать содержание? ";
	cin >> answer;
	if (answer == '1')
		attrs |= FILE_ATTRIBUTE_NOT_CONTENT_INDEXED;
	else
		attrs &= ~FILE_ATTRIBUTE_NOT_CONTENT_INDEXED;
	cout << "Доступен без сети?";
	cin >> answer;
	if (answer == '1')
		attrs |= FILE_ATTRIBUTE_OFFLINE;
	else
		attrs &= ~FILE_ATTRIBUTE_OFFLINE;
	cout << "Сделать доступным только для чтения?";
	cin >> answer;
	if (answer == '1')
		attrs |= FILE_ATTRIBUTE_READONLY;
	else
		attrs &= ~FILE_ATTRIBUTE_READONLY;
	cout << "Сделать системным? ";
	cin >> answer;
	if (answer == '1')
		attrs |= FILE_ATTRIBUTE_SYSTEM;
	else
		attrs &= ~FILE_ATTRIBUTE_SYSTEM;
	cout << "Сделать временным? ";
	cin >> answer;
	if (answer == '1')
		attrs |= FILE_ATTRIBUTE_TEMPORARY;
	else
		attrs &= ~FILE_ATTRIBUTE_TEMPORARY;

	if (SetFileAttributesA(fileName, attrs))
		cout << "Аттрибуты успешно установленны!\n";
	else
		cout << "Произошла ошибка, аттрибуты не были установлены!\n";
	system("pause");
}

void changeCreationTime() {
	wchar_t filename[250];
	char fileCharName[250];
	cout << "Введите имя файла: ";
	cin >> fileCharName;
	mbstowcs(filename, fileCharName, 250);
	HANDLE hFile = CreateFile(filename, FILE_WRITE_ATTRIBUTES, 0, NULL, OPEN_EXISTING, 0, NULL);

	FILETIME fileTime;
	SYSTEMTIME systemTimeNow;
	GetSystemTime(&systemTimeNow);
	SystemTimeToFileTime(&systemTimeNow, &fileTime);

	if (SetFileTime(hFile, &fileTime, NULL, NULL))
		cout << "Время успешно установленно\n" << systemTimeNow.wDay << "." << systemTimeNow.wMonth << "."
		<< systemTimeNow.wYear << " " << systemTimeNow.wHour + 3 << ":" << systemTimeNow.wMinute << "\n";
	else
		cout << "Произошла ошибка, время установить не удалось\n";

	CloseHandle(hFile);
	system("pause");
}



