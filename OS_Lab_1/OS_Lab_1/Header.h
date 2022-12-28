#pragma once
#include <iostream>
#include <windows.h>

using namespace std;

#define SLEEP_TIME -1
//#define BlockSize 4096 //размер блока = размеру кластера в байтах
//#define NumOperations 1

int SelectMenuItem();
void RunAllTask();
HANDLE GiveMeHandleForReadPls(); //получаем хендл
HANDLE GiveMeHandleForWritePls();
void FileIOCompletionRoutine(DWORD, DWORD, LPOVERLAPPED);//ф-я для ф-ий чтения и записи
int ReadFileExPls(HANDLE, char**, LPOVERLAPPED, long long*);   //чтение информации
bool WriteFileExPls(HANDLE, char**, LPOVERLAPPED, int);  //запись информации

int Counter; //глобальный счетчик
int BlockSize;
int NumOperations;

int main()
{
    system("chcp 1251"); //кодировка консоли меняется на виндовскую
    RunAllTask();
    return 0;
}

//тут выбор пункта менюшки
int SelectMenuItem()
{
    int MenuItem;
    do
    {
        system("cls");
        cout << "1. Copy file\n";
        cout << "0. Exit\n";
        cout << "\nYou chose this point: ";
        cin >> MenuItem;
        if(MenuItem < 0 || MenuItem > 1)
        {
            cout << "You chose a wrong number! Please, try again!\n";
            system("pause");
        }
    } while (MenuItem < 0 || MenuItem > 1);
    return MenuItem;
}

//тут менюшка и весь запуск
void RunAllTask()
{
    int MenuItem;       //пунктик менюшки
    LARGE_INTEGER LargeInteger;
    HANDLE HandleRead, HandleWrite;

    do
    {
        MenuItem = SelectMenuItem();
        if (MenuItem == 1)
        {

            system("cls");
            cout << "Input the name of existing file to copy: ";
            HandleRead = GiveMeHandleForReadPls();
            cout << "Input the name of file to copy in: ";
            HandleWrite = GiveMeHandleForWritePls();
            cout << "Input the block size 4096 * ";
            do
            {
                cin >> BlockSize;
                if(BlockSize < 1)
                {
                    cout << "You inputted a wrong number! Please, try again!\n";
                }
            }while (BlockSize < 1);
            BlockSize *= 4096;
            cout << "Input the number of operations: ";
            do
            {
                cin >> NumOperations;
                if(NumOperations < 1)
                {
                    cout << "You inputted a wrong number! Please, try again!\n";
                }
            }while (NumOperations < 1);
            LARGE_INTEGER RealFileSize;
            GetFileSizeEx(HandleRead, &RealFileSize);
            long long FileSize = LargeInteger.QuadPart = RealFileSize.QuadPart;
            char** Buffer = new char*[NumOperations];
            for(int i = 0; i < NumOperations; i++)
            {
                Buffer[i] = new char[BlockSize];
            }
            OVERLAPPED* Overlapped = new OVERLAPPED[NumOperations];
            LargeInteger.QuadPart = 0LL;
            int RealNumOperations;
            DWORD Start = GetTickCount();
            do
            {
                for (int i = 0; i < NumOperations; i++)
                {
                    if(FileSize > 0LL)
                    {
                        Overlapped[i].Offset = LargeInteger.LowPart;
                        Overlapped[i].OffsetHigh = (DWORD)LargeInteger.HighPart;
                        LargeInteger.QuadPart += (long long)BlockSize;
                    }
                }
                //cout << LargeInteger.LowPart << " " << LargeInteger.HighPart << '\n';
                RealNumOperations = ReadFileExPls(HandleRead, Buffer, Overlapped, &FileSize);
                if(RealNumOperations < 0)
                {
                    LargeInteger.QuadPart = 0LL;
                    break;
                }
                while (Counter != RealNumOperations)
                {
                    SleepEx(-1, TRUE);
                }
                Counter = 0;
                if(!WriteFileExPls(HandleWrite, Buffer, Overlapped, RealNumOperations))
                {
                    LargeInteger.QuadPart = 0LL;
                    break;
                }
                while (Counter != RealNumOperations)
                {
                    SleepEx(-1, TRUE);
                }
                Counter = 0;
            } while (FileSize > 0LL);
            cout << "Copied in " << ((double)(GetTickCount() - Start) / 1000) << " sec\n";
            SetFilePointerEx(HandleWrite, RealFileSize, NULL, FILE_BEGIN);
            SetEndOfFile(HandleWrite);
            for (int i = 0; i < NumOperations; i++)
            {
                delete Buffer[i];
            }
            delete Buffer;
            delete Overlapped;
            CloseHandle(HandleRead);
            CloseHandle(HandleWrite);
            system("pause");
        }
    } while (MenuItem != 0);
}

