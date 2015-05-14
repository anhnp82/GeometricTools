#include "Shape.h"
#include <conio.h>

int Shape::NoNodes = 0;

Shape::Shape(void)
{
	m_id = Shape::NoNodes++;
	Shape::NoNodes++;
	ranking = 1000000.0f;
}

Shape::~Shape(void)
{
}

void Shape::printNoNodes()
{
	_cprintf("number of BVH nodes: %d \n", Shape::NoNodes);
}

int Shape::getId()
{
	return m_id;
}

Shape* Shape::getParent()
{
	return parent;
}

