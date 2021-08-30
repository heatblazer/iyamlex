#ifndef YAML2EX_HPP
#define YAML2EX_HPP
#include "yaml.h"

#include <cstring>
#include <assert.h>
#include <cstdio>
#include <cstdlib>
#include <unordered_map>
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <stack>



struct YamlParser
{

    struct YamlNode
    {
        std::string k;
        std::unordered_map<std::string, YamlNode*> map;
        
        YamlNode() : k("[[default]]") //just give some default name, rather than empty string, [[default]] will point to null node..
        {
            map[k] = nullptr;
        }

        ~YamlNode()
        {
            for (auto it = map.begin(); it != map.end(); it++) {
                if (it->second) {
                    delete it->second;
                    it->second = nullptr;
                }
            }
        }

        void dump()
        {
            for (auto it = map.begin(); it != map.end(); it++) {
                std::cout << "[" << it->first << "]"; 
                if (it->second)
                   it->second->dump();
            }
            std::cout << "\r\n";
        }

        YamlNode* operator[](const std::string& key) {
            if(map.find(key) != map.end()) {
                return  map[key];
            }
            return nullptr;
        }
    };

    YamlParser() : m_filePtr(nullptr), m_rootNode (nullptr)
    {
        memset(&m_parser, 0 , sizeof(m_parser));
        memset(&m_event, 0, sizeof(m_event));
        m_rootNode = new YamlNode;
    }
    ~YamlParser()
    {
        yaml_event_delete(&m_event);
        yaml_parser_delete(&m_parser);
        fclose(m_filePtr);
        cleanup(m_rootNode);
        
    }

    bool Load(const char* fname)
    {
        bool bres = false;
        m_filePtr = fopen(fname, "r");
        if (!m_filePtr) return  bres;
        else {
            if (!yaml_parser_initialize(&m_parser))
                return  bres;
            bres = true;
        }
        yaml_parser_set_input_file(&m_parser, m_filePtr);

        return bres;;
    }

    

    void Parse()
    {
        yaml_parser_t*p = &m_parser;
        yaml_event_t* e = &m_event;

        skip_event(p, e);
        skip_event(p, e);
        parse_internal(p, m_rootNode);
        // skip document end and stream end
        skip_event(p, e);
        skip_event(p, e);
//        dbgprn(&m_rootNode);
    }

    YamlNode* root()
    {
        return  m_rootNode;
    }

    void Print()
    {
        m_rootNode->dump();
    }
private:

    yaml_parser_t m_parser;
    yaml_event_t  m_event;   /* New variable */
    FILE* m_filePtr ;
    YamlNode* m_rootNode;


    void skip_event(yaml_parser_t* p, yaml_event_t* e)
    {
        yaml_parser_parse(p, e);
        yaml_event_delete(e);
    }

    void cleanup(YamlNode* root)
    {
        if (!root)
            return;
        delete root; 
//        for (auto it = root->map.begin(); it != root->map.end(); root++) {
//            cleanup(it->second);
//            delete root;
//            root = nullptr;
        
    }

    void parse_internal(yaml_parser_t *p, YamlNode* node) {
        if (!node) 
            return;
        yaml_event_t e;
        yaml_parser_parse(p, &e);
        switch (e.type) {
        case YAML_SEQUENCE_START_EVENT: {
            yaml_event_delete(&e);
            yaml_parser_parse(p, &e);
            while (e.type != YAML_SEQUENCE_END_EVENT) {
                //not supported yet...
                std::string ttt ((char*)e.data.scalar.value);
                yaml_parser_parse(p, &e);
            }
            break;
        }
        case YAML_MAPPING_START_EVENT:
            yaml_event_delete(&e);
            yaml_parser_parse(p, &e);
            while (e.type != YAML_MAPPING_END_EVENT) {
                // assume scalar key
                assert(e.type == YAML_SCALAR_EVENT);
                std::string scalar((char*) e.data.scalar.value );
                if (!scalar.empty()) {
                    yaml_event_delete(&e);
//                    node->k = scalar;
                    node->map[scalar] = new YamlNode;
 //                   node->map[scalar]->k = node->k;
 //                   parse_internal(p, node->map[scalar]);
                    parse_internal(p, node->operator[](scalar));
                    yaml_parser_parse(p, &e);
                }
            }
            break;
        case YAML_SCALAR_EVENT: {
            std::string val ((char*)e.data.scalar.value);
            node->map[node->k] = new YamlNode;
            node->map[node->k]->k = val;
//            node->map[node->k]->map[val] = new YamlNode;
            break;
        }
        default:
            break;
            assert(false);
        }
        yaml_event_delete(&e);
    }

};




#endif // YAML2EX_HPP
