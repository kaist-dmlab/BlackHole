#include "nodeCollection.hpp"
#include "edgeReader.hpp"
#include "memoryManager.hpp"
#include <ctime>
#include <vector>
#include <cstdio>
#include <sstream>
#include <string>
#include "Dim.hpp"

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))
#pragma once


class ClusterPlay{
public :
	 void play(char* inputfile, double alpha, char* alphaChar, const char* fileFullName){
		clock_t start_time, end_time;      // clock_t 
		int iter = 100;

		nodeCollection nc;
		edgeReader fr;
		
		// Read inputfile and make a graph
		fr.readFile(inputfile, &nc);

		std::cout<<"check it!"<<std::endl;
		nc.copyToVector();	//To use vector (vector is much faster than set,)
		nc.degreeSet();

		exponentVar expVar(alpha, 0.0, nc.getSumOfDegree());

		memoryManager memMgr(nc.getNodeVec()->size());

		// Build OCT TREE
		OctTree* tt = buildOctTree(&nc, &memMgr);

		std::vector<blackHoleNode*>* vecp = nc.getNodeVec();

		// To get initial energy
		double initEnergy = 0.0;
		for(unsigned int k = 0; k < (*vecp).size(); k++){
			initEnergy += nc.getEnergy((*vecp)[k], expVar, tt); 
		}
		std::cout<<"initEnergy = "<<initEnergy<<std::endl;

		start_time = clock(); 

		//#########################UPDATE FUNCTION#########################
		for(int i = 0; i < iter; i++){
			// Memory point initialize
			memMgr.setChildCurrent(0);
			memMgr.setCurrent(0);

			// Update Barnes hut algorithm
			updateBarneshut(i, &nc, expVar, &memMgr, iter);	//UPDATE FUNCTION
		}
		//#############################END#############################

		end_time = clock();  
		printf("Time : %f\n", ((double)(end_time-start_time)) / CLOCKS_PER_SEC);  

		std::cout<<"Current = "<<memMgr.getCurrent()<<"\t ChildCurrent = "<<memMgr.getChildCurrent()<<std::endl;

		std::string fileName(fileFullName);

		char *outputName=new char[fileName.size()+1];
		outputName[fileName.size()]=0;
		memcpy(outputName,fileName.c_str(),fileName.size());

		// Write graph information to file
		fr.writeFile(outputName,&nc);

		delete outputName;


		printf("######################################\nLAYOUT IS FINISHED!"
				"\n######################################\n");

		nc.clearAll();
		memMgr.dealloc();
	}


	 bool updateBarneshut(int currentIter, nodeCollection* p, exponentVar& expVar, memoryManager* mgr, int nrIteration){

		double energySum = 0.0;

		std::vector<blackHoleNode*>* vect = p->getNodeVec();
		
		OctTree* octTree = buildOctTree(p, mgr);

		adjustComponent(currentIter+1, nrIteration, expVar);
		
		double attrExponent =  expVar.getAttrExponent();
		double repuExponent = expVar.getRepuExponent();

		double* XY = new double[DIMENSION];
		double* X1Y1 = new double[DIMENSION];
		double* oldXY = new double[DIMENSION];
		double* bestDir = new double[DIMENSION];

		for(int i = 0; i < DIMENSION; i++){
			XY[i] = 0.0f;
			X1Y1[i] = 0.0f;
			oldXY[i] = 0.0f;
		}



		for(unsigned int i = 0; i < (*vect).size(); i++){

			for(int z = 0; z < DIMENSION; z++){
				bestDir[z] = 0.0f;
			}

			double oldEnergy = p->getEnergy((*vect)[i], expVar, octTree);
			p->setDir((*vect)[i], bestDir, expVar, octTree);
			for (int k = 0; k < DIMENSION; k++){
				X1Y1[k] = (*vect)[i]->getValue(k);
				XY[k] = X1Y1[k];
				oldXY[k] = X1Y1[k];
			}
			
			double bestEnergy = oldEnergy;
			
			int bestMultiple = 0;

			for (int k = 0; k < DIMENSION; k++){
				bestDir[k] = bestDir[k] / 32.0;
			}
						
			for (int multiple = 32;	 multiple >= 1 && (bestMultiple==0 || bestMultiple/2==multiple); multiple /= 2) {
				octTree->removeNode((*vect)[i],XY, 0, mgr);
				for (int ss = 0; ss < DIMENSION; ss++){
					(*vect)[i]->setValue(oldXY[ss] + bestDir[ss] * multiple, ss);
					XY[ss] = oldXY[ss] + bestDir[ss] * multiple;
				}

				octTree->addNode((*vect)[i], XY, 0, mgr);
				double curEnergy = p->getEnergy((*vect)[i], expVar, octTree);		

				if(curEnergy < bestEnergy){
					bestEnergy = curEnergy;
					bestMultiple = multiple;
				}
			}


			for (int multiple = 64;  multiple <= 128 && bestMultiple == multiple/2;  multiple *= 2) {
				octTree->removeNode((*vect)[i],XY, 0, mgr);

				for (int ss = 0; ss < DIMENSION; ss++){
					(*vect)[i]->setValue(oldXY[ss] + bestDir[ss] * multiple, ss);
					XY[ss] = oldXY[ss] + bestDir[ss] * multiple;
				}
				octTree->addNode((*vect)[i], XY, 0, mgr);

				double curEnergy = p->getEnergy((*vect)[i], expVar, octTree);

				if(curEnergy < bestEnergy){
					bestEnergy = curEnergy;
					bestMultiple = multiple;
				}
			}

			octTree->removeNode((*vect)[i],XY, 0, mgr);
			for (int ss = 0; ss < DIMENSION; ss++){
				(*vect)[i]->setValue(oldXY[ss] + bestDir[ss] * bestMultiple, ss);
				XY[ss] = oldXY[ss] + bestDir[ss] * bestMultiple;
			}

			octTree->addNode((*vect)[i], XY, 0, mgr);
			energySum += bestEnergy;
		}

		std::cout<<"#### ITER[" <<currentIter+1<<"]  E = " <<energySum <<"  ATT = "<<attrExponent<<"  REP = " <<repuExponent<<std::endl;
		
		delete XY;
		delete X1Y1;
		delete oldXY;
		delete bestDir;

		return true;
	}

