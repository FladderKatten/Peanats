#include "gtest/gtest.h"
#include "mock_parser.h"

using namespace peanats;

ANON_NAMESPACE_BEGIN


TEST(test_parser_msg, minimalistic_case)
{
    MockParser p;
    auto& c = p.msg_capture;

    p.mock_parse("MSG subject 1 0\r\n\r\n");
    EXPECT_EQ(c.back().subject, "subject");
    EXPECT_EQ(c.back().sid, 1);
    EXPECT_EQ(c.back().replyto, "");
    EXPECT_EQ(c.back().payload, "");
}

TEST(test_parser_msg, minimalistic_with_payload)
{
    MockParser p;
    auto& c = p.msg_capture;
    
    p.mock_parse("MSG subject 11 3\r\n...\r\n");
    EXPECT_EQ(c.back().subject, "subject");
    EXPECT_EQ(c.back().sid, 11);
    EXPECT_EQ(c.back().replyto, "");
    EXPECT_EQ(c.back().payload, "...");
}

TEST(test_parser_msg, minimalistic_with_replyto)
{
    MockParser p;
    auto& c = p.msg_capture;
    
    p.mock_parse("MSG subject 11 inbox 0\r\n\r\n");
    EXPECT_EQ(c.back().subject, "subject");
    EXPECT_EQ(c.back().sid, 11);
    EXPECT_EQ(c.back().replyto, "inbox");
    EXPECT_EQ(c.back().payload, "");
}

TEST(test_parser_msg, full_msg)
{
    MockParser p;
    auto& c = p.msg_capture;
    
    p.mock_parse("MSG subject 555 inbox 3\r\n...\r\n");
    EXPECT_EQ(c.back().subject, "subject");
    EXPECT_EQ(c.back().sid, 555);
    EXPECT_EQ(c.back().replyto, "inbox");
    EXPECT_EQ(c.back().payload, "...");
}

TEST(test_parser_msg, partial_payload)
{
    MockParser p;
    auto& c = p.msg_capture;

    p.mock_parse("MSG subject 555 inbox 6\r\n1234");
    EXPECT_EQ(c.size(), 0);
    p.mock_parse("56\r\n");
    EXPECT_EQ(c.size(), 1);

    EXPECT_EQ(c.back().subject, "subject");
    EXPECT_EQ(c.back().sid, 555);
    EXPECT_EQ(c.back().replyto, "inbox");
    EXPECT_EQ(c.back().payload, "123456");
}

ANON_NAMESPACE_END