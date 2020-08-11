#include "gtest/gtest.h"
#include "mock_parser.h"

using namespace peanats;

ANON_NAMESPACE_BEGIN

TEST(test_parser_err, simple_string)
{
  MockParser p;

  // Test a simple string
  p.mock_parse("-ERR 'test'\r\n");
  ASSERT_EQ(p.err_capture.size(), 1);
  EXPECT_EQ(p.err_capture.front(), "test");
}

// Test a empty string
TEST(test_parser_err, empty_string)
{
  MockParser p;
  p.err_capture.clear();
  p.mock_parse("-ERR ''\r\n'");
  ASSERT_EQ(p.err_capture.size(), 1);
  EXPECT_EQ(p.err_capture.front(), "");
}

TEST(test_parser_err, double_packet)
{
  MockParser p;
  p.err_capture.clear();
  p.mock_parse("-ERR 'abcd'\r\n-ERR 'efgh'\r\n");
  ASSERT_EQ(p.err_capture.size(), 2);
  EXPECT_EQ(p.err_capture.front(), "abcd");
  EXPECT_EQ(p.err_capture.back(), "efgh");
  }

TEST(test_parser_err, partial_read)
{
  MockParser p;
  p.err_capture.clear();
  p.mock_parse("-ERR '012345");
  ASSERT_EQ(p.err_capture.size(), 0);
  p.mock_parse("6789'\r\n");
  ASSERT_EQ(p.err_capture.size(), 1);
  EXPECT_EQ(p.err_capture.back(), "0123456789");
}

TEST(test_parser_err, expected_fails)
{
  MockParser p;
  p.err_capture.clear();
  p.mock_parse("-ERR ''\r\r");
  EXPECT_FALSE(p.err_capture.size());
  p.mock_parse("-ERR ''\r");
  EXPECT_FALSE(p.err_capture.size());
  p.mock_parse("-ERR ''");
  EXPECT_FALSE(p.err_capture.size());
  p.mock_parse("-ERR '");
  EXPECT_FALSE(p.err_capture.size());
  p.mock_parse("-ERR ");
  EXPECT_FALSE(p.err_capture.size());
  p.mock_parse("-ERR");
  EXPECT_FALSE(p.err_capture.size());
}

ANON_NAMESPACE_END
