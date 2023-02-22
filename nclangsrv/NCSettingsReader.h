#pragma once

#include <string>

#include <GeneralParserDefines.h>

namespace nclangsrv {
class NCSettingsReader
{
public:
    NCSettingsReader(const std::string& ncSettingsPath);

    bool read();

    std::string getNcSettingsPath() const
    {
        return mNcSettingsPath;
    }
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
    parser::EMachineTool getMachineTool() const
    {
        return mMachineTool;
    }
    parser::EMachineToolType getMachineToolType() const
    {
        return mMachineToolType;
    }
    parser::EFanucParserType getFanucParserType() const
    {
        return mFanucParserType;
    }

private:
    void setDefaults();

private:
    std::string               mNcSettingsPath;
    parser::ZeroPoint         mZeroPoint{};
    parser::MachinePointsData mMachinePointsData{};
    parser::Kinematics        mKinematics{};
    parser::CncDefaultValues  mCncDefaultValues{};
    parser::EMachineTool      mMachineTool{};
    parser::EMachineToolType  mMachineToolType{};
    parser::EFanucParserType  mFanucParserType{};
    bool                      mRead{};
};

} // namespace nclangsrv