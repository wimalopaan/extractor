#include <cstdlib>

//[pragma
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused"
//[Abc
class Abc {
public:
    Abc() {}
private:
    int mX = 0;
};
//]
#pragma GCC diagnostic pop
//]
int main() {
    return EXIT_SUCCESS;
}
