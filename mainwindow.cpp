#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QThread>
#include <QVariant>
#include <QGraphicsDropShadowEffect>
#include <QDir>
#include <QDesktopServices>
#include <QScrollBar>
#include <QKeyEvent>
#include <QtMath>
#include <QClipboard>
#include <QDebug>
#include "database.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Config file.
    configFile = new QSettings("./config.ini", QSettings::IniFormat);

    //Window frameless.
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    //Load QSS file.
    QFile qssFile(":/qss/mainwindow.qss");
    if(qssFile.open(QFile::ReadOnly))
    {
        QString qssString = qssFile.readAll();
        //Set style sheet.
        setStyleSheet(qssString);
        qssFile.close();
    }

    //Language action group.
    languageActionGroup = new QActionGroup(this);
    languageActionGroup->addAction(ui->actionChinese);
    languageActionGroup->addAction(ui->actionEnglish);

    //Default English.
    defaultLanguage = QLocale::English;

    //Create child window.
    aboutWindow = new About(this);
    newWindow = new New(this);
    openWindow = new Open(this);
    addWindow = new Add(this);
    editWindow = new Edit(this);
    deleteWindow = new Delete(this);

    //ToolBar action.
    ui->toolBar->addAction(ui->actionNew);
    ui->toolBar->addAction(ui->actionOpen);
    ui->toolBar->addAction(ui->actionClose);
    ui->toolBar->addAction(ui->actionAdd);
    ui->toolBar->addAction(ui->actionEdit);
    ui->toolBar->addAction(ui->actionCopy);
    ui->toolBar->addAction(ui->actionDelete);

    //TableWidget context menu separator.
    QAction *actionTraversalSeparator = new QAction(this);
    actionTraversalSeparator->setSeparator(true);

    //TableWidget traversal context menu.
    ui->tableWidgetTraversal->setContextMenuPolicy(Qt::CustomContextMenu);
    tableWidgetTraversalMenu = new QMenu(this);
    tableWidgetTraversalMenu->addAction(ui->actionNew);
    tableWidgetTraversalMenu->addAction(ui->actionOpen);
    tableWidgetTraversalMenu->addAction(ui->actionClose);
    tableWidgetTraversalMenu->addAction(actionTraversalSeparator);
    tableWidgetTraversalMenu->addAction(ui->actionAdd);
    tableWidgetTraversalMenu->addAction(ui->actionEdit);
    tableWidgetTraversalMenu->addAction(ui->actionCopy);
    tableWidgetTraversalMenu->addAction(ui->actionDelete);

    //Menu shadow widget list.
    menuShadowList = QList<MenuShadow*>();

    //Menu shadow.
    menuShadow(ui->menubar->actions());
    menuShadow(tableWidgetTraversalMenu);

    //Event filter.
    ui->tableWidgetTraversal->installEventFilter(this);

    //Action state.
    editable = false;
    copiable = false;
    deletable = false;

    //Traversal init.
    traversalPosition = TraversalPosition::positionEnum::none;
    traversalKey = QString();
    traversalNumber = 0;
    traversalReverse = false;
    tableColumnWidth1 = 0;
    tableColumnWidth2 = 0;

    qRegisterMetaType<Qt::Orientation>("Qt::Orientation");
    qRegisterMetaType<QVector<int>>("QVector<int>");
    qRegisterMetaType<QItemSelection>("QItemSelection");

    //Data base.
    QThread *thread = new QThread;
    Database *database = new Database;
    database->moveToThread(thread);
    thread->start();

    //Translate init.
    languageInit();

    //Connect signal slot.
    connect(this, SIGNAL(signalTableWidget(QTableWidget*)), database, SLOT(slotTableWidget(QTableWidget*)));
    connect(newWindow, SIGNAL(signalNew(const QString&, bool)), database, SLOT(slotOpen(const QString&, bool)));
    connect(openWindow, SIGNAL(signalOpen(const QString&, bool)), database, SLOT(slotOpen(const QString&, bool)));
    connect(this, SIGNAL(signalClose(bool)), database, SLOT(slotClose(bool)));
    connect(this, SIGNAL(signalTraversal(TraversalPosition::positionEnum*, const QString&, int, bool, bool, bool)), database, SLOT(slotTraversal(TraversalPosition::positionEnum*, const QString&, int, bool, bool, bool)));
    connect(addWindow, SIGNAL(signalAdd(const QString&, const QString&, bool)), database, SLOT(slotAddItem(const QString&, const QString&, bool)));
    connect(editWindow, SIGNAL(signalEdit(const QString&, const QString&, const QString&, const QString&, bool)), database, SLOT(slotEditItem(const QString&, const QString&, const QString&, const QString&, bool)));
    connect(deleteWindow, SIGNAL(signalDelete(const QStringList&, bool)), database, SLOT(slotDeleteItem(const QStringList&, bool)));
    connect(this, SIGNAL(signalRefresh(bool)), database, SLOT(slotRefresh(bool)));
    connect(this, SIGNAL(signalPageUp(bool)), database, SLOT(slotPageUp(bool)));
    connect(this, SIGNAL(signalPageDown(bool)), database, SLOT(slotPageDown(bool)));
    connect(database, SIGNAL(signalOpened(const QString&)), this, SLOT(slotOpened(const QString&)));
    connect(database, SIGNAL(signalClose()), this, SLOT(slotClose()));
    connect(database, SIGNAL(signalBusy(bool)), this, SLOT(slotBusy(bool)));
    connect(database, SIGNAL(signalTableHorizontalHeaderComplete(bool)), this, SLOT(slotTableHorizontalHeaderComplete(bool)));
    connect(database, SIGNAL(signalTraversalComplete(bool)), this, SLOT(slotTraversalComplete(bool)));
    connect(ui->tableWidgetTraversal->horizontalHeader(), SIGNAL(sectionResized(int, int, int)), this, SLOT(slotTableColumnWidth(int, int, int)));
    connect(ui->tableWidgetTraversal, SIGNAL(currentItemChanged(QTableWidgetItem*, QTableWidgetItem*)), this, SLOT(slotTableCurrentItemChanged(QTableWidgetItem*, QTableWidgetItem*)));
    connect(ui->tableWidgetTraversal, SIGNAL(itemSelectionChanged()), this, SLOT(slotTableItemSelectionChanged()));
    connect(ui->tableWidgetTraversal, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(slotTableItemDoubleClicked(QTableWidgetItem*)));
    connect(ui->tableWidgetTraversal, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(slotTableContextMenu(const QPoint&)));
    connect(database, SIGNAL(signalInfo(const QString&)), this, SLOT(slotInfo(const QString&)));

    emit signalTableWidget(ui->tableWidgetTraversal);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->tableWidgetTraversal)
    {
        if(event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            if(keyEvent->key() == Qt::Key_Enter)
            {
                tableEditRow(ui->tableWidgetTraversal->currentIndex().row());
            }
            else if(keyEvent->key() == Qt::Key_Delete)
            {
                tableDeleteSelection();
            }
        }
    }

    return false;
}

