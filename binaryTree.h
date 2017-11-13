#pragma once

#include "binaryTreeInterface.h"
#include <vector>
#include <iostream>
#include <cmath>

template <class T>
class binaryTree : public binaryTreeInterface
{
public:
	binaryTree();
	~binaryTree();
	void addNode(T);
	void remove_all(T);
	bool remove_one(T);
	int find_index(T);
	bool isEmpty();
	int size();
	void preOrderPrint();
	void postOrderPrint();
	void inOrderPrint();
	void preOrder(int);
	void inOrder(int);
	void postOrder(int);
	int height();

protected:
	std::vector<T> nodeVec;
	int count;
};

template <class T>
binaryTree::binaryTree()
{
	this.nodeVec.push_back(NULL);
	this.count = 0;
}

template <class T>
binaryTree::~binaryTree()
{
}

template<class T>
void binaryTree<T>::remove_all(T itm)
{
	/*
	int index = find_index(itm);
	while (index != 0) 
	{
		this.nodeVec.erase(index);
		this.count--;
		index = find_index(itm);
	}
	*/
	while (remove_one(itm)) continue;
}

template<class T>
bool binaryTree<T>::remove_one(T itm)
{
	int index = find_index(itm);
	if (index != 0) 
	{ 
		this.nodeVec.erase(index);
		this.count--;
		return true;
	}
	return false;
}

template<class T>
int binaryTree<T>::find_index(T itm)
{
	for (int i = 1; i <= this.count; i++)
		if (this->nodeVec[i] == itm)
			return i;
	return 0;
}

template<class T>
bool binaryTree<T>::isEmpty()
{
	if (this.count == 0) return true;
	return false;
}

template<class T>
int binaryTree<T>::size()
{
	return this.count;
}

template<class T>
void binaryTree<T>::preOrderPrint()
{
	preOrder(1);
}

template<class T>
void binaryTree<T>::postOrderPrint()
{
	postOrder(1);
}

template<class T>
void binaryTree<T>::inOrderPrint()
{
	inOrder(1);
}

template<class T>
void binaryTree<T>::preOrder(int index)
{
	if(index <= this.count && this.nodeVec[index] != NULL)
	{	
		std::cout << this.nodeVec[index] << " ";
		preOrder(index * 2);
		preOrder(index * 2 + 1);
	}
	 
}

template<class T>
void binaryTree<T>::inOrder(int index)
{
	if (index <= this.count && this.nodeVec[index] != NULL)
	{
		inOrder(index * 2);
		std::cout << this.nodeVec[index] << " ";
		inOrder(index * 2 + 1);
	}
}

template<class T>
void binaryTree<T>::postOrder(int index)
{
	if (index <= this.count && this.nodeVec[index] != NULL)
	{
		postOrder(index * 2);
		postOrder(index * 2 + 1);
		std::cout << this.nodeVec[index] << " ";
	}
}

template<class T>
int binaryTree<T>::height()
{
	return floor(log2(this.count)) + 1;
}



template <class T>
void addNode(T itm)
{
	this.nodeVec.push_back(itm);
	this.count++;
}