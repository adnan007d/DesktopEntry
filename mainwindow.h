#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFrame>
#include <QScrollArea>
#include <addentry.h>
#include "flowlayout.h"




QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void Init();
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QFrame *frame = nullptr;
    FlowLayout *flowLayout = nullptr;
    QScrollArea *scrollArea = nullptr;
    QWidget *scrollAreaWidget = nullptr;
    //    AddEntry *A = nullptr;
    void createCard(const DesktopEntryContext &context);
    void resizeEvent(QResizeEvent *event) override;
    QStringList getDesktopEntryFilesNames();
    QVector<RowContext> parseDesktopEntry(const QString &file);
    void renderCards();
    QPixmap getIcon(const DesktopEntryContext &cardContext);
    static constexpr int _iconIndex = 4;
    static constexpr int _nameIndex = 0;

    void deleteEntry(const QString &filename, const QString &name);
};
#endif // MAINWINDOW_H
