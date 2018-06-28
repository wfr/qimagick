#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QSlider>
#include <QProcess>
#include <QSlider>
#include <QMessageBox>
#include <QScrollBar>

#include "parameterwidget.h"

/*
 * TODO: Lock scroll bars
 */

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    effects.append(Effect("Brightness/Contrast")
                   .addIntParameter("Brightness", -100, 100, 0)
                   .addIntParameter("Contrast", -100, 100, 0)
                   .setCommandFormat(QStringList({"-brightness-contrast", "${Brightness}x${Contrast}"})));

    effects.append(Effect("Gamma")
                   .addFloatParameter("Gamma", 0.1, 10.0, 1.0)
                   .setCommandFormat(QStringList({"-level", "0%,100%,${Gamma}"})));

    effects.append(Effect("Modulate")
                   .addIntParameter("Hue", 0, 200, 100)
                   .addIntParameter("Saturation", 0, 200, 100)
                   .addIntParameter("Lightness", 0, 200, 100)
                   .setCommandFormat(QStringList({"-modulate", "${Lightness},${Saturation},${Hue}"})));

    effects.append(Effect("Radial Blur")
                   .addIntParameter("Angle", 0, 360, 90)
                   .setCommandFormat(QStringList({"-radial-blur", "${Angle}"})));

    effects.append(Effect("Level")
                   .addIntParameter("Black point", 0, 99, 0)
                   .addIntParameter("White point", 1, 100, 100)
                   .setCommandFormat(QStringList({"-level", "${Black point}%,${White point}%"})));

    effects.append(Effect("Normalize")
                   .setCommandFormat(QStringList({"-normalize"})));

    effects.append(Effect("Equalize")
                   .setCommandFormat(QStringList({"-equalize"})));

    effects.append(Effect("Adaptive Blur")
                   .addIntParameter("Radius", 1, 100, 5)
                   .addFloatParameter("Sigma", 0.1, 50, 1.0)
                   .setCommandFormat(QStringList({"-adaptive-blur", "${Radius}x${Sigma}"})));

    effects.append(Effect("Adaptive Sharpen")
                   .addIntParameter("Radius", 1, 100, 5)
                   .addFloatParameter("Sigma", 0.1, 50, 1.0)
                   .setCommandFormat(QStringList({"-adaptive-sharpen", "${Radius}x${Sigma}"})));

    effects.append(Effect("Blur")
                   .addIntParameter("Radius", 1, 100, 5)
                   .addFloatParameter("Sigma", 0.1, 50, 1.0)
                   .setCommandFormat(QStringList({"-blur", "${Radius}x${Sigma}"})));

    effectListModel = new EffectListModel(this);
    ui->listViewEffects->setModel(effectListModel);

    QMenu* menu = ui->menuBar->addMenu("Effects");
    for(const Effect& effect : effects) {
        QAction* action = menu->addAction(effect.name);
        connect(action, &QAction::triggered, this, [action, effect, this]() {
            QModelIndex i = effectListModel->addEffect(effect);
            ui->listViewEffects->setCurrentIndex(i);
            onEffectSelect(i);
            updateEffects();
        });
    }

    connect(ui->listViewEffects, &QListView::entered, this, &MainWindow::onEffectSelect);
    connect(ui->listViewEffects, &QListView::clicked, this, &MainWindow::onEffectSelect);

    connect(ui->btnEffectUp, &QToolButton::clicked, this, [=]() {
        QModelIndex src = ui->listViewEffects->currentIndex();
        if (src.isValid() && src.row() > 0) {
            QModelIndex dst = effectListModel->index(src.row() - 1);
            effectListModel->moveRow(QModelIndex(), src.row(), QModelIndex(), dst.row() );
            ui->listViewEffects->setCurrentIndex(dst);
            updateEffects();
        }
    });

    connect(ui->btnEffectDown, &QToolButton::clicked, this, [=]() {
        QModelIndex src = ui->listViewEffects->currentIndex();
        if (src.isValid() && src.row() < effectListModel->rowCount() - 1) {
            QModelIndex dst = effectListModel->index(src.row() + 1);
            effectListModel->moveRow(QModelIndex(), src.row(), QModelIndex(), dst.row() );
            ui->listViewEffects->setCurrentIndex(dst);
            updateEffects();
        }
    });

    connect(ui->btnEffectRemove, &QToolButton::clicked, this, [=]() {
        QModelIndex idx = ui->listViewEffects->currentIndex();
        if (idx.isValid()) {
            effectListModel->removeRow(idx.row());
            onEffectSelect(ui->listViewEffects->currentIndex());
            updateEffects();
        }
    });

    connect(&process,
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this,
            &MainWindow::onProcessFinished);


    connect(ui->actionOpen, &QAction::triggered, [=]() {
        QString path = QFileDialog::getOpenFileName(
                    this,
                    tr("Open image"),
                    QString(),
                    tr("Images (*.jpg, *.png)"));
        if (QFileInfo::exists(path)) {
            loadImage(path);
        }
    });

    connect(ui->gvOriginal->horizontalScrollBar(), &QScrollBar::valueChanged, this, [=](int value) {
       ui->gvPreview->horizontalScrollBar()->setValue(value);
    });
    connect(ui->gvOriginal->verticalScrollBar(), &QScrollBar::valueChanged, this, [=](int value) {
       ui->gvPreview->verticalScrollBar()->setValue(value);
    });
    connect(ui->gvPreview->horizontalScrollBar(), &QScrollBar::valueChanged, this, [=](int value) {
       ui->gvOriginal->horizontalScrollBar()->setValue(value);
    });
    connect(ui->gvPreview->verticalScrollBar(), &QScrollBar::valueChanged, this, [=](int value) {
       ui->gvOriginal->verticalScrollBar()->setValue(value);
    });

    origScene = new QGraphicsScene();
    ui->gvOriginal->setScene(origScene);
    previewScene = new QGraphicsScene();
    ui->gvPreview->setScene(previewScene);

    auto exampleFile = new QTemporaryFile("qimagick.example");
    exampleFile->setAutoRemove(true);
    exampleFile->open();
    qDebug() << "Example image:" << exampleFile->fileName();
    QFile lenna(":/examples/Lenna.png");
    lenna.open(QIODevice::ReadOnly);
    exampleFile->write(lenna.readAll());

    tempFile = new QTemporaryFile("qimagick.preview");
    tempFile->setAutoRemove(true);
    tempFile->open();
    qDebug() << "Preview image:" << tempFile->fileName();

    loadImage(exampleFile->fileName());
}

