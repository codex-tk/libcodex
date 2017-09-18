#ifndef MOUSEEVENTSIGNALER_H
#define MOUSEEVENTSIGNALER_H

#include <QObject>
#include <QWidget>
#include <QMouseEvent>

class MouseEventSignaler : public QObject
{
    Q_OBJECT
public:
    explicit MouseEventSignaler(QObject *parent = nullptr);

    bool eventFilter(QObject * obj, QEvent * ev) Q_DECL_OVERRIDE;
public:
    Q_SIGNAL void mouseButtonEvent(QWidget *, QMouseEvent *);
    void installOn(QWidget * widget);
signals:

public slots:


};

#endif // MOUSEEVENTSIGNALER_H
