#include "Hamjet/NeuralNet.hpp"
#include "gtest/gtest.h"

using Hamjet::NeuralNet;
using Hamjet::NeuralNetNode;
using Hamjet::NeuralNetConnection;

class NeuralNetTest : public ::testing::Test {
protected:
	NeuralNet net;

	std::shared_ptr<NeuralNetNode> in1;
	std::shared_ptr<NeuralNetNode> out1;
	

protected:
	virtual void SetUp() {
		in1 = std::make_shared<NeuralNetNode>();
		out1 = std::make_shared<NeuralNetNode>();

		net.addNode(in1);
		net.addNode(out1);

		out1->connect(std::make_unique<NeuralNetConnection>(in1, 0.5f));
	}
};

TEST_F(NeuralNetTest, WithoutInputStaysZero) {
	for (int i = 0; i < 3; i++) {
		net.stepNetwork();
		EXPECT_FLOAT_EQ(0, out1->value());
	}
}

TEST_F(NeuralNetTest, SettingInputHighTriggersOutput) {
	in1->setValue(1.0f);
	net.stepNetwork();
	EXPECT_LT(0.1f, out1->value());
}

TEST_F(NeuralNetTest, InputsResetLowAfterTriggering) {
	in1->setValue(1.0f);
	net.stepNetwork();
	EXPECT_FLOAT_EQ(0, in1->value());
}

TEST_F(NeuralNetTest, OutputsResetLowAfterTriggering) {
	in1->setValue(1.0f);
	net.stepNetwork();
	net.stepNetwork();
	EXPECT_FLOAT_EQ(0, out1->value());
}