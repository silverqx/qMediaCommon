#include "baseservice.h"

#ifdef MANGO_COMMON_NAMESPACE
using namespace MANGO_COMMON_NAMESPACE;
#endif

BaseService::BaseService(EntityManager &em)
    : m_em(em)
{}
