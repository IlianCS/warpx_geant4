#include "G4RunManager.hh"
#include "FTFP_BERT.hh"            // physique standard simplifiée

#include "construction.hh"         // monde + coques sphériques
#include "action.hh"               // PrimaryGenerator + RunAction

#include <filesystem>

#include <openPMD/openPMD.hpp>
#include <cmath>    // pour std::ceil

int main(int argc, char** argv)
{
    if (argc < 4) {
        G4cerr << "openPMD_path, species and iteration must be specified" << G4endl;
        return 1;
    }

    std::string openPMD_path = argv[1];
    std::string species = argv[2];
    int iteration = std::stoi(argv[3]);

    // ────────────────────────────────────────
    // 1) Lecture du fichier openPMD pour compter les particules
    openPMD::Series series(openPMD_path, openPMD::Access::READ_ONLY);
    if (series.iterations.count(iteration) == 0) {
        G4cerr << "Iteration " << iteration << " not found in file!" << G4endl;
        return 1;
    }
    auto it = series.iterations[iteration];

    if (it.particles.count(species) == 0) {
        G4cerr << "Species '" << species << "' not found!" << G4endl;
        return 1;
    }

    auto &px = it.particles[species]["momentum"]["x"];
    uint64_t nb_particles = px.getExtent()[0];  // taille du dataset = nombre de particules

    uint64_t nEvents = static_cast<uint64_t>(std::ceil(nb_particles));
    G4cout << "Launching BeamOn with " << nEvents << " events (100% of " 
           << nb_particles << " particles)" << G4endl;

    // ────────────────────────────────────────
    // 2) Initialisation Geant4
    auto* runManager = new G4RunManager;

    runManager->SetUserInitialization(new MyDetectorConstruction());
    runManager->SetUserInitialization(new FTFP_BERT);
    runManager->SetUserInitialization(new MyActionInitialization(openPMD_path, species, iteration));

    runManager->Initialize();

    // ────────────────────────────────────────
    // 3) BeamOn avec 10% des particules
    runManager->BeamOn(nEvents);

    delete runManager;
    return 0;
}
