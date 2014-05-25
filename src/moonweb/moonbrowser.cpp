
#include "core.h"
#include "playlist.h"
#include "basegui.h"

#include "webvideo.h"
#include "moonplaylist.h"

#include "ui_moonbrowser.h"
#include "moonbrowser.h"

MoonBrowser::MoonBrowser(BaseGui *bui, Core *mpcore, Playlist *mplc)
    : QWidget()
    , mbui(bui)
    , mpcore(mpcore)
    , mplc(mplc)
{
    mui = new Ui::MoonBrowser;
    mui->setupUi(this);

    mwv = mui->widget_2;
    mplm = mui->widget;

    QObject::connect(mplm, SIGNAL(fileSelected(const QString&)),
                     this, SLOT(onFileSelected(const QString&)));

    /*
    QObject::connect(mpcore, SIGNAL(mediaFinished()),
                     this, SLOT(onMediaFinished()));
    */

    QObject::connect(mplc, SIGNAL(playlistEnded()),
                     this, SLOT(onMediaFinished()));
}

MoonBrowser::~MoonBrowser()
{
}

void MoonBrowser::onFileSelected(const QString &url)
{
    qDebug()<<url;
    mbui->openURL(url);
}

void MoonBrowser::onMediaFinished()
{
    qDebug()<<"hehehehe"<<sender();
    mplm->playNext();
    // QTimer::singleShot(500, mplm, SLOT(playNext()));
}

