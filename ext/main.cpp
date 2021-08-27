#include <yaml.h>
#include <cstdlib>
#include <cstdio>
#include <assert.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
void print_escaped(yaml_char_t * str, size_t length);
int usage(int ret);


struct YamlVal
{
    std::string m_strval;
    yaml_event_t event;
    YamlVal()
    {
        memset(&event, 0, sizeof(event));
    }

    YamlVal(const yaml_char_t* val) : m_strval{(char*)val}
    {
    }

    YamlVal& operator=(const YamlVal& l) {
        this->event = l.event;
        this->m_strval = l.m_strval;
        return *this;
    }

    YamlVal(const YamlVal& l) {
        this->event = l.event;
        this->m_strval = l.m_strval;
    }
};

struct YamlSeq
{
    std::vector<YamlVal> sequences;
};

struct YamlMap
{
    bool hasChildren;
    bool isSequence;
    YamlVal val;
    YamlSeq secquences;
    std::vector<YamlMap> children;
    YamlMap() : hasChildren{false} , isSequence{false} { }
};

class YamlObj
{
    yaml_parser_t m_parser;
    FILE* p_yamlFile;
    std::vector<YamlMap> m_maps;
    bool m_isOk;
public:
    YamlObj() : p_yamlFile {nullptr}, m_isOk{false}
    {
        memset(&m_parser, 0, sizeof(m_parser));
    }

    ~YamlObj() = default;

    void Load(const std::string& fname)
    {
        p_yamlFile = fopen(fname.c_str(), "rb");
        if (!p_yamlFile)
            return;
        if (yaml_parser_initialize(&m_parser)) {
            m_isOk = true;
            yaml_parser_set_input_file(&m_parser, p_yamlFile);
        }
    }

    void Unload()
    {
        if (p_yamlFile && m_isOk) {
            assert(fclose(p_yamlFile)) ;
            p_yamlFile = nullptr;
            yaml_parser_delete(&m_parser);
            m_isOk = false;
        }
    }

    bool Ready() const { return  m_isOk; }

    void dbg_print(YamlMap& root)
    {

        if (root.isSequence) {
            for (auto seq : root.secquences.sequences) {
                std::cout<< "[" << seq.m_strval << "]\r\n";
            }
        } else {
            std::cout<< "$$: " << root.val.m_strval << "\r\n";
        }
        if (root.hasChildren) {
            for(size_t i=0; i < root.children.size(); ++i) {
                dbg_print(root.children.at(i));
            }
        }
    }


