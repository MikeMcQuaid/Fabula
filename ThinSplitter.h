#ifndef THINSPLITTER_H
#define THINSPLITTER_H

#include <QSplitter>

class ThinSplitter : public QSplitter
{
    Q_OBJECT
public:
    explicit ThinSplitter(QWidget *parent = 0);
protected:
    QSplitterHandle *createHandle();
};

#endif // THINSPLITTER_H
