#include "yaml2ex.hpp"

int main(int argc, char** argv)
{
    YamlParser yp;
    yp.Load(argv[1]);
    yp.Parse();
    auto rnode = yp.root();
    YamlParser::YamlNode* knownApps= rnode["Producer"]->operator[]("knownApps");
    if (knownApps) {
        for (auto it : knownApps->map) {
            std::cout << it.first << std::endl;
        }
    }
    YamlParser::YamlNode* waapath= rnode["Producer"]->operator[]("waaPath");
    if (waapath) {
        for (auto it : waapath->map) {
            std::cout << it.first << std::endl;
            std::cout << it.second->k << std::endl;

        }
    }
    return 0;
}
