#include "databaseconnection.h"

#include <QDebug>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

#include "mysql.h"

#ifdef MANGO_COMMON_NAMESPACE
using namespace MANGO_COMMON_NAMESPACE;
#endif

const char *DatabaseConnection::CONNECTION_NAME     = const_cast<char *>("crystal");
const char *DatabaseConnection::SAVEPOINT_NAMESPACE = const_cast<char *>("export");

/*!
    \class DatabaseConnection
    \brief The DatabaseConnection class handles a connection to the database.

    \ingroup database
    \inmodule Export

    Wrapper around QSqlDatabase class, many methods are only proxies with
    some error handling.
    Savepoints (nested transactions) are not managed automatically like eg
    in Laravel's Eloquent ORM, because I want to be more explicit, so when
    I need to start Savepoint, I will call savepoint() method and not
    transcation(). The same is true for rollback(), so I will not call
    rollback() for both, to end transaction and to end savepoint, instead,
    I will call rollback() for transaction and rollbackToSavepoint("xx_1")
    for savepoint. This makes it clear at a glance what is happening.
*/

DatabaseConnection::DatabaseConnection()
{
    auto db = QSqlDatabase::addDatabase("QMYSQL", CONNECTION_NAME);
    db.setHostName("127.0.0.1");
#ifdef QT_DEBUG
    db.setDatabaseName("q_media_test");
#else
    db.setDatabaseName("q_media");
#endif
    db.setUserName("szachara");
    db.setPassword("99delfinu*");

    if (db.open())
        return;

    qDebug() << "Connect to DB failed :"
             << db.lastError().text();
}

QSqlQuery DatabaseConnection::query() const
{
    return QSqlQuery(database());
}

QSqlDatabase DatabaseConnection::database()
{
    return QSqlDatabase::database(CONNECTION_NAME);
}

bool DatabaseConnection::pingDatabase()
{
    auto db = database();

    const auto getMysqlHandle = [&db]() -> MYSQL *
    {
        auto driverHandle = db.driver()->handle();
        if (qstrcmp(driverHandle.typeName(), "MYSQL*") == 0)
            return *static_cast<MYSQL **>(driverHandle.data());
        return nullptr;
    };
    const auto mysqlPing = [getMysqlHandle]()
    {
        auto mysqlHandle = getMysqlHandle();
        if (mysqlHandle == nullptr)
            return false;
        auto ping = mysql_ping(mysqlHandle);
        auto errNo = mysql_errno(mysqlHandle);
        /* So strange logic, because I want interpret CR_COMMANDS_OUT_OF_SYNC errno as
           successful ping. */
        if ((ping != 0) && (errNo == CR_COMMANDS_OUT_OF_SYNC)) {
            // TODO log to file, how often this happen silverqx
            qWarning("mysql_ping() returned : CR_COMMANDS_OUT_OF_SYNC(%ud)", errNo);
            return true;
        }
        else if (ping == 0)
            return true;
        else if (ping != 0)
            return false;
        else {
            qWarning() << "Unknown behavior during mysql_ping(), this should never happen :/";
            return false;
        }
    };

    if (db.isOpen() && mysqlPing()) {
        showDbConnected();
        return true;
    }

    // The connection was lost here
    showDbDisconnected();
    // Database connection have to be closed manually
    // isOpen() check is called in MySQL driver
    db.close();
    // Reset in transaction state
    m_inTransaction = false;
    // Reset the savepoints counter
    m_savepoints = 0;
    return false;
}

bool DatabaseConnection::transaction()
{
    Q_ASSERT(m_inTransaction == false);
    const auto ok = database().transaction();
    if (!ok)
        return false;
    m_inTransaction = true;
    return true;
}

bool DatabaseConnection::commit()
{
    Q_ASSERT(m_inTransaction);
    const auto ok = database().commit();
    if (!ok)
        return false;
    m_inTransaction = false;
    return true;
}

bool DatabaseConnection::rollback()
{
    Q_ASSERT(m_inTransaction);
    const auto ok = database().rollback();
    if (!ok)
        return false;
    m_inTransaction = false;
    return true;
}

bool DatabaseConnection::savepoint(const QString &id)
{
    Q_ASSERT(m_inTransaction);
    auto savePoint = query();
    const auto query = QStringLiteral("SAVEPOINT %1_%2").arg(SAVEPOINT_NAMESPACE, id);
    // Execute a savepoint query
    const auto ok = savePoint.exec(query);
    Q_ASSERT_X(ok,
               "DatabaseConnection::savepoint(QString &id)",
               query.toUtf8().constData());
    if (!ok) {
        const auto savepointId = QStringLiteral("%1_%2").arg(SAVEPOINT_NAMESPACE, id);
        qDebug() << "Savepoint" << savepointId << "failed :"
                 << savePoint.lastError().text();
    }
    ++m_savepoints;
    return ok;
}

bool DatabaseConnection::rollbackToSavepoint(const QString &id)
{
    Q_ASSERT(m_inTransaction);
    Q_ASSERT(m_savepoints > 0);
    auto rollbackToSavepoint = query();
    const auto query = QStringLiteral("ROLLBACK TO SAVEPOINT %1_%2")
                       .arg(SAVEPOINT_NAMESPACE, id);
    // Execute a rollback to savepoint query
    const auto ok = rollbackToSavepoint.exec(query);
    Q_ASSERT_X(ok,
               "DatabaseConnection::rollbackToSavepoint(QString &id)",
               query.toUtf8().constData());
    if (!ok) {
        const auto savepointId = QStringLiteral("%1_%2").arg(SAVEPOINT_NAMESPACE, id);
        qDebug() << "Rollback to Savepoint" << savepointId << "failed :"
                 << rollbackToSavepoint.lastError().text();
    }
    m_savepoints = std::max<int>(0, m_savepoints - 1);
    return ok;
}

void DatabaseConnection::showDbDisconnected()
{
    if (m_dbDisconnectedShowed)
        return;
    m_dbDisconnectedShowed = true;

    // Reset connected flag
    m_dbConnectedShowed = false;

    qWarning() << "No active database connection, torrent additions / removes will "
                  "not be commited";
}

void DatabaseConnection::showDbConnected()
{
    if (m_dbConnectedShowed)
        return;
    m_dbConnectedShowed = true;

    // Reset disconnected flag
    m_dbDisconnectedShowed = false;

    qInfo() << "Database connected";
}
