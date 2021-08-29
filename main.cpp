#include "yaml2ex.hpp"

int main(int argc, char** argv)
{
    YamlParser yp;
    yp.Load(argv[1]);
    yp.Parse();
    auto rnode = yp.root();

    YamlParser::YamlNode* it = rnode->operator[]("Producer");
    std::cout << "Producer...\r\n";
    std::cout << "-------------------------\r\n";

    if (it) {
        for (auto ii : it->map) {
            std::cout << ii.first << std::endl;
        }
    }
    std::cout << "-------------------------\r\n";
    std::cout << "Producer known apps...\r\n";
    std::cout << "-------------------------\r\n";

    YamlParser::YamlNode* knownApps= rnode->operator[]("Producer")->operator[]("knownApps");
    if (knownApps) {

        for (auto it : knownApps->map) {
            std::cout << it.first << std::endl;
        }
    }
    std::cout << "-------------------------\r\n";


    std::cout << "Producer waaPath... \r\n";
    std::cout << "-------------------------\r\n";

    YamlParser::YamlNode* waapath= rnode->operator[]("Producer")->operator[]("waaPath");
    if (waapath) {
        for (auto it : waapath->map) {
            std::cout << it.second->k << std::endl;

        }
    }
    return 0;
}