//получаем хендл http://delphiexpert.ru/createfile-winapi-funkczii.html
HANDLE GiveMeHandleForReadPls()
{
    string FileName;
    DWORD DesiredAccess,
        ShareMode;
    int MenuItem1,
        MenuItem2;
    /*pFileName //имя файла
    dwDesiredAccess //Тип доступа выбор
    dwShareMode //тип того, как объект может быть разделен выбор
    -lpSecurityAttributes //что-то безопасности мб NULL
    -DWORD dwCreationDisposition //Определяет случай когда файлы существуют crete-new
    -DWORD dwFlagsAndAttributes //Установать нормал
    -THandle hTemplateFile //не поняла, что это, но нужно попробовать нулл*/
    cin >> FileName;
    HANDLE OpenIt = CreateFile(
        (LPCWSTR)FileName.c_str(),
        GENERIC_READ,
        0,
        NULL,
        OPEN_ALWAYS, //если такого файла нет, будет создан
        FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED,
        NULL);
    return OpenIt;
}

HANDLE GiveMeHandleForWritePls()
{
    string FileName;
    DWORD DesiredAccess,
        ShareMode;
    int MenuItem1,
        MenuItem2;
    /*pFileName //имя файла
    dwDesiredAccess //Тип доступа выбор
    dwShareMode //тип того, как объект может быть разделен выбор
    -lpSecurityAttributes //что-то безопасности мб NULL
    -DWORD dwCreationDisposition //Определяет случай когда файлы существуют crete-new
    -DWORD dwFlagsAndAttributes //Установать нормал
    -THandle hTemplateFile //не поняла, что это, но нужно попробовать нулл*/
    cin >> FileName;
    HANDLE OpenIt = CreateFile(
        (LPCWSTR)FileName.c_str(),
        GENERIC_WRITE,
        0,
        NULL,
        OPEN_ALWAYS, //если такого файла нет, будет создан
        FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED,
        NULL);
    return OpenIt;
}

//Функция
void FileIOCompletionRoutine(DWORD ErrorCode, DWORD NumberOfBytesTransfered, LPOVERLAPPED Overlapped)
{
    Counter++;
    /*DWORD dwErrorCode,                // код завершения
    DWORD dwNumberOfBytesTransfered,  // передаваемое число байтов
    LPOVERLAPPED lpOverlapped         // буфер информации I/O*/
}

//считывать данные
int ReadFileExPls(HANDLE FileHandle, char** Buffer, LPOVERLAPPED Overlapped, long long* FileSize)
{
    //OVERLAPPED OverlappedTest[NumOperations];
    int RealNumOperation = 0;
    /*BOOL ReadFileEx(
    HANDLE hFile,                        // дескриптор файла
    LPVOID lpBuffer,                     // буфер данных
    DWORD nNumberOfBytesToRead,          // число читаемых байтов
    LPOVERLAPPED lpOverlapped,           // смещение
    LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine // процедура завершения*/

    for (int i = 0; (i < NumOperations) && (*FileSize > 0LL); i++, RealNumOperation++)
    {
        //cout << Overlapped[i].Offset << " And " << i << '\n';
        bool ReadIt = ReadFileEx(
        FileHandle,
        Buffer[i],
        BlockSize,
        &Overlapped[i],
        //&OverlappedTest[i],
        FileIOCompletionRoutine);
        if (ReadIt)
        {
            //cout << "The file was opened successfully!\n";
            //CloseHandle(ReadIt); //закрыть указатель объекта, если он открылса
        }
        else
        {
            cout << "Oh, something went wrong with read... Error code is " << GetLastError() << ".\n";
            return -1;
        }
        *FileSize -= (long long)BlockSize;
    }
    return RealNumOperation;
}
bool WriteFileExPls(HANDLE FileHandle, char** Buffer, LPOVERLAPPED Overlapped, int RealNumOperations)
{
        /*HANDLE hFile,                      // дескриптор файла вывода
    LPCVOID lpBuffer,                  // буфер данных
    DWORD nNumberOfBytesToWrite,       // число байтов для записи
    LPOVERLAPPED lpOverlapped,         // асинхронный буфер
    LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine    // процедура завершения*/

    //cout << RealNumOperations << '\n';
    for (int i = 0; i < RealNumOperations; i++)
    {
        //cout << Overlapped[i].Offset << " and " << i << '\n';
        bool WriteIt = WriteFileEx(
        FileHandle,
        Buffer[i],
        BlockSize,
        &Overlapped[i],
        FileIOCompletionRoutine);
        if (WriteIt)
        {
            //cout << "The file was opened successfully!\n";
            //CloseHandle(WriteIt); //закрыть указатель объекта, если он открылса
        }
        else
        {
            cout << "Oh, something went wrong with write... Error code is " << GetLastError() << ".\n";
            return FALSE;
        }
    }
    return TRUE;
}