    bool internal_parse(yaml_event_t& event, YamlMap& inmap)
    {

        yaml_event_type_t type ;
        if (!yaml_parser_parse(&m_parser, &event)) {
            return false;
        }

        type = event.type;
        if (type == YAML_NO_EVENT)
            printf("???\n");
        else if (type == YAML_STREAM_START_EVENT)
            printf("+STR\n");
        else if (type == YAML_STREAM_END_EVENT)
            printf("-STR\n");
        else if (type == YAML_DOCUMENT_START_EVENT) {
            printf("+DOC");
            if (!event.data.document_start.implicit)
                printf(" ---");
            printf("\n");
        }
        else if (type == YAML_DOCUMENT_END_EVENT) {
            printf("-DOC");
            if (!event.data.document_end.implicit)
                printf(" ...");
            printf("\n");
        }
        else if (type == YAML_MAPPING_START_EVENT) {
            printf("+MAP");
            if (event.data.mapping_start.style == YAML_FLOW_MAPPING_STYLE)
                printf("# {}");
            if (event.data.mapping_start.anchor)
                printf("## &%s", event.data.mapping_start.anchor);
            if (event.data.mapping_start.tag)
                printf("### <%s>", event.data.mapping_start.tag);
            printf("\n");
            YamlMap newMap;
            inmap.hasChildren = true;
            inmap.children.push_back(newMap);
            return internal_parse(event, newMap); // go to next
        }
        else if (type == YAML_MAPPING_END_EVENT) {
            inmap.hasChildren = false;
            printf("-MAP\n");
            return internal_parse(event, inmap); // go to next
        }
        else if (type == YAML_SEQUENCE_START_EVENT) {
            printf("+SEQ");
            if (event.data.sequence_start.style == YAML_FLOW_SEQUENCE_STYLE)
                printf(" []");
            if (event.data.sequence_start.anchor)
                printf(" &%s", event.data.sequence_start.anchor);
            if (event.data.sequence_start.tag)
                printf(" <%s>", event.data.sequence_start.tag);
            printf("\n");
            inmap.isSequence = true;
            //return internal_parse(event, inmap); // go to next
        }
        else if (type == YAML_SEQUENCE_END_EVENT) {
            printf("-SEQ\n");
            inmap.isSequence = false;
        }
        else if (type == YAML_SCALAR_EVENT) {
            printf("=VAL");
            if (event.data.scalar.anchor)
                printf(" &%s", event.data.scalar.anchor);
            if (event.data.scalar.tag)
                printf(" <%s>", event.data.scalar.tag);
            switch (event.data.scalar.style) {
                case YAML_PLAIN_SCALAR_STYLE:
                    break;
                case YAML_SINGLE_QUOTED_SCALAR_STYLE:
                    break;
                case YAML_DOUBLE_QUOTED_SCALAR_STYLE:
                    break;
                case YAML_LITERAL_SCALAR_STYLE:
                    break;
                case YAML_FOLDED_SCALAR_STYLE:
                    break;
                case YAML_ANY_SCALAR_STYLE:
                    abort();
                }
           printf("-------[%s] : [%d] \r\n", (char*)event.data.scalar.value, event.data.scalar.length);
            if (inmap.isSequence) {
               inmap.secquences.sequences.push_back(YamlVal{event.data.scalar.value});
            } else {
                inmap.val = YamlVal{event.data.scalar.value}; // tbd ... todo :)
            }
        }
        else if (type == YAML_ALIAS_EVENT)
            printf("=ALI *%s\n", event.data.alias.anchor); // not supported
        else
            abort();

        yaml_event_delete(&event);

        if (type == YAML_STREAM_END_EVENT)
            return false;
        return true;
    }

    bool Parse()
    {
        yaml_event_t event;
        if (!Ready())
            return  false;
        YamlMap root;
        while (internal_parse(event, root));

        dbg_print(root);
    }

};

int main(int argc, char** argv)
{
    YamlObj obj;
    obj.Load(argv[1]);
    if (obj.Parse()) {
        printf("OK!\r\n");
    } else {
        printf("fail\r\n");
    }
    return 0;
}


