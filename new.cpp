#include "new.h"
#include "ui_new.h"

#include <QFile>
#include <QTranslator>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <QScreen>
#include <QFileDialog>
#include <QDebug>

New::New(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::New)
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

    traversalNumber = 0;
}

New::~New()
{
    delete ui;
}


void New::translate()
{
    ui->retranslateUi(this);
}

bool New::eventFilter(QObject *watched, QEvent *event)
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
void New::on_pushButtonClose_clicked()
{
    reject();
}

//Select directory.
void New::on_pushButtonSelect_clicked()
{
    ui->lineEditDirectory->setText(QFileDialog::getExistingDirectory(this, tr("Select directory")));
}

//New ok.
void New::on_pushButtonOk_clicked()
{
    if(ui->lineEditDirectory->text().isEmpty())
    {
        message(tr("Please enter directory."), true);
        return;
    }

    if(ui->lineEditName->text().isEmpty())
    {
        message(tr("Please enter name."), true);
        return;
    }

    if(QDir(ui->lineEditDirectory->text()).exists())
    {
        if(QDir(ui->lineEditDirectory->text()).cd(ui->lineEditName->text()))
        {
            message(tr("Name alread exists."), true);
        }
        else
        {
            emit signalNew(QDir::cleanPath(ui->lineEditDirectory->text() + "/" + ui->lineEditName->text()), true);
            message(tr("New: ") + QDir::cleanPath(ui->lineEditDirectory->text() + "/" + ui->lineEditName->text()), false);
            accept();
        }
    }
    else
    {
        message(tr("Directory error."), true);
    }
}

//New cancel.
void New::on_pushButtonCancel_clicked()
{
    reject();
}

//Set traversal number.
void New::setTraversalNumber(int number)
{
    traversalNumber = number;
}

//Accept override.
void New::accept()
{
    reset();
    QDialog::accept();
}

//Reject override.
void New::reject()
{
    reset();
    QDialog::reject();
}

//New reset.
void New::reset()
{
    ui->lineEditDirectory->clear();
    ui->lineEditName->clear();
    ui->labelInfo->clear();
    ui->lineEditDirectory->repaint();
    ui->lineEditName->repaint();
    message(tr("New reset."), false);
}

//New message.
void New::message(const QString &message, bool info)
{
    if(info)
    {
        ui->labelInfo->setText(message);
    }
    qDebug() << message;
}

