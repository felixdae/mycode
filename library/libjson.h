#ifndef LIBJSON_H
#define LIBJSON_H 

#include <string>
#include <vector>
#include "json.h"

namespace comm
{
    namespace library
    {
        class JsonParser;
        class JsonBuilder;
        class JsonPath
        {
            public:
                friend class JsonParser;
                friend class JsonBuilder;
                enum class KeyType
                {
                    kTypeInteger,
                    kTypeString,
                };
                JsonPath();
                virtual ~JsonPath();
                JsonPath& Append(uint32_t index);
                JsonPath& Append(const std::string& key);
                JsonPath& Retain(uint32_t to);
                std::string GetHumanPath();
            private:
                std::vector<std::pair<std::string, JsonPath::KeyType>> path_stack_;
        };

        class JsonParser
        {
            public:
                typedef json_value JsonValue;
                explicit JsonParser(const std::string& json_doc);
                ~JsonParser();
                int32_t GetValueByPath(const JsonPath& path, int64_t& value);
                int32_t GetValueByPath(const JsonPath& path, double& value);
                int32_t GetValueByPath(const JsonPath& path, std::string& value);
                int32_t GetValueByPath(const JsonPath& path, bool& value);
            private:
                int32_t GetValueByPath(const JsonPath& path, JsonValue& value);
                JsonValue *value_;
        };

        class JsonBuilder
        {
        };
    }
}
#endif /* LIBJSON_H */
