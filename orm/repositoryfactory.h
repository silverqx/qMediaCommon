#ifndef REPOSITORYFACTORY_H
#define REPOSITORYFACTORY_H

#include <typeindex>

#ifdef MANGO_COMMON_NAMESPACE
namespace MANGO_COMMON_NAMESPACE
{
#endif
    class BaseRepository;
    class EntityManager;

    /*! This factory is used to create default repository objects for entities at runtime. */
    class RepositoryFactory final
    {
        Q_DISABLE_COPY(RepositoryFactory)

    public:
        explicit RepositoryFactory(EntityManager &em);

        template<typename Repository>
        QSharedPointer<Repository> getRepository() const;

    private:
        EntityManager &m_em;
        mutable std::unordered_map<std::type_index, QSharedPointer<BaseRepository>> m_repositoryList;
    };

    template<typename Repository>
    QSharedPointer<Repository> RepositoryFactory::getRepository() const
    {
        std::type_index index(typeid (Repository));
        if (m_repositoryList.count(index) == 0) {
            auto repository = QSharedPointer<Repository>::create(m_em);
            m_repositoryList[index] = repository;
            return repository;
        }

        return m_repositoryList.at(index).dynamicCast<Repository>();
    }
#ifdef MANGO_COMMON_NAMESPACE
}
#endif

#endif // REPOSITORYFACTORY_H
