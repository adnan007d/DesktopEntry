#include "addentry.h"
#include "ui_addentry.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QMessageBox>

AddEntry::AddEntry(QWidget *parent) : QDialog(parent),
                                      ui(new Ui::AddEntry)
{
    ui->setupUi(this);
    initStaticFields();
    Init();
}

AddEntry::AddEntry(DesktopEntryContext desktopEntryContext, QWidget *parent) : QDialog{parent}, ui(new Ui::AddEntry)
{
    ui->setupUi(this);
    rowContexts = desktopEntryContext.rowStructs;
    filename = desktopEntryContext.filename;
    isEdit = desktopEntryContext.isEdit;
    Init();
}

void AddEntry::Init()
{
    mainWidget = new QWidget(this);

    setFixedSize(width_, height_);
    mainWidget->setGeometry(0, 0, width_, height_);
    initDialogButtonsFrame();
    displayFields();
}

void AddEntry::initStaticFields()
{
    rowContexts.emplaceBack(0, "Name", "");
    rowContexts.emplaceBack(1, "Type", "");
    rowContexts.emplaceBack(2, "Keywords", "", "( Seperated by ';' )");
    rowContexts.emplaceBack(3, "Exec", "", "", true);
    rowContexts.emplaceBack(4, "Icon", "", "", true);

    for (const auto &row : rowContexts)
    {
        qDebug() << row.index << " " << row.name << " " << row.helperText << " " << row.hasDialog << "\n";
    }
}

void AddEntry::fileHandler(const QString &item, const int &index)
{
    QString filename = QFileDialog ::getOpenFileName(
        this,
        "Select " + item + " file",
        QDir ::homePath(),
        "All files (*.*)");

    if (!filename.isNull())
    {
        entryFields.at(index).second->setText(filename);
    }
    else
    {
        // Error Handling
        //        exit(1);
    }
}

void AddEntry::displayFields()
{
    vboxLayout = new QVBoxLayout(mainWidget);
    for (const auto &field : rowContexts)
    {
        // Row Widget With HBoxLayout
        QWidget *row = new QWidget(mainWidget);
        QHBoxLayout *hboxLayout = new QHBoxLayout(row);

        // Label
        hboxLayout->addWidget(new QLabel(field.name + " " + field.helperText, row));

        // Text Field
        QLineEdit *lineEdit = new QLineEdit(field.defaultText, row);
        hboxLayout->addWidget(lineEdit, 1);

        // Checking if the field should have a file dialog to choose files
        if (field.hasDialog)
        {
            QPushButton *fileDialogButton = new QPushButton(tr("?"), row);
            hboxLayout->addWidget(fileDialogButton);
            connect(fileDialogButton, &QPushButton::clicked, [=]()
                    { fileHandler(field.name, field.index); });
        }

        vboxLayout->addWidget(row);

        // Adding to gloabl list of elements
        entryFields.push_back(qMakePair(field.name, lineEdit));
    }

    vboxLayout->addStretch(vboxLayout->count() - 1);
}

void AddEntry::initDialogButtonsFrame()
{
    dialogButtons = new QFrame(this);

    constexpr int height = 40;

    dialogButtons->setGeometry(10, height_ - height, width_ - 20, height);

    QHBoxLayout *hboxlayout = new QHBoxLayout(dialogButtons);

    QPushButton *cancelButton = new QPushButton(tr("cancel"), dialogButtons);
    QPushButton *saveButton = new QPushButton(tr("Save"), dialogButtons);

    hboxlayout->addWidget(cancelButton, 0);
    hboxlayout->addWidget(saveButton, 0);

    connect(cancelButton, &QPushButton::clicked, [=]()
            { this->reject(); });
    connect(saveButton, &QPushButton::clicked, [=]()
            { this->onSavePressed(); });
}

void AddEntry::onSavePressed()
{
    for (const auto &name : entryFields)
    {
        if (name.second->text().isEmpty())
        {
            QMessageBox msg;
            msg.setWindowTitle("All fields are required");
            msg.setText("<b>All Fields are required</b>");
            msg.setTextFormat(Qt::TextFormat::RichText);
            msg.setIcon(QMessageBox ::Critical);
            msg.exec();
            return;
        }
    }

    createDesktopEntryString();

    this->accept();
}

void AddEntry::createDesktopEntryString()
{
    QString _name = "";

    desktopEntryString = "[Desktop Entry]\nVersion=1.0\n";

    for (const auto &[name, lineEdit] : entryFields)
    {
        desktopEntryString += name + "=" + lineEdit->text() + "\n";
        if (name == "Name")
            _name = lineEdit->text().replace(" ", "-");
    }
    desktopEntryString += "Terminal=false\n";

    const QString _fileName = isEdit ? filename : getFileName(_name);

    qDebug() << desktopEntryString << _fileName;

    QFile file(_fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Text);

    QTextStream out(&file);

    out << desktopEntryString;

    file.close();
}

QString AddEntry::getFileName(const QString &name)
{
    // const QString dirPath = QDir::homePath() + "/.local/share/applications/";
    const QString dirPath = "";
    qDebug() << dirPath;
    QString fileName = dirPath + "DN-" + name;

    if (!QFileInfo::exists(fileName + ".desktop"))
        return fileName + ".desktop";

    return getNewFileName(fileName);
}

QString AddEntry::getNewFileName(const QString &fileName)
{
    int i = 1;

    while (true)
    {
        if (QFileInfo::exists(fileName + "-" + QString::number(i) + ".desktop"))
            ++i;
        else
            return fileName + "-" + QString::number(i) + ".desktop";
    }
}

AddEntry::~AddEntry()
{
    delete ui;
}
