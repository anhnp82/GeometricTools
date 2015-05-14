#include "BoundingSphere.h"

BoundingSphere::BoundingSphere(const BoundingSphere::Vector3 &pole1, const BoundingSphere::Vector3 &pole2)
{
	init(pole1, pole2);
}



void BoundingSphere::operator +=(const BoundingSphere &other)
{
	Vector3 toOther, otherFarEnd, toSelf, selfNearEnd;
	bool containOther, containSelf;
	
	containOther = ContainsOther(*this, other, toOther, otherFarEnd);
	if (containOther) return;

	containSelf = ContainsOther(other, *this, toSelf, selfNearEnd);
	if (containSelf)
	{
		m_Center = other.getCenter();
		m_Radius = other.GetRadius();
		m_SquareRadius = other.GetSquaredRadius();
		m_ElongatedDim = other.GetElongatedDim();
		return;
	}

	init(selfNearEnd, otherFarEnd);

	//if (!Contains(other, toOther, otherFarEnd))
	//{
	//	//Vector3 selfNearEnd = m_Center - m_Radius*toOther;
	//	//init(selfNearEnd, otherFarEnd);
	//} 
}



void BoundingSphere::init(const BoundingSphere::Vector3 &pole1, const BoundingSphere::Vector3 &pole2)
{
	m_Center = (pole1 + pole2) / 2.0f;
	Vector3 poleVec = (pole1 - pole2);
	m_SquareRadius = poleVec.Length2();
	m_Radius = sqrt(m_SquareRadius);

	float maxExtent = fabs(poleVec[0]);
	m_ElongatedDim = 0;
	if ( maxExtent < fabs(poleVec[1]) )
	{
		maxExtent = fabs(poleVec[1]);
		m_ElongatedDim = 1;
	}

	if ( maxExtent < fabs(poleVec[2]) )
	{
		maxExtent = fabs(poleVec[2]);
		m_ElongatedDim = 2;
	}
}



bool BoundingSphere::Contains(const BoundingSphere &other, Vector3 & toOther, Vector3 & otherFarEnd)
{
	toOther = other.getCenter() - m_Center;
	toOther.Normalize();
	otherFarEnd = other.getCenter() + toOther*other.GetRadius();
	
	Vector3 toOtherFarEnd = otherFarEnd - m_Center;
	return toOtherFarEnd.Length2() < m_SquareRadius;
}



bool BoundingSphere::ContainsOther(const BoundingSphere & self, const BoundingSphere & other, Vector3 & toOther, Vector3 & otherFarEnd)
{

	if (self.getCenter() == other.getCenter())
	{
		return other.GetRadius() < self.GetRadius();
	}

	toOther = other.getCenter() - self.getCenter();
	toOther.Normalize();
	otherFarEnd = other.getCenter() + toOther*other.GetRadius();

	Vector3 toOtherFarEnd = otherFarEnd - self.getCenter();
	return toOtherFarEnd.Length2() < self.GetSquaredRadius();
}



bool BoundingSphere::Contains(const Vector3 & point) const
{
	float squareDist = (m_Center - point).Length2();
	return squareDist <= m_SquareRadius;
}



BoundingSphere::~BoundingSphere(){}
BoundingSphere::BoundingSphere(){}