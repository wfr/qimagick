#ifndef EFFECTLISTMODEL_H
#define EFFECTLISTMODEL_H

#include <QAbstractListModel>
#include <QList>
#include "effect.h"

class EffectListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit EffectListModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    void moveRow(const QModelIndex &sourceParent, int sourceRow, const QModelIndex &destinationParent, int destinationChild);

    QModelIndex addEffect(const Effect& effect);
    Effect* getEffect(const QModelIndex &index);

private:
    QList<Effect> effects;
};

#endif // EFFECTLISTMODEL_H
