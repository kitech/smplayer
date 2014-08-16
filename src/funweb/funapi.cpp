#include <libguile.h>

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


FunApi::FunApi()
{
    const char * plugin_dir = "./funweb/plugins";
    setenv("GUILE_LOAD_PATH", plugin_dir, 1);
}

FunApi::~FunApi()
{

}





