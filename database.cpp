#include "database.h"
#include <QDir>
#include "leveldb/write_batch.h"
#include <QDebug>

Database::Database(QObject *parent)
    : QObject(parent)
{
    db = Q_NULLPTR;
    traversalIterator = QSharedPointer<leveldb::Iterator>();
    tableWidget = Q_NULLPTR;
    busyCount = 0;
    databaseDirectory = QString();
    traversalPosition = TraversalPosition::positionEnum::none;
    traversalKey = QString();
    traversalNumber = 0;
    traversalReverse = false;
    getValueKey = QString();
    existKey = QString();
    addKey = QString();
    addValue = QString();
    editOldKey = QString();
    editOldValue = QString();
    editNewKey = QString();
    editNewValue = QString();
    deleteKeyList = QStringList();
}

//Database tableWidget slots.
void Database::slotTableWidget(QTableWidget *table)
{
    tableWidget = table;
}

//Database open slots.
bool Database::slotOpen(const QString &directory, bool info)
{
    mutex.tryLock(100);
    QMutexLocker lock{&mutex};
    busy(true);

    databaseDirectory = QDir::cleanPath(directory);
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options, databaseDirectory.toStdString(), &db);

    if(status.ok())
    {
        emit signalOpened(databaseDirectory);
        message(databaseDirectory + tr(" Successfully opened."), info);
        busy(false);
        return true;
    }
    else
    {
        slotClose(false);
    }

    message(databaseDirectory + tr(" Opening failed."), info);
    busy(false);
    return false;
}

//Database close slots.
void Database::slotClose(bool info)
{
    mutex.tryLock(100);
    QMutexLocker lock{&mutex};
    busy(true);

    close();
    clear();
    traversalPosition = TraversalPosition::positionEnum::none;
    traversalKey = QString();
    traversalNumber = 0;
    traversalReverse = false;
    getValueKey = QString();
    existKey = QString();
    addKey = QString();
    addValue = QString();
    editOldKey = QString();
    editOldValue = QString();
    editNewKey = QString();
    editNewValue = QString();
    deleteKeyList = QStringList();

    emit signalClose();
    message(databaseDirectory + tr(" Close completed."), info);
    databaseDirectory = QString();
}

//Database close.
void Database::close()
{
    traversalIterator.clear();
    if(db != Q_NULLPTR)
    {
        delete db;
        db = Q_NULLPTR;
    }
    clear();
}

//Database traversal slots.
bool Database::slotTraversal(TraversalPosition::positionEnum *position, const QString &key, int number, bool reverse, bool initTableColumnWidth, bool info)
{
    mutex.tryLock(100);
    QMutexLocker lock{&mutex};
    busy(true);

    traversalPosition = *position;
    traversalKey = key;
    traversalNumber = number;
    traversalReverse = reverse;

    if(!traversalIterator.isNull())
    {
        traversalIterator.clear();
    }
    clear();

    setTableHorizontalHeader(initTableColumnWidth);
    traversalIterator = QSharedPointer<leveldb::Iterator>(db->NewIterator(leveldb::ReadOptions()));

    if(traversalPosition == TraversalPosition::positionEnum::first)
    {
        traversalIterator->SeekToFirst();
    }
    else if(traversalPosition == TraversalPosition::positionEnum::last)
    {
        traversalIterator->SeekToLast();
    }
    else if(traversalPosition == TraversalPosition::positionEnum::key)
    {
        if(traversalKey.isEmpty())
        {
            message(tr("Traversal failed."), info);
            busy(false);
            return false;
        }
        traversalIterator->Seek(traversalKey.toStdString());
    }

    if(traversal())
    {
        emit signalTraversalComplete(initTableColumnWidth);
        message(tr("Traversal completed."), info);
        busy(false);
        return true;
    }

    message(tr("Traversal failed."), info);
    busy(false);
    return false;
}

//Database traversal.
bool Database::traversal()
{
    if(traversalPosition == TraversalPosition::positionEnum::key && QString::fromStdString(traversalIterator->key().ToString()) != traversalKey)
    {
        return false;
    }

    if(traversalReverse)
    {
        for(int index = 0; traversalIterator->Valid() && index < traversalNumber; traversalIterator->Prev(), index++)
        {
            setTraversalRow(index);
        }
        if(!traversalIterator->Valid())
        {
            traversalIterator->SeekToFirst();
        }
    }
    else
    {
        for(int index = 0; traversalIterator->Valid() && index < traversalNumber; traversalIterator->Next(), index++)
        {
            setTraversalRow(index);
        }
        if(!traversalIterator->Valid())
        {
            traversalIterator->SeekToLast();
        }
    }

    if(traversalIterator->status().ok())
    {
        return true;
    }

    return false;
}

