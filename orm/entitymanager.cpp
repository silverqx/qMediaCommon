#include "entitymanager.h"

#include <QtSql/QSqlQuery>

#ifdef MANGO_COMMON_NAMESPACE
using namespace MANGO_COMMON_NAMESPACE;
#endif

/*!
    \class EntityManager
    \brief The EntityManager class manages repositories and a connection
    to the database.

    \ingroup database
    \inmodule Export

    EntityManager is the base class to work with the database, it creates
    and manages repository classes by helping with the RepositoryFactory
    class.
    Creates the database connection which is represented by
    DatabaseConnection class.
    EntityManager should be used in controllers ( currently TorrentExporter
    is like a controller class ), services, and repository classes to access
    the database. There is no need to use the QSqlDatabase or the
    DatabaseConnection classes directly.
    EntityManager is also injected into a repository and a service
    classes constructors.
    The circular dependency problem is solved by including entitymanager.h
    in the baserepository.h file.
*/

EntityManager::EntityManager()
    : m_repositoryFactory(*this)
{}

QSqlQuery EntityManager::query() const
{
    return m_db.query();
}

bool EntityManager::pingDatabase()
{
    return m_db.pingDatabase();
}

bool EntityManager::transaction()
{
    return m_db.transaction();
}

bool EntityManager::commit()
{
    return m_db.commit();
}

bool EntityManager::rollback()
{
    return m_db.rollback();
}

bool EntityManager::savepoint(const QString &id)
{
    return m_db.savepoint(id);
}

bool EntityManager::rollbackToSavepoint(const QString &id)
{
    return m_db.rollbackToSavepoint(id);
}
