#pragma once

#include "cfgbase.hpp"

struct TConf {
    cfg_inst_t conf_instance = { "test.conf", "test", sizeof(*this) };
    struct _String {
        cfg_group_t __header = { "String", sizeof(*this) };
        cfg_string string = { "VarName", "VarValue" };
    } String;
    struct _Integer {
        cfg_group_t __header = { "Integer", sizeof(*this) };
        cfg_int32_t i32 = { "i32", -0 };
        cfg_uint32_t ui32 = { "ui32", 0 };
        cfg_int32_t i64 = { "i64", -0 };
        cfg_int32_t ui64 = { "ui64", 0 };
    } Integer;
    struct _Real {
        cfg_group_t __header = { "Real", sizeof(*this) };
        cfg_float floating = { "floating", .0 };
        cfg_double dblsize = { "dblsize", .0 };
    } Real;
};
