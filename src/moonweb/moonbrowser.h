#ifndef _MOONBROWSER_H_
#define _MOONBROWSER_H_

#include <QtCore>
#include <QtGui>

class BaseGui;
class Core;

class WebVideo;
class MoonPlaylist;

namespace Ui {
    class MoonBrowser;
};

class MoonBrowser : public QWidget
{
    Q_OBJECT;
public:
    explicit MoonBrowser(BaseGui *bui, Core *mpcore);
    virtual ~MoonBrowser();

public slots:
    void onFileSelected(const QString &url);
    void onMediaFinished();

private:
    Ui::MoonBrowser *mui = NULL;
    BaseGui *mbui = NULL;
    WebVideo *mwv = NULL;
    MoonPlaylist *mpl = NULL;
    Core *mpcore = NULL;
};

#endif /* _MOONBROWSER_H_ */
