#include <stdio.h>
#include <Windows.h>

int main(void)
{
  MessageBox(

      NULL,
      "Hello. Press OK if you are happy with life OR cancel to end it.",
      "Welcome",
      MB_OKCANCEL | MB_ICONASTERISK);
  return EXIT_SUCCESS;
}