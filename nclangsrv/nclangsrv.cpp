#include "stdafx.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

#include <filesystem>

#include "JsonMessageHandler.h"

namespace fs = std::filesystem;

int main(int argc, char* argv[])
{
    std::unique_ptr<std::ofstream> logger;

    if (argc > 1)
    {
        logger = std::make_unique<std::ofstream>(argv[1], std::ios::app);
        *logger << "argv[0]: " << argv[0] << std::endl;
    }

    const auto         executablePath = fs::path(argv[0]);
    JsonMessageHandler jsonMessageHandler(logger.get(), executablePath.parent_path().string());

    if (logger)
        *logger << __func__ << ": Current path is " << fs::current_path() << std::endl;

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
            *logger << __func__ << ": Content-Length: [" << str << "(" << str.size() << ")"
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
            *logger << __func__ << ": " << buffer << std::endl;

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