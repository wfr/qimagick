#include "parameterwidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QSlider>

ParameterWidget::ParameterWidget(QWidget *parent, Effect* effect)
    : QWidget(parent), effect(effect)
{
    QVBoxLayout* layout = new QVBoxLayout(parent);
    for (auto it = effect->parameters.begin(); it != effect->parameters.end(); it++) {
        EffectParameter* param = &(*it);
        QLayout *labelLayout = new QHBoxLayout();
        QLabel *labelId = new QLabel(param->id);
        QLabel *labelValue = new QLabel(QString("%1").arg(param->value));
        labelLayout->addWidget(labelId);
        labelLayout->addWidget(labelValue);
        labelLayout->setAlignment(labelValue, Qt::AlignRight);
        layout->addLayout(labelLayout);

        if (param->type == ParameterType::FLOAT || param->type == ParameterType::INT) {
            QSlider* slider = new QSlider(Qt::Horizontal);
            slider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
            if (param->type == ParameterType::FLOAT) {
                slider->setMinimum(param->min * 10.0f);
                slider->setMaximum(param->max * 10.0f);
                slider->setValue(param->value * 10.0f);
            } else {
                slider->setMinimum(param->min);
                slider->setMaximum(param->max);
                slider->setValue(param->value);
            }
            slider->setVisible(true);
            layout->addWidget(slider);

            connect(slider, &QSlider::valueChanged, this, [=] {
                float v = slider->value();
                if (param->type == ParameterType::FLOAT) {
                   v /= 10.0f;
                }
                param->value = v;
                labelValue->setText(QString("%1").arg(param->value));
                emit valueChanged();
            });
        }
        if (param->type == ParameterType::FLOAT) {

        }
        labelId->setVisible(true);
        labelValue->setVisible(true);
    }
    parent->setLayout(layout);
}
