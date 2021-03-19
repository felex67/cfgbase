#include <cstdio>
#include <iostream>

int main(const int argc, const char* argv[]) {
    std::string name = "hello";
    const char *var = "hello=world\n";
    const char *N = name.c_str();
    size_t n = name.length();
    for (size_t i = 0; i < n; i++) {
        if (N[i] == var[i]);
        else {
            std::cout << "Not same" << std::endl;
        }
    }
    std::cout << "Same" << std::endl;
    return 0;
}