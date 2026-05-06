#pragma once

#include <string>
#include <vector>

#include "Material.hpp"

class MaterialLibrary {
public:
	static Material aluminum();
	static Material copper();
	static Material steel();
	static Material ceramic();

	static std::vector<Material> all();

	static Material findByName(const std::string& name);

};