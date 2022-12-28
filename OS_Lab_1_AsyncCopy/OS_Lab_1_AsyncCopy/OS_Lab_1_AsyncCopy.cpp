#include<iostream>
#include<windows.h>
#include<fileapi.h>
#include<string>

int callback = 0;
DWORD counterInfo = 0;

VOID CALLBACK FileIOCompletionRoutine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped) {
    callback++;
}


void readFileOverlapped(HANDLE firstHandle, DWORD64 fileSize, DWORD blockSize,
    int operations, OVERLAPPED* overlappeds, char** buffer)
{
    DWORD64 writtenBytes = 0, highWriten = 0;
    int operations_counter = 0;
    DWORD counterInfoOld = counterInfo;

    for (int i = 0; i < operations && fileSize > counterInfo; i++) {
        operations_counter++;
        ReadFileEx(firstHandle, buffer[i], blockSize, &overlappeds[i], FileIOCompletionRoutine);
        counterInfo += blockSize;
    }

    while (callback < operations_counter) {
        SleepEx(-1, true);
    }
    for (int i = 0; i < operations; i++) {
        writtenBytes = ((DWORD64)overlappeds[i].Offset) + (DWORD64)(blockSize * operations) +
            ((DWORD64)overlappeds[i].OffsetHigh << 32);
        highWriten = writtenBytes;
        overlappeds[i].OffsetHigh = (highWriten >> 32);

        if (overlappeds[i].OffsetHigh == 0) {
            overlappeds[i].Offset = writtenBytes;
        }
        else {
            overlappeds[i].Offset = writtenBytes - ((DWORD64)overlappeds[i].OffsetHigh << 32);
        }
    }
    callback = 0;
    counterInfo = counterInfoOld;
}

void writeFileOverlapped(HANDLE secondHandle, DWORD64 fileSize, DWORD blockSize,
    int operations, OVERLAPPED* overlappeds, char** buffer)
{
    int operations_counter = 0;
    DWORD64 writtenBytes = 0, highWriten = 0;

    for (int i = 0; i < operations && fileSize > counterInfo; i++) {
        operations_counter++;
        WriteFileEx(secondHandle, buffer[i], blockSize, &overlappeds[i], FileIOCompletionRoutine);
        counterInfo += blockSize;
    }

    while (callback < operations_counter) {
        SleepEx(-1, true);
    }

    for (int i = 0; i < operations; i++) {
        highWriten = ((DWORD64)overlappeds[i].Offset) + (DWORD64)(blockSize * operations) + ((DWORD64)overlappeds[i].OffsetHigh << 32);
        overlappeds[i].OffsetHigh = (highWriten >> 32);
        if (!overlappeds[i].OffsetHigh) {
            overlappeds[i].Offset = highWriten;
        }
        else
            overlappeds[i].Offset = highWriten - ((DWORD64)overlappeds[i].OffsetHigh << 32);

    }

    callback = 0;
}

void asycnFileCopy(DWORD64 sizeFileOnDisk, HANDLE handleInput,
    HANDLE handleOutputint, char* readingBytes, char* pointerToBeginning, DWORD blockSize,
    DWORD64 sizeFile, DWORD high, DWORD low, std::string input, std::string output, int numberOfThreads)
{

    DWORD64 curSize;
    char** buffer = new char* [numberOfThreads];
    OVERLAPPED* overForRead = new OVERLAPPED[numberOfThreads], * overForWrite = new OVERLAPPED[numberOfThreads];

    for (int i = 0; i < numberOfThreads; i++) {
        buffer[i] = new char[blockSize];
        overForRead[i].OffsetHigh = overForWrite[i].OffsetHigh = ((DWORD64)(i * blockSize) >> 32);

        if (!overForWrite[i].OffsetHigh) {
            overForRead[i].Offset = overForWrite[i].Offset = (DWORD64)(i * blockSize);
        }
        else {
            overForRead[i].Offset = overForWrite[i].Offset =
                ((DWORD64)(i * blockSize)) - ((DWORD64)overForRead[i].OffsetHigh << 32);
        }
        overForRead[i].hEvent = overForWrite[i].hEvent = NULL;
    }

    curSize = 0;
    int start = GetTickCount();


    while (curSize < sizeFileOnDisk) {
        readFileOverlapped(handleInput, sizeFile, blockSize, numberOfThreads, overForRead, buffer);
        writeFileOverlapped(handleOutputint, sizeFile, blockSize, numberOfThreads, overForWrite, buffer);
        curSize += (DWORD64)(blockSize * numberOfThreads);
    }

    int finish = GetTickCount();
    counterInfo = 0;

    long long_high = (long)high;

    SetFilePointer(handleOutputint, sizeFile, &long_high, FILE_BEGIN);
    SetEndOfFile(handleOutputint);
    CloseHandle(handleInput);
    CloseHandle(handleOutputint);
    std::cout << "Время копирования: " << finish - start << " ms/";
}


void startAsyncSopy(int factorCluster, std::string input, std::string output, int numberOfThreads) {
    HANDLE handleInput, handleOutput;
    char* readingBytes, * pointerToBeginning;
    DWORD block;
    DWORD64 sizeFile, sizeFileOnDisk;
    DWORD low, high, clusterSize = 4096;

    handleInput = CreateFileA(
        input.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED,
        NULL
    );

    handleOutput = CreateFileA(
        output.c_str(),
        GENERIC_WRITE,
        FILE_SHARE_WRITE,
        NULL,
        CREATE_ALWAYS,
        FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED,
        NULL
    );
    block = factorCluster * clusterSize;
    low = GetFileSize(handleInput, &high);
    sizeFile = ((DWORD64)high << 32) + low;
    sizeFileOnDisk = 0;
    int time;

    while (sizeFileOnDisk < sizeFile) {
        sizeFileOnDisk += block;
    }

    readingBytes = new char[sizeFileOnDisk + 1];
    pointerToBeginning = readingBytes;

    asycnFileCopy(sizeFileOnDisk, handleInput, handleOutput, readingBytes, pointerToBeginning,
        block, sizeFile, high, low, input, output, numberOfThreads);
}

int main() {
    setlocale(LC_ALL, "Russian");
    int num, clusterSize, threadsCount, time, k;
    char proceed;
    do {
        system("cls");
        std::cout << "Введите 1 для асинхронного копирования\n";
        std::cout << "Введите 0 для выхода \n";
        std::cin >> num;
        if (num) {
            std::string fileInp, fileOut;
            std::cout << "Введите путь до файла (C:\\papka\\test.txt): ";
            std::cin >> fileInp;
            std::cout << "Введите путь для скопированого файла (C:\\papka\\copy.txt): ";
            std::cin >> fileOut;
            std::cout << "Количество перекрывающихся потоков ввода и вывода: ";
            std::cin >> threadsCount;
            std::cout << "Размер кластера <<4096*K>> Введите K: ";
            std::cin >> clusterSize;
            startAsyncSopy(clusterSize, fileInp, fileOut, threadsCount);
            //std::cout << "Время копирования: " << time << " ms/";
            std::cout << "Введите любую клавишу: ";
            std::cin >> proceed;
        }
    } while (num != 0);

    return 0;
}