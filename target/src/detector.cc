#include "detector.hh"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4RunManager.hh"
#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"

MySensitiveDetector::MySensitiveDetector(const G4String& name)
: G4VSensitiveDetector(name)
{}

MySensitiveDetector::~MySensitiveDetector()
{}

G4bool MySensitiveDetector::ProcessHits(G4Step* aStep,
                                        G4TouchableHistory*)
{
    // 1) Récupère la particule et son vecteur impulsionnel (en MeV/c)
    G4Track* track    = aStep->GetTrack();
    auto     momentum = track->GetMomentum();
    std::cout << "[SensitiveDetector] ProcessHits: momentum = ("
              << momentum.x() << ", "
              << momentum.y() << ", "
              << momentum.z() << ")\n";

    // 2) (Optionnel) ID de l'événement pour traçabilité
    G4int eventID = G4RunManager::GetRunManager()
                       ->GetCurrentEvent()->GetEventID();
    std::cout << "[DEBUG SD] ProcessHits evt=" << eventID << "\n";
    // 3) Enregistrement uniquement de px, py, pz
    auto* man = G4AnalysisManager::Instance();
    man->FillNtupleIColumn(0, eventID);        // colonne 0 : eventID
    man->FillNtupleDColumn(1, momentum.x());   // colonne 1 : px
    man->FillNtupleDColumn(2, momentum.y());   // colonne 2 : py
    man->FillNtupleDColumn(3, momentum.z());   // colonne 3 : pz
    man->AddNtupleRow(0);

    // 4) Arrête la particule une fois détectée
    track->SetTrackStatus(fStopAndKill);

    return true;
}