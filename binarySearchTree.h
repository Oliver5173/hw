#pragma once
//http://www2.cs.sfu.ca/CourseCentral/225/gsheikhs/assignments/assignment4/4-binary-tree.html

#include "binaryTree.h"
#include <vector>
#include <iostream>
#include <cmath>
using namespace std;


template <class T>
class binarySearchTree : public binaryTree
{
public:
	binarySearchTree();
	~binarySearchTree ();
	void addNode(T);
	void remove_all(T);
	bool remove_one(T);
	int find_index(T);
	void node_fix(int,vector<T>*);
};

binarySearchTree::binarySearchTree()
{
	nodeVec.assign(1000, NULL);
	count = 0;
}

binarySearchTree::~binarySearchTree()
{
}

template<class T>
void binarySearchTree<T>::addNode(T itm)
{
	int index = 1;
	if (count == 0)
	{
		nodeVec[index] = itm;
		count++;
		return;
	}
	while (true) 
	{
		if (nodeVec[index] != NULL && nodeVec[index] >= itm)
		{
			index = 2 * index;
			continue;
		}
		else if (nodeVec[index] != NULL && nodeVec[index] < itm) 
		{
			index = 2 * index + 1;
			continue;
		}
		nodeVec[index] = itm;
		count++;
		return;
	}
}

template<class T>
void binarySearchTree<T>::remove_all(T itm)
{
	while (remove_one(itm)) continue;
}

template<class T>
bool binarySearchTree<T>::remove_one(T itm)
{
	int index = find_index(itm);
	if (index != 0) 
	{
			nodeVec[index] = NULL;
			count--;
			vector<T> temp;
			node_fix(index, &temp);
			for (int i = 0; i < temp.size(); i++)
			{
				addNode(temp[i]);
				count--;
			}
			return true;
	}
	return false;
}

template<class T>
int binarySearchTree<T>::find_index(T itm)
{
	for (int i = 1; i < nodeVec.size(); i++)
		if (nodeVec[i] == itm)
			return i;
	return 0;
}

template<class T>
void binarySearchTree<T>::node_fix(int index,vector<T> *temp; )
{
	int right = 2 * index + 1;
	int left = 2 * index;
	if (nodeVec[left] == NULL && nodeVec[right] == NULL) return;
	if (nodeVec[left] != NULL) 
	{
		temp->push_back(nodeVec[left]);
		node_fix(left,temp);
	}
	if (nodeVec[right] != NULL)
	{
		temp->push_back(nodeVec[right]);
		nodeVec[right] = NULL;
		node_fix(right,temp);
	}

}
