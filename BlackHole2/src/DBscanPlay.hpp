#include "Util.hpp"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <vector>
#include <set>
#include <sstream>
#include <cstring>
#include "Dim.hpp"

const double PI = 3.141592653589793238463;

// To express points of distances
class point{
public:
	float x;
	float y;
};

// calculate N-Dimensional matrix's distances
double calcDist(float* a[], int i, int j ){
		double sum = 0.0;
		for (int z = 0; z < DIMENSION; z++){
			sum = sum + ((a[z][i] - a[z][j]) * (a[z][i] - a[z][j]));
		}
		return sqrt(sum);
}



class DBscanPlay{
public :
	static void dbscanCalculator(char * inputFile, int minPts, float removePercentage){

		clock_t start_time, end_time;	  // clock_t
		start_time = clock();				  // Start_Time

		int nodeNum = 0;
		char oneLine[256];
		std::string line;
		std::string x, y, z, z_1, del, cluster;

		std::ifstream nodeToCommunity1; 	std::ifstream nodeToCommunity2;
		std::ofstream ofs;		std::ofstream distanceOutputStream;
		std::string inputFileName(inputFile);

		std::stringstream out;
		out << minPts;

		std::ostringstream ss;
		ss << removePercentage * 100;
		std::string removeP(ss.str());

		nodeToCommunity1.open(inputFile);

		if (nodeToCommunity1.is_open()){//calc maximal cluster size
			while (getline(nodeToCommunity1, line)){
				strcpy(oneLine, line.c_str());
				del = strtok(oneLine, "\t ");
				x = strtok(NULL, "\t ");
				y = strtok(NULL, "\t ");
				nodeNum++;
			}
		}
		else{
			std::cout << "can't read file" << std::endl;
		}

		nodeToCommunity1.close();
		nodeToCommunity2.open(inputFile);

		int* communitySelf = new int[nodeNum];

		std::cout<<"Dimension : "<<DIMENSION<<std::endl;
		float* points[DIMENSION];
		for(int i = 0; i < DIMENSION; i++){
			points[i] = new float[nodeNum];
		}

		bool* visited = new bool[nodeNum];
		int* countN = new int[nodeNum];
		int* communityInfo = new int[nodeNum];
		bool* isSeed = new bool[nodeNum];
		for (int ttt = 0; ttt < nodeNum; ttt++){
			for(int j = 0; j < DIMENSION; j++){
				points[j][ttt] = 0.0f;
			}
			visited[ttt] = false;
			countN[ttt] = 0;
			communityInfo[ttt] = -1;
			isSeed[ttt] = false;
		}

		int counter = 0;
		std::string tempVar;
		if (nodeToCommunity2.is_open()){
			while (getline(nodeToCommunity2, line)){
				strcpy(oneLine, line.c_str());
				del = strtok(oneLine, "\t ");
				for(int j = 0; j < DIMENSION; j++){
					tempVar = strtok(NULL, "\t ");
					points[j][counter] = atof(tempVar.c_str());
				}
				counter++;
			}
		}

		else{
			std::cout << "can't read file" << std::endl;
		}

		float* dist_vec = new float[nodeNum];
		float* dist_sorted = new float[nodeNum];
		float eps;

		for (int i = 0; i < nodeNum; i++){
			for (int j = 0; j < nodeNum; j++){
				dist_sorted[j] = calcDist(points, i, j);		//precalc
			}
			std::sort(dist_sorted, dist_sorted + nodeNum);
			dist_vec[i] = dist_sorted[minPts - 1];
		}

		//##############################################################
		//To Select EPS
		//##############################################################

		std::sort(dist_vec, dist_vec + nodeNum, std::greater< float>());

		//NORMALIZATION ********************
		//Removing outlier to maximal value

		double trunc = (nodeNum * removePercentage);
		int tr = (int)trunc;
		std::cout << "Truncated # = " << tr << std::endl;

		for (int i = 0; i < tr; i++){
			dist_vec[i] = 0;
		}

		std::sort(dist_vec, dist_vec + nodeNum, std::greater< float>());

		//**********************************

//		std::string outputdistance = inputFileName + "_MinPts_" + out.str() + "_RemovePercent_" + removeP + "_distance.dat";
//		distanceOutputStream.open(outputdistance.c_str());
//		for (int ppo = 0; ppo < nodeNum; ppo++){
//			distanceOutputStream << ppo + 1 << "\t" << dist_vec[ppo] << std::endl;
//		}
//		distanceOutputStream.close();

		//Save to point array
		point* original = new point[nodeNum];
		for (int i = 0; i < nodeNum; i++){
			original[i].x = i;
			original[i].y = dist_vec[i];
		}

		//find minVal, maxVal of Y
		float maxVal = -1;
		float minVal = 999999;
		for (int i = 0; i < nodeNum; i++){
			if (original[i].y >= maxVal){
				maxVal = original[i].y;
			}
			if (original[i].y <= minVal){
				minVal = original[i].y;
			}
		}
		//min-max normalization
		for (int i = 0; i < nodeNum; i++){
			original[i].x = ((original[i].x - 0) / nodeNum) * 1;
			original[i].y = ((original[i].y - minVal) / (maxVal - minVal));
		}

		//rotation
		for (int i = 0; i < nodeNum; i++){
			original[i].x = cos(-PI / 4.0f) * original[i].x + sin(-PI / 4.0f)*(original[i].y - 1.0f);
			original[i].y = -sin(-PI / 4.0f) * original[i].x + cos(-PI / 4.0f)*(original[i].y - 1.0f);
		}

		minVal = 999999;
		int minValueIdx = -1;
		for (int i = 0; i < nodeNum; i++){
			if (original[i].y <= minVal){
				minVal = original[i].y;
				minValueIdx = i;
			}
		}

		std::cout << "Approximated Value for DBSCAN = " << dist_vec[minValueIdx] << std::endl;
		eps = dist_vec[minValueIdx];

		delete  dist_sorted;
		delete  dist_vec;

		//##############################################################
		//Algorithm Start
		//##############################################################

		for (int i = 0; i < nodeNum; i++){
			for (int j = 0; j < nodeNum; j++){
				if (calcDist(points, i, j) <= eps){
					countN[i]++;
				}
			}
		}


		int currentCmty = 0;
		int icmty;
		std::set< int> setN;

		for (int i = 0; i < nodeNum; i++){
			visited[i] = true;   //Mark P as visited

			setN.clear();

			if (countN[i] >= minPts){   //NeighborPts = regionQuery(P, eps)
				isSeed[i] = true;

				if (communityInfo[i] == -1){
					communityInfo[i] = ++currentCmty;
				}
				icmty = communityInfo[i];

				for (int j = 0; j < nodeNum; j++){  //insert one hop
					if (i == j)
						continue;

					if (calcDist(points, i, j) <= eps){
						setN.insert(j);
						if (countN[j] >= minPts){
							isSeed[j] = true;
						}
					}
				}


				for (std::set<int >::iterator IterPos = setN.begin(); IterPos != setN.end();){
					IterPos = setN.begin();

					int cur = *IterPos;
					setN.erase(IterPos++);
					if (visited[cur] == false){
						visited[cur] = true;
						for (int k = 0; k < nodeNum; k++){
							if (cur == k)
								continue;

							if (calcDist(points, cur, k) <= eps){
								setN.insert(k);
								if (countN[k] >= minPts){
									isSeed[k] = true;
								}
							}
						}
					}

					if (communityInfo[cur] == -1 || communityInfo[cur] == 0)
						communityInfo[cur] = icmty;
				}

				for (int j = 0; j < nodeNum; j++){
					if (i == j)
						continue;

					if (calcDist(points, i, j) <= eps){
						if (visited[j] == false){   //unvisited
							visited[j] = true;
							communityInfo[j] = communityInfo[i];
						}
					}
				}
			}

			else {  //mark P as noise
				if (communityInfo[i] == -1)
					communityInfo[i] = 0;
			}
		}
		end_time = clock();				   // End_Time


		std::ostringstream oout;
		oout << eps;
		std::string varAs = oout.str();
		std::string outputname = inputFileName + "_MinPts_" + out.str() + "_RemovePercent_" + removeP + "_EPS_" + varAs + ".dat";
		ofs.open(outputname.c_str());
		for (int z = 0; z< nodeNum; z++){
			ofs << z + 1 << "\t" << communityInfo[z] << "\t" << isSeed[z] << std::endl;
		}

		printf("Time : %f\n", ((double)(end_time - start_time)) / CLOCKS_PER_SEC);
		printf("######################################\nDBSCAN IS FINISHED!");

		delete[] original;
		ofs.close();
		nodeToCommunity2.close();

		for(int z = 0; z < DIMENSION; z++){
			delete[] points[z];
		}

		//delete points;

		delete[] communityInfo;
		delete[] visited;
		delete[] isSeed;
		delete[]communitySelf;
	}
};
