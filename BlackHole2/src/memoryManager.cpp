
#include "memoryManager.hpp"

#include "Dim.hpp"

memoryManager::memoryManager(int nodeNum){
	current = 0;
	child_current = 0;
	maxNum = nodeNum * 12;
	prior = -1;
	temp = new OctTree[maxNum];
	
	for(int j = 0; j < maxNum; j++){
		childVec.push_back(new OctTree*[(int)pow(2.0, DIMENSION)]);
	}
}

OctTree** memoryManager::get_children(){
	return childVec[child_current++];
}

OctTree* memoryManager::get_Instance(){
	return &(temp[current++]);
}

int memoryManager::getCurrent(){
	return current;
}

int memoryManager::getChildCurrent(){
	return child_current;
}

void memoryManager::setCurrent(int x){
	current = x;
}

void memoryManager::setChildCurrent(int x){
	child_current = x;
}

void memoryManager::dealloc(){

	delete[] temp;

	for(int i = 0; i < maxNum; i++){
		delete childVec[i];
	}
	std::cout << "childVec removed!" << std::endl;

	childVec.clear();
}

void memoryManager::takeAPicture(){
	prior = current;
}

void memoryManager::restore(){
	current = prior;
	child_current = prior;
}

void memoryManager::swap(OctTree* swapper){
	if(prior == -1 && current == prior)
		return;
	prior++;
	OctTree s = *swapper;
	*swapper = temp[prior];
	temp[prior] = s;

	OctTree** ss = swapper->children;
	swapper->children = temp[prior].children;
	temp[prior].children = ss;
}
