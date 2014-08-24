#ifndef FUNWEB_H
#define FUNWEB_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <QtNetwork>

namespace Ui {
    class FunWeb;
};

class FunApi;

class FunWeb : public QWidget
{ 
    Q_OBJECT;
public:
    FunWeb();
    virtual ~FunWeb();

public slots:
    void onSearch();
    void onMediaClicked(const QModelIndex &index);

    // callbacks
    void onSearchDone(QJsonDocument doc);
    void onGeturlDone(QJsonDocument doc);

private slots:
    void onConnection();
    void onReadyRead();
    void onClosed();

private:
    FunApi *m_api = NULL;
    Ui::FunWeb *mui = NULL;
    QStringListModel *m_model = NULL;
    QStringList m_urls;

    QTcpServer *m_serv = NULL;
    int m_playid = 0;
};


#endif /* FUNWEB_H */
