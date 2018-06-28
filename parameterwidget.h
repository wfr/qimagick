#ifndef PARAMETERWIDGET_H
#define PARAMETERWIDGET_H

#include <QWidget>
#include "effect.h"

class ParameterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ParameterWidget(QWidget *parent, Effect* effect);

protected:
    Effect* effect;

signals:
    void valueChanged();
};

#endif // PARAMETERWIDGET_H
