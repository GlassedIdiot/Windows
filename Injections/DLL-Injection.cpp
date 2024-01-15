#include <windows.h>
#include <iostream>
#include <string>
#define okay(msg, ...) printf("[+] " msg "\n", ##__VA_ARGS__)
#define warn(msg, ...) printf("[-] " msg "\n", ##__VA_ARGS__)
#define info(msg, ...) printf("[i]" msg "\n", ##__VA_ARGS__)

using namespace std;

bool __stdcall DllMain(HINSTANCE hmodule, DWORD dwReason, LPVOID lpvreserved)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:

        MessageBoxW(NULL, L"HelloWorld", L"~Hola amigo~", MB_OK);
        break;
    }
    return TRUE;
}

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

int main(int argc, char *argv[])
{

    // Variable Declarations.
    HMODULE mydll = NULL;
    wchar_t path_to_dll[MAX_PATH] = L"your-path";
    DWORD PID = NULL, TID = NULL;
    LPVOID rBuffer = NULL;
    HANDLE rProcess = NULL, rThread = NULL;
    HMODULE hkernell32 = NULL;
    int path_size = sizeof(path_to_dll);
    int bytesWritten = 0;

    getProcessIdFromArgs(argc, argv);

    mydll = LoadLibraryW(path_to_dll);

    cout << "The path of the dll file is" << path_to_dll << endl;

    info("Trying to get a handle to the %S", path_to_dll);

    if (!mydll)
    {
        warn("Was not able to get a handle to the %S with the error: 0x%lx", path_to_dll, GetLastError());
        return EXIT_FAILURE;
    }
    okay("got a handle to awesome.dll!");
    info("---0x%p", mydll);

    info("freeing the module");
    FreeLibrary(mydll);
    okay("finished, press <enter> to exit");
    getchar();

CLEANUP:
    if (rProcess && rThread)
    {
        cout << "Cleaning up the remote process.";
        CloseHandle(rProcess);
        CloseHandle(rThread);
    }

    return EXIT_SUCCESS;
}