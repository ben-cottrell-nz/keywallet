#include <QFile>
#include <QStandardPaths>
#include "kw_session.h"
//#include "modes.h"
#ifdef Q_OS_ANDROID
#if QT_VERSION_MAJOR < 6
#include <QtAndroid>
#else
#include <QJniEnvironment>
#include <QJniObject>
#endif
#endif
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>
#include <QSqlField>
#include <QDebug>
#include <iostream>
#include <QDir>
#include <openssl/aes.h>
#include <openssl/evp.h>
//SHA_DIGEST_LENGTH
#include <openssl/sha.h>
#include <sodium.h>
#include <sstream>
#include <ostream>
#include <iomanip>

//helper function to print the digest bytes as a hex string
std::string bytes_to_hex_string(const std::vector<uint8_t>& bytes)
{
    std::ostringstream stream;
    for (uint8_t b : bytes)
    {
        stream << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(b);
    }
    return stream.str();
}

//perform the SHA3-512 hash
std::string sha3_512(const std::string& input)
{
    uint32_t digest_length = SHA512_DIGEST_LENGTH;
    const EVP_MD* algorithm = EVP_sha3_512();
    uint8_t* digest = static_cast<uint8_t*>(OPENSSL_malloc(digest_length));
    EVP_MD_CTX* context = EVP_MD_CTX_new();
    EVP_DigestInit_ex(context, algorithm, nullptr);
    EVP_DigestUpdate(context, input.c_str(), input.size());
    EVP_DigestFinal_ex(context, digest, &digest_length);
    EVP_MD_CTX_destroy(context);
    std::string output = bytes_to_hex_string(std::vector<uint8_t>(digest, digest + digest_length));
    OPENSSL_free(digest);
    return output;
}

kw_session::kw_session(QObject *parent) : QObject(parent),
    _credential_store()
{
    qDebug("load_config_db");
    load_config_db();
}

kw_session::~kw_session()
{
    //TODO: regenerate salt
    _sql_db.close();
}

credential_store* kw_session::credential_model()
{
    return &_credential_store;
}

QVariant kw_session::is_first_session()
{
    using std::cout;
    if (get_salt() == "") {
        generate_and_update_salt();
    }
    QSqlQuery q(_sql_db);
    q.prepare("SELECT is_first_run FROM app_config LIMIT 1");
    if (q.exec() && q.isSelect()) {
        q.next();
    }
//    qDebug << "is_first_session called\n";
    //qDebug("%s: %d", __FUNCTION__ , q.record().field(0).value().toUInt());
    int value = q.value(0).toUInt();
//    qDebug("%s: %s", __FUNCTION__, q.value("is_first_run").toString());
    if (value == 1) {
        qDebug() << "changing is_first_run to 0";
        QSqlQuery update_query(_sql_db);
        update_query.prepare("UPDATE app_config SET is_first_run=0 WHERE ROWID=1");
        update_query.exec();
        _sql_db.commit();
    }
    qDebug() << "is_first_run: " << value;
    return value == 1;
}

bool kw_session::authenticate(const QString &user, const QString &pass)
{
    using std::string;
    QSqlQuery query(_sql_db);
    query.prepare("SELECT name,password FROM users WHERE name=:user");
    query.bindValue(":user", user);
    query.exec();
    if (query.exec() && query.isSelect()) {
        query.next();
    }
    if (query.size() == 0) {
        return false;
    }
    QString hashed_password;
    string salt = get_salt().toStdString();
    hashed_password = sha3_512(pass.toStdString()
                               + salt).c_str();
    //compare password hashes
    if (query.value("password").toString() == hashed_password) {

        return true;
    }
    return false;
}

bool kw_session::add_user(const QString &user, const QString &pass)
{
    QSqlQuery add_user_query(_sql_db);
    //uid,name,password
    add_user_query.prepare("INSERT INTO users(name,password) VALUES (:name,:password)");
    add_user_query.bindValue(":name",user);
    //check if the password meets criteria
    if (pass.length() < 6) {
        return false;
    }
    //hash the password by using the salt and the password as the
    //input to the SHA3 hash
    using std::string;
    QString hashed_password;
    string salt = get_salt().toStdString();
    hashed_password = sha3_512(pass.toStdString()
                               + salt).c_str();
    add_user_query.bindValue(":password",hashed_password);
    if (!add_user_query.exec()) {
        return false;
    }
    _sql_db.commit();
    return true;
}

QString kw_session::get_salt()
{
    QString salt;
    QSqlQuery query(_sql_db);
    query.prepare("SELECT salt FROM app_config WHERE ROWID=1");
    if (query.exec() && query.isSelect()) {
        query.next();
    }
    salt = query.value("salt").toString();
    return salt;
}

void kw_session::generate_and_update_salt()
{
    QString salt;
    using namespace std;
    const int BITS = 512;
    vector<uint8_t> salt_bytes;
    salt_bytes.resize(BITS/8);
    randombytes_buf(&salt_bytes[0], salt_bytes.size());
    salt = bytes_to_hex_string(salt_bytes).c_str();
    qDebug() << "new salt: " << salt;
    //update the db
    QSqlQuery query(_sql_db);
    query.prepare(
        "UPDATE app_config "
        "SET salt=:salt WHERE ROWID=1");
    query.bindValue(":salt",salt);
    query.exec();
}
#ifdef Q_OS_ANDROID
void KWSession::load_config_db()
{
    using namespace QtAndroid;
    //TODO: request permissions directly, look into qt source code to see how its done
    const QStringList android_perms = {
        "android.permission.READ_EXTERNAL_STORAGE",
        "android.permission.WRITE_EXTERNAL_STORAGE",
        };
    requestPermissions(android_perms, [&](PermissionResultMap map){
        Q_UNUSED(map)
    });
    const QString DATABASE_FILENAME = "config.db";
    QString db_file_path = ":/database/" + DATABASE_FILENAME;
    QString db_destination_path =
        QStandardPaths::writableLocation(
            QStandardPaths::AppLocalDataLocation)
            .append("/" + DATABASE_FILENAME);
    //copy the db to the writable data directory if it doesn't already exist
    if (!QFile::exists(db_destination_path)) {
        QFile db_file(db_file_path);
        QFile db_destination_file(db_destination_path);
        db_file.copy(db_destination_path);
    }
    open_db(db_destination_path);
}
#else
void kw_session::load_config_db()
{
    const QString DATABASE_FILENAME = "config.db";
    QString db_file_path = ":/database/" + DATABASE_FILENAME;
    QString destination_path_prefix = QStandardPaths::writableLocation(
        QStandardPaths::AppLocalDataLocation);
    QString db_destination_path =
        destination_path_prefix + "/" + DATABASE_FILENAME;
    if (!QDir(destination_path_prefix).exists()) {
        QDir(destination_path_prefix).mkpath(destination_path_prefix);
    }
    qDebug() << "db_destination_path: " << db_destination_path;
    //copy the db to the writable data directory if it doesn't already exist
    if (!QFile::exists(db_destination_path)) {
        QFile db_file(db_file_path);
        QFile db_destination_file(db_destination_path);
        db_file.copy(db_destination_path);
        QFile::setPermissions(db_destination_path, QFile::WriteOwner | QFile::ReadOwner);
    }
    open_db(db_destination_path);
}
#endif
void kw_session::open_db(const QString& db_destination_path)
{
    _sql_db = QSqlDatabase::addDatabase("QSQLITE","Connection");
    _sql_db.setDatabaseName(db_destination_path);
    if (!_sql_db.open()) {
        qDebug("%s: _sql_db.open() returned false", __FUNCTION__);
    }
}
