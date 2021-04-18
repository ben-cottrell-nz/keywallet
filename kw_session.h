#ifndef KW_SESSION_H
#define KW_SESSION_H

#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QVariant>
#include <QString>
#include "credential_store.h"

class kw_session : public QObject
{
    Q_OBJECT
public:
    kw_session(QObject *parent = nullptr);
    ~kw_session();
    Q_INVOKABLE credential_store* credential_model();
    Q_INVOKABLE QVariant is_first_session();
    Q_INVOKABLE bool authenticate(const QString& user, const QString& pass);
    Q_INVOKABLE bool add_user(const QString& user, const QString& pass);
private:
    credential_store _credential_store;
    QString get_salt();
    void generate_and_update_salt();
    void load_config_db();
    void open_db(const QString&);
    QSqlDatabase _sql_db;
};

#endif // KW_SESSION_H
