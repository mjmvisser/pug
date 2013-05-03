#include "console.h"
#include "logger.h"

Console *Console::s_instance = 0;

Console::Console(QObject *parent) :
    QObject(parent),
    m_log(new Log(this))
{
}

Log *Console::log()
{
    if (s_instance == 0)
        s_instance = new Console();

    return s_instance->m_log;
}
