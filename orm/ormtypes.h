#ifndef ORMTYPES_H
#define ORMTYPES_H

#ifdef MANGO_COMMON_NAMESPACE
namespace MANGO_COMMON_NAMESPACE
{
#endif
    struct WhereItem
    {
        QString  column;
        QVariant value;
        QString  comparison {"="};
    };
    struct WhereConditionItem
    {
        QString  column;
        QVariant value;
        QString  comparison {"="};
        QString  condition  {"AND"};
    };
    struct AssignmentListItem
    {
        QString  column;
        QVariant value;
    };

    class AssignmentList final : public QVector<AssignmentListItem>
    {
    public:
        // Inherit all base class constructors, wow 😲✨
        using QVector<AssignmentListItem>::QVector;

        AssignmentList(const QVariantHash &variantHash);
    };
#ifdef MANGO_COMMON_NAMESPACE
}
#endif

#endif // ORMTYPES_H