//Set table horizontal header.
void Database::setTableHorizontalHeader(bool initTableColumnWidth)
{
    tableWidget->setColumnCount(2);
    tableWidget->setHorizontalHeaderLabels({"Key", "Value"});
    emit signalTableHorizontalHeaderComplete(initTableColumnWidth);
}

//Set database iterator row.
void Database::setTraversalRow(int row)
{
    QString key = QString::fromStdString(traversalIterator->key().ToString());
    QString value = QString::fromStdString(traversalIterator->value().ToString());
    setTableRow(row, key, value);
}

//Set table row.
void Database::setTableRow(int row, const QString &key, const QString &value)
{
    tableWidget->setRowCount(row + 1);
    tableWidget->setVerticalHeaderItem(row, new QTableWidgetItem(QString::number(row + 1)));
    tableWidget->setItem(row, 0, new QTableWidgetItem(key));
    tableWidget->setItem(row, 1, new QTableWidgetItem(value));
}

//Database get value slots.
QString Database::slotGetValue(const QString &key, bool info)
{
    mutex.tryLock(100);
    QMutexLocker lock{&mutex};
    busy(true);

    getValueKey = key;
    QString value;
    value = getValue(key);
    if(!value.isEmpty())
    {
        message(tr("Get ") + key + " - " + value + tr(" completed."), info);
    }
    else
    {
        message(tr("Get ") + key + tr(" failed."), info);
    }

    busy(false);
    return value;
}

//Database get value.
QString Database::getValue(const QString &key)
{
    std::string value;
    leveldb::Status status = db->Get(leveldb::ReadOptions(), key.toStdString(), &value);
    if(status.ok())
    {
        return QString::fromStdString(value);
    }

    return QString();
}

//Database exist key.
bool Database::isExistKey(const QString &key, bool info)
{
    existKey = key;

    if(!slotGetValue(key, false).isEmpty())
    {
        message(tr("Exist ") + key, info);
        return true;
    }

    message(tr("Absent ") + key, info);

    return false;
}

//Database add item slots.
bool Database::slotAddItem(const QString &key, const QString &value, bool info)
{
    mutex.tryLock(100);
    QMutexLocker lock{&mutex};
    busy(true);

    addKey = key;
    addValue = value;

    if(!isExistKey(key, false))
    {
        if(addItem(key, value))
        {
            message(tr("Add ") + key + " - " + value + tr(" completed."), info);
            update(false);
            busy(false);
            return true;
        }
    }

    message(tr("Add ") + key + " - " + value + tr(" failed."), info);
    busy(false);
    return false;
}

//Database add item.
bool Database::addItem(const QString &key, const QString &value)
{
    leveldb::Status status = db->Put(leveldb::WriteOptions(), key.toStdString(), value.toStdString());

    if(status.ok())
    {
        return true;
    }

    return false;
}

//Database edit item slots.
bool Database::slotEditItem(const QString &oldKey, const QString &oldValue, const QString &newKey, const QString &newValue, bool info)
{
    mutex.tryLock(100);
    QMutexLocker lock{&mutex};
    busy(true);

    editOldKey = oldKey;
    editOldValue = oldValue;
    editNewKey = newKey;
    editNewValue = newValue;
    bool isOk = true;

    isOk = isExistKey(oldKey, false);

    if(oldKey == newKey && oldValue == newValue && isOk)
    {

    }
    else if(isOk)
    {
        isOk = editItem(oldKey, newKey, newValue);
    }

    if(isOk)
    {
        message(tr("Edit ") + oldKey + " - " + oldValue + " → " + newKey + " - " + newValue + tr(" completed."), info);
        update(false);
        busy(false);
        return true;
    }

    message(tr("Edit ") + oldKey + " - " + oldValue + " → " + newKey + " - " + newValue + tr(" failed."), info);
    busy(false);
    return false;
}

//Database edit item.
bool Database::editItem(const QString &oldKey, const QString &newKey, const QString &newValue)
{
    if(oldKey == newKey)
    {
        return addItem(oldKey, newValue);
    }
    else
    {
        leveldb::WriteBatch writeBatch;
        writeBatch.Delete(oldKey.toStdString());
        writeBatch.Put(newKey.toStdString(), newValue.toStdString());
        leveldb::Status status = db->Write(leveldb::WriteOptions(), &writeBatch);

        if(status.ok())
        {
            return true;
        }
    }

    return false;
}

