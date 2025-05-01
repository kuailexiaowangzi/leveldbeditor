#include "edit.h"
#include "ui_edit.h"

#include <QFile>
#include <QTranslator>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <QScreen>
#include <QFileDialog>
#include <QDebug>

Edit::Edit(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Edit)
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

Edit::~Edit()
{
    delete ui;
}


void Edit::translate()
{
    ui->retranslateUi(this);
}

bool Edit::eventFilter(QObject *watched, QEvent *event)
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

//Windows close.
void Edit::on_pushButtonClose_clicked()
{
    reject();
}

//Edit ok.
void Edit::on_pushButtonOk_clicked()
{
    if(ui->lineEditKey->text().isEmpty())
    {
        message(tr("Please enter key."), true);
        return;
    }

    if(ui->lineEditValue->text().isEmpty())
    {
        message(tr("Please enter value."), true);
        return;
    }

    if(oldKey == ui->lineEditKey->text() && oldValue == ui->lineEditValue->text())
    {
        return;
    }

    emit signalEdit(oldKey, oldValue, ui->lineEditKey->text(), ui->lineEditValue->text(), true);
    message(tr("Edit: ") + ui->lineEditKey->text() + " - " + ui->lineEditValue->text(), false);
    accept();
}

//Edit cancel.
void Edit::on_pushButtonCancel_clicked()
{
    reject();
}

//Set edit.
void Edit::setEdit(const QString &key, const QString &value)
{
    oldKey = key;
    oldValue = value;
    ui->lineEditKey->setText(key);
    ui->lineEditValue->setText(value);
}

//Accept override.
void Edit::accept()
{
    reset();
    QDialog::accept();
}

//Reject override.
void Edit::reject()
{
    reset();
    QDialog::reject();
}

//Edit reset.
void Edit::reset()
{
    ui->lineEditKey->clear();
    ui->lineEditValue->clear();
    ui->lineEditKey->repaint();
    ui->lineEditValue->repaint();
    ui->labelInfo->clear();
}

//Edit message.
void Edit::message(const QString &message, bool info)
{
    if(info)
    {
        ui->labelInfo->setText(message);
    }
    qDebug() << message;
}