//Translate init.
void MainWindow::languageInit()
{
    QVariant language = configFile->value("Option/Language");

    if(language.isValid())
    {
        if(language == "Chinese")
        {
            translateLanguage(QLocale::Chinese);
            ui->actionChinese->setChecked(true);
        }
        else
        {
            translateLanguage(QLocale::English);
            ui->actionEnglish->setChecked(true);
        }
    }
    else
    {
        if(QLocale::system().language() == QLocale::Chinese)
        {
            translateLanguage(QLocale::Chinese);
            ui->actionChinese->setChecked(true);
        }
        else
        {
            translateLanguage(QLocale::English);
            ui->actionEnglish->setChecked(true);
        }
    }
}

//Translate language.
void MainWindow::translateLanguage(enum QLocale::Language language)
{
    if(language == QLocale::Chinese)
    {
        if(translator.load(":/language/zh_cn.qm"))
        {
            qApp->installTranslator(&translator);
        }
        if(translatorWidgets.load(":/language/widgets.qm"))
        {
            qApp->installTranslator(&translatorWidgets);
        }
        if(configFile->value("Option/Language") != "Chinese")
        {
            configFile->setValue("Option/Language", "Chinese");
        }
        defaultLanguage = QLocale::Chinese;
        qDebug() << tr("Language: Chinese");
    }
    else
    {
        qApp->removeTranslator(&translator);
        qApp->removeTranslator(&translatorWidgets);
        if(configFile->value("Option/Language") != "English")
        {
            configFile->setValue("Option/Language", "English");
        }
        defaultLanguage = QLocale::English;
        qDebug() << tr("Language: English");
    }
    ui->retranslateUi(this);

    //Child window translate.
    aboutWindow->translate();
    newWindow->translate();
    openWindow->translate();
    addWindow->translate();
    editWindow->translate();
    deleteWindow->translate();
}

