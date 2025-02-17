#pragma once

#include "parsemodule.h"
class ParseID10020 : public ParseModule
{
public:
    ParseID10020(const QByteArray &BA);

    struct Point8x
    {
        std::array<float, 6> An;
    };

public:
    bool Parse(quint32 id, const S2DataTypes::OscHeader &header, TrendViewModel *model) override;
};
