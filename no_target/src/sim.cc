#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "G4PhysListFactory.hh"
#include "G4VModularPhysicsList.hh"
#include "G4SystemOfUnits.hh"

#include "construction.hh"
#include "action.hh"

#include <openPMD/openPMD.hpp>

// Constante pour activer/désactiver l'UI
constexpr bool ENABLE_UI = false;   // <- change à true si tu veux toujours UI

int main(int argc, char** argv)
{
    if (argc < 5) {
        std::fprintf(stderr,
            "Usage: %s <openPMD_path> <species> <iteration> <thickness_mm> [fraction_percent]\n",
            (argv && argv[0]) ? argv[0] : "read_warpx_particles");
        return 1;
    }

    const std::string opmdPath   = argv[1];
    const std::string species    = argv[2];
    const int iteration          = std::stoi(argv[3]);
    const double thickness_mm    = std::atof(argv[4]);
    const double fraction_pct    = (argc >= 6) ? std::atof(argv[5]) : 10.0;

    if (thickness_mm <= 0.0) {
        G4cerr << "Error: thickness_mm must be > 0.\n";
        return 1;
    }
    if (fraction_pct <= 0.0) {
        G4cerr << "Error: fraction_percent must be > 0.\n";
        return 1;
    }

    const G4double thickness = thickness_mm * mm;
    const double fraction    = fraction_pct / 100.0;

    // --- Lecture openPMD pour compter les particules
    openPMD::Series series(opmdPath, openPMD::Access::READ_ONLY);

    if (series.iterations.count(iteration) == 0) {
        G4cerr << "Iteration " << iteration << " not found in series!\n";
        return 1;
    }
    auto it = series.iterations[iteration];

    if (it.particles.count(species) == 0) {
        G4cerr << "Species '" << species << "' not found!\n";
        return 1;
    }

    auto &px = it.particles[species]["momentum"]["x"];
    const uint64_t nb_particles = px.getExtent()[0];

    uint64_t nEvents = static_cast<uint64_t>(std::ceil(fraction * static_cast<double>(nb_particles)));
    if (nEvents == 0) nEvents = 1;
    if (nEvents > nb_particles) nEvents = nb_particles;

    G4cout << "[openPMD] particles=" << nb_particles
           << " | fraction=" << fraction_pct << "% -> nEvents=" << nEvents << G4endl;

    // --- Initialisation Geant4
    auto* runManager = new G4RunManager();

    runManager->SetUserInitialization(new MyDetectorConstruction(thickness));

    G4PhysListFactory factory;
    G4VModularPhysicsList* physicsList = factory.GetReferencePhysList("QGSP_BERT_EMZ");
    runManager->SetUserInitialization(physicsList);

    runManager->SetUserInitialization(new MyActionInitialization(opmdPath, species, iteration));

    runManager->Initialize();

    // --- UI / batch
    G4VisManager* visManager = new G4VisExecutive();
    visManager->Initialize();

    G4UImanager* UImanager = G4UImanager::GetUIpointer();

    // alias N utilisable dans run.mac : /run/beamOn {N}
    {
        std::ostringstream oss;
        oss << nEvents;
        UImanager->ApplyCommand(G4String("/control/alias N ") + oss.str());
    }

    if (ENABLE_UI) {
        G4UIExecutive* ui = new G4UIExecutive(argc, argv);

        // vis.mac si présent
        std::ifstream vismac("vis.mac");
        if (vismac.good()) {
            UImanager->ApplyCommand("/control/execute vis.mac");
        }

        // run.mac si présent
        std::ifstream runmac("run.mac");
        if (runmac.good()) {
            UImanager->ApplyCommand("/control/execute run.mac");
        }

        ui->SessionStart();
        delete ui;
    } else {
        // batch
        G4cout << "[batch] Calling BeamOn(" << nEvents << ").\n";
        runManager->BeamOn(nEvents);
    }

    delete visManager;
    delete runManager;
    return 0;
}