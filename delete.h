#ifndef DELETE_H
#define DELETE_H

#include <QDialog>
#include <QLocale>
#include <QPoint>

namespace Ui {
class Delete;
}

class Delete : public QDialog
{
    Q_OBJECT

public:
    explicit Delete(QWidget *parent = nullptr);
    ~Delete();
    void translate();
    void setDelete(QStringList keyList);

private:
    Ui::Delete *ui;
    QWidget *window;
    bool isTitleBarMousePress;
    QPoint titleBarPressPos;
    enum QLocale::Language language;
    QStringList deleteKeyList;

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
    void signalDelete(const QStringList &keyList, bool info);
};

#endif // DELETE_H
