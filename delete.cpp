#include "delete.h"
#include "ui_delete.h"

#include <QFile>
#include <QTranslator>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <QScreen>
#include <QFileDialog>
#include <QDebug>

Delete::Delete(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Delete)
{
    ui->setupUi(this);

    //Window frameless.
    setWindowFlag(Qt::FramelessWindowHint);
    setWindowFlag(Qt::Dialog);
    setAttribute(Qt::WA_TranslucentBackground);

    //Define window;
    window = ui->widgetBackground;

    //Window shadow.
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setOffset(0, 0);
    shadow->setColor(QColor(64, 64, 64));
    shadow->setBlurRadius(10);
    window->setGraphicsEffect(shadow);

    //Load QSS file.
    QFile qssFile(":/qss/dialog.qss");
    if(qssFile.open(QFile::ReadOnly))
    {
        QString qssString = qssFile.readAll();
        //Set style sheet.
        setStyleSheet(qssString);
        qssFile.close();
    }

    //Install event filter.
    ui->widgetTitleBar->installEventFilter(this);

    //labelTitle not accepted mouse event.
    ui->labelTitle->setAttribute(Qt::WA_TransparentForMouseEvents);

    //Window mouse press.
    isTitleBarMousePress = false;
}

Delete::~Delete()
{
    delete ui;
}


void Delete::translate()
{
    ui->retranslateUi(this);
}

bool Delete::eventFilter(QObject *watched, QEvent *event)
{
    //Current screen available rect.
    QRect screenRect = screen()->availableGeometry();

    //widgetTitleBar mouse event.
    if(watched == ui->widgetTitleBar)
    {
        //widgetTitleBar mouse press positon of relative QWidget.
        if(event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *mousePressEvent = static_cast<QMouseEvent*>(event);
            isTitleBarMousePress = true;
            titleBarPressPos = ui->widgetTitleBar->mapTo(this, mousePressEvent->pos());
        }

        //widgetTitleBar mouse move event.
        if(event->type() == QEvent::MouseMove && isTitleBarMousePress)
        {
            QMouseEvent *mouseMoveEvent = static_cast<QMouseEvent*>(event);
            //widgetTitleBar mouse drag, window move.
            move(mouseMoveEvent->globalPos() - titleBarPressPos);

            //Screen limit window mouse.
            QCursor screenCursor;
            if(cursor().pos().x() < screenRect.x())
            {
                screenCursor.setPos(screenRect.x(), cursor().pos().y());
                setCursor(screenCursor);
            }
            else if(cursor().pos().x() > screenRect.right())
            {
                screenCursor.setPos(screenRect.right(), cursor().pos().y());
                setCursor(screenCursor);
            }
            if(cursor().pos().y() < screenRect.y())
            {
                screenCursor.setPos(cursor().pos().x(), screenRect.y());
                setCursor(screenCursor);
            }
            else if(cursor().pos().y() > screenRect.bottom())
            {
                screenCursor.setPos(cursor().pos().x(), screenRect.bottom());
                setCursor(screenCursor);
            }
        }

        //widgetTitleBar mouse release.
        if(event->type() == QEvent::MouseButtonRelease && isTitleBarMousePress)
        {
            isTitleBarMousePress = false;

            //Window limit screen top.
            if(window->mapToGlobal(window->pos()).y() < screenRect.y())
            {
                move(x(), screenRect.y() - window->y());
            }
        }
    }

    return false;
}

//Window close.
void Delete::on_pushButtonClose_clicked()
{
    reject();
}

//Delete ok.
void Delete::on_pushButtonOk_clicked()
{
    emit signalDelete(deleteKeyList, true);
    message(tr("Delete ") + QString::number(deleteKeyList.count()) + tr(" item."), false);
    accept();
}

//Delete cancel.
void Delete::on_pushButtonCancel_clicked()
{
    reject();
}

//Set key.
void Delete::setDelete(QStringList keyList)
{
    deleteKeyList = keyList;

    //Show delete info.
    ui->labelInfo->setText(tr("Delete ") + QString::number(deleteKeyList.count()) + tr(" item?"));
}

//Accept override.
void Delete::accept()
{
    reset();
    QDialog::accept();
}

//Reject override.
void Delete::reject()
{
    reset();
    QDialog::reject();
}

//Delete reset.
void Delete::reset()
{
    ui->labelInfo->clear();
}

//Delete message.
void Delete::message(const QString &message, bool info)
{
    if(info)
    {
        ui->labelInfo->setText(message);
    }
    qDebug() << message;
}

