#ifndef MENUSHADOW_H
#define MENUSHADOW_H

#include <QWidget>
#include <QMenu>

namespace Ui {
class MenuShadow;
}

class MenuShadow : public QWidget
{
    Q_OBJECT

public:
    explicit MenuShadow(QWidget *parent = nullptr);
    ~MenuShadow();

private:
    Ui::MenuShadow *ui;
    QWidget *window;
    QMenu *shadowMenu;
    void resizeMenu();
    void moveMenu(const QPoint &pos);
    void setGeometry(const QRect& rect);
    void setGeometry(int x, int y, int w, int h);
    QRect widgetMapToGlobalRect(const QWidget *widget) const;
    QRect mapToGlobalRect(const QWidget *widget, const QRect &rect) const;
    QRect mapFromGlobalRect(const QWidget *widget, const QRect &rect) const;
    QRect widgetMapToWidgetRect(const QWidget *source, const QWidget *target) const;

public:
    QMenu* menu() const;
    void setMenu(QMenu *menu);
    QRect geometryInnerEdge() const;
    QMargins margin() const;
    void setMargin(const QMargins &margin);
    void setMargin(int left, int top, int right, int bottom);

    // QObject interface
public:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
};

#endif // MENUSHADOW_H