//Menu shadow.
void MainWindow::menuShadow(QMenu *menu)
{
    for(int index = 0; index < menuShadowList.count(); index++)
    {
        if(menuShadowList[index]->menu() == menu)
        {
            break;
        }
        if(index == menuShadowList.count() - 1)
        {
            setMenuShadow(menu);
        }
    }
    if(menuShadowList.isEmpty())
    {
        setMenuShadow(menu);
    }

    menuShadow(menu->actions());
}

//Menu shadow.
void MainWindow::menuShadow(const QList<QAction*> actionList)
{
    for(const QAction *action : actionList)
    {
        if(action->menu() != Q_NULLPTR)
        {
            menuShadow(action->menu());
        }
    }
}

//Set menu shadow widget.
void MainWindow::setMenuShadow(QMenu *menu)
{
    MenuShadow *menuShadow = new MenuShadow(this);
    menuShadow->setMenu(menu);
    menuShadowList << menuShadow;
}

//Menubar resize menu.
void MainWindow::menubarResizeMenu()
{
    for(QAction *action : ui->menubar->actions())
    {
        if(action->menu() != Q_NULLPTR)
        {

        }
    }
}

//Table edit row popup.
void MainWindow::tableEditRow(int row)
{
    if(!editable)
    {
        return;
    }

    QTableWidgetItem *key = ui->tableWidgetTraversal->item(row, 0);
    QTableWidgetItem *value = ui->tableWidgetTraversal->item(row, 1);
    if(key == Q_NULLPTR || value == Q_NULLPTR)
    {
        return;
    }
    editWindow->setEdit(key->text(), value->text());
    editWindow->exec();
}

//Table delete selection popup.
void MainWindow::tableDeleteSelection()
{
    if(!deletable)
    {
        return;
    }

    QModelIndexList indexList = ui->tableWidgetTraversal->selectionModel()->selectedRows();
    QStringList keyList;
    for(int index = 0; index < indexList.count(); index++)
    {
        keyList << indexList[index].data().toString();
    }

    deleteWindow->setDelete(keyList);
    deleteWindow->exec();
}

//Show traversal.
void MainWindow::on_pushButtonTraversalShow_clicked()
{
    traversal(true);
}

//Refresh.
void MainWindow::on_pushButtonRefresh_clicked()
{
    emit signalRefresh(true);
}

//Page up.
void MainWindow::on_pushButtonPageUp_clicked()
{
    emit signalPageUp(true);
}

//Page down.
void MainWindow::on_pushButtonPageDown_clicked()
{
    emit signalPageDown(true);
}

//Explore directory.
void MainWindow::on_pushButtonInfoExplore_clicked()
{
    if(ui->lineEditInfoDirectory->text().isEmpty())
    {
        return;
    }

    QDesktopServices::openUrl(QUrl(ui->lineEditInfoDirectory->text()));
}

//New popup.
void MainWindow::on_actionNew_triggered()
{
    newWindow->setTraversalNumber(ui->spinBoxTraversalNumber->value());
    newWindow->exec();
}

//Open popup.
void MainWindow::on_actionOpen_triggered()
{
    openWindow->setTraversalNumber(ui->spinBoxTraversalNumber->value());
    openWindow->exec();
}

