#ifndef BASEPRESENTER_H
#define BASEPRESENTER_H

class QSqlQuery;

#ifdef MANGO_COMMON_NAMESPACE
namespace MANGO_COMMON_NAMESPACE::Presenter
{
#else
namespace Presenter
{
#endif
    class BasePresenter
    {
    public:
        explicit BasePresenter(QSqlQuery &query);

    protected:
        QSqlQuery &m_query;
    };
}

#endif // BASEPRESENTER_H
