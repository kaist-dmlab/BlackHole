#include <cmath>
#include <iostream>

class exponentVar{
private :
	double attrExponent;
	double repuExponent;
	double repuFactor;
	double finalAttrExponent;
	double finalRepuExponent;

public :
	exponentVar(double a, double r, int degree) : attrExponent(a), repuExponent(r) {
		finalAttrExponent = attrExponent;
		finalRepuExponent = repuExponent;
		int sumOfDegree = degree;
		double density = (1.0 / sumOfDegree);
		repuFactor = density * pow(sumOfDegree, 0.5*(attrExponent-repuExponent));
	};

	inline double getAttrExponent() { return attrExponent; }
	inline double getRepuExponent() { return repuExponent; }
	inline double getRepuFactor()	{ return repuFactor;	}
	inline double getFinalAttrExponent() { return finalAttrExponent;}
	inline double getFinalRepuExponent() { return finalRepuExponent;}

	inline void setAttrExponent(double v)   {  attrExponent = v; }
	inline void setRepuExponent(double v)  {  repuExponent = v; }
	inline void setRepuFactor(double v) 	{  repuFactor = v; }
	inline void setFinalAttrExponent(double v) {  finalAttrExponent= v;}
	inline void setFinalRepuExponent(double v) {  finalRepuExponent= v;}
	
};
