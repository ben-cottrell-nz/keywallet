#ifndef CREDENTIAL_STORE_H
#define CREDENTIAL_STORE_H

#include <QObject>
#include <QAbstractTableModel>

class credential_store : public QAbstractTableModel
{
    Q_OBJECT
public:
    struct entry {
        QString name;
        QString value;
    };
    int rowCount(const QModelIndex&) const override;
    int columnCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    QVector<entry>& entries();
private:
    QVector<entry> _credentials;
signals:

};

#endif // CREDENTIAL_STORE_H
