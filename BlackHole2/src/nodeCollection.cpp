#include "nodeCollection.hpp"



void nodeCollection::putNode(int nNodeId, int other){
	if(degMat[nNodeId-1] == 0){	//not exists!
		degMat[nNodeId-1]++;
		blackHoleNode* newNode = new blackHoleNode(nNodeId, other);
		nodeMap[nNodeId] = newNode;
	}
	else{	
		if(nodeMap[nNodeId]->findEdge(nNodeId, other) == false){	
			degMat[nNodeId-1]++;
			nodeMap[nNodeId]->setEdge(other);
		}
	}
}

void nodeCollection::copyToVector(){

	for(std::map<int, blackHoleNode*>::iterator it = nodeMap.begin(); it != nodeMap.end(); it++){
		nodeVec.push_back(it->second);
	}
	std::sort(nodeVec.begin(), nodeVec.end(), by_id());
}

void nodeCollection::setAdjMat(int maxValue){
	int len = maxValue;
	adjMat = new float*[len];
	for(int i = 0; i < len; i++){
		adjMat[i] = new float[len];
		//memset(adjMat[i], 0, sizeof(float)*len);
		for(int k = 0; k < len; k++){
			adjMat[i][k] = 0;
		}
	}
}

void nodeCollection::setDegMat(int maxValue){
	int len = maxValue;
	degMat = new int[len];
	for(int i = 0; i < len; i++){
		degMat[i] = 0;
	}
}

bool nodeCollection::checkEdge(int id1_notMinus, int id2_notMinus){
	return adjMat[id1_notMinus-1][id2_notMinus-1] != 0;
}

int nodeCollection::getSumOfDegree(){
	std::vector<blackHoleNode*>* vect = &nodeVec;
	int s = 0;
	for(unsigned int i = 0; i < nodeVec.size(); i++){
		s += (*vect)[i]->getDegree();
	}
	return s;
}


double nodeCollection::findInitEnergy(exponentVar& expVar, OctTree* octTree){
	
	std::vector<blackHoleNode*>* vect = &nodeVec;
	double s = 0.0;

	for(unsigned int i = 0; i < nodeVec.size(); i++){
		s += getEnergy((*vect)[i], expVar, octTree);
	}
	return s;
}

void nodeCollection::degreeSet(){
	std::vector<blackHoleNode*>* vect = &nodeVec;
	for(unsigned int i = 0; i < nodeVec.size(); i++){
		(*vect)[i]->setDegree(degMat[i]);
	}
}

void nodeCollection::clearAll(){
	delete [] degMat;

	for(unsigned int i = 0; i < nodeMap.size(); i++){
		delete nodeMap[i];
	}


	nodeMap.clear();
	nodeVec.clear();
}

std::vector<blackHoleNode*>* nodeCollection::getNodeVec(){
	return &nodeVec;
}

std::map<int, blackHoleNode*>& nodeCollection::getNodeMap(){
	return nodeMap;
}
/*Use*/
double nodeCollection::getEnergyR(blackHoleNode* unp, exponentVar& expVar, OctTree* tree){

	if(tree == NULL || unp->getDegree() == 0 )
		return 0.0;
		
	if(tree->node == unp)
		return 0.0;	

	double repuExponent = expVar.getRepuExponent();	
	double repuFactor = expVar.getRepuFactor();

	double* pos = unp->getValues();
	
	double* pos2 = tree->getValues();

	double treeWidth = tree->getWidth();
	double dist = CalculateUtil::calcDist_DIM(pos, pos2);
	if (dist == 0.0) return 0.0;

	if ( tree->childCount > 0 && dist < 1.0 * treeWidth){
		double energy = 0.0;
		for(int i = 0; i < tree->getLength(); i++){
				energy += getEnergyR(unp, expVar, tree->children[i]);
				//std::cout<<energy<<"\t";
		}
		return energy;
	}

	if (dist == 0.0) return 0.0;

	if(repuExponent == 0.0){
		return -repuFactor * unp->getDegree() * tree->weight * log(dist);
	}
	else{
		return -repuFactor * unp->getDegree() * tree->weight * pow(dist, repuExponent) / repuExponent;
	}
}

