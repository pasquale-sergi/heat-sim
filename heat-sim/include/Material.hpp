#pragma once

#include <string>

class Material {
public:
	Material(
		std::string name,
		double thermalConductivity,
		double density,
		double specificHeat
	);

	const std::string& name() const;
	double thermalConductivity() const;
	double density() const;
	double specificHeat() const;

	double thermalDiffusivity() const;
private:
	std::string name_;
	double thermalConductivity_;
	double density_;
	double specificHeat_;
};