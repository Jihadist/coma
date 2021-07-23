#pragma once
#include "../ctti/type_id.hpp"
#include "../gen/modules.h"

#include <variant>
namespace delegate
{

enum class ItemType : int
{
    ModbusItem
};

using WidgetGroup = int;

struct Widget
{
    Widget(const ctti::unnamed_type_id_t type_) : type(type_)
    {
    }
    Widget(const ctti::unnamed_type_id_t type_, const WidgetGroup group_) : type(type_), group(group_)
    {
    }
    Widget(const ctti::unnamed_type_id_t type_, const QString &desc_, const WidgetGroup group_, const QString &toolTip_)
        : type(type_), desc(desc_), group(group_), toolTip(toolTip_)
    {
    }
    ctti::unnamed_type_id_t type;
    QString desc;
    WidgetGroup group;
    QString toolTip;
};

struct DoubleSpinBoxWidget : Widget
{
    DoubleSpinBoxWidget(const ctti::unnamed_type_id_t type_, const WidgetGroup group_) : Widget(type_, group_)
    {
    }
    double min;
    double max;
    int decimals;
};
struct Group
{
    int count;
    QStringList items;
};
struct DoubleSpinBoxGroup : DoubleSpinBoxWidget, Group
{
    DoubleSpinBoxGroup(const ctti::unnamed_type_id_t type_, const WidgetGroup group_)
        : DoubleSpinBoxWidget(type_, group_)
    {
    }
};
struct CheckBoxGroup : Widget, Group
{
    CheckBoxGroup(const ctti::unnamed_type_id_t type_, const WidgetGroup group_) : Widget(type_, group_)
    {
    }
};

struct QComboBox : Widget, Group
{
    enum PrimaryField : int
    {
        index = 0,
        data = 1,
        bitfield = 2
    };
    QComboBox(const ctti::unnamed_type_id_t type_, const WidgetGroup group_) : Widget(type_, group_)
    {
    }
    PrimaryField primaryField = index;
};
struct Item : Widget
{
    enum ModbusColumns : int
    {
        SensorType = 0,
        BaudRate,
        Parity,
        StopBits,
        Timeout,
        Address,
        FuncCode,
        DataType,
        Register
    };
    Item(const ctti::unnamed_type_id_t type_) : Widget(type_)
    {
    }
    Item(const ctti::unnamed_type_id_t type_, const ItemType itype_) : Widget(type_), itemType(itype_)
    {
    }
    Item(const ctti::unnamed_type_id_t type_, const ItemType itype_, const BciNumber parent_)
        : Widget(type_), itemType(itype_), parent(parent_)
    {
    }
    Item(const ctti::unnamed_type_id_t type_, const ItemType itype_, const BciNumber parent_, const WidgetGroup group_)
        : Widget(type_, group_), itemType(itype_), parent(parent_)
    {
    }
    ItemType itemType;
    BciNumber parent;
};

template <class... Args> using tempVariant = std::variant<std::variant<Args...>, Args...>;

using itemVariant = std::variant<Widget, QComboBox, DoubleSpinBoxGroup, DoubleSpinBoxWidget, CheckBoxGroup, Item>;
}
using categoryMap = QMap<delegate::WidgetGroup, QString>;
using widgetMap = std::map<BciNumber, delegate::itemVariant>;
