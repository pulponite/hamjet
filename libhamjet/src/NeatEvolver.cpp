#include "Hamjet\NeatEvolver.hpp"

#include <algorithm>

namespace Hamjet {

	Genome::Genome(int in, int on, int hn, std::vector<Gene> gs) :
		inNodes(in), outNodes(on), hiddenNodes(hn),
		genes(gs), fitness(0) {}

	Genome::Genome(const Genome& o) : Genome(o.inNodes, o.outNodes, o.hiddenNodes, o.genes) {}

	std::shared_ptr<NeuralNet> Genome::buildNeuralNet() {
		auto net = std::make_shared<NeuralNet>();

		for (int i = 0; i < inNodes + outNodes + hiddenNodes; i++) {
			auto n = std::make_shared<NeuralNetNode>();

			if (i >= inNodes && i < inNodes + outNodes) {
				n->setProcessStage(1);
			}
			else {
				n->setProcessStage(0);
			}
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
		Species s1;
		Species s2;


		for (int i = 0; i < generationSize; i++) {
			auto g = std::make_shared<Genome>(simulator->getNumInputs(), simulator->getNumOutputs(), 0, std::vector<Gene>());
			generation.push_back(g);

			if (i % 2 == 0) {
				s1.genomes.push_back(g);
			}
			else {
				s2.genomes.push_back(g);
			}
		}

		speciatedGeneration.push_back(s1);
		speciatedGeneration.push_back(s2);

		mutateGeneration(generation);

		generationCount++;
	}

	void NeatEvolver::addGenomeToGeneration(std::shared_ptr<Genome> genome, SpeciatedGeneration& generation) {
		const float maxDistance = 1;

		bool foundSpot = false;
		for (auto& species : generation) {
			float distance = genomeDistance(*genome, **species.genomes.begin());
			if (genomeDistance(*genome, **species.genomes.begin()) < maxDistance) {
				species.genomes.push_back(genome);
				foundSpot = true;
				break;
			}
		}

		if (!foundSpot) {
			Species s;
			s.genomes.push_back(genome);
			s.genomes.push_back(genome);
			s.genomes.push_back(genome);
			generation.push_back(s);
		}
	}

	void NeatEvolver::evolveNextGeneration() {
		// Sort each species by fitness & cull bottom half
		const int generationSize = 150;
		const float combineChance = 0.75;
		float totalFitness = 0;
		SpeciatedGeneration newSpeciatedGeneration;
		Generation newGeneration;

		// Delete loser half of each species, calculate species fitness
		for (auto& species : speciatedGeneration) {
			std::sort(species.genomes.begin(), species.genomes.end(), Genome::fitnessComp);
			species.genomes.resize(species.genomes.size() / 2);
			std::random_shuffle(species.genomes.begin(), species.genomes.end());

			int speciesFitness = 0;
			int maxFitness = 0;
			for (auto& genome : species.genomes) {
				speciesFitness += genome->fitness;
				if (genome->fitness > maxFitness) {
					maxFitness = genome->fitness;
				}
			}
			species.sharedFitness = (float)speciesFitness / species.genomes.size();

			if (maxFitness > species.maxFitness) {
				species.maxFitness = maxFitness;
				species.staleness = 0;
			}
			else {
				species.staleness++;
			}
		}

		// Purge empty or unimproving species
		speciatedGeneration.remove_if([](Species species) -> bool {
			return species.genomes.empty() || species.staleness > 500;
		});

		if (speciatedGeneration.empty()) {
			Species s1;
			Species s2;
			s1.sharedFitness = 1;
			s2.sharedFitness = 1;

			int i = 0;
			for (auto& g : generation) {
				if (i++ % 2 == 0) {
					s1.genomes.push_back(g);
				}
				else {
					s2.genomes.push_back(g);
				}
			}

			speciatedGeneration.push_back(s1);
			speciatedGeneration.push_back(s2);
		}

		// Calculate total fitness and seed surviving species
		for (auto& species : speciatedGeneration) {
			totalFitness += species.sharedFitness;
			Species newSpecies;
			newSpecies.maxFitness = species.maxFitness;
			newSpecies.staleness = species.staleness;
			newSpecies.genomes.push_back(*species.genomes.begin());
			newSpeciatedGeneration.push_back(newSpecies);
		}

		// Make offspring from remaining generations
		for (auto& species : speciatedGeneration) {
			int offspring = int(150 * (species.sharedFitness / totalFitness));

			for (int i = 0; i < offspring; i++) {
				float r = (float)twister() / (float)twister.max();

				int g1 = offspring % species.genomes.size();
				int g2 = twister() % species.genomes.size();

				if (r < combineChance) {
					auto newGenome = breed(*species.genomes[g1], *species.genomes[g2]);
					addGenomeToGeneration(newGenome, newSpeciatedGeneration);
					newGeneration.push_back(newGenome);
				}
				else {
					auto newGenome = std::make_shared<Genome>(*species.genomes[g1]);
					addGenomeToGeneration(newGenome, newSpeciatedGeneration);
					newGeneration.push_back(newGenome);
				}
			}
		}
		
		// Purge empty
		speciatedGeneration.remove_if([](Species species) -> bool {
			return species.genomes.empty();
		});

		mutateGeneration(newGeneration);
		generation = newGeneration;
		speciatedGeneration = newSpeciatedGeneration;
		generationCount++;
	}

	void NeatEvolver::mutateGeneration(Generation& gen) {
		InnovationSet innovations;
		for (auto& g : gen) {
			randomInnovation(*g, innovations);
			mutateGenes(*g);
		}
	}


	float NeatEvolver::genomeDistance(const Genome& g1, const Genome& g2) {
		const float c1 = 2;
		const float c2 = 2;
		const float c3 = 0.4f;
		
		float n =fmax((float)g1.genes.size(), (float)g2.genes.size());
		if (g1.genes.size() < 20 || g2.genes.size() < 20) {
			n = 1;
		}

		float weightDifference = 0;
		int numMatchingGenes = 0;
		int numDisjointGenes = 0;
		int numExcessGenes = 0;

		auto genes1 = g1.genes.begin();
		auto genes2 = g2.genes.begin();

		while (genes1 != g1.genes.end() || genes2 != g2.genes.end()) {
			bool has1 = genes1 != g1.genes.end();
			bool has2 = genes2 != g2.genes.end();
			int innovation1 = has1 ? genes1->innovationNumber : 0;
			int innovation2 = has2 ? genes2->innovationNumber : 0;

			if (has1 && has2) {
				if (innovation1 == innovation2) {
					// We're in the matching gene prefix
					weightDifference += genes1->weight - genes2->weight;
					numMatchingGenes++;
					genes1++;
					genes2++;
				}
				else if (innovation1 > innovation2) {
					numDisjointGenes++;
					genes1++;
				}
				else {
					numDisjointGenes++;
					genes2++;
				}
			}
			else if (has1) {
				numExcessGenes++;
				genes1++;
			}
			else {
				numExcessGenes++;
				genes2++;
			}
		}

		return (c1 * numExcessGenes / n) + (c2 * numDisjointGenes / n) + (c3 * weightDifference / fmax(1,numMatchingGenes));
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