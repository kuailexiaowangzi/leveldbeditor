#ifndef EDIT_H
#define EDIT_H

#include <QDialog>
#include <QLocale>
#include <QPoint>

namespace Ui {
class Edit;
}

class Edit : public QDialog
{
    Q_OBJECT

public:
    explicit Edit(QWidget *parent = nullptr);
    ~Edit();
    void translate();
    void setEdit(const QString &key, const QString &value);

private:
    Ui::Edit *ui;
    QWidget *window;
    bool isTitleBarMousePress;
    QPoint titleBarPressPos;
    enum QLocale::Language language;
    QString oldKey;
    QString oldValue;

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
    void signalEdit(const QString &oldKey, const QString &oldValue, const QString &newKey, const QString &newValue, bool info);
};

#endif // EDIT_H
