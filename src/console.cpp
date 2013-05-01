#include "console.h"

Console *Console::s_instance = 0;

Log *Console::log()
{
    if (s_instance == 0)
        s_instance = new Console();

    return s_instance;
}
