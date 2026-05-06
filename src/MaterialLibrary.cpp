#include "MaterialLibrary.hpp"

#include <algorithm>
#include <cctype>
#include <stdexcept>

namespace {
    std::string toLower(std::string text) {
        std::transform(
            text.begin(),
            text.end(),
            text.begin(),
            [](unsigned char c) {
                return static_cast<char>(std::tolower(c));
            }
        );

        return text;
    }
}

//Legend of parameters for each material
// - thermal conductivity (W/(m*K))
// - density (kg/m^3)
// - specific heat (J/(kg*K))

Material MaterialLibrary::aluminum() {
    return Material(
        "Aluminum",
        237.0,  
        2700.0, 
        900.0    
    );
}

Material MaterialLibrary::copper() {
    return Material(
        "Copper",
        401.0,  
        8960.0, 
        385.0 
    );
}

Material MaterialLibrary::steel() {
    return Material(
        "Steel",
        50.0,   
        7850.0,  
        470.0   
    );
}

Material MaterialLibrary::ceramic() {
    return Material(
        "Ceramic",
        2.0,    
        3000.0,  
        800.0    
    );
}

std::vector<Material> MaterialLibrary::all() {
    return {
        aluminum(),
        copper(),
        steel(),
        ceramic()
    };
}

Material MaterialLibrary::findByName(const std::string& name) {
    const std::string requestedName = toLower(name);

    for (const auto& material : all()) {
        if (toLower(material.name()) == requestedName) {
            return material;
        }
    }

    throw std::invalid_argument("Unknown material: " + name);
}