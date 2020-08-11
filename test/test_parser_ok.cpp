#include "gtest/gtest.h"
#include "mock_parser.h"

using namespace peanats;

ANON_NAMESPACE_BEGIN

TEST(Test_Parser, test_ok)
{
    MockParser p;

    // Try a regular packet
    p.counters.ok = 0;
    p.mock_parse("+OK\r\n");
    EXPECT_EQ(p.counters.ok, 1);

    // Try two packets
    p.counters.ok = 0;
    p.mock_parse("+OK\r\n+OK\r\n");
    EXPECT_EQ(p.counters.ok, 2);

    // Try one partial
    p.counters.ok = 0;
    p.mock_parse("+O");
    p.mock_parse("K\r\n");
    EXPECT_EQ(p.counters.ok, 1);

    // Try a full+partial and a partial
    p.counters.ok = 0;
    p.mock_parse("+OK\r\n+OK");
    p.mock_parse("\r\n");
    EXPECT_EQ(p.counters.ok, 2);

    /* The rest should fail */
    p.counters.ok = 0;
    p.mock_parse("+OK\r\r");
    EXPECT_FALSE(p.counters.ok);
    p.mock_parse("+OK\r");
    EXPECT_FALSE(p.counters.ok);
    p.mock_parse("+OK");
    EXPECT_FALSE(p.counters.ok);
}

ANON_NAMESPACE_END
