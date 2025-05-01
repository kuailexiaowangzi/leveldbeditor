#ifndef OPEN_H
#define OPEN_H

#include <QDialog>
#include <QLocale>
#include <QPoint>

namespace Ui {
class Open;
}

class Open : public QDialog
{
    Q_OBJECT

public:
    explicit Open(QWidget *parent = nullptr);
    ~Open();
    void translate();
    void setTraversalNumber(int number);

private:
    Ui::Open *ui;QWidget *window;
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
    void on_pushButtonBrowse_clicked();
    void on_pushButtonOk_clicked();
    void on_pushButtonCancel_clicked();

signals:
    void signalOpen(const QString &directory, bool info);
};

#endif // OPEN_H
