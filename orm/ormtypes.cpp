#include "ormtypes.h"

#ifdef MANGO_COMMON_NAMESPACE
using namespace MANGO_COMMON_NAMESPACE;
#endif

AssignmentList::AssignmentList(const QVariantHash &variantHash)
{
    auto itHash = variantHash.constBegin();
    while (itHash != variantHash.constEnd()) {
        *this << AssignmentListItem({itHash.key(), itHash.value()});
        ++itHash;
    }
}
