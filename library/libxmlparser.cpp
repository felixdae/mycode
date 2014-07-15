#include "libxmlparser.h"

namespace comm
{
    namespace library
    {
        XmlParser::XmlParser()
        :doc_(NULL),
        xpath_context_(NULL)
        //xpath_object_(NULL),
        {
        }

        XmlParser::~XmlParser()
        {
            if (this->doc_ != NULL)
            {
                xmlFreeDoc(this->doc_);
            }
            if (this->xpath_context_ != NULL)
            {
                xmlXPathFreeContext(this->xpath_context_);
            }
            //if (this->xpath_object_ != NULL)
            //{
            //    xmlXPathFreeObject(this->xpath_object_);
            //}
            xmlCleanupParser();
        }

        int32_t XmlParser::ReadXml(const std::string& path)
        {
            this->doc_ = xmlParseFile(path.c_str());
            if (this->doc_ == NULL)
            {
                return -1;
            }
            this->xpath_context_ = xmlXPathNewContext(this->doc_);
            if (this->xpath_context_ == NULL)
            {
                return -1;
            }
            return 0;
        }

        int32_t XmlParser::ParseString(const std::string& input)
        {
            this->doc_ = xmlParseMemory(input.c_str(), input.length());
            if (this->doc_ == NULL)
            {
                return -1;
            }
            this->xpath_context_ = xmlXPathNewContext(this->doc_);
            if (this->xpath_context_ == NULL)
            {
                return -1;
            }
            return 0;
        }
        int32_t XmlParser::GetNodesValue(const std::string& xpath, std::vector<int32_t>& res)
        {
            res.clear();
            std::vector<std::string> str_res;
            if (this->GetNodesValue(xpath, str_res) != 0)
            {
                return -1;
            }
            for (auto it = str_res.begin(); it != str_res.end(); ++it)
            {
                res.push_back(std::stoi(*it));
            }
            return 0;
        }

        int32_t XmlParser::GetNodesValue(const std::string& xpath, std::vector<std::string>& res)
        {
            res.clear();
            this->xpath_object_ = xmlXPathEvalExpression((const xmlChar*)xpath.c_str(), this->xpath_context_);
            if (this->xpath_object_ == NULL)
            {
                return -1;
            }

            this->node_set_ = this->xpath_object_->nodesetval;
            int size = this->node_set_? this->node_set_->nodeNr : 0;
            for (int i = 0; i < size; ++i)
            {
                if(this->node_set_->nodeTab[i]->xmlChildrenNode == NULL)
                {
                    res.push_back("");
                    continue;
                }
                xmlChar* node = xmlNodeListGetString(this->doc_, this->node_set_->nodeTab[i]->xmlChildrenNode, 0);
                if (node == NULL)
                {
                    xmlXPathFreeObject(this->xpath_object_);
                    return -1;
                }
                res.push_back((char*)node);
                xmlFree(node);
            }
            xmlXPathFreeObject(this->xpath_object_);
            return 0;
        }

        int32_t XmlParser::GetNodesAttribute(const std::string& xpath, const std::string& attr, std::vector<int32_t>& res)
        {
            res.clear();
            std::vector<std::string> str_res;
            if (this->GetNodesAttribute(xpath, attr, str_res) != 0)
            {
                return -1;
            }
            for (auto it = str_res.begin(); it != str_res.end(); ++it)
            {
                res.push_back(std::stoi(*it));
            }
            return 0;
        }

        int32_t XmlParser::GetNodesAttribute(const std::string& xpath, const std::string& attr, std::vector<std::string>& res)
        {
            res.clear();
            this->xpath_object_ = xmlXPathEvalExpression((const xmlChar*)xpath.c_str(), this->xpath_context_);
            if (this->xpath_object_ == NULL)
            {
                return -1;
            }

            this->node_set_ = this->xpath_object_->nodesetval;
            int size = this->node_set_? this->node_set_->nodeNr : 0;
            for (int i = 0; i < size; ++i)
            {
                xmlChar* val = xmlGetProp(this->node_set_->nodeTab[i], (const xmlChar*)attr.c_str());
                if (val == NULL)
                {
                    xmlXPathFreeObject(this->xpath_object_);
                    return -1;
                }
                res.push_back((char*)val);
                xmlFree(val);
            }
            xmlXPathFreeObject(this->xpath_object_);
            return 0;
        }
    }
}

