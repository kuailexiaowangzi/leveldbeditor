#ifndef NEW_H
#define NEW_H

#include <QDialog>
#include <QLocale>
#include <QPoint>

namespace Ui {
class New;
}

class New : public QDialog
{
    Q_OBJECT

public:
    explicit New(QWidget *parent = nullptr);
    ~New();
    void translate();
    void setTraversalNumber(int number);

private:
    Ui::New *ui;
    QWidget *window;
    bool isTitleBarMousePress;
    QPoint titleBarPressPos;
    enum QLocale::Language language;
    int traversalNumber;

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
    void on_pushButtonSelect_clicked();
    void on_pushButtonOk_clicked();
    void on_pushButtonCancel_clicked();

signals:
    void signalNew(const QString &directory, bool info);
};

#endif // NEW_H