//Databse delete item slot.
bool Database::slotDeleteItem(const QStringList &keyList, bool info)
{
    mutex.tryLock(100);
    QMutexLocker lock{&mutex};
    busy(true);

    deleteKeyList = keyList;
    bool isOk = true;
    int index;

    for(index = 0; index < keyList.count() && isOk; index++)
    {
        if(isExistKey(keyList[index], false))
        {
            isOk = deleteItem(keyList[index]);
        }
        else
        {
            isOk = false;
        }

        if(isOk)
        {
            message(tr("Delete ") + keyList[index] + tr(" completed."), info);
        }
        else
        {
            message(tr("Delete ") + keyList[index] + tr(" failed."), info);
            busy(false);
            return false;
        }
    }

    if(index == keyList.count())
    {
        update(false);
        busy(false);
        return true;
    }

    busy(false);
    return false;
}

//Database delete item.
bool Database::deleteItem(const QString &key)
{
    leveldb::Status status = db->Delete(leveldb::WriteOptions(), key.toStdString());
    if(status.ok())
    {
        return true;
    }

    return false;
}

//Database refresh.
bool Database::slotRefresh(bool info)
{
    mutex.tryLock(100);
    QMutexLocker lock{&mutex};
    busy(true);

    clear();
    setTableHorizontalHeader(true);

    if(!traversalReverse)
    {
        for(int index = 0; traversalIterator->Valid() && index < traversalNumber; traversalIterator->Prev(), index++);
        if(!traversalIterator->Valid())
        {
            traversalIterator->SeekToFirst();
        }
    }
    else
    {
        for(int index = 0; traversalIterator->Valid() && index < traversalNumber; traversalIterator->Next(), index++);
        if(!traversalIterator->Valid())
        {
            traversalIterator->SeekToLast();
        }
    }

    if(traversal())
    {
        emit signalTraversalComplete(true);
        message(tr("Refresh completed."), info);
        busy(false);
        return true;
    }

    message(tr("Refresh failed."), info);
    busy(false);
    return false;
}

//Database page up.
bool Database::slotPageUp(bool info)
{
    mutex.tryLock(100);
    QMutexLocker lock{&mutex};
    busy(true);

    clear();
    setTableHorizontalHeader(true);

    if(!traversalReverse)
    {
        for(int index = 0; traversalIterator->Valid() && index < traversalNumber * 2; traversalIterator->Prev(), index++);
        if(!traversalIterator->Valid())
        {
            traversalIterator->SeekToFirst();
        }
    }

    if(traversal())
    {
        emit signalTraversalComplete(true);
        message(tr("Page up completed."), info);
        busy(false);
        return true;
    }

    message(tr("Page up failed."), info);
    busy(false);
    return false;
}

//Database page down.
bool Database::slotPageDown(bool info)
{
    mutex.tryLock(100);
    QMutexLocker lock{&mutex};
    busy(true);

    clear();
    setTableHorizontalHeader(true);

    if(traversalReverse)
    {
        for(int index = 0; traversalIterator->Valid() && index < traversalNumber * 2; traversalIterator->Next(), index++);
        if(!traversalIterator->Valid())
        {
            traversalIterator->SeekToLast();
        }
    }

    if(traversal())
    {
        emit signalTraversalComplete(true);
        message(tr("Page down completed."), info);
        busy(false);
        return true;
    }

    message(tr("Page down failed."), info);
    busy(false);
    return false;
}

//Database update.
bool Database::update(bool info)
{
    if(slotTraversal(&traversalPosition, traversalKey, traversalNumber, traversalReverse, false, false))
    {
        message(tr("Refresh completed."), info);
        return true;
    }
    else
    {
        message(tr("Refresh failed."), info);
    }
    return false;
}

//Database clear.
void Database::clear()
{
    tableWidget->clear();
    tableWidget->setColumnCount(0);
    tableWidget->setRowCount(0);
    tableWidget->clearFocus();
}

//Database busy.
void Database::busy(bool busy)
{
    mutex.tryLock(100);
    QMutexLocker lock{&mutex};

    if(busyCount == 0)
    {
        if(busy == true)
        {
            busyCount++;
            emit signalBusy(true);
        }
    }
    else
    {
        if(busy == true)
        {
            busyCount++;
        }
        else
        {
            busyCount--;
            if(busyCount == 0)
            {
                emit signalBusy(false);;
            }
        }
    }
}

//Database message.
void Database::message(const QString &message, bool info)
{
    if(info)
    {
        emit signalInfo(message);
    }
    qDebug() << message;
}
