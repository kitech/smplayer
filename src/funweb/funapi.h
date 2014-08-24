#ifndef _FUNAPI_H_
#define _FUNAPI_H_

void m_hehe(void *abc);

#include <QtCore>

class FunApi : public QThread
{
    Q_OBJECT;

public:
    FunApi();
    virtual ~FunApi();

    virtual void run();

    QString m_cmd;
    QString m_args;
};

#endif











