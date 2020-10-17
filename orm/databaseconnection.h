#ifndef DATABASECONNECTION_H
#define DATABASECONNECTION_H

#include <QtSql/QSqlDatabase>

#ifdef MANGO_COMMON_NAMESPACE
namespace MANGO_COMMON_NAMESPACE
{
#endif
    class DatabaseConnection final
    {
        Q_DISABLE_COPY(DatabaseConnection)

    public:
        /*! Return underlying database connection. */
        static QSqlDatabase database();

        DatabaseConnection();

        QSqlQuery query() const;
        /*! Check database connection and show warnings when the state changed. */
        bool pingDatabase();
        bool transaction();
        bool commit();
        bool rollback();
        bool savepoint(const QString &id);
        bool rollbackToSavepoint(const QString &id);

    private:
        static const char *CONNECTION_NAME;
        static const char *SAVEPOINT_NAMESPACE;

        void showDbDisconnected();
        void showDbConnected();

        bool m_dbDisconnectedShowed = false;
        bool m_dbConnectedShowed = false;
        /*! The connection is in the transaction state. */
        bool m_inTransaction = false;
        /*! Active savepoints counter. */
        uint m_savepoints = 0;
    };
#ifdef MANGO_COMMON_NAMESPACE
}
#endif

#endif // DATABASECONNECTION_H
