#pragma once
#include "../gen/datatypes.h"
#include "../gen/modules.h"
#include "../gen/timefunc.h"
#include "trendviewmodel.h"

#include <QAbstractItemModel>
#include <QByteArray>
#include <QVariant>
#include <memory>

struct SwjModel
{
    std::unique_ptr<QAbstractItemModel> commonModel;
    std::unique_ptr<QAbstractItemModel> detailModel;
};

namespace File
{
using Variant = std::variant<S2DataTypes::OscHeader, std::unique_ptr<TrendViewModel>, SwjModel>;
using Vector = std::vector<Variant>;
}

template <typename T> class FileManager
{
protected:
    using OscHeader = S2DataTypes::OscHeader;
    using FileStruct = DataTypes::FileStruct;
    using Record = T;

public:
    FileManager() = default;

    File::Vector virtual loadFromFile(const QString &filename) const = 0;
    bool virtual loadRecords(const DataTypes::S2FilePack &input, File::Vector &output) const = 0;
    void clearBuffer()
    {
        files.clear();
    }

protected:
    DataTypes::S2FilePack files;

    const static inline auto isOscHeader = [](const DataTypes::S2Record &record) { return (record.ID == MT_HEAD_ID); };
};
