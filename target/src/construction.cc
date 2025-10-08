#include "construction.hh"

#include <G4NistManager.hh>
#include <G4Material.hh>
#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4SystemOfUnits.hh>
#include <G4SDManager.hh>
#include <G4VisAttributes.hh>
#include <G4Colour.hh>

#include "detector.hh" // ton MySensitiveDetector (déclare une classe dérivée de G4VSensitiveDetector)

MyDetectorConstruction::MyDetectorConstruction(double thickness)
: m_thickness(thickness)
{}

MyDetectorConstruction::~MyDetectorConstruction() = default;

G4VPhysicalVolume* MyDetectorConstruction::Construct()
{
    auto* nist = G4NistManager::Instance();

    // Matériaux
    G4Material* worldMat   = nist->FindOrBuildMaterial("G4_Galactic"); // vide
    G4Material* targetMat  = nist->FindOrBuildMaterial("G4_Pb");       // plomb
    G4Material* pixelMat   = nist->FindOrBuildMaterial("G4_Si");       // silicium

    // Monde : boîte de demi-longueurs 1 m -> volume total 2m x 2m x 2m
    auto* solidWorld  = new G4Box("solidWorld", 1.0*m, 1.0*m, 1.0*m);
    auto* logicWorld  = new G4LogicalVolume(solidWorld, worldMat, "logicWorld");
    auto* physWorld   = new G4PVPlacement(nullptr, {}, logicWorld, "physWorld", nullptr, false, 0, true);

    // Cible : 1m x 1m en XY, épaisseur physique m_thickness
    const G4double halfX = 0.5*m;
    const G4double halfY = 0.5*m;
    const G4double halfZ = 0.5*m_thickness; // ATTENTION: semi-longueur

    auto* solidTarget = new G4Box("solidTarget", halfX, halfY, halfZ);
    auto* logicTarget = new G4LogicalVolume(solidTarget, targetMat, "logicTarget");

    // Position cible au centre z = 0.60 m
    const G4double Target_Zpos = 0.60*m;
    new G4PVPlacement(
        nullptr,
        G4ThreeVector(0., 0., Target_Zpos),
        logicTarget,
        "physTarget",
        logicWorld,
        false,
        0,
        true
    );

    // Détecteur plan pixellisé, à z = 0.99 m
    const G4double Detector_Zpos = 0.99*m;

    // Un pixel : demi-dimensions 5 mm x 5 mm x 5 mm (cube 1 cm^3)
    const G4double pixHalfXY = 5.0*mm;
    const G4double pixHalfZ  = 5.0*mm;

    auto* solidPixel = new G4Box("solidDetectorPixel", pixHalfXY, pixHalfXY, pixHalfZ);
    m_logicDetectorPixel = new G4LogicalVolume(solidPixel, pixelMat, "logicDetectorPixel");

    // Tapis de pixels couvrant [-1m, +1m] en X et Y
    const G4int nX = 200;
    const G4int nY = 200;
    const G4double widthX = 2.0*m;
    const G4double widthY = 2.0*m;
    const G4double stepX  = widthX / nX;
    const G4double stepY  = widthY / nY;

    // Placement des pixels
    for (G4int i = 0; i < nX; ++i)
    {
        for (G4int j = 0; j < nY; ++j)
        {
            const G4double xPos = -1.0*m + (i + 0.5)*stepX;
            const G4double yPos = -1.0*m + (j + 0.5)*stepY;

            new G4PVPlacement(
                nullptr,
                G4ThreeVector(xPos, yPos, Detector_Zpos),
                m_logicDetectorPixel,
                "physDetectorPixel",
                logicWorld,
                false,
                j + i*nY,   // copyNo unique
                true        // checkOverlaps
            );
        }
    }

    // (Optionnel) un peu de couleur pour le visu
    logicWorld->SetVisAttributes(G4VisAttributes::GetInvisible());

    auto* visTarget = new G4VisAttributes(G4Colour(0.3,0.3,0.8,0.6)); // bleu translucide
    visTarget->SetForceSolid(true);
    logicTarget->SetVisAttributes(visTarget);

    auto* visPixel = new G4VisAttributes(G4Colour(0.8,0.2,0.2,0.6)); // rouge translucide
    visPixel->SetForceSolid(true);
    m_logicDetectorPixel->SetVisAttributes(visPixel);

    return physWorld;
}

void MyDetectorConstruction::ConstructSDandField()
{
    // Crée et enregistre le détecteur sensible
    auto* sd = new MySensitiveDetector("PixelSD");
    auto* sdm = G4SDManager::GetSDMpointer();
    sdm->AddNewDetector(sd);

    // Attache le SD à la logique des pixels
    m_logicDetectorPixel->SetSensitiveDetector(sd);
}