#include "Hamjet/Math.hpp"

#include "gtest/gtest.h"

TEST(FVector3Test, NormalizedLengthAround1) {
	Hamjet::FVector3 v1(10, 10, 10);
	EXPECT_FLOAT_EQ(v1.normalize().magnitude(), 1);
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}