#include <iostream>
#include "TConf.hpp"

int main (const int argc, const char* argv[]) {
    TConf dflt, work;
    cfg_inst_t &conf = work.conf_instance;
    dflt.conf_instance.set_name("default.conf");
    dflt.conf_instance.save();
    cfg_inst_t &winst = work.conf_instance;
    work.Integer.i32 = -123456;
    work.Integer.i64 = -987654321;
    work.Integer.ui32 = 1232456;
    work.Integer.ui64 = 987654321;
    work.Real.floating = 1.23456789;
    work.Real.dblsize = 123.456789;
    work.String.string = "Changed in code";
    winst.set_name("saved.conf");
    winst.save();
    winst.set_name("test.conf");
    winst.load();
    std::cout << winst.toString() << std::endl;
    winst.set_name("loaded.conf");
    winst.save();
    return 0;
}