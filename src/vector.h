class Vector
{
public:
	Vector(float x, float y, float z)
	{
		m_x = x;
		m_y = y;
		m_z = z;
	}

	float GetX() const { return m_x; }
	float GetY() const { return m_y; }
	float GetZ() const { return m_z; }

	Vector operator+(Vector &other) { return Vector(m_x+other.GetX(), m_y+other.GetY(), m_z+other.GetZ()); }
	Vector operator-(Vector &other) { return Vector(m_x-other.GetX(), m_y-other.GetY(), m_z-other.GetZ()); }
	Vector operator*(Vector &other) { return Vector(m_x*other.GetX(), m_y*other.GetY(), m_z*other.GetZ()); }
	Vector operator/(Vector &other) { return Vector(m_x/other.GetX(), m_y/other.GetY(), m_z/other.GetZ()); }

	float DotProduct(Vector &other) { return (m_x*other.GetX()) + (m_y*other.GetY()) + (m_z*other.GetZ()); }
	Vector CrossProduct(Vector &other) { return Vector(m_y*other.GetZ() - m_z*other.GetY(), m_z*other.GetX() - m_x*other.GetZ(), m_x*other.GetY() - m_y*other.GetX()); }

	Vector NormalizeVector() {
		float length = sqrt(m_x*m_x + m_y*m_y + m_z*m_z);

		return Vector(m_x/length, m_y/length, m_z/length);
	}
private:
	float m_x, m_y, m_z;
};