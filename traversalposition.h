#ifndef TRAVERSALPOSITION_H
#define TRAVERSALPOSITION_H

#include <QObject>

class TraversalPosition : public QObject
{
    Q_OBJECT
public:
    explicit TraversalPosition(QObject *parent = nullptr);

    enum class positionEnum
    {
        none,
        first,
        last,
        key
    };
    Q_ENUM(positionEnum)
};

#endif // TRAVERSALPOSITION_H
