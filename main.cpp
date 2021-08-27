#include <iostream>
#include <vector>
#include <cstring>
#include <ostream>
#include <sstream>
#include <fstream>
#include <unordered_map>

using namespace std;

namespace utils {

    bool isspace(const char ch)
    {
        return ch == '\t' || ch == ' ';
    }

    bool starts_with(const std::string& ref, const char delimiter)
    {
        for (auto ch : ref) {
            if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r')
                continue;
            if (ch == delimiter) {
                return true;
            } else {
                return false;
            }
        }
    }

    bool ends_with(const std::string& ref, const char delimiter)
    {
        for (size_t it = ref.length()-1; it != 0 ; it--) {
            if (ref[it] == ' ' || ref[it] == '\t' || ref[it] == '\n' || ref[it]== '\r')
                continue;
            if (ref[it] == delimiter) {
                return true;
            } else {
                return false;
            }
        }
    }



    void split(const char* str, const char* delim, std::vector<std::string>& out, int counter, const int ntimes)
    {
        const char* begin = str;
        const char* it = strstr(str, delim);
        if (it != NULL)
        {
            if (ntimes != 0)
                if (counter == ntimes)
                    return;
            counter++;
            std::string data{begin, it};
            out.push_back(data);
            it++;
            split(it, delim, out, counter, ntimes);
        } else {
            std::string data{str};
            out.push_back(data);
        }
    }
}

struct YamlParser
{
    template<typename K, typename V>
    struct YamlNode
    {
        bool isRoot;
        K key;
        V value;
        std::unordered_map<K, V> m_mapped; //optional ...
        std::vector<YamlNode> children;
// operator [] if needed
        YamlNode() : isRoot{false} {};

        YamlNode(const K& k, const V& v, bool root=false) : isRoot{root}
        {
            m_mapped[k] = v;
        }

        void AddNode(const YamlNode<K, V> newNode)
        {
            children.push_back(newNode);
        }

        bool hasChildren() const { return children.size() > 0; }

        void print()
        {
            if (isRoot) {
                std::cout << "ROOT NODE:" << "\r\n";
            }
            std::cout << "KEY:[" << key << "]" << std::endl;
            std::cout << "VAL:[" << value << "]" << std::endl;

            for (auto cc : children) {
                cc.print();
            }
        }
    };


    using YamlNodeStrStr = YamlNode<std::string, std::string>;
    using YamlMap = std::unordered_map<std::string, YamlNodeStrStr> ;

    YamlMap m_nodes;

    std::vector<std::string> m_raw;

    YamlParser() = default;

    ~YamlParser() = default;

    YamlNodeStrStr& GetVal(const std::string& key)
    {
        bool bFound  = m_nodes.find(key) != m_nodes.end();
        if (bFound) {
            return m_nodes[key];
        }
    }

    void dbg_print()
    {
#if 0
        for (auto it: m_raw) {
            std::cout << it << "\r\n";
        }
#else
        for (auto it : m_nodes) {
            it.second.print();
        }

#endif
    }
    /**
     * @brief Load
     * @param fname - filename
     * @return - true on success reading, false else
     */
    bool Load(const std::string& fname)
    {
        std::ifstream infile(fname);
        std::string line;
        while (std::getline(infile, line))
        {
            std::istringstream iss(line);
            if (line.at(line.size()-1) == '\r' || line.at(line.size()-1) == '\n') {
                line.pop_back();
            }
            m_raw.push_back(line);
        }
        return true; //todo
    }
private:

protected:
    /**
     * @brief parse_internal
     * parses the yaml file, previously loaded as 'raw' array
     */
    virtual void parse_internal()
    {
        std::string last;
        for(size_t it = 0; it < m_raw.size(); it++) {
            std::vector<std::string> out;
            const char* s = m_raw.at(it).c_str();
            YamlNodeStrStr node;
            if (s[m_raw.at(it).length()-1] == ':'){
                const char* rawit = m_raw.at(it).c_str();
                while (utils::isspace(*rawit++));
                node.key = std::string(--rawit);
                char ch = m_raw.at(it).at(0);
                node.isRoot = true;
                if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') //is nested root
                {
                    m_nodes[last].AddNode(node);
                    continue;
                }
                last  = m_raw.at(it);
                m_nodes[last] = node;
            } else {
                utils::split(s, ":", out,  0, 1);
                if (out.size() >= 2) {
                    const char* kit = out[0].c_str();
                    const char* vit = out[1].c_str();
                    while (utils::isspace(*kit++));
                    while (utils::isspace(*vit++));
                    node.key = std::string(--kit);
                    node.value = std::string(--vit);
                }
                m_nodes[last].AddNode(node);
            }
        }
    }
public:
    void Parse()
    {
        parse_internal();
    }

};


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


    YamlParser::YamlNodeStrStr node = ymlp.GetVal("CoinbaseCustodial:");
    for (auto it : node.children) {
        if (it.isRoot) {
            std::cout << it.key << std::endl;
        }
    }
//    node.print();

    return 0;
}
