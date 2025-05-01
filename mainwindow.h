#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QActionGroup>
#include <QTranslator>
#include <QTableWidget>
#include "about.h"
#include "new.h"
#include "open.h"
#include "add.h"
#include "edit.h"
#include "delete.h"
#include "traversalposition.h"
#include "menushadow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    About *aboutWindow;
    New *newWindow;
    Open *openWindow;
    Add *addWindow;
    Edit *editWindow;
    Delete *deleteWindow;
    QSettings *configFile;
    QActionGroup *languageActionGroup;
    QTranslator translator;
    QTranslator translatorWidgets;
    enum QLocale::Language defaultLanguage;
    int widgetSide;
    QPoint widgetMousePressPos;
    int widgetPressHeight;
    enum class widgetTopBottomMargin
    {
        none,
        topMargin,
        bottomMargin
    };
    enum widgetTopBottomMargin widgetMouseMargin;
    enum TraversalPosition::positionEnum traversalPosition;
    QString traversalKey;
    int traversalNumber;
    bool traversalReverse;
    int tableColumnWidth1;
    int tableColumnWidth2;
    QMenu *tableWidgetTraversalMenu;
    QList<MenuShadow*> menuShadowList;
    bool editable;
    bool copiable;
    bool deletable;

    void languageInit();
    void translateLanguage(enum QLocale::Language language);
    void menuShadow(QMenu *menu);
    void menuShadow(const QList<QAction*> actionList);
    void setMenuShadow(QMenu *menu);
    void menubarResizeMenu();
    void traversal(bool initTableColumnWidth);
    void tableEditRow(int row);
    void tableDeleteSelection();
    QRect widgetMapToGlobalRect(const QWidget *widget) const;
    QRect mapToGlobalRect(const QWidget *widget, const QRect &rect) const;
    QRect mapFromGlobalRect(const QWidget *widget, const QRect &rect) const;
    QRect widgetMapToWidgetRect(const QWidget *source, const QWidget *target) const;

private slots:
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionClose_triggered();
    void on_actionAdd_triggered();
    void on_actionEdit_triggered();
    void on_actionCopy_triggered();
    void on_actionDelete_triggered();
    void on_actionChinese_triggered();
    void on_actionEnglish_triggered();
    void on_actionAbout_triggered();
    void on_actionQuit_triggered();

    void on_pushButtonTraversalShow_clicked();

    void on_pushButtonInfoExplore_clicked();

    void on_pushButtonRefresh_clicked();

    void on_pushButtonPageUp_clicked();

    void on_pushButtonPageDown_clicked();

public slots:
    void slotOpened(const QString &directory);
    void slotClose();
    void slotBusy(bool busy);
    void slotTableHorizontalHeaderComplete(bool initTableColumnWidth);
    void slotTraversalComplete(bool initTableColumnWidth);
    void slotTableColumnWidth(int columnIndex, int oldWidth, int newWidth);
    void slotTableCurrentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);
    void slotTableItemSelectionChanged();
    void slotTableItemDoubleClicked(QTableWidgetItem *item);
    void slotTableContextMenu(const QPoint &pos);
    void slotInfo(const QString &message);

signals:
    void signalTableWidget(QTableWidget *table);
    void signalClose(bool info);
    void signalTraversal(enum TraversalPosition::positionEnum *position, const QString &key, int number, bool reverse, bool initTableColumnWidth, bool info);
    void signalRefresh(bool info);
    void signalPageUp(bool info);
    void signalPageDown(bool info);

    // QObject interface
public:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
};

#endif // MAINWINDOW_H
