#ifndef HAMJET_NEURALNET_HPP
#define HAMJET_NEURALNET_HPP

#include <memory>
#include <list>
#include <vector>

namespace Hamjet {

	class NeuralNetNode;

	class NeuralNetConnection {
	public:
		std::weak_ptr<NeuralNetNode> from;
		float weight;

	public:
		NeuralNetConnection(std::weak_ptr<NeuralNetNode> f, float w);
		float getContribution();
	};

	class NeuralNetNode {
	private:
		float currentValue;
		float nextValue;

		int stepStage = 0;

		std::list<std::unique_ptr<NeuralNetConnection>> incomingConnections;

	public:
		void collectNextValue();
		void progress();
		float value();
		void setValue(float v);
		void connect(std::unique_ptr<NeuralNetConnection>& c);
		static float sigmoid(float x);
		bool processAtStage(int s);
		void setProcessStage(int s);
	};

	class NeuralNet {
	private:
		std::vector<std::shared_ptr<NeuralNetNode>> nodes;

	public:
		void addNode(std::shared_ptr<NeuralNetNode>& node);
		std::shared_ptr<NeuralNetNode> getNode(int index);
		void stepNetwork();
	};

}

#endif