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
        for (auto ii = it->map.cbegin(); ii != it->map.cend(); ii++) {
            std::cout << ii->first << std::endl;
        }
    }
    std::cout << "-------------------------\r\n";
    std::cout << "Producer known apps...\r\n";
    std::cout << "-------------------------\r\n";

    YamlParser::YamlNode* knownApps= rnode->operator[]("Producer")->operator[]("knownApps");
    if (knownApps) {

        for (auto it = knownApps->map.begin(); it != knownApps->map.end(); ++it) {
            std::cout << it->first << std::endl;
        }
    }
    std::cout << "-------------------------\r\n";


    std::cout << "Producer waaPath... \r\n";
    std::cout << "-------------------------\r\n";

    YamlParser::YamlNode* waapath= rnode->operator[]("Producer")->operator[]("waaPath");
    if (waapath) {
        for (auto it = waapath->map.begin(); it != waapath->map.end(); ++it) {
            std::cout << it->second->k << std::endl;
        }
    }
    std::cout << "\r\n----------------------------------------------\r\n";
    std::string celium =    "MyCelium";
    std::string ssldomains =    "sslDomains";

    YamlParser::YamlNode* ncelium = rnode->operator[](celium);
    if (ncelium) {
        YamlParser::YamlNode* nssl = ncelium->operator[](ssldomains);
        if (nssl) {
            for (auto iii = nssl->map.begin(); iii != nssl->map.end(); ++iii) {
                std::cout << iii->first << std::endl;
            }
        }
    }
    std::cout << "\r\n----------------------------------------------\r\n";


   //justtest dump... it's slow
   // yp.Print();
return 0;
}
