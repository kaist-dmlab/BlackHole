#include "Util.hpp"
#include "blackHoleNode.hpp"
#include "Dim.hpp"



blackHoleNode::~blackHoleNode(){
	delete points;
}

bool node::setPosition(int nI, double* ar){
	nodeId = nI;
	points = new double[DIMENSION];
	for (int temp = 0; temp < DIMENSION; temp++){
		points[temp] = ar[temp];
	}
	return true;
}

int node::getID() const{
	return nodeId;
}

double node::getValue(int idx){
	return points[idx];
}

void node::setValue(double value, int idx){
	points[idx] = value;
}


double* blackHoleNode::getValues(void){
	return points;
}

void blackHoleNode::setDegree(int x){
	degree = x;
}

void blackHoleNode::setClusterId(int x){
	clusterId = x;
}

int blackHoleNode::getClusterId(){
	return clusterId;
}

bool blackHoleNode::isnot_labeled(){
	if(clusterId == -1){
		return false;
	}
	return true;
}

std::vector<int>* blackHoleNode::getEdgeSet(){
	return &eSet;
}

bool blackHoleNode::setEdge(int e){
	eSet.push_back(e);
	return true;
}

bool blackHoleNode::findEdge(int origin, int id){
	return std::binary_search(eSet.begin(), eSet.end(), id);
}

blackHoleNode::blackHoleNode(int nNodeId, int ep){
	nodeId = nNodeId;
	points = new double[DIMENSION];
	for (int i = 0; i < DIMENSION; i++){
		points[i] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX)-0.5f;
	}
	clusterId = -1;
	degree = 1;
	eSet.push_back(ep);
}


int blackHoleNode::getDegree(){
	return degree;
}
