#include "mwMachSimVerifier.hpp"

class BoundingSphere
{
public:

	typedef cadcam::mwTPoint3d<float> Vector3;

	BoundingSphere(const Vector3 & center, const float & radius);

	BoundingSphere(const Vector3 & pole1, const Vector3 & pole2);

	void init(const Vector3 & pole1, const Vector3 & pole2);

	BoundingSphere();
	~BoundingSphere();

	void operator += (const BoundingSphere & other);

	const Vector3 & getCenter() const
	{
		return m_Center;
	}

	const float & GetRadius() const
	{
		return m_Radius;
	}

	const int & GetElongatedDim() const
	{
		return m_ElongatedDim;
	}

	const float & GetSquaredRadius() const
	{
		return m_SquareRadius;
	}

	bool Contains(const BoundingSphere & other, Vector3 & toOther, Vector3 & otherFarEnd);

	bool ContainsOther(const BoundingSphere & self, const BoundingSphere & other, Vector3 & toOther, Vector3 & otherFarEnd);

	bool Contains(const Vector3 & point) const;

protected:
private:

	Vector3	m_Center;
	float m_SquareRadius;
	float m_Radius;
	int m_ElongatedDim;

};