//Close popup.
void MainWindow::on_actionClose_triggered()
{
    emit signalClose(true);
}

//Add popup.
void MainWindow::on_actionAdd_triggered()
{
    addWindow->exec();
}

//Edit action.
void MainWindow::on_actionEdit_triggered()
{
    tableEditRow(ui->tableWidgetTraversal->currentIndex().row());
}

//Copy action.
void MainWindow::on_actionCopy_triggered()
{
    if(!copiable)
    {
        return;
    }

    QTableWidgetItem *currentItem = ui->tableWidgetTraversal->currentItem();
    if(currentItem == Q_NULLPTR)
    {
        return;
    }
    QApplication::clipboard()->setText(currentItem->text());
}

//Delete popup.
void MainWindow::on_actionDelete_triggered()
{
    tableDeleteSelection();
}

//Chinese select.
void MainWindow::on_actionChinese_triggered()
{
    if(defaultLanguage != QLocale::Chinese)
    {
        translateLanguage(QLocale::Chinese);
    }
}

//English select.
void MainWindow::on_actionEnglish_triggered()
{
    if(defaultLanguage != QLocale::English)
    {
        translateLanguage(QLocale::English);
    }
}

//About popup.
void MainWindow::on_actionAbout_triggered()
{
    aboutWindow->show();
}

//Menu action quit.
void MainWindow::on_actionQuit_triggered()
{
    QApplication::quit();
}

//Close database.
void MainWindow::slotClose()
{
    ui->pushButtonTraversalShow->setEnabled(false);
    ui->pushButtonInfoExplore->setEnabled(false);
    ui->lineEditInfoDirectory->clear();
    ui->actionClose->setEnabled(false);
    ui->actionAdd->setEnabled(false);
    ui->actionEdit->setEnabled(false);
    ui->actionCopy->setEnabled(false);
    ui->actionDelete->setEnabled(false);
    editable = false;
    copiable = false;
    deletable = false;
    ui->pushButtonRefresh->setEnabled(false);
    ui->pushButtonPageUp->setEnabled(false);
    ui->pushButtonPageDown->setEnabled(false);
}

//Database opened.
void MainWindow::slotOpened(const QString &directory)
{
    if(!QDir(directory).exists())
    {
        return;
    }
    ui->pushButtonInfoExplore->setEnabled(true);
    ui->lineEditInfoDirectory->setText(QDir(directory).absolutePath());

    traversal(true);
}

//Traversal.
void MainWindow::traversal(bool initTableColumnWidth)
{
    if(ui->radioButtonTraversalKey->isChecked() && ui->lineEditTraversalKey->text().isEmpty())
    {
        return;
    }

    if(ui->radioButtonTraversalFirst->isChecked())
    {
        traversalPosition = TraversalPosition::positionEnum::first;
    }
    else if(ui->radioButtonTraversalLast->isChecked())
    {
        traversalPosition = TraversalPosition::positionEnum::last;
    }
    else if(ui->radioButtonTraversalKey->isChecked())
    {
        traversalPosition = TraversalPosition::positionEnum::key;
    }

    traversalKey = ui->lineEditTraversalKey->text();
    traversalNumber = ui->spinBoxTraversalNumber->value();

    if(ui->checkBoxTraversalReverse->isChecked())
    {
        traversalReverse = true;
    }
    else
    {
        traversalReverse = false;
    }

    if(ui->radioButtonTraversalKey->isChecked())
    {
        emit signalTraversal(&traversalPosition, traversalKey, traversalNumber, traversalReverse, initTableColumnWidth, true);
    }
    else
    {
        emit signalTraversal(&traversalPosition, QString(), traversalNumber, traversalReverse, initTableColumnWidth, true);
    }
}

