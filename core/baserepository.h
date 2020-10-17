#ifndef BASEREPOSITORY_H
#define BASEREPOSITORY_H

#include <QDebug>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

#include "orm/entitymanager.h"
#include "orm/ormtypes.h"

#ifdef MANGO_COMMON_NAMESPACE
namespace MANGO_COMMON_NAMESPACE
{
#endif
    class BaseRepository
    {
        Q_DISABLE_COPY(BaseRepository)

    public:
        explicit BaseRepository(EntityManager &m_em);
        virtual ~BaseRepository() = default;

        virtual const QString &tableName() const = 0;
        /*! Columns, when select clause contains *. */
        virtual const QString &columns() const = 0;

        inline void setForwardOnly(const bool forwardOnly)
        { m_forwardOnly = forwardOnly; }
        inline void disableForwardOnly()
        { m_forwardOnly = false; }
        inline void enableForwardOnly()
        { m_forwardOnly = true; }

        template<typename Presenter = void>
        auto findWhere(const QVector<WhereConditionItem> &where,
                       const QStringList &select = {{"*"}}) const;

        template<typename Presenter, typename PresenterData>
        auto findWhere(const QVector<WhereConditionItem> &where,
                       const PresenterData &data,
                       const QStringList &select = {{"*"}}) const;

        template<typename Presenter = void, typename T>
        auto findWhereIn(const QString &column, const QVector<T> &whereIn,
                         const QStringList &select = {{"*"}}) const;

        template<typename Presenter, typename T, typename PresenterData>
        auto findWhereIn(const QString &column, const QVector<T> &whereIn,
                         const PresenterData &data,
                         const QStringList &select = {{"*"}}) const;

        std::tuple<bool, QSqlQuery>
        update(const AssignmentList &updates, quint64 id) const;

        /*! Update all rows. */
        std::tuple<bool, QSqlQuery>
        update(const AssignmentList &updates) const;

        template<typename T>
        std::tuple<bool, QSqlQuery>
        updateWhereIn(const AssignmentList &updates, const QString &column,
                      const QVector<T> &whereIn) const;

        std::tuple<bool, QSqlQuery>
        updateWhere(const AssignmentList &updates,
                    const QVector<WhereConditionItem> &where) const;

        std::tuple<bool, QSqlQuery>
        deleteBy(quint64 id) const;

        std::tuple<bool, QSqlQuery>
        deleteWhere(const QVector<WhereConditionItem> &where) const;

    protected:
        template<typename Presenter>
        inline auto presentResult(QSqlQuery &query) const
        { return Presenter(query).present(); }

        /*! Special case, when Presenter is void and query was success, first return value is
            the result returned from query.exec() command and the second value is the instance
            of the query ( reference ). */
        template<>
        inline auto presentResult<void>(QSqlQuery &query) const
        { return std::make_tuple(true, query); }

        template<typename Presenter, typename PresenterData>
        inline auto presentResult(QSqlQuery &query, const PresenterData &data) const
        { return Presenter(query).present(data); }

        template<typename Presenter>
        inline auto defaultReturnValue(QSqlQuery &) const
        { return std::invoke_result_t<decltype(&Presenter::present), Presenter>(); }

        /*! Special case, when Presenter is void and query failed, first return value is
            the result returned from query.exec() command and the second value is the instance
            of failed query ( reference ). */
        template<>
        inline auto defaultReturnValue<void>(QSqlQuery &query) const
        { return std::make_tuple(false, query); }

        template<typename Presenter, typename PresenterData>
        inline auto defaultReturnValue(QSqlQuery &, const PresenterData &) const
        { return std::invoke_result_t<decltype(&Presenter::present), Presenter, PresenterData &>(); }

        /*! Used in all SELECT queries. */
        bool m_forwardOnly = true;
        EntityManager &m_em;

    private:
        /* Qt doesn't provide query builder, so I had to write this simple parsing methods. */
        template<typename T>
        QString prepareWhereInClause(const QString &column, const QVector<T> &values) const;
        QString prepareWhereClause(const QVector<WhereItem> &expresions,
                                   const QString &condition = "AND") const;
        QString prepareWhereCondClause(const QVector<WhereConditionItem> &expresions) const;
        QString prepareSelectClause(const QStringList &columns = {{"*"}}) const;
        QString prepareAssignmentList(const AssignmentList &attributes) const;

        QSqlQuery findWherePrivate(const QVector<WhereConditionItem> &where,
                                   const QStringList &select = {{"*"}}) const;
        template<typename T>
        QSqlQuery findWhereInPrivate(const QString &column, const QVector<T> &whereIn,
                                     const QStringList &select = {{"*"}}) const;
    };

