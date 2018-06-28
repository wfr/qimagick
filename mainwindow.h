#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTemporaryFile>
#include <QStringListModel>
#include <QGraphicsScene>
#include <QStandardItemModel>
#include <QProcess>

#include "effect.h"
#include "effectlistmodel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void onEffectSelect(const QModelIndex& idx);

    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    Ui::MainWindow *ui;

    QString srcPath;
    QGraphicsScene *origScene, *previewScene;

    QProcess process;
    QTemporaryFile *tempFile;

    QList<Effect> effects;
    EffectListModel *effectListModel;

    void loadImage(const QString& path);

    void updateEffects();
    void render(const QStringList& args);
};

#endif // MAINWINDOW_H
