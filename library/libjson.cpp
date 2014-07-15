#include <cassert>
#include "libjson.h"

namespace comm
{
    namespace library
    {
        JsonPath::JsonPath()
        {
        }

        JsonPath::~JsonPath()
        {
        }

        JsonPath& JsonPath::Append(uint32_t index)
        {
            this->path_stack_.push_back(std::make_pair(std::to_string(index), JsonPath::KeyType::kTypeInteger));
            return *this;
        }

        JsonPath& JsonPath::Append(const std::string& key)
        {
            this->path_stack_.push_back(std::make_pair(key, JsonPath::KeyType::kTypeString));
            return *this;
        }

        JsonPath& JsonPath::Retain(uint32_t to)
        {
            if (to <= this->path_stack_.size())
            {
                auto it = this->path_stack_.begin();
                std::advance(it, to);
                this->path_stack_.erase(it, this->path_stack_.end());
            }
            return *this;
        }

        std::string JsonPath::GetHumanPath()
        {
            std::string path;
            for (auto it = this->path_stack_.begin(); it != this->path_stack_.end(); ++it)
            {
                if (it->second == JsonPath::KeyType::kTypeString)
                {
                    path.append("." + it->first);
                }
                else if (it->second == JsonPath::KeyType::kTypeInteger)
                {
                    path.append("[" + it->first + "]");
                }
            }
            return path;
        }

        JsonParser::JsonParser(const std::string& json_doc)
            :value_(NULL)
        {
            this->value_ = json_parse(json_doc.c_str(), json_doc.length());
        }

        JsonParser::~JsonParser()
        {
            if (this->value_ != NULL)
            {
                json_value_free(this->value_);
            }
        }
        
        int32_t JsonParser::GetValueByPath(const JsonPath& path, JsonValue& ret_value)
        {
            if (this->value_ == NULL)
            {
                return -1;
            }

            JsonValue tmp_value = *(this->value_);
            for (auto it = path.path_stack_.begin(); it != path.path_stack_.end(); ++it)
            {
                std::string key = it->first;
                JsonPath::KeyType key_type = it->second;
                if(key_type == JsonPath::KeyType::kTypeInteger)
                {
                    tmp_value = tmp_value[std::stol(key)];
                }
                else if(key_type == JsonPath::KeyType::kTypeString)
                {
                    tmp_value = tmp_value[key.c_str()];
                }

                if (&tmp_value == &json_value_none)
                {
                    return 1;
                }  
            }
            ret_value = tmp_value;
            return 0;
        }

        int32_t JsonParser::GetValueByPath(const JsonPath& path, int64_t& value)
        {
            JsonValue raw_value;
            int32_t rc = this->GetValueByPath(path, raw_value);
            if (rc != 0)
            {
                return rc;
            }
            if (raw_value.type != json_integer)
            {
                return 1;
            }
            value = (int64_t)raw_value;
            return 0;
        }

        int32_t JsonParser::GetValueByPath(const JsonPath& path, double& value)
        {
            JsonValue raw_value;
            int32_t rc = this->GetValueByPath(path, raw_value);
            if (rc != 0)
            {
                return rc;
            }
            if (raw_value.type != json_double)
            {
                return 1;
            }
            value = (double)raw_value;
            return 0;
        }

        int32_t JsonParser::GetValueByPath(const JsonPath& path, std::string& value)
        {
            JsonValue raw_value;
            int32_t rc = this->GetValueByPath(path, raw_value);
            if (rc != 0)
            {
                return rc;
            }
            if (raw_value.type != json_string)
            {
                return 1;
            }
            value = (const char*)raw_value;
            return 0;
        }

        int32_t JsonParser::GetValueByPath(const JsonPath& path, bool& value)
        {
            JsonValue raw_value;
            int32_t rc = this->GetValueByPath(path, raw_value);
            if (rc != 0)
            {
                return rc;
            }
            if (raw_value.type != json_boolean)
            {
                return 1;
            }
            value = (bool)raw_value;
            return 0;
        }
    }
}
