#ifndef LOGQUERY_H
#define LOGQUERY_H

#include <QtSql/QSqlQuery>

#ifdef MANGO_COMMON_NAMESPACE
namespace MANGO_COMMON_NAMESPACE
{
#endif

#ifdef QT_DEBUG
    Q_DECL_UNUSED
    QString parseExecutedQuery(const QSqlQuery &query);
#endif

#ifndef PARSE_EXECUTED_QUERY
#  ifdef QT_DEBUG
#    define PARSE_EXECUTED_QUERY(query) parseExecutedQuery(query)
#  else
#    define PARSE_EXECUTED_QUERY(query) \
        "PARSE_EXECUTED_QUERY() macro enabled only in QT_DEBUG mode."
#  endif
#endif

#ifdef QT_DEBUG
#  ifndef Q_CC_MSVC
    Q_NORETURN
#  endif
    Q_DECL_UNUSED
    void logExecutedQuery(const QSqlQuery &query);
#endif

#ifndef LOG_EXECUTED_QUERY
#  ifdef QT_DEBUG
#    define LOG_EXECUTED_QUERY(query) logExecutedQuery(query)
#  else
#    define LOG_EXECUTED_QUERY(query) qt_noop()
#  endif
#endif

#ifdef MANGO_COMMON_NAMESPACE
}
#endif

#endif // LOGQUERY_H
