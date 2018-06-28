#include "effect.h"
#include <QRegularExpression>
#include <QDebug>

Effect::Effect(const QString& name)
    : name(name)
{

}

Effect::Effect(const Effect& other) {
    name = other.name;
    description = other.description;
    for (const EffectParameter& param : other.parameters) {
        parameters.append(param);
    }
    args = other.args;
}

Effect& Effect::addFloatParameter(const QString& id, float min, float max, float value, float step) {
    EffectParameter param;
    param.type = ParameterType::FLOAT;
    param.id = id;
    param.min = min;
    param.max = max;
    param.step = step;
    param.value = value;
    this->parameters.append(param);
    return *this;
}

Effect& Effect::addIntParameter(const QString &id, int min, int max, int value) {
    EffectParameter param;
    param.type = ParameterType::INT;
    param.id = id;
    param.min = min;
    param.max = max;
    param.value = value;
    this->parameters.append(param);
    return *this;
}

Effect& Effect::setCommandFormat(const QStringList& args) {
    this->args = args;
    return *this;
}

QStringList Effect::getCommand() {
    QStringList result;

//    QRegularExpression re("\\${.*?}");
//    QRegularExpressionMatchIterator mi;
//    for (const QString& arg : args) {
//        mi = re.globalMatch(arg);
//        while (mi.hasNext()) {
//            QRegularExpressionMatch match = mi.next();
//        }
//        qDebug() << match.captured();
//        result.append(arg);
//    }

    for (const QString& arg : args) {
        QString x = arg;
        for (const EffectParameter& param : parameters) {
            QString before = QString("${%1}").arg(param.id);
            QString after = QString("%1").arg(param.value);
            x.replace(before, after);
        }
        result.append(x);
    }

    return result;
}
