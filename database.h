#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QTableWidget>
#include <QMutex>
#include <QThread>
#include "traversalposition.h"
#include "leveldb/db.h"

class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(QObject *parent = nullptr);

private:
    leveldb::DB *db;
    QSharedPointer<leveldb::Iterator> traversalIterator;
    QTableWidget *tableWidget;
    QMutex mutex{QMutex::Recursive};
    int busyCount;
    QString databaseDirectory;
    enum TraversalPosition::positionEnum traversalPosition;
    QString traversalKey;
    int traversalNumber;
    bool traversalReverse;
    QString getValueKey;
    QString existKey;
    QString addKey;
    QString addValue;
    QString editOldKey;
    QString editOldValue;
    QString editNewKey;
    QString editNewValue;
    QStringList deleteKeyList;

    void close();
    bool traversal();
    QString getValue(const QString &key);
    bool addItem(const QString &key, const QString &value);
    bool editItem(const QString &oldKey, const QString &newKey, const QString &newValue);
    bool deleteItem(const QString &key);
    void setTableHorizontalHeader(bool initTableColumnWidth);
    void setTraversalRow(int row);
    void setTableRow(int row, const QString &key, const QString &value);
    bool isExistKey(const QString &key, bool info);
    bool update(bool info);
    void clear();
    void busy(bool busy);
    void message(const QString &message, bool info);

public slots:
    void slotTableWidget(QTableWidget *table);
    bool slotOpen(const QString &directory, bool info);
    void slotClose(bool info);
    bool slotTraversal(TraversalPosition::positionEnum *position, const QString &key, int number, bool reverse, bool initTableColumnWidth, bool info);
    QString slotGetValue(const QString &key, bool info);
    bool slotAddItem(const QString &key, const QString &value, bool info);
    bool slotEditItem(const QString &oldKey, const QString &oldValue, const QString &newKey, const QString &newValue, bool info);
    bool slotDeleteItem(const QStringList &keyList, bool info);
    bool slotRefresh(bool info);
    bool slotPageUp(bool info);
    bool slotPageDown(bool info);

signals:
    void signalClose();
    void signalOpened(const QString &directory);
    void signalBusy(bool busy);
    void signalTableHorizontalHeaderComplete(bool initTableColumnWidth);
    void signalTraversalComplete(bool initTableColumnWidth);
    void signalInfo(const QString &info);
};

#endif // DATABASE_H
