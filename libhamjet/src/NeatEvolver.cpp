#include "Hamjet\NeatEvolver.hpp"

namespace Hamjet {

	Genome::Genome(int in, int on, int hn, std::vector<Gene> gs) :
		inNodes(in), outNodes(on), hiddenNodes(hn),
		genes(gs), fitness(0) {}

	std::shared_ptr<NeuralNet> Genome::buildNeuralNet() {
		auto net = std::make_shared<NeuralNet>();

		for (int i = 0; i < inNodes + outNodes + hiddenNodes; i++) {
			auto n = std::make_shared<NeuralNetNode>();
			net->addNode(n);
		}

		for (auto& g : genes) {
			if (!g.disabled) {
				net->getNode(g.nodeTo)->connect(std::make_unique<NeuralNetConnection>(net->getNode(g.nodeFrom), g.weight));
			}
		}

		return net;
	}

	bool Genome::fitnessComp(const std::shared_ptr<Genome>& g1, const std::shared_ptr<Genome>& g2) {
		return g1->fitness > g2->fitness;
	}

	NeatEvolver::NeatEvolver(std::shared_ptr<NeatSimulator>& sim) : twister(0), simulator(sim) {
		firstGeneration();
	}

	void NeatEvolver::firstGeneration() {
		for (int i = 0; i < generationSize; i++) {
			auto g = std::make_shared<Genome>(simulator->getNumInputs(), simulator->getNumOutputs(), 0, std::vector<Gene>());
			generation.push_back(g);
		}

		mutateGeneration(generation);
	}

	void NeatEvolver::evolveNextGeneration() {
		Generation nextGen;

		generation.sort(Genome::fitnessComp);

		auto gi = generation.begin();
		int totalFitness = 0;
		for (int i = 0; i < generation.size() / 2; i++) {
			totalFitness += (*gi++)->fitness;
		}
		std::vector<std::shared_ptr<Genome>> winningGenomes(std::begin(generation), gi);

		int generationSize = generation.size();
		int offspringLeft = generationSize;

		int secondTotal = 0;
		for (auto genome : winningGenomes) {
			secondTotal += genome->fitness;
			int offspring = (int)ceil((float)generationSize * ((float)genome->fitness / (float)totalFitness));
			if (offspringLeft < offspring) {
				offspring = offspringLeft;
			}
			offspringLeft -= offspring;

			for (int i = 0; i < offspring; i++) {
				int otherIndex = twister() % winningGenomes.size();
				auto& otherGenome = *winningGenomes[otherIndex];

				nextGen.push_back(breed(*genome, otherGenome));
			}
		}

		mutateGeneration(nextGen);

		printf("New generation size: %d\n", nextGen.size());

		generation = nextGen;
	}

	void NeatEvolver::mutateGeneration(Generation& gen) {
		InnovationSet innovations;
		for (auto& g : gen) {
			randomInnovation(*g, innovations);
			mutateGenes(*g);
		}
	}


	float NeatEvolver::genomeDistance(Genome& g1, Genome& g2) {
		auto genes1 = g1.genes.begin();
		auto genes2 = g2.genes.begin();

		return 1;
	}

