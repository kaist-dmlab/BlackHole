
#ifndef _MEM_H_
#define _MEM_H_

#include "octTree.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdio>


class OctTree;

class memoryManager{
private :
	OctTree* temp;
	std::vector<OctTree**> childVec;	//new OctTree*[8]
	int current;
	int child_current;
	int prior;
	int maxNum;

public :

	memoryManager(int nodeNum);

	OctTree* get_Instance();

	OctTree** get_children();

	void dealloc();

	int getCurrent();

	int getChildCurrent();

	void setCurrent(int x);

	void setChildCurrent(int x);

	void takeAPicture();

	void restore();

	void swap(OctTree* swapper);

};

#endif
