#include "mouseeventsignaler.h"

MouseEventSignaler::MouseEventSignaler(QObject *parent) : QObject(parent)
{

}

bool MouseEventSignaler::eventFilter(QObject * obj, QEvent * ev)  {
  if ((ev->type() == QEvent::MouseButtonPress
      || ev->type() == QEvent::MouseButtonRelease
      || ev->type() == QEvent::MouseButtonDblClick)
      && obj->isWidgetType())
    emit mouseButtonEvent(static_cast<QWidget*>(obj),
                          static_cast<QMouseEvent*>(ev));
  return false;
}

void MouseEventSignaler::installOn(QWidget * widget) {
  widget->installEventFilter(this);
}
