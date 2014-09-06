#include <libguile.h>
#include <QJsonDocument>

#include "funapi.h"

#define SCM2C(sym) scm_variable_ref(scm_c_lookup(sym))

static void * hehe(void * abc)
{
    SCM u8loc = scm_from_locale_string("en_US.UTF-8");
    SCM lc_all = scm_variable_ref(scm_c_lookup("LC_ALL"));
    SCM lc_ctype = SCM2C("LC_CTYPE");

    scm_setlocale(lc_all, u8loc);

    // setenv("GUILE_LOAD_PATH", "/home/gzleo/opensource/smplayer-mix/src/funweb/plugins", 1);
    //scm_c_eval_string("(add-to-load-path \"/home/gzleo/opensource/smplayer-mix/src/funweb/plugins\")");
    
    scm_primitive_load_path(scm_from_utf8_string("plugin_test.scm"));

    SCM a;
    scm_is_integer(a);

    SCM fun;
    fun = scm_variable_ref(scm_c_lookup("scm_hehe"));
    // fun = scm_from_locale_symbol("scm_hehe"); // wrong way
    scm_call_0(fun);

}

static void test_link_libguile()
{
    scm_with_guile(hehe, 0);
}


void m_hehe(void *abc)
{
    setenv("GUILE_LOAD_PATH", "./funweb/plugins", 1);

    // hehe(abc);
    scm_with_guile(hehe, 0);

    /*
    scm_init_guile();
    scm_init_guile();
    scm_init_guile();
    scm_init_guile();
    scm_init_guile();
    scm_init_guile();

    SCM a;
    scm_is_integer(a);
    scm_c_eval_string("(add-to-load-path \"/home/gzleo/opensource/smplayer-mix/src/funweb/plugins\")");
    scm_c_eval_string("(add-to-load-path \"/home/gzleo/opensource/smplayer-mix/src/funweb/plugins\")");
    scm_c_eval_string("(add-to-load-path \"/home/gzleo/opensource/smplayer-mix/src/funweb/plugins\")");
    scm_c_eval_string("(add-to-load-path \"/home/gzleo/opensource/smplayer-mix/src/funweb/plugins\")");
    scm_c_eval_string("(add-to-load-path \"/home/gzleo/opensource/smplayer-mix/src/funweb/plugins\")");

    printf("abcccc\n");
    */
}

static void *guile_runner(void *data)
{
    FunApi *api = (FunApi*)data;

    SCM u8loc = scm_from_locale_string("en_US.UTF-8");
    SCM lc_all = scm_variable_ref(scm_c_lookup("LC_ALL"));
    SCM lc_ctype = SCM2C("LC_CTYPE");

    scm_setlocale(lc_all, u8loc);

    scm_primitive_load_path(scm_from_utf8_string("plugin_youku.scm"));

    char *c_ret = NULL;

    SCM f_search = SCM2C("search");
    SCM f_youget = SCM2C("get_you_url");
    SCM f_list_link = SCM2C("list_links");
    SCM f_ret;
    SCM f_args;

    if (api->m_cmd == "youget") {
        f_args = scm_from_locale_string(api->m_args.toLatin1().data());
        f_ret = scm_call_1(f_youget, f_args);
    } else if (api->m_cmd == "search") {
        qDebug()<<api->m_args<<api->m_args.length();
        f_args = scm_from_locale_string(QUrl::toPercentEncoding(api->m_args).data());
        SCM f_args2 = scm_from_locale_string("1");
        f_ret = scm_call_2(f_search, f_args, f_args2);
    }

    if (scm_is_string(f_ret)) {
        qDebug()<<"is string...";
        c_ret = scm_to_locale_string(f_ret);
    } else {
        qDebug()<<"is not string";
    }

    return c_ret;
}

FunApi::FunApi()
    : QThread()
{
    const char * plugin_dir = "./funweb/plugins";
    setenv("GUILE_LOAD_PATH", plugin_dir, 1);
    setenv("GUILE_AUTO_COMPILE", "0", 1);
    setenv("GUILE_WARN_DEPRECATED", "detailed", 1);

    QObject::connect(this, &QThread::finished, this, &FunApi::onFinished);
}

FunApi::~FunApi()
{
    
}

void FunApi::run()
{
    void *ret = scm_with_guile(guile_runner, this);
    qDebug()<<ret<<QString((const char*)ret);
    QByteArray bret = QByteArray((const char*)ret);
    bret.replace("'", "\""); // json need "
    QJsonDocument jret = QJsonDocument::fromJson(bret);
    qDebug()<<jret;    

    m_ret = jret;
}

void FunApi::search(QString keywords)
{
    this->m_cmd = "search";
    this->m_args = keywords;

    this->start();
}

void FunApi::geturl(QString vid)
{
    this->m_cmd = "youget";
    this->m_args = vid;

    this->start();
}

void FunApi::onFinished()
{
    if (this->m_cmd == "search") {
        emit this->searchDone(m_ret);
    } else if (this->m_cmd == "youget") {
        emit this->geturlDone(m_ret);
    } else {
        qDebug()<<"unknown cmd:"<<m_cmd;
    }
}
