#ifndef EFFECT_H
#define EFFECT_H

#include <QList>
#include <QStringList>

enum class ParameterType {
    BOOL,
    INT,
    COLOR,
    FLOAT,
};

class EffectParameter {
public:
    QString id;
    ParameterType type;
    float min, max, step, value;
};

class Effect
{
public:
    Effect(const QString& name = QString());
    Effect(const Effect& other);

    QString name, description;
    QList<EffectParameter> parameters;

    Effect& addFloatParameter(const QString& id, float min, float max, float value, float step = 0.1);
    Effect& addIntParameter(const QString& id, int min, int max, int value);

    Effect& setCommandFormat(const QStringList& args);
    QStringList getCommand();
private:
    QStringList args;
};

#endif // EFFECT_H
