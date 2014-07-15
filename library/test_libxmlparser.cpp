#include "gtest/gtest.h"
#include "libxmlparser.h"

TEST(XmlParser, ReadXml)
{
    comm::library::XmlParser xml;
    ASSERT_EQ(0, xml.ReadXml("test.xml"));
    std::vector<std::string> node;
    std::vector<std::string> attr;
    EXPECT_EQ(0, xml.GetNodesValue("/doc/b", node));
    EXPECT_EQ(0, xml.GetNodesAttribute("/doc/b", "t", attr));
    EXPECT_EQ(3, node.size());
    EXPECT_EQ(3, attr.size());
}

TEST(XmlParser, ParseString)
{
    std::string doc="<doc>"
        //"<a uu=\"12\"></a>"
        "<a uu=\"\"></a>"
        "<b t=\"1\">xxx</b>"
        "<b t=\"2\">yyy</b>"
        "<b t=\"3\">zzz</b>"
        "</doc>";
    comm::library::XmlParser xml;
    ASSERT_EQ(0, xml.ParseString(doc));
    std::vector<std::string> node;
    std::vector<std::string> attr;
    EXPECT_EQ(0, xml.GetNodesValue("/doc/a", node));
    EXPECT_NE(0, xml.GetNodesAttribute("/doc/a", "u", attr));
    EXPECT_EQ(0, xml.GetNodesAttribute("/doc/a", "uu", attr));
    EXPECT_EQ(1, node.size());
    EXPECT_EQ("", node[0]);
    EXPECT_EQ(1, attr.size());
    EXPECT_EQ("", attr[0]);
}
