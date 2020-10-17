#ifndef REPOSITORYERROR_H
#define REPOSITORYERROR_H

#ifdef MANGO_COMMON_NAMESPACE
namespace MANGO_COMMON_NAMESPACE
{
#endif
    class RepositoryError final : public std::runtime_error
    {
    public:
        explicit inline RepositoryError(const char *Message)
            : std::runtime_error(Message)
        {}
    };
#ifdef MANGO_COMMON_NAMESPACE
}
#endif

#endif // REPOSITORYERROR_H
