#include "baserepository.h"

#include "core/repositoryerror.h"

#ifdef MANGO_COMMON_NAMESPACE
using namespace MANGO_COMMON_NAMESPACE;
#endif

BaseRepository::BaseRepository(EntityManager &em)
    : m_em(em)
{}

std::tuple<bool, QSqlQuery>
BaseRepository::update(const AssignmentList &updates, const quint64 id) const
{
    Q_ASSERT(!updates.isEmpty());
    Q_ASSERT(id > 0);

    // Prepare query
    auto query = m_em.query();
    query.prepare(QStringLiteral("UPDATE %1 SET %2 WHERE id = ?")
                  .arg(tableName(),
                       prepareAssignmentList(updates)));

    // Prepare query bindings
    auto itUpdate = updates.constBegin();
    while (itUpdate != updates.constEnd()) {
        query.addBindValue(itUpdate->value);
        ++itUpdate;
    }
    // id
    query.addBindValue(id);

    const auto ok = query.exec();
    if (!ok)
        qDebug() << "Update in BaseRepository::update(updates, id) failed :"
                 << query.lastError().text();

    return {ok, query};
}

std::tuple<bool, QSqlQuery>
BaseRepository::update(const AssignmentList &updates) const
{
    Q_ASSERT(!updates.isEmpty());

    // Prepare query
    auto query = m_em.query();
    query.prepare(QStringLiteral("UPDATE %1 SET %2")
                  .arg(tableName(),
                       prepareAssignmentList(updates)));

    // Prepare query bindings
    auto itUpdate = updates.constBegin();
    while (itUpdate != updates.constEnd()) {
        query.addBindValue(itUpdate->value);
        ++itUpdate;
    }

    const auto ok = query.exec();
    if (!ok)
        qDebug() << "Update in BaseRepository::update(updates) failed :"
                 << query.lastError().text();

    return {ok, query};
}

std::tuple<bool, QSqlQuery>
BaseRepository::updateWhere(const AssignmentList &updates,
                            const QVector<WhereConditionItem> &where) const
{
    Q_ASSERT(!updates.isEmpty());
    Q_ASSERT(!where.isEmpty());

    // Prepare query string
    auto query = m_em.query();
    query.prepare(QStringLiteral("UPDATE %1 SET %2 WHERE %3")
                  .arg(tableName(),
                       prepareAssignmentList(updates),
                       prepareWhereCondClause(where)));

    // Prepare query bindings for an assignment list
    auto itUpdate = updates.constBegin();
    while (itUpdate != updates.constEnd()) {
        query.addBindValue(itUpdate->value);
        ++itUpdate;
    }
    // Prepare query bindings for the WHERE clause
    auto itValue = where.constBegin();
    while (itValue != where.constEnd()) {
        query.addBindValue(itValue->value);
        ++itValue;
    }

    const auto ok = query.exec();
    if (!ok) {
        qDebug() << "Update torrents in updateWhere() failed :"
                 << query.lastError().text();
        qDebug() << "Query string :"
                 << query.lastQuery();
    }

    return {ok, query};
}

std::tuple<bool, QSqlQuery>
BaseRepository::deleteBy(const quint64 id) const
{
    Q_ASSERT(id > 0);

    // Prepare query
    auto query = m_em.query();
    query.prepare(QStringLiteral("DELETE FROM %1 WHERE id = ?")
                  .arg(tableName()));

    // Prepare the id query binding
    query.addBindValue(id);

    const auto ok = query.exec();
    if (!ok)
        qDebug() << "Delete in BaseRepository::deleteBy() failed :"
                 << query.lastError().text();

    return {ok, query};
}

std::tuple<bool, QSqlQuery>
BaseRepository::deleteWhere(const QVector<WhereConditionItem> &where) const
{
    Q_ASSERT(!where.isEmpty());

    // Prepare query
    auto query = m_em.query();
    query.prepare(QStringLiteral("DELETE FROM %1 WHERE %2")
                  .arg(tableName(),
                       prepareWhereCondClause(where)));

    // Prepare query bindings
    auto itItem = where.constBegin();
    while (itItem != where.constEnd()) {
        query.addBindValue(itItem->value);
        ++itItem;
    }

    const auto ok = query.exec();
    if (!ok)
        qDebug() << "Delete in BaseRepository::deleteWhere() failed :"
                 << query.lastError().text();

    return {ok, query};
}

QString BaseRepository::prepareWhereClause(const QVector<WhereItem> &expresions,
                                           const QString &condition) const
{
    QStringList result;
    const auto expressionTemplate = QStringLiteral("%1 %2 ?");
    auto itExpression = expresions.constBegin();
    while (itExpression != expresions.constEnd()) {
        result << expressionTemplate.arg(itExpression->column,
                                         itExpression->comparison);
        ++itExpression;
    }

    return result.join(QStringLiteral(" %1 ").arg(condition));
}

QString BaseRepository::prepareWhereCondClause(const QVector<WhereConditionItem> &expresions) const
{
    Q_ASSERT(!expresions.isEmpty());

    // TODO unify this where condition checks silverqx
    /* Have to be runtime check too, expressions list have to conatain at least one item,
       to make the code work properly. */
    if (expresions.isEmpty())
        throw RepositoryError("expressions parameter have to contain at least one item.");

    QStringList result;
    const auto expressionTemplate = QStringLiteral("%1 %2 ? %3");

    auto itExpression = expresions.constBegin();
    // Don't process last item
    while (itExpression != std::prev(expresions.constEnd())) {
        result += expressionTemplate.arg(itExpression->column,
                                         itExpression->comparison,
                                         itExpression->condition);
        ++itExpression;
    }

    // Last item doesn't contain a condition, process separately
    const auto lastExpression = expresions.last();
    result += QStringLiteral("%1 %2 ?").arg(lastExpression.column,
                                            lastExpression.comparison);

    return result.join(" ");
}

QString BaseRepository::prepareSelectClause(const QStringList &columns) const
{
    /* Qt don't know how to iterate the result with json column, so I have to manually manage
       columns in the select clause. */
    if ((columns.size() == 1) && (columns.at(0) == "*"))
        return this->columns();

    return columns.join(", ");
}

QString BaseRepository::prepareAssignmentList(const AssignmentList &attributes) const
{
    const auto attributeTemplate = QStringLiteral("%1 = ?");
    QStringList attributesList;

    auto itAttribute = attributes.constBegin();
    while (itAttribute != attributes.constEnd()) {
        attributesList << attributeTemplate.arg(itAttribute->column);
        ++itAttribute;
    }

    return attributesList.join(", ");
}

QSqlQuery BaseRepository::findWherePrivate(const QVector<WhereConditionItem> &where,
                                           const QStringList &select) const
{
    Q_ASSERT(!where.isEmpty());
    Q_ASSERT(!select.isEmpty());

    // Prepare query string
    auto query = m_em.query();
    query.setForwardOnly(m_forwardOnly);
    query.prepare(QStringLiteral("SELECT %1 FROM %2 WHERE %3")
                  .arg(prepareSelectClause(select),
                       tableName(),
                       prepareWhereCondClause(where)));

    // Prepare query bindings
    auto itValue = where.constBegin();
    while (itValue != where.constEnd()) {
        query.addBindValue(itValue->value);
        ++itValue;
    }

    return query;
}
