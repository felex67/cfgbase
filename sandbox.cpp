#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <cstdio>

std::vector<std::string> split_path(const std::string &Src) {
    std::vector<std::string> vpath;
    size_t pos = (Src[0] == '/' ? 1 : 0), len = Src.length();
    const char *src = Src.c_str();
    for (size_t i = 0; i < len; i++) {
        std::string t;
        if ('\n' != src[i]);
        else {
            t = Src.substr(pos, i - pos);
            if (1 < t.length()) vpath.push_back(t);
            pos = ++i;
        }
    }
    if (pos < len) vpath.push_back(Src.substr(pos));
    return vpath;
}

int main(const int argc, const char* argv[]) {
    std::vector<std::string> vpath = split_path(std::getenv("PWD"));
    std::string r;
    for (auto i = vpath.begin(); vpath.end() != i; i++) {
        r += ("/" + *i);
    }
    std::cout << r << std::endl;
    return 0;
}