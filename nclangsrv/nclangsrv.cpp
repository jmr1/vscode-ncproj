#include "stdafx.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

#include <boost/program_options.hpp>
#include <filesystem>

#include <GeneralParserDefines.h>

#include "JsonMessageHandler.h"
#include "Logger.h"
#include "NCSettingsReader.h"

namespace fs = std::filesystem;
namespace po = boost::program_options;

#define VERIFY_OPTION(LOGGER, NAME, VARIABLE)                                                                          \
    if (vm.count(NAME))                                                                                                \
    {                                                                                                                  \
        LOGGER << NAME << " was set to [" << VARIABLE << "]." << std::endl;                                            \
    }                                                                                                                  \
    else                                                                                                               \
    {                                                                                                                  \
        LOGGER << "ERROR: " << NAME << " was not set." << std::endl;                                                   \
        return 1;                                                                                                      \
    }

#define OPTIONAL_OPTION(LOGGER, NAME, VARIABLE)                                                                        \
    if (vm.count(NAME))                                                                                                \
    {                                                                                                                  \
        LOGGER << NAME << " was set to [" << VARIABLE << "]." << std::endl;                                            \
    }

#define LOGGER (*logger)()

int main(int argc, char* argv[])
{
    parser::AxesRotatingOption axes_rotating_option;
    bool                       single_line_output;
    bool                       convert_length{};
    bool                       calculate_path_time{};
    bool                       rotate{};
    std::string                ncsetting_path;
    std::string                macro_desc_path;
    std::string                log_path;

    // Declare the supported options.
    po::options_description desc("Allowed options");
    // clang-format off
    desc.add_options()
        ("help,h", "produce help message")
        ("single-line-output,s", po::value<bool>(&single_line_output)->default_value(true), "whether the error output is single line")
        ("convert-length", po::value<bool>(&convert_length)->default_value(false), "whether to convert length units")
        ("calculate-path", po::value<bool>(&calculate_path_time)->default_value(false), "whether to calculate path length and time")
        ("rotate", po::value<bool>(&rotate)->default_value(false), "whether to rotate carthesian system")
        ("rotate-option", po::value<parser::AxesRotatingOption>(&axes_rotating_option)->default_value(parser::AxesRotatingOption::Xrotate90degrees), "Axes rotating option: [X90, X180, X270, Y90, Y180, Y270, Z90, Z180, Z270]")
        ("ncsetting-path,n", po::value<std::string>(&ncsetting_path), "path to ncsettings file")
        ("macro-desc-path,m", po::value<std::string>(&macro_desc_path), "path to macro descriptions json file")
        ("log-path,l", po::value<std::string>(&log_path), "path to log file")
    ;
    // clang-format on

    po::positional_options_description p;

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << std::endl;
        return 1;
    }

    std::unique_ptr<nclangsrv::Logger> logger;
    if (not log_path.empty())
    {
        logger = std::make_unique<nclangsrv::Logger>(log_path.c_str() /*, std::ios::app*/);
        LOGGER << "log path: " << log_path << std::endl;
    }

    if (logger)
    {
        VERIFY_OPTION(LOGGER, "single-line-output", single_line_output);
        VERIFY_OPTION(LOGGER, "convert-length", convert_length);
        VERIFY_OPTION(LOGGER, "calculate-path", calculate_path_time);
        VERIFY_OPTION(LOGGER, "rotate", rotate);
        VERIFY_OPTION(LOGGER, "rotate-option", axes_rotating_option);
        OPTIONAL_OPTION(LOGGER, "log-path", log_path);
        OPTIONAL_OPTION(LOGGER, "ncsetting-path", ncsetting_path);
        OPTIONAL_OPTION(LOGGER, "macro-desc-path", macro_desc_path);
    }

    const auto                    executablePath = fs::path(argv[0]);
    nclangsrv::NCSettingsReader   ncSettingsReader(ncsetting_path, logger.get());
    nclangsrv::JsonMessageHandler jsonMessageHandler(logger.get(), executablePath.parent_path().string(),
                                                     ncSettingsReader, calculate_path_time, macro_desc_path);

    if (logger)
        LOGGER << __func__ << ": Current path is " << fs::current_path() << std::endl;

    while (true)
    {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1ms);

        char        c;
        std::string str;

        while (std::cin.get(c))
        {
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(1ms);
            if (str.empty() and c != 'C')
                continue;
            str += c;
            if (str == "Content-Length: ")
                break;
        }

        if (str.empty())
            continue;

        str.clear();

        while (std::cin.get(c))
        {
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(1ms);
            if (c == '\r' or c == '\n')
                break;
            str += c;
        }

        uint32_t length = std::atoi(str.c_str());
        if (logger)
            LOGGER << __func__ << ": Content-Length: [" << str << "(" << str.size() << ")"
                   << "], " << length << std::endl;
        if (length == 0)
        {
            continue;
        }

        std::string buffer;
        while (std::cin.get(c))
        {
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(1ms);
            if (c == '\r' or c == '\n')
                continue;
            buffer += c;
            break;
        }
        for (uint32_t x = 1; x < length; ++x)
            buffer += std::cin.get();
        if (logger)
            LOGGER << __func__ << ": " << buffer << std::endl;

        jsonMessageHandler.parse(buffer);
        if (jsonMessageHandler.exit())
        {
            if (logger)
            {
                logger->flush();
                logger->close();
            }
            break;
        }
    }

    return 0;
}