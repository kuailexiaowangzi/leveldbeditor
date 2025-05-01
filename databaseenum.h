#ifndef DATABASEENUM_H
#define DATABASEENUM_H

#include <QObject>

class DatabaseEnum : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseEnum(QObject *parent = nullptr);

    enum class databaseType
    {
        none,
        LevelDB,
        RocksDB
    };
    Q_ENUM(databaseType);
};

#endif // DATABASEENUM_H
