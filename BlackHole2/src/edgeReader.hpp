#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>      
#include <vector>
#include <cstring>
#include <cstdlib>
#include "nodeCollection.hpp"
#pragma once

class edgeReader{
private :
	std::ofstream outputFileStream;
	std::ifstream inputFileStream1;
	std::ifstream inputFileStream2;
public :

	// Read edgelist input file
	inline void readFile(char* filePath, nodeCollection* ncp){
		inputFileStream1.open(filePath);
		std::string line;
		std::string x, y, del;
		int maxValue, a, b;
		char oneLine[256];
		maxValue = 0;

		//To find max Value (to make dynamic array)
		if(inputFileStream1.is_open()){
			int lineNumber = 0;
			while(getline(inputFileStream1, line)){
				strcpy(oneLine,line.c_str());
				del = strtok(oneLine,"\t ");
				x = del;
				y = strtok(NULL,"\t ");
				lineNumber++;
				a = atoi(x.c_str());
				b = atoi(y.c_str());

				if ( a > b){
					if(a > maxValue){
						maxValue = a;}
				}
				else{
					if(b > maxValue){
						maxValue = b;}
				}
			}
		}
		else{
			std::cout<<"Error Occured!"<<std::endl;
		}


		ncp->setDegMat(maxValue);	//Make degMatrix
		inputFileStream1.seekg(0, inputFileStream1.beg);		//to read again
		inputFileStream1.close();
		inputFileStream2.open(filePath);

		if(inputFileStream2.is_open()){
			while(getline(inputFileStream2, line)){

				strcpy(oneLine,line.c_str());

				//std::cout<<oneLine<<std::endl;
				del = strtok(oneLine,"\t ");
				x = del;
				y = strtok(NULL,"\t ");
				a = atoi(x.c_str());
				b = atoi(y.c_str());
				ncp->putNode(a, b);
				ncp->putNode(b, a);
			}
		}
		else{
			std::cout<<"Error Occured!"<<std::endl;
		}
		std::cout<<"####  FILE READ & SETUP FINISHED####"<<std::endl;
		inputFileStream2.close();


	}

	// Write information to output file
	inline void writeFile(char* filePath, nodeCollection* ncp){
		outputFileStream.open(filePath);
		std::vector<blackHoleNode*>* vect = ncp->getNodeVec();
		for(unsigned int i = 0; i < (*vect).size(); i++){
			outputFileStream << (*vect)[i]->getID() << "\t";
			for (int p = 0; p < DIMENSION; p++){
				outputFileStream << (*vect)[i]->getValue(p) << "\t";
			}
			//outputFileStream <<(*vect)[i]->getClusterId()<<std::endl;
			outputFileStream << std::endl;
		}
		std::cout<<"####  FILE WRITE FINISHED####"<<std::endl;
		outputFileStream.close();
	}
};
