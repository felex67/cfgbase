#include <cstdio>
#include <iostream>

#include "config.hpp"

struct _LogConfig {
    cfg_inst_t Conf = {"log.conf", "config", sizeof(*this) };
    struct _Mode {
        cfg_group_t __group = { "Mode", sizeof(*this) };
        cfg_string_t RunAs = { "RunAs", "thread" };
    } Mode;
    struct _Path {
        cfg_group_t __group = { "Path", sizeof(*this) };
        cfg_string_t Pipe = { "Pipe", "/var/tmp/flxwd-logpipe.pipe" };
    } Path;
    struct _Format {
        cfg_group_t __group = { "Format", sizeof(*this) };
        cfg_string_t Datetime = { "Datetime", "%X %x" };
    } Format;
} LogConfig;

int main(const int argc, const char* argv[]) {
    LogConfig.Conf.load();
    std::cout << LogConfig.Conf.toString() << std::endl;
    LogConfig.Conf.save();
    return 0;
}