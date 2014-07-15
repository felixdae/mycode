#ifndef LIBXML_H
#define LIBXML_H 
#include <string>
#include <vector>
#include <libxml/xpath.h>

namespace comm
{
    namespace library
    {
        class XmlParser
        {
            public:
                XmlParser();
                virtual ~XmlParser();
                int32_t ReadXml(const std::string& path);
                int32_t ParseString(const std::string& input);
                int32_t GetNodesValue(const std::string& xpath, std::vector<std::string>& res);
                int32_t GetNodesValue(const std::string& xpath, std::vector<int32_t>& res);
                int32_t GetNodesAttribute(const std::string& xpath, const std::string& attr, std::vector<std::string>& res);
                int32_t GetNodesAttribute(const std::string& xpath, const std::string& attr, std::vector<int32_t>& res);
            private:
                xmlDocPtr doc_;
                xmlXPathContextPtr xpath_context_;
                xmlXPathObjectPtr xpath_object_;
                xmlNodeSetPtr node_set_;
        };
    }
}
#endif /* LIBXML_H */
