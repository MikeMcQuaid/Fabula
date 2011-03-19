#include "ThinSplitter.h"

#include <QPaintEvent>
#include <QPainter>
#include <QDebug>

class ThinSplitterHandle : public QSplitterHandle
{
public:
    ThinSplitterHandle(Qt::Orientation orientation, QSplitter *parent);
protected:
    void paintEvent(QPaintEvent *event);
};

ThinSplitterHandle::ThinSplitterHandle(Qt::Orientation orientation, QSplitter *parent) :
    QSplitterHandle(orientation, parent)
{
}

void ThinSplitterHandle::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(event->rect(), QColor(210, 210, 210));
}

ThinSplitter::ThinSplitter(QWidget *parent) :
    QSplitter(parent)
{
    setHandleWidth(1);
}

QSplitterHandle* ThinSplitter::createHandle()
{
    return new ThinSplitterHandle(orientation(), this);
}
