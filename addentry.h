#ifndef ADDENTRY_H
#define ADDENTRY_H

#include <QWidget>
#include <QDialog>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QFileDialog>

/**
 * This is for me
 * 1) Name
 * 2) Exec (command) (will have a file dialog to select the executable)
 * 3) Icon (file dialog)
 * 4) keywords (seperated by ;)
 * 5) Type
 * 6) Working Directory
 */

namespace Ui
{
    class AddEntry;
}

struct RowContext
{
    int index;
    QString name;
    QString defaultText;
    QString helperText;
    //    QLineEdit *lineEdit;
    bool hasDialog;

    RowContext(int _index, QString _name, QString _defaultText = "", QString _helperText = "", bool _hasDialog = false) : index{_index}, name{qMove(_name)}, defaultText(qMove(_defaultText)), helperText(qMove(_helperText)), hasDialog{_hasDialog}
    {
    }
};

struct DesktopEntryContext
{
    QString filename {};
    QVector<RowContext> rowStructs {};
    bool isEdit = false;
};

class AddEntry : public QDialog
{
    Q_OBJECT

public:
    explicit AddEntry(QWidget *parent = nullptr);
    AddEntry(DesktopEntryContext desktopEntryContext, QWidget *parent = nullptr);
    ~AddEntry();

private:
    Ui::AddEntry *ui;

    static constexpr int width_ = 450;
    static constexpr int height_ = 600;

    QVBoxLayout *vboxLayout = nullptr;
    QWidget *mainWidget = nullptr;

    QVector<QPair<QString, QLineEdit *>> entryFields{};
    QVector<RowContext> rowContexts{};

    QFrame *dialogButtons = nullptr;
    QString desktopEntryString{};

    QString filename {};
    bool isEdit {};


    // Methods
    void Init();
    void initStaticFields();

    void fileHandler(const QString &item, const int &index);
    void displayFields();
    void initDialogButtonsFrame();

    void onSavePressed();
    void createDesktopEntryString();
    QString getFileName(const QString &name);
    QString getNewFileName(const QString &fileName);
};

#endif // ADDENTRY_H
