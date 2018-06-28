#include "effectlistmodel.h"

EffectListModel::EffectListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

QVariant EffectListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
}

int EffectListModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return effects.length();
}

QVariant EffectListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (role) {
    case Qt::DisplayRole:
        return QVariant(effects[index.row()].name);
    }

    return QVariant();
}

bool EffectListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    effects.insert(row, Effect());
    endInsertRows();
}

bool EffectListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    effects.removeAt(row);
    endRemoveRows();
}

QModelIndex EffectListModel::addEffect(const Effect& effect) {
    int i = effects.size();
    beginInsertRows(QModelIndex(), i, i);
    effects.push_back(Effect(effect));
    endInsertRows();
    return this->index(i);
}

Effect* EffectListModel::getEffect(const QModelIndex& index) {
    return &effects[index.row()];
}

void EffectListModel::moveRow(const QModelIndex &sourceParent, int sourceRow, const QModelIndex &destinationParent, int destinationChild) {
    if (destinationChild < rowCount() && destinationChild >= 0 && sourceRow != destinationChild) {
        Effect e = effects[destinationChild];
        effects[destinationChild] = effects[sourceRow];
        effects[sourceRow] = e;
        emit dataChanged(
                    index(std::min(sourceRow, destinationChild)),
                    index(std::max(sourceRow, destinationChild)));
    }
}
