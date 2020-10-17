#include "basepresenter.h"

#ifdef MANGO_COMMON_NAMESPACE
using namespace MANGO_COMMON_NAMESPACE;
#endif

Presenter::BasePresenter::BasePresenter(QSqlQuery &query)
    : m_query(query)
{}
