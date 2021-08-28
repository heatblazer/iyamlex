#ifndef YAML2EX_HPP
#define YAML2EX_HPP
#include <yaml.h>

#include <cstring>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <cstdio>
#include <cstdlib>
#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>
#include <stack>



struct YamlParser
{

    yaml_parser_t parser;
    yaml_event_t  event;   /* New variable */
    FILE* fp ;

    YamlParser() : fp{nullptr}
    {
        memset(&parser, 0 , sizeof(parser));
        memset(&event, 0, sizeof(event));

    }
    ~YamlParser()
    {
        yaml_event_delete(&event);
        yaml_parser_delete(&parser);
        fclose(fp);
    }

    bool Load(const char* fname)
    {
        bool bres = false;
        fp = fopen(fname, "r");
        if (!fp) return  bres;
        else {
            if (!yaml_parser_initialize(&parser))
                return  bres;
            bres = true;
        }
        yaml_parser_set_input_file(&parser, fp);

        return bres;;
    }

    void dbgprn()
    {

    }

    void Parse()
    {
        yaml_parser_t*p = &parser;
        yaml_event_t* e = &event;
        yaml_parser_parse(p, e);
        yaml_event_delete(e);
        yaml_parser_parse(p, e);
        yaml_event_delete(e);

        char cur_key[100] = {'\0'};
        struct Input input[400];
        struct Input *input_end = input;
        //        append_all(p, &input_end, cur_key, 0);
        append_all2(p, m_rootNode);
        // skip document end and stream end
        yaml_parser_parse(p, e);
        yaml_event_delete(e);
        yaml_parser_parse(p, e);
        yaml_event_delete(e);

        yaml_parser_delete(p);


        for (struct Input *cur = input; cur < input_end; ++cur) {
            printf("%s = %s\n", cur->key, cur->value);
        }
    }


    struct YamlNode
    {
        std::string k, v;
        std::unordered_map<std::string, YamlNode*> map;
    };

private:


    std::stack<YamlNode> m_helper;
    YamlNode m_rootNode;

    struct Input {
        char key[100];
        char value[100];
    };

    struct Input gen(const char *key, const char *value) {
        struct Input ret;
        strcpy(ret.key, key);
        strcpy(ret.value, value);
        return ret;
    }

    void append_all(yaml_parser_t *p, struct Input **target,
                    char cur_key[100], size_t len) {
        yaml_event_t e;
        yaml_parser_parse(p, &e);
        switch (e.type) {
        case YAML_MAPPING_START_EVENT:
            yaml_event_delete(&e);
            yaml_parser_parse(p, &e);
            while (e.type != YAML_MAPPING_END_EVENT) {
                // assume scalar key
                assert(e.type == YAML_SCALAR_EVENT);
                if (len != 0) cur_key[len++] = '.';
                memcpy(cur_key + len, e.data.scalar.value,
                       strlen((char*)e.data.scalar.value) + 1);
                const size_t new_len = len + strlen((char*)e.data.scalar.value);
                yaml_event_delete(&e);
                append_all(p, target, cur_key, new_len);
                if (len != 0) --len;
                cur_key[len] = '\0'; // remove key part
                yaml_parser_parse(p, &e);
            }
            break;
        case YAML_SCALAR_EVENT:
            *(*target)++ = gen(cur_key, (char*)e.data.scalar.value);
            break;
        default: assert(false);
        }
        yaml_event_delete(&e);
    }

    void append_all2(yaml_parser_t *p, YamlNode& node) {
        yaml_event_t e;
        yaml_parser_parse(p, &e);
        switch (e.type) {
        case YAML_SEQUENCE_START_EVENT: {
            yaml_event_delete(&e);
            yaml_parser_parse(p, &e);
            while (e.type != YAML_SEQUENCE_END_EVENT) {
                //                append_all2(p, *node.map[scalar]);
                std::string ttt {(char*)e.data.scalar.value};
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
                std::string scalar{(char*) e.data.scalar.value };
                if (!scalar.empty()) {
                    yaml_event_delete(&e);
                    node.k = scalar;
                    node.map[scalar] = new YamlNode;
                    //                    m_helper.push(node);
                    append_all2(p, *node.map[scalar]);
                    yaml_parser_parse(p, &e);
                }
            }
            break;
        case YAML_SCALAR_EVENT: {
            std::string val {(char*)e.data.scalar.value};

            //            *(*target)++ = gen(cur_key, (char*)e.data.scalar.value);
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
