#include "repositoryfactory.h"

#ifdef MANGO_COMMON_NAMESPACE
using namespace MANGO_COMMON_NAMESPACE;
#endif

RepositoryFactory::RepositoryFactory(EntityManager &em)
    : m_em(em)
{}
