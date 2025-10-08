#ifndef DETECTOR_HH
#define DETECTOR_HH

#include "G4VSensitiveDetector.hh"
#include "G4Step.hh"
#include "G4TouchableHistory.hh"

/// Enregistre uniquement les composantes (px, py, pz) du vecteur impulsionnel
class MySensitiveDetector : public G4VSensitiveDetector
{
public:
    explicit MySensitiveDetector(const G4String& name);
    ~MySensitiveDetector() override;

    /// Appelé à chaque pas dans un volume sensible
    G4bool ProcessHits(G4Step* aStep,
                       G4TouchableHistory* history) override;
};

#endif // DETECTOR_HH
