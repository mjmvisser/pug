#ifndef CONSOLE_H
#define CONSOLE_H

#include "logger.h"

class Console : public Log
{
    Q_OBJECT
public:
    static Log *log();
private:
    static Console *s_instance;
};

#endif