	std::shared_ptr<Genome> NeatEvolver::breed(Genome& g1, Genome& g2) {
		Genome& winner = g1.fitness > g2.fitness ? g1 : g2;
		Genome& loser = g1.fitness > g2.fitness ? g2 : g1;

		bool equalFitness = winner.fitness == loser.fitness;

		auto winnerGenes = winner.genes.begin();
		auto loserGenes = loser.genes.begin();

		int newHiddenNodes = winner.hiddenNodes > loser.hiddenNodes ? winner.hiddenNodes : loser.hiddenNodes;
		std::vector<Gene> newGenes;

		while (winnerGenes != winner.genes.end() || loserGenes != loser.genes.end()) {
			bool hasWinner = winnerGenes != winner.genes.end();
			bool hasLoser = loserGenes != loser.genes.end();
			int winnerInnovation = hasWinner ? winnerGenes->innovationNumber : 0;
			int loserInnovation = hasLoser ? loserGenes->innovationNumber : 0;

			if (hasWinner && hasLoser) {
				// We have entries in both
				if (winnerInnovation == loserInnovation) {
					// They're both the same gene
					Gene g = *winnerGenes;
					if (winnerGenes->disabled || loserGenes->disabled) {
						g.disabled = true;
					}
					newGenes.push_back(g);

					winnerGenes++;
					loserGenes++;
				}
				else if (winnerInnovation > loserInnovation) {
					// We're on a disjoint loser gene, which we ignore unless it's a draw
					if (equalFitness) {
						newGenes.push_back(*loserGenes);
					}
					loserGenes++;
				}
				else if (loserInnovation > winnerInnovation) {
					// Always take winner genes
					newGenes.push_back(*winnerGenes);
					winnerGenes++;
				}
			}
			else if (hasWinner) {
				// We only have winner genes left
				newGenes.push_back(*winnerGenes);
				winnerGenes++;
			}
			else {
				// We only have loser genes left, drop them unless it's a draw
				if (equalFitness) {
					newGenes.push_back(*loserGenes);
				}
				loserGenes++;
			}
		}

		return std::make_shared<Genome>(g1.inNodes, g1.outNodes, newHiddenNodes, newGenes);
	}

	void NeatEvolver::randomInnovation(Genome& genome, InnovationSet& innovations) {
		float r = (float)twister() / (float)twister.max();
		if (r < addNodeChance && !genome.genes.empty()) {
			int splitIndex = twister() % genome.genes.size();

			int newHiddenNode = genome.inNodes + genome.outNodes + genome.hiddenNodes;
			genome.hiddenNodes++;

			Gene* oldGene = &genome.genes[splitIndex];
			oldGene->disabled = true;

			Gene g1 = { -1, oldGene->nodeFrom, newHiddenNode, oldGene->weight, false };
			Gene g2 = { -1, newHiddenNode, oldGene->nodeTo, 1, false };

			tryAddGene(genome, g1, innovations);
			tryAddGene(genome, g2, innovations);
		}
		else if (r < addConnectionChance) {
			// Can only come from an input or hidden node
			int possibleFroms = genome.inNodes + genome.hiddenNodes;
			// Can only go to a hidden or output node
			int possibleTos = genome.outNodes + genome.hiddenNodes;

			// Get the index of the from node, shifting past the output nodes if needs be.
			int from = twister() % possibleFroms;
			if (from >= genome.inNodes) {
				from = from + genome.outNodes;
			}

			// Get the index of the to node, shifting it past the input nodes.
			int to = (twister() % possibleTos) + genome.inNodes;

			// New gene with a weight somewhere randomly between -0.5 and 0.5
			Gene g = { -1, from, to, ((float)twister() / (float)twister.max()) - 0.5f, false };
			tryAddGene(genome, g, innovations);
		}
	}

	void NeatEvolver::mutateGenes(Genome& genome) {
		for (auto gene : genome.genes) {
			// Shift the gene by a random amount between -0.2f, and 0.2f;
			gene.weight += (((float)twister() / (float)twister.max()) - 0.5f) * 0.2f;
		}
	}

	void NeatEvolver::tryAddGene(Genome& genome, Gene& gene, InnovationSet& innovations) {
		auto og = innovations.find(gene);
		if (og != innovations.end()) {
			genome.genes.push_back(*og);
		}
		else {
			gene.innovationNumber = globalInnovationNumber++;
			genome.genes.push_back(gene);
			innovations.insert(gene);
		}
	}

	std::shared_ptr<Genome> NeatEvolver::processGeneration() {
		std::shared_ptr<Genome> winner;

		for (auto& g : generation) {
			g->fitness = simulator->evaluateGenome(*g);

			if (!winner.get() || winner->fitness < g->fitness) {
				winner = g;
			}
		}

		evolveNextGeneration();

		return winner;
	}
}