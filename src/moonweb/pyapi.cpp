#include "pyapi.h"
#include "settings.h"
#include "webvideo.h"
#include "downloader.h"
#include "moonplaylist.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QApplication>
#include <QColor>
#include <QMessageBox>
#include <QNetworkProxy>
#include <QDir>

/*****************************************
 ******** Some useful functions **********
 ****************************************/
#define RETURN_IF_ERROR(retval)  if ((retval) == NULL){PyErr_Print(); return;}
#define EXIT_IF_ERROR(retval)    if ((retval) == NULL){PyErr_Print(); exit(-1);}

void call_py_func_vsi(PyObject *func, const char *first, int second)
{
    PyObject *ret = PyObject_CallFunction(func, "si", first, second);
    RETURN_IF_ERROR(ret)
    Py_DecRef(ret);
}

/******************************************
 ** Define get_url() function for python **
 *****************************************/

GetUrl *geturl_obj = NULL;
PyObject *exc_GetUrlError = NULL;

GetUrl::GetUrl(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(qApp);
    reply = NULL;
    callbackFunc = NULL;
    data = NULL;
    exc_GetUrlError = PyErr_NewException("moonplayer.GetUrlError", NULL, NULL);
}

void GetUrl::start(const char *url, PyObject *callback, PyObject *_data)
{
    //save callback function
    callbackFunc = callback;
    data = _data;
    Py_IncRef(callbackFunc);
    Py_IncRef(data);
    //init network manager
    if (Settings::proxy.isEmpty())
        manager->setProxy(QNetworkProxy(QNetworkProxy::NoProxy));
    else
        manager->setProxy(QNetworkProxy(QNetworkProxy::HttpProxy, Settings::proxy, Settings::port));
    //start request
    QNetworkRequest request = QNetworkRequest(QString::fromUtf8(url));
    request.setRawHeader("User-Agent", "moonplayer");
    reply = manager->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(onFinished()));
    // Set moonplayer.final_url in Python
    PyObject *str = PyString_FromString(url);
    PyObject_SetAttrString(apiModule, "final_url", str);
    Py_DecRef(str);
}

void GetUrl::onFinished()
{
    //check redirection
    int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (status == 301 || status == 302)
    {
        reply->deleteLater();
        QByteArray final_url = reply->rawHeader("Location");
        PyObject *str = PyString_FromString(final_url.constData());
        PyObject_SetAttrString(apiModule, "final_url", str);
        Py_DecRef(str);
        //start request
        QNetworkRequest request = QNetworkRequest(QString::fromUtf8(final_url));
        request.setRawHeader("User-Agent", "moonplayer");
        reply = manager->get(request);
        connect(reply, SIGNAL(finished()), this, SLOT(onFinished()));
        return;
    }
    QByteArray barray = reply->readAll();
    PyObject *callback = callbackFunc;
    PyObject *_data = data;
    callbackFunc = NULL;
    data = NULL;
    reply->deleteLater();
    reply = 0;
    PyObject *retVal = PyObject_CallFunction(callback, "sO", barray.constData(), _data);
    Py_DecRef(_data);
    Py_DecRef(callback);
    RETURN_IF_ERROR(retVal)
    Py_DecRef(retVal);
}

static PyObject *get_url(PyObject *, PyObject *args)
{
    PyObject *callback, *data;
    const char *url;
    if (!PyArg_ParseTuple(args, "sOO", &url, &callback, &data))
        return NULL;
    if (geturl_obj->hasTask())
    {
        PyErr_SetString(exc_GetUrlError, "Another task is running.");
        return NULL;
    }
    geturl_obj->start(url, callback, data);
    Py_IncRef(Py_None);
    return Py_None;
}


/********************
 * Dialog functions *
 ********************/
static PyObject *warn(PyObject *, PyObject *args)
{
    const char *msg;
    if (!PyArg_ParseTuple(args, "s", &msg))
        return NULL;
    QMessageBox::warning(NULL, "Warning", QString::fromUtf8(msg));
    Py_IncRef(Py_None);
    return Py_None;
}

