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
int main(int argc, char *argv[])
{

    HMODULE mydll = NULL;

    wchar_t path_to_dll[MAX_PATH] = L"E:\\Shit\\Windows\\Injections\\awesome-dll.dll";

    mydll = LoadLibraryW(path_to_dll);

    cout << "The path of the dll file is" << path_to_dll;

    info("Trying to get a handle to the %s", path_to_dll);

    if (!mydll)
    {
        warn("Was not able to get a handle to the %s with the error: 0x%lx", path_to_dll, GetLastError());
        return EXIT_FAILURE;
    }
    okay("got a handle to awesome.dll!");
    info("---0x%p", mydll);

    info("freeing the module");
    FreeLibrary(mydll);
    okay("finished, press <enter> to exit");
    getchar();

    return EXIT_SUCCESS;
}