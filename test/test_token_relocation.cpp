#include "gtest/gtest.h"
#include "mock_parser.h"

using namespace peanats;

ANON_NAMESPACE_BEGIN


// This test simulates a partial read at the end of
// the buffer and tests if peanats can correctly shift
// the saved tokens to the start of the buffer

TEST(test_token_relocation, minimalistic_test_case)
{
  // create a parser with only 15 bytes of recv buffer
  MockParser p(15);

  // This string will not fit in 15 bytes and thus the second
  // packet will have to get shifted to the start of the buffer
  // before receiving the last 5 bytes
  p.mock_parse("-ERR '1'\r\n-ERR '2'\r\n");
  
  // Checking the results should be enough
  EXPECT_EQ(p.err_capture.size(), 2);
  EXPECT_EQ(p.err_capture.front(), "1");
  EXPECT_EQ(p.err_capture.back(),  "2");
}

PEANATS_NAMESPACE_END