static PyObject *question(PyObject *, PyObject *args)
{
    const char *msg;
    if (!PyArg_ParseTuple(args, "s", &msg))
        return NULL;
    if (QMessageBox::question(NULL, "question", QString::fromUtf8(msg), QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
    {
        Py_IncRef(Py_True);
        return Py_True;
    }
    Py_IncRef(Py_False);
    return Py_False;
}

/************************
 ** WebVideo functions **
 ************************/
static PyObject *set_list_item_color(PyObject *, PyObject *args)
{
    int n;
    const char *color;
    if (!PyArg_ParseTuple(args, "is", &n, &color))
        return NULL;
    webvideo->setListItemColor(n, QColor(color));
    Py_IncRef(Py_None);
    return Py_None;
}

static PyObject *show_list(PyObject *, PyObject *args)
{
    PyObject *list;
    if (!PyArg_ParseTuple(args, "O", &list))
        return NULL;
    return webvideo->showList(list);
}

static PyObject *show_album(PyObject *, PyObject *args)
{
    PyObject *list;
    if (!PyArg_ParseTuple(args, "O", &list))
        return NULL;
    return webvideo->showAlbum(list);
}

static PyObject *download(PyObject *, PyObject *args)
{
    //read args
    PyObject *list;
    const char *childDir = NULL;
    int ok;
    ok = PyArg_ParseTuple(args, "O|s", &list, &childDir);
    if (!ok)
        return NULL;
    int size = PyList_Size(list);
    if (size < 0)
        return NULL;
    //set save dir
    QDir dir(Settings::downloadDir);
    if (childDir)
    {
        QString child = QString::fromUtf8(childDir);
        if (!dir.cd(child))
        {
            dir.mkdir(child);
            dir.cd(child);
        }
    }
    //add task
    PyObject *item;
    const char *str;
    for (int i = 0; i < size; i += 2)
    {
         if ((item = PyList_GetItem(list, i)) == NULL)
             return NULL;
         if ((str = PyString_AsString(item)) == NULL)
             return NULL;
        QString name = QString::fromUtf8(str);
        if ((item = PyList_GetItem(list, i+1)) == NULL)
            return NULL;
        if ((str = PyString_AsString(item)) == NULL)
            return NULL;
        QString url = QString::fromUtf8(str);
        downloader->addTask(url, dir.filePath(name), (bool) childDir);
    }
    QMessageBox::information(webvideo, "Message", "Add task successfully.");
    Py_IncRef(Py_None);
    return Py_None;
}

static PyObject *play(PyObject *, PyObject *args)
{
    PyObject *list;
    if (!PyArg_ParseTuple(args, "O", &list))
        return NULL;
    int size = PyList_Size(list);
    if (size < 0)
        return NULL;
    PyObject *item;
    const char *str;
    for (int i = 0; i < size; i += 2)
    {
         if ((item = PyList_GetItem(list, i)) == NULL)
             return NULL;
         if ((str = PyString_AsString(item)) == NULL)
             return NULL;
        QString name = QString::fromUtf8(str);
        if ((item = PyList_GetItem(list, i+1)) == NULL)
            return NULL;
        if ((str = PyString_AsString(item)) == NULL)
            return NULL;
        QString url = QString::fromUtf8(str);
        if (i == 0)
            moonplaylist->addFileAndPlay(name, url);
        else {
            moonplaylist->addFile(name, url);
        }
    }
    Py_IncRef(Py_None);
    return Py_None;
}

/*******************
 ** Define module **
 *******************/

static PyMethodDef methods[] = {
    {"get_url",          get_url,    METH_VARARGS, "Get url"},
    {"warn",             warn,       METH_VARARGS, "Show warning message"},
    {"question",       question,  METH_VARARGS, "Show a question dialog"},
    {"show_list",       show_list, METH_VARARGS, "Show searching result on the list"},
    {"show_album", show_album, METH_VARARGS, "Show album result on the list"},
    {"set_list_item_color", set_list_item_color, METH_VARARGS, "Set the color of list items"},
    {"download",  download,  METH_VARARGS, "Download file"},
    {"play",      play,      METH_VARARGS, "Play online"},
    {NULL, NULL, 0, NULL}
};

PyObject *apiModule = NULL;

void initAPI()
{
    //init python
    Py_Initialize();
    if (!Py_IsInitialized())
    {
        qDebug("Cannot initialize python.");
        exit(-1);
    }
    //init module
    geturl_obj = new GetUrl(qApp);
    apiModule = Py_InitModule("moonplayer", methods);
    PyModule_AddIntConstant(apiModule, "OPT_QL_HIGH",  OPT_QL_HIGH);
    PyModule_AddIntConstant(apiModule, "OPT_QL_SUPER", OPT_QL_SUPER);
    PyModule_AddIntConstant(apiModule, "OPT_DOWNLOAD",  OPT_DOWNLOAD);
    PyModule_AddStringConstant(apiModule, "final_url", "");
    Py_IncRef(exc_GetUrlError);
    PyModule_AddObject(apiModule, "GetUrlError", exc_GetUrlError);
}
