#ifndef BASESERVICE_H
#define BASESERVICE_H

#ifdef MANGO_COMMON_NAMESPACE
namespace MANGO_COMMON_NAMESPACE
{
#endif
    class EntityManager;

    class BaseService
    {
        Q_DISABLE_COPY(BaseService)

    public:
        explicit BaseService(EntityManager &em);

    protected:
        EntityManager &m_em;
    };
#ifdef MANGO_COMMON_NAMESPACE
}
#endif

#endif // BASESERVICE_H