	 std::string currentDateTime() {
	    time_t     now = time(0);
	    struct tm  tstruct;
	    char       buf[80];
	    tstruct = *localtime(&now);
	    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
	    return buf;
	}

	 OctTree* buildOctTree(nodeCollection* ncp, memoryManager* mgr){

		double* temp = new double[DIMENSION];
		double* positionTemp = new double[DIMENSION];
		double* minPos = new double[DIMENSION];
		double* position = new double[DIMENSION];
		double* maxPos = new double[DIMENSION];


		for(int i = 0; i < DIMENSION; i++){
			minPos[i] = 0.0f;
			maxPos[i] = 0.0f;
			position[i] = 0.0f;
			positionTemp[i] = 0.0f;
			temp[i] = 0.0f;
		}


		for (int i = 0; i < DIMENSION; i++){
			minPos[i] = 999999;
			maxPos[i] = -999999;
		}

		std::vector<blackHoleNode*>* vect = ncp->getNodeVec();
		for(unsigned int s = 0; s < (*vect).size(); s++){	//to search all nodes
			if((*vect)[s]->getDegree() == 0)
				continue;

			for (int z = 0; z < DIMENSION; z++){
				position[z] = (*vect)[s]->getValue(z);
			}

			for (int d = 0; d < DIMENSION; d++){
				minPos[d] = min(position[d], minPos[d]);
				maxPos[d] = max(position[d], maxPos[d]);
			}
		}

		// provide additional space for moving nodes
		for (int d = 0; d < DIMENSION; d++){
			double posDiff = maxPos[d] - minPos[d];
			maxPos[d] += posDiff / 2;
			minPos[d] -= posDiff / 2;
		}
		
		// add nodes with non-zero weight to the octtree
		OctTree* result = mgr->get_Instance();
		result->setElement(NULL, temp, minPos, maxPos, mgr);

		for(unsigned int s = 0; s <  (*vect).size(); s++){	//to all nodes search
			for (int kk = 0; kk < DIMENSION; kk++){
				positionTemp[kk] = (*vect)[s]->getValue(kk);
			}
			blackHoleNode* t = (*vect)[s];
			result->addNode(t, positionTemp, 0, mgr);
		}

		delete minPos;
		delete maxPos;
		delete position;
		delete positionTemp;
		delete temp;

		return result;
	}


	 static void adjustComponent(int i, int iter, exponentVar& expVar){
		if (iter >= 50 && expVar.getFinalRepuExponent() < 1.0) {
			expVar.setAttrExponent(expVar.getFinalAttrExponent());
			expVar.setRepuExponent(expVar.getFinalRepuExponent());

			if (i <= 0.6*iter) {
				// use energy model with few local minima 
				expVar.setAttrExponent(expVar.getAttrExponent() + 1.1 * ( 1.0 - expVar.getFinalRepuExponent() ));
				expVar.setRepuExponent(expVar.getRepuExponent() + 0.9 * ( 1.0 - expVar.getFinalRepuExponent() ));
			} else if (i <= 0.9*iter) {
				expVar.setAttrExponent(expVar.getAttrExponent() + 1.1 * ( 1.0 - expVar.getFinalRepuExponent() ) * (0.9 - ((double)i)/iter) / 0.3 );
				expVar.setRepuExponent(expVar.getRepuExponent() + 0.9 * ( 1.0 - expVar.getFinalRepuExponent() ) * (0.9 - ((double)i)/iter) / 0.3 );		
			}
		}
	}
};





