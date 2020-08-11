#include "gtest/gtest.h"
#include "mock_parser.h"

using namespace peanats;

ANON_NAMESPACE_BEGIN

TEST(test_parser_pong, pong)
{
  MockParser p;

  // Try a regular ping
  p.counters.pong = 0;
  p.mock_parse("PONG\r\n");
  EXPECT_EQ(p.counters.pong, 1);

  // Try two pings in same package
  p.counters.pong = 0;
  p.mock_parse("PONG\r\nPONG\r\n");
  EXPECT_EQ(p.counters.pong, 2);

  // Try one partial
  p.counters.pong = 0;
  p.mock_parse("PO");
  p.mock_parse("NG\r\n");
  EXPECT_EQ(p.counters.pong, 1);

  // Try a full+half and a partial
  p.counters.pong = 0;
  p.mock_parse("PONG\r\nPONG");
  p.mock_parse("\r\n");
  EXPECT_EQ(p.counters.pong, 2);

  // The rest should fail
  p.counters.pong = 0;
  p.mock_parse("PONG\r\r");
  EXPECT_EQ(p.counters.pong, 0);
  p.mock_parse("PONG\r");
  EXPECT_EQ(p.counters.pong, 0);
  p.mock_parse("PONG");
  EXPECT_EQ(p.counters.pong, 0);
}

ANON_NAMESPACE_END