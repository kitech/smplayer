#include "funapi.h"

#include "funweb.h"
#include "ui_funweb.h"

#include <qcombobox.h>


FunWeb::FunWeb()
    : QWidget()
{
    mui = new Ui::FunWeb;
    mui->setupUi(this);

    m_model = new QStringListModel;
    mui->listView->setModel(m_model);

    m_serv = new QTcpServer;
    m_serv->listen(QHostAddress::Any, 5000);
    QObject::connect(m_serv, &QTcpServer::newConnection, this, &FunWeb::onConnection);

    m_api = new FunApi;
    QObject::connect(m_api, &FunApi::searchDone, this, &FunWeb::onSearchDone);
    QObject::connect(m_api, &FunApi::geturlDone, this, &FunWeb::onGeturlDone);

    QObject::connect(mui->pushButton, &QPushButton::clicked,
                     this, &FunWeb::onSearch);
    QObject::connect(mui->listView, &QListView::doubleClicked, this, &FunWeb::onMediaClicked);
}

FunWeb::~FunWeb()
{
}


void FunWeb::onSearch()
{
    QString keywords = mui->comboBox->currentText();
    qDebug()<<keywords;

    /*
    FunApi *api = new FunApi();
    QObject::connect(api, &FunApi::searchDone, this, &FunWeb::onSearchDone);
    QObject::connect(api, &FunApi::geturlDone, this, &FunWeb::onGeturlDone);
    */
    
    m_api->search(keywords);
}

void FunWeb::onMediaClicked(const QModelIndex &index)
{
    qDebug()<<index<<m_model->data(index, Qt::DisplayRole);
    QString name = m_model->data(index, Qt::DisplayRole).toString();
    QString vid = name.split("###").at(0);

    m_api->geturl(vid);
}

void FunWeb::onSearchDone(QJsonDocument doc)
{
    QJsonArray arr = doc.array();
    m_urls.clear();
    for (int i = 0; i < arr.size(); i++) {
        qDebug()<<i<<arr.at(i).toString();
        QString name = arr.at(i).toString();

        {
            m_model->insertRows(0, 1);
            m_model->setData(m_model->index(0, 0), name, Qt::DisplayRole);
        }

        {
            m_urls << name;
        }
    }

    
}

void FunWeb::onGeturlDone(QJsonDocument doc)
{
    if (!doc.isEmpty()) {
        qDebug()<<"invalid url list";
    }

    QJsonArray arr = doc.array();
    qDebug()<< arr;

    QString m3u;
    m3u = "#EXTM3U\r\n"
        "#EXT-X-TARGETDURATION:300\r\n"
        "#EXT-X-VERSION:3\r\n";

    foreach (QJsonValue v, arr) {
        m3u += QString("#EXTINF:,\r\n%1\r\n").arg(v.toString());
    }

    m3u += "#EXT-X-ENDLIST\r\n";


    QFile fp(QString("./tmp/%1.m3u").arg(++m_playid));
    qDebug()<<"got play file:"<<fp.fileName();

    fp.open(QIODevice::ReadWrite);
    fp.resize(0);
    fp.write(m3u.toLatin1());
    fp.close();
}


/////////////
void FunWeb::onConnection()
{
    QTcpSocket *sock = m_serv->nextPendingConnection();
    QObject::connect(sock, &QTcpSocket::readyRead, this, &FunWeb::onReadyRead);
    QObject::connect(sock, &QTcpSocket::disconnected, this, &FunWeb::onClosed);
}

void FunWeb::onReadyRead()
{
    QTcpSocket *sock = (QTcpSocket*)(sender());
    QByteArray data = sock->readAll();
    qDebug()<<data;

    
    QStringList headers = QString(data).split("\r\n");
    qDebug()<<headers;

    QStringList parts;
    foreach (QString line, headers) {
        if (line.startsWith("GET /")) {
            parts = line.split(" ");
            break;
        }
    }

    QString fname = parts.at(1).trimmed();
    qDebug()<<fname;

    QString docroot = ".";
    QString full_path = docroot + fname;
    
    QFile fp(full_path);
    fp.open(QIODevice::ReadOnly);
    QByteArray content = fp.readAll();
    fp.close();

    QString resp_str = QString("HTTP/1.0 200 OK\r\n"
                               // "Content-Type: application/octet-stream\r\n"
                               "Content-Type: application/vnd.apple.mpegurl\r\n"
                               "Content-Length: %1\r\n"
                               "Connection: close\r\n"
                               "\r\n"
                               "%2\n")
        .arg(content.length() + 1)
        .arg(QString(content));
    sock->write(resp_str.toLatin1());

    // sock->deleteLater();
}

void FunWeb::onClosed()
{
    QTcpSocket *sock = (QTcpSocket *)(sender());
    qDebug()<<(sender());
    
    sock->deleteLater();
}

