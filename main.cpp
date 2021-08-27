#include "YamlParser.hpp"

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cout << "provide argument...\r\n";
        return 1;
    }
    std::string fname {argv[1]};
    YamlParser ymlp;

    ymlp.Load(fname);

    ymlp.Parse();


    YamlParser::YamlNodeStrStr node = ymlp.GetVal("Classifier:");
     node.print();

    return 0;
}
