#include "menushadow.h"
#include "ui_menushadow.h"

#include <QFile>
#include <QGraphicsDropShadowEffect>
#include <QMoveEvent>
#include <QDebug>

MenuShadow::MenuShadow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MenuShadow)
{
    ui->setupUi(this);

    //Window frameless.
    setWindowFlag(Qt::FramelessWindowHint);
    setWindowFlag(Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);

    //Hide.
    hide();

    //Define window;
    window = ui->widgetBackground;

    //Window shadow.
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setOffset(0, 0);
    shadow->setColor(QColor(64, 64, 64));
    shadow->setBlurRadius(10);
    window->setGraphicsEffect(shadow);

    //Load QSS file.
    QFile qssFile(":/qss/menushadow.qss");
    if(qssFile.open(QFile::ReadOnly))
    {
        QString qssString = qssFile.readAll();
        //Set style sheet.
        setStyleSheet(qssString);
        qssFile.close();
    }

    shadowMenu = Q_NULLPTR;
}

MenuShadow::~MenuShadow()
{
    delete ui;
}


bool MenuShadow::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == menu())
    {
        if(event->type() == QEvent::Show)
        {
            show();
        }
        else if(event->type() == QEvent::Hide)
        {
            hide();
        }
        else if(event->type() == QEvent::Resize)
        {
            resizeMenu();
        }
        else if(event->type() == QEvent::Move)
        {
            QMoveEvent *moveEvent = static_cast<QMoveEvent*>(event);
            moveMenu(moveEvent->pos());
        }
    }

    return false;
}

//Menu.
QMenu* MenuShadow::menu() const
{
    return shadowMenu;
}

//Set menu.
void MenuShadow::setMenu(QMenu *menu)
{
    if(menu == Q_NULLPTR)
    {
        return;
    }

    shadowMenu = menu;
    shadowMenu->installEventFilter(this);
    shadowMenu->setWindowFlags(menu->windowFlags() | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    shadowMenu->setAttribute(Qt::WA_TranslucentBackground);
}

//Changed menu.
void MenuShadow::resizeMenu()
{
    setGeometry(widgetMapToGlobalRect(menu()));
}

//Move menu.
void MenuShadow::moveMenu(const QPoint &pos)
{
    move(pos.x() - ui->verticalLayout->contentsMargins().left(), pos.y() - ui->verticalLayout->contentsMargins().top());
}

//Window geometry.
QRect MenuShadow::geometryInnerEdge() const
{
    return widgetMapToGlobalRect(window);
}

//Set window geometry.
void MenuShadow::setGeometry(const QRect& rect)
{
    QWidget::setGeometry(rect.marginsAdded(ui->verticalLayout->contentsMargins()));
}

//Set window geometry.
void MenuShadow::setGeometry(int x, int y, int w, int h)
{
    QWidget::setGeometry(QRect(x, y, w, h).marginsAdded(ui->verticalLayout->contentsMargins()));
}

//Margin.
QMargins MenuShadow::margin() const
{
    return ui->verticalLayout->contentsMargins();
}

//Set margin.
void MenuShadow::setMargin(const QMargins &margin)
{
    ui->verticalLayout->setContentsMargins(margin);
}

//Set margin.
void MenuShadow::setMargin(int left, int top, int right, int bottom)
{
    ui->verticalLayout->setContentsMargins(left, top, right, bottom);
}

//Widget map to global rect.
QRect MenuShadow::widgetMapToGlobalRect(const QWidget *widget) const
{
    return QRect(widget->pos(), widget->size());
}

//Map from global rect.
QRect MenuShadow::mapFromGlobalRect(const QWidget *widget, const QRect &rect) const
{
    return QRect(widget->mapFromGlobal(rect.topLeft()), rect.size());
}

//Widget map to widget rect.
QRect MenuShadow::widgetMapToWidgetRect(const QWidget *source, const QWidget *target) const
{
    return mapFromGlobalRect(target, widgetMapToGlobalRect(source));
}
