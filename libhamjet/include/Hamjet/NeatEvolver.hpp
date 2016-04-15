#ifndef HAMJET_NEATEVOLVER_HPP
#define HAMJET_NEATEVOLVER_HPP

#include <memory>
#include <random>
#include <set>
#include <list>
#include <vector>

#include "Hamjet/NeuralNet.hpp"

namespace Hamjet {

	class Gene {
	public:
		int innovationNumber;
		int nodeFrom;
		int nodeTo;
		float weight;
		bool disabled;
	};

	class GeneInnovationComp {
	public:
		bool operator()(const Gene& g1, const Gene& g2) const {
			return (g1.nodeFrom <= g2.nodeFrom) && ((g1.nodeFrom != g2.nodeFrom) || (g1.nodeTo < g2.nodeTo));
		}
	};

	class Genome {
	public:
		int fitness;

		int inNodes;
		int outNodes;
		int hiddenNodes;
		std::vector<Gene> genes;

	public:
		Genome(int in, int on, int hn, std::vector<Gene> gs);
		Genome(const Genome& o);
		std::shared_ptr<NeuralNet> buildNeuralNet();
		static bool fitnessComp(const std::shared_ptr<Genome>& g1, const std::shared_ptr<Genome>& g2);
	};

	class Species {
	public:
		std::vector<std::shared_ptr<Genome>> genomes;
		float sharedFitness = 0;
		float maxFitness = 0;
		int staleness = 0;
	};

	class NeatSimulator {
	public:
		virtual int getNumInputs() = 0;
		virtual int getNumOutputs() = 0;

		virtual int evaluateGenome(Genome&) = 0;
	};

	class NeatEvolver {
	public:
		typedef std::set<Gene, GeneInnovationComp> InnovationSet;
		typedef std::list<std::shared_ptr<Genome>> Generation;
		typedef std::list<Species> SpeciatedGeneration;

		const int generationSize = 150;
		const float addNodeChance = 0.15f;
		const float addConnectionChance = 0.30f;

		std::shared_ptr<NeatSimulator> simulator;

		int generationCount = 0;
		int globalInnovationNumber = 0;
		Generation generation;
		SpeciatedGeneration speciatedGeneration;

		std::mt19937 twister;

	public:
		NeatEvolver(std::shared_ptr<NeatSimulator>& sim);
		void firstGeneration();
		void addGenomeToGeneration(std::shared_ptr<Genome> genome, SpeciatedGeneration& generation);
		void evolveNextGeneration();
		void mutateGeneration(Generation& gen);
		float genomeDistance(const Genome& g1, const Genome& g2);
		std::shared_ptr<Genome> breed(Genome& g1, Genome& g2);
		void randomInnovation(Genome& genome, InnovationSet& innovations);
		void mutateGenes(Genome& genome);
		void tryAddGene(Genome& genome, Gene& gene, InnovationSet& innovations);
		std::shared_ptr<Genome> processGeneration();
	};

}

#endif