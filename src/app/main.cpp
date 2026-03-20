#include <windows.h>

#include "Application.h"

int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    keyviz::Application application;
    return application.Run();
}