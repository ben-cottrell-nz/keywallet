#include "credential_store.h"

int credential_store::rowCount(const QModelIndex &) const
{
    return _credentials.size();
}

int credential_store::columnCount(const QModelIndex &) const
{
    return 2;
}

QVariant credential_store::data(const QModelIndex &index, int role) const
{
    switch (role) {
     case Qt::DisplayRole:
        {
            QString name, value;
            name = _credentials.at(index.row()).name;
            value = _credentials.at(index.row()).value;
            return QString("%1, %2").arg(name).arg(value);
        }
     default:
        break;
     }

     return QVariant();
}

QHash<int, QByteArray> credential_store::roleNames() const
{
    return { {Qt::DisplayRole, "display"} };
}

QVector<credential_store::entry>& credential_store::entries()
{
    return _credentials;
}
