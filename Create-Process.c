<<<<<<< HEAD
<<<<<<< HEAD
#include <stdio.h>
#include <windows.h>

int main(void)
{

    STARTUPINFOW startup_info = {0};

    PROCESS_INFORMATION process_info = {0};

    if (!CreateProcessW(
            L"C:\\Windows\\System32\\notepad.exe",
            NULL,
            NULL,
            NULL,
            FALSE,
            BELOW_NORMAL_PRIORITY_CLASS,
            NULL,
            NULL,
            &startup_info,
            &process_info))
    {
        printf("(-)Failed to create process, error: %ld", GetLastError());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
=======
#include <stdio.h>
#include <windows.h>

int main(void)
{

    STARTUPINFOW startup_info = {0};

    PROCESS_INFORMATION process_info = {0};

    if (!CreateProcessW(
            L"C:\\Windows\\System32\\notepad.exe",
            NULL,
            NULL,
            NULL,
            FALSE,
            BELOW_NORMAL_PRIORITY_CLASS,
            NULL,
            NULL,
            &startup_info,
            &process_info))
    {
        printf("(-)Failed to create process, error: %ld", GetLastError());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
>>>>>>> 742305cb6126e51427c0cfc5919f67e3c8696942
=======
#include <stdio.h>
#include <windows.h>

int main(void)
{

    STARTUPINFOW startup_info = {0};

    PROCESS_INFORMATION process_info = {0};

    if (!CreateProcessW(
            L"C:\\Windows\\System32\\notepad.exe",
            NULL,
            NULL,
            NULL,
            FALSE,
            BELOW_NORMAL_PRIORITY_CLASS,
            NULL,
            NULL,
            &startup_info,
            &process_info))
    {
        printf("(-)Failed to create process, error: %ld", GetLastError());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
>>>>>>> 742305cb6126e51427c0cfc5919f67e3c8696942
}