    template<typename Presenter>
    auto BaseRepository::findWhere(const QVector<WhereConditionItem> &where,
                                   const QStringList &select) const
    {
        auto query = findWherePrivate(where, select);

        if (query.exec())
            return presentResult<Presenter>(query);

        qDebug() << "Select torrents in findWhere<Presenter>() failed :"
                 << query.lastError().text();
        qDebug() << "Query string :"
                 << query.lastQuery();
        return defaultReturnValue<Presenter>(query);
    }

    template<typename Presenter, typename PresenterData>
    auto BaseRepository::findWhere(const QVector<WhereConditionItem> &where,
                                   const PresenterData &data,
                                   const QStringList &select) const
    {
        auto query = findWherePrivate(where, select);

        if (query.exec())
            return presentResult<Presenter, PresenterData>(query, data);

        qDebug() << "Select torrents in findWhere<Presenter>() failed :"
                 << query.lastError().text();
        qDebug() << "Query string :"
                 << query.lastQuery();
        return defaultReturnValue<Presenter, PresenterData>(query, data);
    }

    template<typename Presenter, typename T>
    auto BaseRepository::findWhereIn(const QString &column, const QVector<T> &whereIn,
                                     const QStringList &select) const
    {
        auto query = findWhereInPrivate(column, whereIn, select);

        if (query.exec())
            return presentResult<Presenter>(query);

        qDebug() << "Select torrents in findWhereIn<Presenter, T>() failed :"
                 << query.lastError().text();
        qDebug() << "Query string :"
                 << query.lastQuery();
        return defaultReturnValue<Presenter>(query);
    }

    template<typename Presenter, typename T, typename PresenterData>
    auto BaseRepository::findWhereIn(const QString &column, const QVector<T> &whereIn,
                                     const PresenterData &data,
                                     const QStringList &select) const
    {
        auto query = findWhereInPrivate(column, whereIn, select);

        if (query.exec())
            return presentResult<Presenter, PresenterData>(query, data);

        qDebug() << "Select torrents in findWhereIn<Presenter, T>() failed :"
                 << query.lastError().text();
        qDebug() << "Query string :"
                 << query.lastQuery();
        return defaultReturnValue<Presenter, PresenterData>(query, data);
    }

    template<typename T>
    std::tuple<bool, QSqlQuery>
    BaseRepository::updateWhereIn(const AssignmentList &updates,
                                  const QString &column,
                                  const QVector<T> &whereIn) const
    {
        Q_ASSERT(!updates.isEmpty());
        Q_ASSERT(!whereIn.isEmpty());

        // Prepare query string
        auto query = m_em.query();
        query.prepare(QStringLiteral("UPDATE %1 SET %2 WHERE %3")
                      .arg(tableName(),
                           prepareAssignmentList(updates),
                           prepareWhereInClause(column, whereIn)));

        // Prepare query bindings for an assignment list
        auto itUpdate = updates.constBegin();
        while (itUpdate != updates.constEnd()) {
            query.addBindValue(itUpdate->value);
            ++itUpdate;
        }
        // Prepare query bindings for the WHERE clause
        auto itValue = whereIn.constBegin();
        while (itValue != whereIn.constEnd()) {
            query.addBindValue(*itValue);
            ++itValue;
        }

        const auto ok = query.exec();
        if (!ok) {
            qDebug() << "Update torrents in updateWhereIn<T>() failed :"
                     << query.lastError().text();
            qDebug() << "Query string :"
                     << query.lastQuery();
        }

        return {ok, query};
    }

    template<typename T>
    QString BaseRepository::prepareWhereInClause(const QString &column,
                                                 const QVector<T> &values) const
    {
        // Assemble query binding placeholders
        auto placeholders = QStringLiteral("?, ").repeated(values.size());
        // Will never be empty, timer is triggered only after new torrent is added
        placeholders.chop(2);

        return QStringLiteral("%1 IN (%2)").arg(column, placeholders);
    }

    template<typename T>
    QSqlQuery BaseRepository::findWhereInPrivate(const QString &column,
                                                 const QVector<T> &whereIn,
                                                 const QStringList &select) const
    {
        Q_ASSERT(!column.isEmpty());
        Q_ASSERT(!whereIn.isEmpty());
        Q_ASSERT(!select.isEmpty());

        // Prepare query string
        auto query = m_em.query();
        query.setForwardOnly(m_forwardOnly);
        query.prepare(QStringLiteral("SELECT %1 FROM %2 WHERE %3")
                      .arg(prepareSelectClause(select),
                           tableName(),
                           prepareWhereInClause(column, whereIn)));

        // Prepare query bindings
        auto itValue = whereIn.constBegin();
        while (itValue != whereIn.constEnd()) {
            query.addBindValue(*itValue);
            ++itValue;
        }

        return query;
    }
#ifdef MANGO_COMMON_NAMESPACE
}
#endif

#endif // BASEREPOSITORY_H