//Database busy.
void MainWindow::slotBusy(bool busy)
{
    if(busy)
    {
        ui->pushButtonTraversalShow->setEnabled(false);
        ui->actionAdd->setEnabled(false);
        editable = false;
        copiable = false;
        deletable = false;
        ui->pushButtonRefresh->setEnabled(false);
        ui->pushButtonPageUp->setEnabled(false);
        ui->pushButtonPageDown->setEnabled(false);
    }
    else
    {
        ui->pushButtonTraversalShow->setEnabled(true);
        ui->actionAdd->setEnabled(true);
        editable = true;
        copiable = true;
        deletable = true;
        ui->pushButtonRefresh->setEnabled(true);
        ui->pushButtonPageUp->setEnabled(true);
        ui->pushButtonPageDown->setEnabled(true);
    }
}

//Table horizontal header complete.
void MainWindow::slotTableHorizontalHeaderComplete(bool initTableColumnWidth)
{
    if(initTableColumnWidth)
    {
        int columnWidth = (ui->tableWidgetTraversal->viewport()->width() - 1) / 2;
        ui->tableWidgetTraversal->setColumnWidth(0, columnWidth);
        ui->tableWidgetTraversal->setColumnWidth(1, columnWidth);
    }
    else
    {
        ui->tableWidgetTraversal->setColumnWidth(0, tableColumnWidth1);
        ui->tableWidgetTraversal->setColumnWidth(1, tableColumnWidth2);
    }
}

//Traversal complete.
void MainWindow::slotTraversalComplete(bool initTableColumnWidth)
{
    if(initTableColumnWidth)
    {
        ui->tableWidgetTraversal->resizeColumnsToContents();
    }
    else
    {
        ui->tableWidgetTraversal->setColumnWidth(0, tableColumnWidth1);
        ui->tableWidgetTraversal->setColumnWidth(1, tableColumnWidth2);
    }

    ui->actionClose->setEnabled(true);
    ui->pushButtonTraversalShow->setEnabled(true);
    ui->actionAdd->setEnabled(true);
    editable = true;
    copiable = true;
    deletable = true;
    ui->pushButtonRefresh->setEnabled(true);
    ui->pushButtonPageUp->setEnabled(true);
    ui->pushButtonPageDown->setEnabled(true);
}

//Table column width.
void MainWindow::slotTableColumnWidth(int columnIndex, int oldWidth, int newWidth)
{
    if(columnIndex == 0)
    {
        tableColumnWidth1 = newWidth;
    }
    else if(columnIndex == 1)
    {
        tableColumnWidth2 = newWidth;
    }
}

//Table current item changed.
void MainWindow::slotTableCurrentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous)
{
    int row = ui->tableWidgetTraversal->currentIndex().row();
    if(ui->tableWidgetTraversal->item(row, 0) != Q_NULLPTR && ui->tableWidgetTraversal->item(row, 1) != Q_NULLPTR && editable)
    {
        ui->actionEdit->setEnabled(true);
    }
    else
    {
        ui->actionEdit->setEnabled(false);
    }

    QTableWidgetItem *currentItem = ui->tableWidgetTraversal->currentItem();
    if(currentItem != Q_NULLPTR && copiable)
    {
        ui->actionCopy->setEnabled(true);
    }
    else
    {
        ui->actionCopy->setEnabled(false);
    }
}

//Table item selection changed.
void MainWindow::slotTableItemSelectionChanged()
{
    if(ui->tableWidgetTraversal->selectionModel()->selectedRows().count() > 0)
    {
        ui->actionDelete->setEnabled(true);
    }
    else
    {
        ui->actionDelete->setEnabled(false);
    }
}

//Table item double clicked.
void MainWindow::slotTableItemDoubleClicked(QTableWidgetItem *item)
{
    if(item->row() == -1)
    {
        return;
    }
    else
    {
        tableEditRow(item->row());
    }
}

//Table context menu.
void MainWindow::slotTableContextMenu(const QPoint &pos)
{
    tableWidgetTraversalMenu->exec(ui->tableWidgetTraversal->viewport()->mapToGlobal(pos));
}

//Info message.
void MainWindow::slotInfo(const QString &message)
{
    if(!message.isEmpty())
    {
        ui->textBrowserInfo->append(message);
    }
}