/*Use*/
double nodeCollection::addRepulsionDir(blackHoleNode* unp, double* dir, exponentVar& expVar, OctTree* tree){

	//if(tree==NULL || tree->node == NULL ||tree->node->getID() == unp->getID() || unp->getDegree() == 0)
	if(tree==NULL ||  tree->node == unp)
		return 0.0;

	if(unp->getDegree()==0){
		return 0.0;
	}


	//std::cout<<"node id = "<<unp->getID()<<"  Tree node id = "<<tree->node->getID()<<std::endl;

	double repuFactor = expVar.getRepuFactor();
	double repuExponent = expVar.getRepuExponent();


	double* pos = unp->getValues();
	double* pos2 = tree->getValues();
	double dist = CalculateUtil::calcDist_DIM(pos, pos2);


	if (dist == 0.0) return 0.0;

	if (tree->childCount > 0 && dist < 1.0 * tree->getWidth()){
		for(int i = 0; i < tree->childLength; i++){
			addRepulsionDir(unp, dir, expVar, tree->children[i]);
		}
		return 0.0;
	}

	double tmp = repuFactor * unp->getDegree() * tree->getWeight() * pow(dist, repuExponent-2);

	for (int z = 0; z < DIMENSION; z++){
		dir[z] -= (pos2[z] - pos[z]) * tmp;
	}
	return tmp * abs(repuExponent-1);
}	

/*Use*/
double nodeCollection::getEnergyAA(blackHoleNode* unp, exponentVar& expVar, OctTree* tree){
	
	double attrExponent = expVar.getAttrExponent();
	std::vector<blackHoleNode*>* vect = &nodeVec;
	
	double* pos = unp->getValues();

	std::vector<int>* edgeList = unp->getEdgeSet();

	double energy = 0.0;

	for(unsigned int i = 0; i < (*edgeList).size(); i++){
		int value = (*edgeList)[i]-1;
		double* pos2 = (*vect)[value]->getValues();
		double dst = CalculateUtil::calcDist_DIM(pos, pos2);
		if (attrExponent == 0.0){
			energy += log(dst);
		}
		else{
			energy += pow(dst, attrExponent) / attrExponent;
		}
	}
	return energy;
}


/*Use*/
double nodeCollection::addAttractionDirA(blackHoleNode* unp, double* dir, exponentVar& expVar, OctTree* tree){
	
	std::vector<blackHoleNode*>* vect = &nodeVec;
	
	double attrExponent = expVar.getAttrExponent();

	if(unp->getDegree() == 0){
		return 0.0;
	}
	double* pos = unp->getValues();

	std::vector<int>* edgeList = unp->getEdgeSet();
	
	for(unsigned int i = 0; i <  (*edgeList).size(); i++){
		int value = (*edgeList)[i]-1;
		double* pos2 = (*vect)[value]->getValues();
		double dist = CalculateUtil::calcDist_DIM(pos, pos2);
		if(dist == 0.0){
			continue;
		}
		double tmp = pow(dist, attrExponent-2);
		
		for (int z = 0; z < DIMENSION; z++){
			dir[z] += (pos2[z] - pos[z]) * tmp;
		}
	}
	return 0.0;
}


/*Use*/
double nodeCollection::getEnergy(blackHoleNode* unp, exponentVar& expVar, OctTree* tree){
	double gr = getEnergyR(unp, expVar, tree);
	double ga = getEnergyAA(unp, expVar, tree);
	
	return gr + ga;
}

/*Use*/
void nodeCollection::setDir(blackHoleNode* unp, double* dir, exponentVar& expVar, OctTree* tree){
	addRepulsionDir(unp, dir, expVar, tree);
	addAttractionDirA(unp, dir, expVar, tree);
}


void nodeCollection::clearClusterId(){
	std::vector<blackHoleNode*>* vect = &nodeVec;
	for (unsigned int i = 0; i < nodeVec.size(); i++){
		(*vect)[i]->setClusterId(-1);
	}
}
