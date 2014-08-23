#ifndef TRANSFORMER_H
#define TRANSFORMER_H

#include <QWidget>
#include <QTreeWidgetItem>
class QProcess;

namespace Ui {
class Transformer;
}

class TransformerItem : public QTreeWidgetItem
{
public:
    TransformerItem(QTreeWidget *view, const QString &file, const QString &outfile);
    TransformerItem(QTreeWidget *view, const QStringList &files, const QString &outfile);
    QStringList files;
    QString file;
    QString outfile;
};

class Transformer : public QWidget
{
    Q_OBJECT
    
public:
    explicit Transformer(QWidget *parent = 0);
    bool hasTask(void);
    ~Transformer();
    
private:
    Ui::Transformer *ui;
    QTimer *timer;
    QProcess *process;
    QStringList args;
    TransformerItem *current;
    int current_rest;
    int prev_percentage;
    void start(TransformerItem *item);

private slots:
    void onAddButton(void);
    void onDelButton(void);
    void onStartButton(void);
    void onFinished(int status);
    void readOutput(void);
};

#endif // TRANSFORMER_H
