

#include <QPushButton>
#include <QFrame>
#include <QDebug>
#include <QScreen>
#include <QLabel>
#include <QSettings>
#include <QImageReader>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "flowlayout.h"
#include "addentry.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setGeometry(0, 0, 600, 500);

    move(screen()->geometry().center() - frameGeometry().center());

    Init();
    renderCards();
}

void MainWindow::Init()
{
    qDebug() << "Inside";
    frame = new QFrame(this);
    scrollArea = new QScrollArea(frame);

    scrollAreaWidget = new QWidget();

    scrollArea->setWidgetResizable(true);

    flowLayout = new FlowLayout(scrollAreaWidget);

    scrollArea->setWidget(scrollAreaWidget);

    setWindowTitle(tr("Flow Layout"));
}

void MainWindow::renderCards()
{
    QLayoutItem *item;
    while ((item = flowLayout->takeAt(0)))
    {
        delete item->widget();
        delete item;
    }

    QStringList desktopFiles = getDesktopEntryFilesNames();

    createCard({.filename = "", .rowStructs = {}});
    for (const auto &file : desktopFiles)
    {
        const auto rowStructs = parseDesktopEntry(file);
        createCard({.filename = file, .rowStructs = rowStructs, .isEdit = true});
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createCard(DesktopEntryContext cardContext)
{

    QFrame *card = new QFrame(scrollAreaWidget);

    card->setFrameStyle(QFrame::StyledPanel | QFrame::Shadow(QFrame::StyledPanel));

    QVBoxLayout *cardLayout = new QVBoxLayout(card);

    const QString iconFile = cardContext.rowStructs.size() > 0 ? cardContext.rowStructs.at(_iconIndex).defaultText : "";

    QImageReader reader;
    reader.setAllocationLimit(20 * (10 * 10 * 10));

    if (QFileInfo::exists(iconFile))
        reader.setFileName(iconFile);
    else
        reader.setFileName("/home/barty/Pictures/wallpaper/thighs.png");
    QPixmap p = QPixmap();
    p.convertFromImage(reader.read());

    QLabel *addIcon = new QLabel(card);
    addIcon->setFixedSize(125, 150);

    addIcon->setPixmap(p.scaled(addIcon->width(), addIcon->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    QHBoxLayout *actionButtons = new QHBoxLayout();

    QPushButton *addButton = nullptr;
    if (cardContext.isEdit)
    {
        addButton = new QPushButton(card);
        addButton->setIcon(QIcon::fromTheme("edit"));
        connect(addButton, &QPushButton::clicked, [=]()
                { AddEntry *A = new AddEntry(cardContext, this); A->exec(); renderCards(); });
        QPushButton *deleteButton = new QPushButton(card);
        deleteButton->setIcon(QIcon::fromTheme("delete"));
        connect(deleteButton, &QPushButton::clicked, [=]()
                { qDebug() << "Delete Clicked"; renderCards(); });
        actionButtons->addWidget(deleteButton);
    }
    else
    {
        addButton = new QPushButton(card);
        addButton->setIcon(QIcon::fromTheme("add"));
        connect(addButton, &QPushButton::clicked, [=]()
                { AddEntry *A = new AddEntry(this); A->exec(); renderCards(); });
    }

    cardLayout->addWidget(addIcon);
    cardLayout->addLayout(actionButtons);
    actionButtons->insertWidget(0, addButton);

    flowLayout->addWidget(card);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    const int width = this->width();
    const int height = this->height();

    qDebug() << "Central Widget " << centralWidget()->width() << "x" << centralWidget()->height();
    qDebug() << "MainWindow " << width << 'x' << height;

    frame->setGeometry(0, 0, width, height);
    scrollArea->setGeometry(0, 0, width, height);

    qDebug() << "Frame " << frame->width() << 'x' << frame->height();
    qDebug() << "ScrollArea " << scrollArea->width() << 'x' << scrollArea->height();
    qDebug() << "ScrollAreaWidget " << scrollAreaWidget->width() << 'x' << scrollAreaWidget->height();

    //    qDebug() << scrollArea->width() << " " << scrollArea->height();
}

QStringList MainWindow::getDesktopEntryFilesNames()
{
    QDir dir = QDir("");
    dir.setNameFilters(QStringList() << "DN-*.desktop");
    QFileInfoList fileList = dir.entryInfoList(QDir ::Filter ::NoDotAndDotDot | QDir::Filter ::AllEntries);
    QStringList files;
    for (QFileInfo &f : fileList)
    {
        qDebug() << f;
        if (f.isFile())
        {
            files.push_back(f.absoluteFilePath());
        }
    }
    return files;
}

QVector<RowContext> MainWindow::parseDesktopEntry(const QString &file)
{
    QSettings settings(file, QSettings::IniFormat);
    settings.beginGroup("Desktop Entry");
    QVector<RowContext> rowStructs;
    rowStructs.emplace_back(
        0,
        "Name",
        settings.value("Name").toString());
    rowStructs.emplace_back(
        1,
        "Type",
        settings.value("Type").toString());
    rowStructs.emplace_back(
        2,
        "Keywords",
        settings.value("Keywords").toString(),
        "( Seperated by ';' )");
    rowStructs.emplace_back(
        3,
        "Exec",
        settings.value("Exec").toString(),
        "",
        true);
    rowStructs.emplace_back(
        4,
        "Icon",
        settings.value("Icon").toString(),
        "",
        true);
    settings.endGroup();

    return rowStructs;
}
