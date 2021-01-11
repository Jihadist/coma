#pragma once
#include "interfacesettings.h"
#include "modbusprivate.h"

#include <QMap>
#include <QtXml/QDomElement>
namespace CommandsMBS
{
// Q_NAMESPACE

// struct ModbusGroup
//{
//    using FuncCode = CommandsMBS::Commands;
//    ModbusGroup(QDomElement domElement);
//    QString id;
//    FuncCode function;
//    TypeId dataType;
//    quint32 startAddr;
//    quint32 count;
//};
struct ModbusGroup : BaseGroup<CommandsMBS::Commands, CommandsMBS::TypeId>
{
    ModbusGroup(QDomElement domElement);
};
}
// class ModbusSettings
//{
// public:
//    using Group = CommandsMBS::ModbusGroup;
//    ModbusSettings();

//    void addGroup(const Group &gr);
//    void clearGroups();

//    QList<Group> groups() const;

//    QMap<quint32, Group> dictionary() const;

// private:
//    QList<Group> m_groups;
//    QMap<quint32, Group> m_dictionary;
//};
