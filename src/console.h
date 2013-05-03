#ifndef CONSOLE_H
#define CONSOLE_H

#include <QObject>

class Log;

class Console : public QObject
{
    Q_OBJECT
public:
    explicit Console(QObject *parent = 0);

    static Log *log();
private:
    Log *m_log;

    static Console *s_instance;
};

#endif
