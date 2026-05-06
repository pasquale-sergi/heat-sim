#include "Material.hpp"

#include <stdexcept>
#include <utility>

Material::Material(
	std::string name,
	double thermalConductivity,
	double density,
	double specificHeat
)
	: name_(std::move(name)),
	thermalConductivity_(thermalConductivity),
	density_(density),
	specificHeat_(specificHeat)
{
	if (thermalConductivity <= 0.0) {
		throw std::invalid_argument("Thermal conductivity must be positive.");
	}
	if (density <= 0.0) {
		throw std::invalid_argument("Density must be positive.");
	}
	if (specificHeat <= 0.0) {
		throw std::invalid_argument("Specific heat must be positive.");
	}
}

const std::string& Material::name() const {
	return name_;
}

double Material::thermalConductivity() const {
	return thermalConductivity_;
}

double Material::density() const {
	return density_;
}

double Material::specificHeat() const {
	return specificHeat_;
}


double Material::thermalDiffusivity() const {
	return thermalConductivity_ / (density_ * specificHeat_);
}