
#include "basegui.h"

#include "webvideo.h"
#include "moonplaylist.h"

#include "ui_moonbrowser.h"
#include "moonbrowser.h"

MoonBrowser::MoonBrowser(BaseGui *bui)
    : QWidget()
    , mbui(bui)
{
    mui = new Ui::MoonBrowser;
    mui->setupUi(this);

    mwv = mui->widget_2;
    mpl = mui->widget;

    QObject::connect(mpl, SIGNAL(fileSelected(const QString&)),
                     this, SLOT(onFileSelected(const QString&)));
}

MoonBrowser::~MoonBrowser()
{
}

void MoonBrowser::onFileSelected(const QString &url)
{
    qDebug()<<url;
    mbui->openURL(url);
}