#if 0
int main(int argc, char *argv[])
{
    FILE *input;
    yaml_parser_t parser;
    yaml_event_t event;
    int flow = -1; /** default no flow style collections */
    int i = 0;
    int foundfile = 0;

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "--flow", 6) == 0) {
            if (i+1 == argc)
                return usage(1);
            i++;
            if (strncmp(argv[i], "keep", 4) == 0)
                flow = 0;
            else if (strncmp(argv[i], "on", 2) == 0)
                flow = 1;
            else if (strncmp(argv[i], "off", 3) == 0)
                flow = -1;
            else
                return usage(1);
        }
        else if (strncmp(argv[i], "--help", 6) == 0)
            return usage(0);
        else if (strncmp(argv[i], "-h", 2) == 0)
            return usage(0);
        else if (!foundfile) {
            input = fopen(argv[i], "rb");
            foundfile = 1;
        }
        else
            return usage(1);
    }
    if (!foundfile) {
        input = stdin;
    }
    assert(input);

    if (!yaml_parser_initialize(&parser)) {
        fprintf(stderr, "Could not initialize the parser object\n");
        return 1;
    }
    yaml_parser_set_input_file(&parser, input);

    while (1) {
        yaml_event_type_t type;
        if (!yaml_parser_parse(&parser, &event)) {
            if ( parser.problem_mark.line || parser.problem_mark.column ) {
                fprintf(stderr, "Parse error: %s\nLine: %lu Column: %lu\n",
                        parser.problem,
                        (unsigned long)parser.problem_mark.line + 1,
                        (unsigned long)parser.problem_mark.column + 1);
            }
            else {
                fprintf(stderr, "Parse error: %s\n", parser.problem);
            }
            return 1;
        }
        type = event.type;

        if (type == YAML_NO_EVENT)
            printf("???\n");
        else if (type == YAML_STREAM_START_EVENT)
            printf("+STR\n");
        else if (type == YAML_STREAM_END_EVENT)
            printf("-STR\n");
        else if (type == YAML_DOCUMENT_START_EVENT) {
            printf("+DOC");
            if (!event.data.document_start.implicit)
                printf(" ---");
            printf("\n");
        }
        else if (type == YAML_DOCUMENT_END_EVENT) {
            printf("-DOC");
            if (!event.data.document_end.implicit)
                printf(" ...");
            printf("\n");
        }
        else if (type == YAML_MAPPING_START_EVENT) {
            printf("+MAP");
            if (flow == 0 && event.data.mapping_start.style == YAML_FLOW_MAPPING_STYLE)
                printf(" {}");
            else if (flow == 1)
                printf(" {}");
            if (event.data.mapping_start.anchor)
                printf(" &%s", event.data.mapping_start.anchor);
            if (event.data.mapping_start.tag)
                printf(" <%s>", event.data.mapping_start.tag);
            printf("\n");
        }
        else if (type == YAML_MAPPING_END_EVENT)
            printf("-MAP\n");
        else if (type == YAML_SEQUENCE_START_EVENT) {
            printf("+SEQ");
            if (flow == 0 && event.data.sequence_start.style == YAML_FLOW_SEQUENCE_STYLE)
                printf(" []");
            else if (flow == 1)
                printf(" []");
            if (event.data.sequence_start.anchor)
                printf(" &%s", event.data.sequence_start.anchor);
            if (event.data.sequence_start.tag)
                printf(" <%s>", event.data.sequence_start.tag);
            printf("\n");
        }
        else if (type == YAML_SEQUENCE_END_EVENT)
            printf("-SEQ\n");
        else if (type == YAML_SCALAR_EVENT) {
            printf("=VAL");
            if (event.data.scalar.anchor)
                printf(" &%s", event.data.scalar.anchor);
            if (event.data.scalar.tag)
                printf(" <%s>", event.data.scalar.tag);
            switch (event.data.scalar.style) {
            case YAML_PLAIN_SCALAR_STYLE:
                printf(" :");
                break;
            case YAML_SINGLE_QUOTED_SCALAR_STYLE:
                printf(" '");
                break;
            case YAML_DOUBLE_QUOTED_SCALAR_STYLE:
                printf(" \"");
                break;
            case YAML_LITERAL_SCALAR_STYLE:
                printf(" |");
                break;
            case YAML_FOLDED_SCALAR_STYLE:
                printf(" >");
                break;
            case YAML_ANY_SCALAR_STYLE:
                abort();
            }
            print_escaped(event.data.scalar.value, event.data.scalar.length);
            printf("\n");
        }
        else if (type == YAML_ALIAS_EVENT)
            printf("=ALI *%s\n", event.data.alias.anchor);
        else
            abort();

        yaml_event_delete(&event);

        if (type == YAML_STREAM_END_EVENT)
            break;
    }

    assert(!fclose(input));
    yaml_parser_delete(&parser);
    fflush(stdout);

    return 0;
}

void print_escaped(yaml_char_t * str, size_t length)
{
    int i;
    char c;

    for (i = 0; i < length; i++) {
        c = *(str + i);
        if (c == '\\')
            printf("\\\\");
        else if (c == '\0')
            printf("\\0");
        else if (c == '\b')
            printf("\\b");
        else if (c == '\n')
            printf("\\n");
        else if (c == '\r')
            printf("\\r");
        else if (c == '\t')
            printf("\\t");
        else
            printf("%c", c);
    }
}

int usage(int ret) {
    fprintf(stderr, "Usage: libyaml-parser [--flow (on|off|keep)] [<input-file>]\n");
    return ret;
}
#endif
