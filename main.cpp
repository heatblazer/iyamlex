#include "yaml2ex.hpp"

int main(int argc, char** argv)
{
    YamlParser yp;
    yp.Load(argv[1]);
    yp.Parse();
    auto rnode = yp.root();
    YamlParser::YamlNode* data = rnode["Producer"]->operator[]("knownApps");
    if (data) {
        for (auto it : data->map) {
            std::cout << it.first << std::endl;
        }
    }
    return 0;
}
