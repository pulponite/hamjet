#include "Hamjet/NeuralNet.hpp"

namespace Hamjet {

	NeuralNetConnection::NeuralNetConnection(std::weak_ptr<NeuralNetNode> f, float w) : from(f), weight(w) {}

	float NeuralNetConnection::getContribution() {
		if (auto f = from.lock()) {
			return f->value() * weight;
		}
		else {
			return 0;
		}
	}


	void NeuralNetNode::collectNextValue() {
		if (incomingConnections.empty()) {
			nextValue = currentValue;
		}
		else {
			float v = 0;
			for (auto& conn : incomingConnections) {
				v += conn->getContribution();
			}
			nextValue = sigmoid(v);
		}
	}

	void NeuralNetNode::progress() {
		currentValue = nextValue;
	}

	float NeuralNetNode::value() {
		return currentValue;
	}

	void NeuralNetNode::setValue(float v) {
		currentValue = v;
	}

	void NeuralNetNode::connect(std::unique_ptr<NeuralNetConnection>& c) {
		incomingConnections.push_back(std::move(c));
	}

	// f(x) is in [-1, 1]
	float NeuralNetNode::sigmoid(float x) {
		float e = exp(x * 4);
		return (e - 1.0f) / (e + 1.0f);
	}

	bool NeuralNetNode::processAtStage(int i) {
		return i == stepStage;
	}

	void NeuralNetNode::setProcessStage(int i) {
		stepStage = i;
	}


	void NeuralNet::addNode(std::shared_ptr<NeuralNetNode>& node) {
		nodes.push_back(node);
	}

	std::shared_ptr<NeuralNetNode> NeuralNet::getNode(int index) {
		return nodes[index];
	}

	void NeuralNet::stepNetwork() {
		int steppedCount = 0;
		int stage = 0;
		while (steppedCount < nodes.size()) {
			for (auto& node : nodes) {
				if (node->processAtStage(stage)) {
					node->collectNextValue();
					node->progress();
					steppedCount++;
				}
			}
			stage++;
		}
	}
}