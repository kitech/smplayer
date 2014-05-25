
#include "core.h"
#include "basegui.h"

#include "webvideo.h"
#include "moonplaylist.h"

#include "ui_moonbrowser.h"
#include "moonbrowser.h"

MoonBrowser::MoonBrowser(BaseGui *bui, Core *mpcore)
    : QWidget()
    , mbui(bui)
    , mpcore(mpcore)
{
    mui = new Ui::MoonBrowser;
    mui->setupUi(this);

    mwv = mui->widget_2;
    mpl = mui->widget;

    QObject::connect(mpl, SIGNAL(fileSelected(const QString&)),
                     this, SLOT(onFileSelected(const QString&)));

    QObject::connect(mpcore, SIGNAL(mediaFinished()),
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
}

