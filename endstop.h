#ifndef ENDSTOP_H
#define ENDSTOP_H
#include <QtCore>
#include <QObject>

class EndStop : public QObject
{
     Q_OBJECT

public:
    EndStop();
    ~EndStop();
};

#endif // ENDSTOP_H
