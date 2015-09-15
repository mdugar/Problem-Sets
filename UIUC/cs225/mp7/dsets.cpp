#include "dsets.h"
#include <vector>


void DisjointSets::addelements(int num)
{
	for(int i = 0; i < num; i++)
		elems.push_back(-1);	
}

int DisjointSets::find(int elem)
{
	if(elems[elem] >= 0)
		return (elems[elem] = find(elems[elem]));
	return elem;
}


void DisjointSets::setunion(int a, int b)
{
	int rootA = find(a);
	int rootB = find(b);
	int newSize = elems[rootA] + elems[rootB];
	if(size(a) >= size(b))
	{
		elems[rootB] = rootA;
		elems[rootA] = newSize;
	}
	else
	{
		elems[rootA] = rootB;
		elems[rootB] = newSize;
	}
}

int DisjointSets::size(int a) //Size of the set "a" belongs to
{
	return -1*elems[find(a)]; 
}
