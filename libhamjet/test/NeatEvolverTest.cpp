#include "Hamjet/NeatEvolver.hpp"
#include "gtest/gtest.h"

using Hamjet::Gene;
using Hamjet::Genome;
using Hamjet::NeatEvolver;
using Hamjet::NeatSimulator;

class NeatEvolverTest : public ::testing::Test {
protected:
	class DummySimulator : public NeatSimulator {
	public:
		virtual int getNumInputs() { return 1; }
		virtual int getNumOutputs() { return 1; }
		virtual int evaluateGenome(Genome&) {
			return 0;
		}
	};

	std::shared_ptr<NeatSimulator> sim;

protected:
	virtual void SetUp() {
		sim = std::make_shared<DummySimulator>();
	}
	
	void genesEqual(Gene& g1, Gene& g2) {
		EXPECT_EQ(g1.disabled, g2.disabled);
		EXPECT_EQ(g1.nodeFrom, g2.nodeFrom);
		EXPECT_EQ(g1.nodeTo, g2.nodeTo);
		EXPECT_EQ(g1.innovationNumber, g2.innovationNumber);
	}
};

TEST_F(NeatEvolverTest, BreedingEqualUnchanged) {
	NeatEvolver e(sim);

	auto g1 = Genome(1, 1, 1, std::vector<Gene>({ { 1, 0, 1, 1.0f, false } }));
	auto g2 = Genome(1, 1, 1, std::vector<Gene>({ { 1, 0, 1, 1.0f, false } }));

	auto bg = e.breed(g1, g2);
	EXPECT_EQ(1, bg->genes.size());
	genesEqual(g1.genes[0], g2.genes[0]);
}

TEST_F(NeatEvolverTest, BreedingLongerLoser) {
	NeatEvolver e(sim);

	auto g1 = Genome(1, 1, 1, std::vector<Gene>({ { 1, 0, 1, 1.0f, false }, { 2, 0, 2, 1.0f, false } }));
	auto g2 = Genome(1, 1, 1, std::vector<Gene>({ { 1, 0, 1, 1.0f, false } }));
	g1.fitness = 1;
	g2.fitness = 2;

	auto bg = e.breed(g1, g2);
	EXPECT_EQ(1, bg->genes.size());
	genesEqual(bg->genes[0], g2.genes[0]);
}

TEST_F(NeatEvolverTest, BreedingLongerWinner) {
	NeatEvolver e(sim);

	auto g1 = Genome(1, 1, 1, std::vector<Gene>({ { 1, 0, 1, 1.0f, false },{ 2, 0, 2, 1.0f, false } }));
	auto g2 = Genome(1, 1, 1, std::vector<Gene>({ { 1, 0, 1, 1.0f, false } }));
	g1.fitness = 2;
	g2.fitness = 1;

	auto bg = e.breed(g1, g2);
	EXPECT_EQ(2, bg->genes.size());
	genesEqual(bg->genes[0], g1.genes[0]);
	genesEqual(bg->genes[1], g1.genes[1]);
}


TEST_F(NeatEvolverTest, BreedingUnevenGenes) {
	NeatEvolver e(sim);

	auto g1 = Genome(1, 1, 1, std::vector<Gene>({ { 1, 0, 1, 1.0f, false },{ 2, 0, 2, 1.0f, false },{ 4, 0, 2, 1.0f, false } }));
	auto g2 = Genome(1, 1, 1, std::vector<Gene>({ { 1, 0, 1, 1.0f, false },{ 3, 0, 2, 1.0f, false },{ 5, 0, 2, 1.0f, false } }));
	g1.fitness = 2;
	g2.fitness = 1;

	auto bg = e.breed(g1, g2);
	EXPECT_EQ(3, bg->genes.size());
	genesEqual(bg->genes[0], g1.genes[0]);
	genesEqual(bg->genes[1], g1.genes[1]);
	genesEqual(bg->genes[2], g1.genes[2]);
}