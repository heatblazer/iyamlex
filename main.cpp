#include "yaml2ex.hpp"

int main(int argc, char** argv)
{
    YamlParser yp;
    yp.Load(argv[1]);
    yp.Parse();
    return 0;
}
