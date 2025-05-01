#ifndef ADD_H
#define ADD_H

#include <QDialog>
#include <QLocale>
#include <QPoint>

namespace Ui {
class Add;
}

class Add : public QDialog
{
    Q_OBJECT

public:
    explicit Add(QWidget *parent = nullptr);
    ~Add();
    void translate();

private:
    Ui::Add *ui;
    QWidget *window;
    bool isTitleBarMousePress;
    QPoint titleBarPressPos;
    enum QLocale::Language language;

    QRect mapToWindowRect(const QWidget *widget);
    void accept() override;
    void reject() override;
    void reset();
    void message(const QString &message, bool info);

    // QObject interface
public:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
private slots:
    void on_pushButtonClose_clicked();
    void on_pushButtonOk_clicked();
    void on_pushButtonCancel_clicked();

signals:
    void signalAdd(const QString &key, const QString &value, bool info);
};

#endif // ADD_H
