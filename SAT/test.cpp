#include <iostream>
#include <string>
//import<gtest/gtest.h>;
import Sat;

TEST(Basic, SetVar) {
  Sat_t Sat({{1, 2, -3}, {-1, 2}});
  // ( x1 | x2 | ~x3 ) & ( ~x1 | x2 )
  auto Output = Sat.setVar(-1);
  EXPECT_STREQ(Output.dumpStr().data(), "( x2 | ~x3 )");
  Output = Sat.setVar(1);
  EXPECT_STREQ(Output.dumpStr().data(), "( x2 )");
  Output = Sat.setVar(2);
  EXPECT_STREQ(Output.dumpStr().data(), "");
  Output = Sat.setVar(3);
  EXPECT_STREQ(Output.dumpStr().data(), "( x1 | x2 ) & ( ~x1 | x2 )");
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}