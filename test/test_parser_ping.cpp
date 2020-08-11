#include "gtest/gtest.h"
#include "mock_parser.h"

ANON_NAMESPACE_BEGIN

using namespace peanats;

TEST(test_parser_ping, ping)
{
    MockParser p;

    // Try a regular ping
    p.counters.ping = 0;
    p.mock_parse("PING\r\n");
    EXPECT_EQ(p.counters.ping, 1);

    // Try two pings in same package
    p.counters.ping = 0;
    p.mock_parse("PING\r\nPING\r\n");
    EXPECT_EQ(p.counters.ping, 2);

    // Try one partial
    p.counters.ping = 0;
    p.mock_parse("PI");
    p.mock_parse("NG\r\n");
    EXPECT_EQ(p.counters.ping, 1);

    // Try a full+half and a partial
    p.counters.ping = 0;
    p.mock_parse("PING\r\nPING");
    EXPECT_EQ(p.counters.ping, 1);
    p.mock_parse("\r\n");
    EXPECT_EQ(p.counters.ping, 2);

    /* The rest should fail */
    p.counters.ping = 0;
    p.mock_parse("PING\r\r");
    EXPECT_EQ(p.counters.ping, 0);
    p.mock_parse("PING\r");
    EXPECT_EQ(p.counters.ping, 0);
    p.mock_parse("PING");
    EXPECT_EQ(p.counters.ping, 0);
}

ANON_NAMESPACE_END