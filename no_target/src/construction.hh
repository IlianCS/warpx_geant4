#ifndef CONSTRUCTION_HH
#define CONSTRUCTION_HH

#include <G4VUserDetectorConstruction.hh>
#include <G4LogicalVolume.hh>

class MyDetectorConstruction : public G4VUserDetectorConstruction
{
public:
    // thickness = épaisseur physique de la cible en mètres
    explicit MyDetectorConstruction(double thickness);
    ~MyDetectorConstruction() override;

    G4VPhysicalVolume* Construct() override;
    void ConstructSDandField() override;

private:
    double m_thickness; // épaisseur physique [m]

    // On garde un pointeur vers le LV des pixels pour lui attacher le SD
    G4LogicalVolume* m_logicDetectorPixel = nullptr;
};

#endif
