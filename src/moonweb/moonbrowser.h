#ifndef _MOONBROWSER_H_
#define _MOONBROWSER_H_

#include <QtCore>
#include <QtGui>

class BaseGui;
class Core;
class Playlist;

class WebVideo;
class MoonPlaylist;

namespace Ui {
    class MoonBrowser;
};

class MoonBrowser : public QWidget
{
    Q_OBJECT;
public:
    explicit MoonBrowser(BaseGui *bui, Core *mpcore, Playlist *mplc);
    virtual ~MoonBrowser();

public slots:
    void onFileSelected(const QString &url);
    void onMediaFinished();

private:
    Ui::MoonBrowser *mui = NULL;
    BaseGui *mbui = NULL;
    WebVideo *mwv = NULL;
    MoonPlaylist *mplm = NULL;
    Core *mpcore = NULL;
    Playlist *mplc = NULL;
};

#endif /* _MOONBROWSER_H_ */