void MainWindow::onEffectSelect(const QModelIndex& idx) {
    for (QObject* obj : ui->frameParameters->children()) {
        delete obj;
    }

    if (idx.isValid()) {
//        QString name = effectListModel->data(idx, Qt::DisplayRole).toString();
        Effect* effect = effectListModel->getEffect(idx);
        auto pw = new ParameterWidget(ui->frameParameters, effect);
        connect(pw, &ParameterWidget::valueChanged, this, [=]() {
            this->updateEffects();
        });
    }
}

MainWindow::~MainWindow()
{
    delete tempFile;
    delete ui;
}

void MainWindow::updateEffects() {
    QStringList args;
    for (int i = 0; i < effectListModel->rowCount(); i++) {
        Effect* effect = effectListModel->getEffect(effectListModel->index(i));
        args.append(effect->getCommand());
    }
    ui->lineEditCmd->setText(args.join(' '));
    render(args);
}

void MainWindow::render(const QStringList &args) {
    qDebug() << "render" << args;
    QStringList args_;
    args_ << "-quality" << "1";
    args_ << srcPath;
    args_.append(args);
    args_ << QString("PNG:%1").arg(tempFile->fileName());
    if (process.state() == QProcess::Running) {
        qDebug() << "kill child process";
        process.kill();
        process.waitForFinished();
    }
    process.start("convert", args_);
    ui->statusBar->showMessage("Processing...");
}

void MainWindow::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    ui->statusBar->clearMessage();
    if (exitCode == 9) { // killed
        return;
    } else if (exitCode == 0) {
        previewScene->clear();
        QPixmap preview(tempFile->fileName());
        previewScene->clear();
        previewScene->addPixmap(preview);
        previewScene->update();

    } else {
        QMessageBox msgBox;
        msgBox.setText(QString("Subprocess convert failed with exit code = %1").arg(exitCode));
        msgBox.exec();
        QApplication::exit(1);
    }
}

void MainWindow::loadImage(const QString &path) {
    srcPath = path;
    QPixmap original(path);
    QPixmap preview = original.copy();
    origScene->clear();
    origScene->addPixmap(original);
    previewScene->clear();
    previewScene->addPixmap(preview);
    updateEffects();
}
