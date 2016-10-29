#include "octTree.hpp"


#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

OctTree::OctTree(){
	children = NULL;
	this->childLength = -1;	
	this->childCount = -1;
	this->node = NULL;
	position = new double[DIMENSION];
	minPos = new double[DIMENSION];
	maxPos = new double[DIMENSION];
	for(int i = 0; i < DIMENSION; i++){
		this->position[i] = -1;
		this->minPos[i] = -1;
		this->maxPos[i] = -1;
	}
	this->weight = node == NULL ? 0.0 : node->getDegree();
}

OctTree::~OctTree(){
	delete minPos;
	delete maxPos;
	delete position;
}

OctTree::OctTree(blackHoleNode* node, double* position, double* minPos, double* maxPos){
	children = NULL;
	this->childLength = (int)pow(2.0, DIMENSION);
	this->childCount = 0;
	this->node = node;
	for (int i = 0; i < DIMENSION; i++){
		this->position[i] = position[i];
		this->minPos[i] = minPos[i];
		this->maxPos[i] = maxPos[i];
	}
	this->weight = node == NULL ? 0.0 : node->getDegree();
}

 void OctTree::setElement(blackHoleNode* node, double* position, double* minPos, double* maxPos, memoryManager* mgr){
	

	children = mgr->get_children();
	
	for (int s = 0; s < (int)pow(2.0, DIMENSION); s++){
		children[s] = NULL;
	}
	this->childLength = (int)pow(2.0, DIMENSION);
	this->childCount = 0;
	this->node = node;


	for (int zp = 0; zp < DIMENSION; zp++){
		this->position[zp] = position[zp];
		this->minPos[zp] = minPos[zp];
		this->maxPos[zp] = maxPos[zp];
	}

	this->weight = node == NULL ? 0.0 : node->getDegree();
}

 double OctTree::getPosX(){
	return position[0];
}

 double OctTree::getPosY(){
	return position[1];
}

 blackHoleNode* OctTree::getNode(){
	return node;
}

 int OctTree::getLength(){
	return childLength;
}

 void OctTree::LengthIncrease(){
	childLength = childLength * 2 ;
}

 double OctTree::getWeight(){
	return weight;
}

 void OctTree::addNode(blackHoleNode* newNode, double* newPos, int depth, memoryManager* mgr){
	if (newNode->getDegree() == 0)
		return;

	if (node != NULL){	//if node is empty
		addNode2(node, position, depth, mgr, false);	
		node = NULL;
	}

	for (int z = 0; z < DIMENSION; z++){
		position[z] = (weight * position[z] + newNode->getDegree() * newPos[z]) / (weight + newNode->getDegree());
	}
	weight += newNode->getDegree();

	addNode2(newNode, newPos, depth, mgr, false);
}


 void OctTree::addNode2(blackHoleNode* newNode, double* newPos, int depth, memoryManager* mgr, bool isOriginalNode){
	
	if(depth == MAX_DEPTH){
		if(childLength == childCount) {	//full
			OctTree** oldChildren = new OctTree*[childLength];
			for(int ss = 0; ss < childLength; ss++){
				oldChildren[ss] = children[ss];	
			}
			children = new OctTree*[ 2 * childLength ];
			LengthIncrease();	

			for(int k = 0; k < childLength/2; k++){ //ArrayCopy
				children[k] = oldChildren[k];	
			}
			for(int k = childLength/2; k < childLength; k++){
				children[k] = NULL;
			}
			delete [] oldChildren;
		}
		children[childCount++] = mgr->get_Instance();
		children[childCount-1]->setElement(newNode, newPos, newPos, newPos, mgr);
		if(isOriginalNode == true){	
			mgr->swap(children[childCount-1]);
		}
		return;
	}

	int childIndex = 0;

	for (int d = 0; d < DIMENSION; d++){
		if(newPos[d] > (minPos[d] + maxPos[d])/2){
			childIndex += 1 << d;
		}
	}

	if(children[childIndex] == NULL){
		double newMinPos[DIMENSION];
		double newMaxPos[DIMENSION];
		for (int d = 0; d < DIMENSION; d++){
			if ((childIndex & 1 << d ) == 0 ) {
				newMinPos[d] = minPos[d];
				newMaxPos[d] = (minPos[d] + maxPos[d]) / 2;
			}
			else{
				newMinPos[d] = (minPos[d] + maxPos[d]) / 2;
				newMaxPos[d] = maxPos[d];
			}
		}
		childCount++;
		children[childIndex] = mgr->get_Instance();
		children[childIndex]->setElement(newNode, newPos, newMinPos, newMaxPos, mgr); 
		if(isOriginalNode == true){
			mgr->swap(children[childIndex]);
		}
		
	}
	else{
		children[childIndex]->addNode(newNode, newPos, depth+1, mgr);
	}
}

 /*Use*/
 void OctTree::removeNode(blackHoleNode* oldNode, double* oldPos, int depth, memoryManager* mgr){
	if(oldNode->getDegree() == 0)
		return;
	if(weight <= oldNode->getDegree()){	//remove in leaf
		weight = 0.0;
		node = NULL;
		for(int i = 0; i < this->childLength; i++){
			children[i] = NULL;
		}
		childCount = 0;
		return;
	}

	for (int d = 0; d < DIMENSION; d++){
		position[d] = (weight*position[d] - oldNode->getDegree() * oldPos[d]) / (weight - oldNode->getDegree());
	}
	weight -= oldNode->getDegree();

	if(depth == MAX_DEPTH){
		int childIndex = 0;
		while(children[childIndex]->node->getID() != oldNode->getID()){	
			childIndex++;
		}
		
		childCount--;
		for(int i = childIndex; i < childCount; i++){	//fadding!
			children[i] = children[i+1];
		}
		children[childCount] = NULL;
	}
	else{
		int childIndex = 0;
		for (int d = 0; d < DIMENSION; d++){
			if ( oldPos[d] > (minPos[d] + maxPos[d])/2){
				childIndex += 1 << d;
			}
		}
		children[childIndex]->removeNode(oldNode, oldPos, depth+1, mgr);
		if(children[childIndex]->getWeight() == 0){
			children[childIndex] = NULL;
			childCount--;
		}
	}
}
 /*Use*/
 void OctTree::clearMemory(){
	clearMemory(this);
}

 /*Use*/
 void OctTree::clearMemory(OctTree* p){
	if(p != NULL) {
		for(int i = 0; i < p->childLength; i++){
			delete p->maxPos;
			delete p->minPos;
			delete p->position;
			clearMemory(p->children[i]);
		}
	}
	delete p;
}

 double* OctTree::getValues(){
	 return position;
 }

 /*Use*/
 double OctTree::getWidth() {
	 double width = 0.0;
	 for (int d = 0; d < DIMENSION; d++){
		 if (maxPos[d] - minPos[d] > width){
			 width = maxPos[d] - minPos[d];
		 }
	 }
	 return width;
 }
 /*Use*/
 int OctTree::getHeight(){
	 int height = -1;

	 for (int s = 0; s < childLength; s++){
		 if (children[s] != NULL){// != NULL){
			 height = max(height, children[s]->getHeight());
		 }
	 }
	 return height + 1;
 }
