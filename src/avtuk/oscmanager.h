#pragma once
#include "../gen/modules.h"
#include "../gen/std_ext.h"
#include "../gen/stdfunc.h"
#include "filemanager.h"
#include "trendviewdialog.h"

#include <optional>

class OscManager : public FileManager<S2DataTypes::OscHeader>
{
public:
    OscManager() = default;
    virtual ~OscManager();
    void loadOscFromFile(const QString &filename);
    void loadOsc(TrendViewModel *model);
    std::unique_ptr<TrendViewModel> load(const FileStruct &fs);
    std::unique_ptr<TrendViewModel> load(const Record &record, const FileStruct &fs);

    void loadSwjFromFile(const QString &filename);
    File::Vector loadFromFile(const QString &filename) override;
    bool loadRecords(const DataTypes::S2FilePack &input, File::Vector &output) override;

    Record loadCommon(const FileStruct &fs) const
    {
        assert(std_ext::to_underlying(fs.filenum) == MT_HEAD_ID);
        assert(fs.filedata.size() == sizeof(Record));
        Record record;
        memcpy(&record, fs.filedata.data(), sizeof(Record));
        return record;
    }
    void setHeader(Record &&header) noexcept
    {
        oscHeader = std::move(header);
    }
    void setHeader(const Record &header) noexcept
    {
        oscHeader = header;
    }

    static inline const auto isOsc = [](const DataTypes::S2Record &record) {
        // ##TODO add other oscs
        return ((record.ID == AVTUK_85::OSC_ID)                                             //
            || (record.ID == AVTUK_8X::OSC_ID)                                              //
            || ((record.ID >= AVTUK_21::OSC_ID_MIN) && (record.ID <= AVTUK_21::OSC_ID_MAX)) //
        );
    };

private:
    std::unique_ptr<TrendViewModel> model;
    UniquePointer<TrendViewDialog> trendDialog;
    std::optional<Record> oscHeader;
    const QStringList phases { "фазы А, В, С", "фаза А", "фаза В", "фаза С" };
};
