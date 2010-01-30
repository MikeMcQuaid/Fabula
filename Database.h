#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>

class Database : public QObject
{
Q_OBJECT
public:
    explicit Database(const QString &path, QObject *parent = 0);
    ~Database();
private:
    bool create();
};

#endif // DATABASE_H
