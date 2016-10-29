#include "edgeReader.hpp"
#include "nodeCollection.hpp"
#include "ClusterPlay.hpp"
#include "DBscanPlay.hpp"
#include "Dim.hpp"
#include <string>
#include <cstdlib>

int main(int argc, char* args[]){

	if(argc < 3){
		std::cerr<<"Error occured! please check the usage!"<<std::endl;
		std::cerr<<"usage : BlackHole.exe [Input] [Dimension] [Alpha] [MinPTS] [PruningPercents]"<<std::endl;
		std::cerr<<"example) ./BlackHole ../Data/football.dat 2 0.01 5 0.1"<<std::endl;
		return -1;
	}

	srand((unsigned int) time( NULL));


	// input parameter processing
	double alpha = atof(args[3]);

	std::cout<<"alpha = "<<alpha<<std::endl;
	DIMENSION = atoi(args[2]);

	std::cout<<"dimens = "<<DIMENSION<<std::endl;
	int minPts = atoi(args[4]);
	std::cout<<"minpts = "<<minPts<<std::endl;

	double pruningFraction = atof(args[5]);
	std::cout<<"priningFactor = "<<pruningFraction<<std::endl;

	ClusterPlay cp;

	std::string fileName(args[1]);
	fileName.append("_dimsnsion_");
	fileName.append(args[2]);
	fileName.append("_alpha_");
	fileName.append(args[3]);
	fileName.append("_minPts_");
	fileName.append(args[4]);
	fileName.append("_pruningFactor_");
	fileName.append(args[5]);
	fileName.append("_position.out");

	cp.play(args[1], alpha, args[3], fileName.c_str());

	// DBSCAN
	DBscanPlay::dbscanCalculator((char *)fileName.c_str(), minPts, pruningFraction/100.0f);

	return 0;
}
