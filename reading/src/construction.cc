// src/construction.cc
#include "construction.hh"

#include <G4NistManager.hh>
#include <G4Box.hh>
#include <G4Sphere.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4ThreeVector.hh>
#include <G4SystemOfUnits.hh>
#include <G4SDManager.hh>

#include "detector.hh"   // votre classe de détecteur sensible

MyDetectorConstruction::MyDetectorConstruction()
 : m_radii{10*cm, 20*cm, 30*cm}   // par exemple 10 cm, 20 cm, 30 cm
 , m_shellThickness(1*mm)         // coque de 1 mm d’épaisseur
{}

G4VPhysicalVolume* MyDetectorConstruction::Construct()
{
    std::cout << "[Detector] Construct world vide + coques sphériques\n";
    // 1) Monde vide
    G4NistManager* nist = G4NistManager::Instance();
    G4Material* worldMat = nist->FindOrBuildMaterial("G4_Galactic");
    G4double halfSize = 1.0*m;
    auto solidWorld = new G4Box("WorldSolid", halfSize, halfSize, halfSize);
    auto logicWorld = new G4LogicalVolume(solidWorld, worldMat, "WorldLogic");
    auto physWorld = new G4PVPlacement(
        nullptr,                    // pas de rotation
        G4ThreeVector(),            // origine
        logicWorld,                 // volume logique
        "WorldPhys",                // nom physique
        nullptr,                    // pas de volume parent
        false, 0, true              // pas de bool, copyNo=0, checkOverlaps
    );

    // 2) Coques sphériques en vide (sensitive)
    //    on utilise le même matériau "G4_Galactic" pour que rien n'absorbe
    for (size_t i = 0; i < m_radii.size(); ++i) {
        double r    = m_radii[i];
        double rIn  = r - m_shellThickness/2;
        double rOut = r + m_shellThickness/2;

        // G4Sphere(name, innerR, outerR, phiStart, phiTotal, thetaStart, thetaTotal)
        auto solidShell = new G4Sphere(
            "Shell" + std::to_string(i),
            rIn, rOut,
            0.*deg, 360.*deg,
            0.*deg, 180.*deg
        );

        auto logicShell = new G4LogicalVolume(
            solidShell,
            worldMat,
            "ShellLogic" + std::to_string(i)
        );

        m_logicDetectors.push_back(logicShell);

        new G4PVPlacement(
            nullptr,
            G4ThreeVector(),                // centré en (0,0,0)
            logicShell,
            "ShellPhys" + std::to_string(i),
            logicWorld,
            false,
            static_cast<G4int>(i),
            true                            // checkOverlaps
        );
    }

    return physWorld;
}

void MyDetectorConstruction::ConstructSDandField()
{
    std::cout << "[Detector] Assignation des détecteurs sensibles\n";
    // 3) Détecteur sensible unique pour toutes les coques
    auto sdManager = G4SDManager::GetSDMpointer();
    auto sensDet = new MySensitiveDetector("SphereSD");
    sdManager->AddNewDetector(sensDet);

    // Associe le détecteur sensible à chacune des coques
    for (auto logicVol : m_logicDetectors) {
        logicVol->SetSensitiveDetector(sensDet);
    }
}