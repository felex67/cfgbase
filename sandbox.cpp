#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <cstdio>



int main(const int argc, const char* argv[]) {
    std::vector<std::string> vsrc;
    std::string Src;
    FILE *fin = fopen("log.cfg", "rb");
    struct stat fstt;
    char *buff = nullptr;
    char *ptr;
    stat("log.cfg", &fstt);
    buff = new char[fstt.st_size + 1];
    fread(buff, fstt.st_size, 1, fin);
    buff[fstt.st_size] = 0;
    Src = buff;
    delete[] buff;
    size_t now, pos = 0;
    while (pos < (now = Src.find('\n'))) {
        vsrc.push_back(Src.substr(pos, now - pos));
        pos = now + 1;
    }
    if (pos < (Src.size() - 1)) vsrc.push_back(Src.substr(pos));
    for (auto i = vsrc.begin(); i != vsrc.end(); i++) {
        std::cout << *i << std::endl;
    }
    return 0;
}