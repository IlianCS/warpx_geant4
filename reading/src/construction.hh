// src/construction.hh
#ifndef CONSTRUCTION_HH
#define CONSTRUCTION_HH

#include <G4VUserDetectorConstruction.hh>
#include <G4VPhysicalVolume.hh>
#include <vector>

class MyDetectorConstruction : public G4VUserDetectorConstruction
{
public:
    MyDetectorConstruction();
    ~MyDetectorConstruction() override = default;

    /** Construit le monde + coques sphériques */
    G4VPhysicalVolume* Construct() override;

    /** Branche les coques en tant que détecteurs sensibles */
    void ConstructSDandField() override;

private:
    std::vector<double>           m_radii;           // rayons des coques
    double                        m_shellThickness;  // épaisseur des coques
    std::vector<G4LogicalVolume*> m_logicDetectors;  // volumes logiques des coques
};

#endif // CONSTRUCTION_HH