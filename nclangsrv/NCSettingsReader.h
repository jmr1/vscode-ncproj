#pragma once

#include <string>

#include <GeneralParserDefines.h>

namespace nclangsrv {
class NCSettingsReader
{
public:
    NCSettingsReader(const std::string& ncSettingsPath);

    bool read();

    parser::ZeroPoint getZeroPoint() const
    {
        return mZeroPoint;
    }
    parser::MachinePointsData getMachinePointsData() const
    {
        return mMachinePointsData;
    }
    parser::Kinematics getKinematics() const
    {
        return mKinematics;
    }
    parser::CncDefaultValues getCncDefaultValues() const
    {
        return mCncDefaultValues;
    }
    parser::EFanucParserType getFanucParserType() const
    {
        return mFanucParserType;
    }

private:
    std::string               mNcSettingsPath;
    parser::ZeroPoint         mZeroPoint{};
    parser::MachinePointsData mMachinePointsData{};
    parser::Kinematics        mKinematics{};
    parser::CncDefaultValues  mCncDefaultValues{};
    parser::EFanucParserType  mFanucParserType{};
    bool                      mRead{};
};

} // namespace nclangsrv