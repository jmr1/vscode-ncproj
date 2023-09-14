#pragma once

#include <fstream>
#include <string>

namespace nclangsrv {

class Logger
{
public:
    explicit Logger(const std::string& path);

    std::ofstream& operator()();

    void flush();
    void close();

private:
    std::ofstream ostr;
};

} // namespace nclangsrv