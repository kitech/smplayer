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

    void search(QString keywords);
    void geturl(QString vid);

    QString m_cmd;
    QString m_args;
    QJsonDocument m_ret;

public slots:
    void onFinished();

signals:
    void searchDone(QJsonDocument);
    void geturlDone(QJsonDocument);
};

#endif











