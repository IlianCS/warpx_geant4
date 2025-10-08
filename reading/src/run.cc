// src/run.cc
#include "run.hh"
#include "G4AnalysisManager.hh"
#include <filesystem>
#include <iostream>

MyRunAction::MyRunAction()
{}

MyRunAction::~MyRunAction()
{}

void MyRunAction::BeginOfRunAction(const G4Run*)
{
    // 1. On voit d’abord où on se trouve
    std::cout << "[DEBUG RunAction] cwd = "
              << std::filesystem::current_path() << "\n";

    // 2. Est-ce que le fichier existait déjà ?
    std::cout << "[DEBUG RunAction] Avant OpenFile, output.root existe ? "
              << std::boolalpha
              << std::filesystem::exists("output.root") << "\n";

    auto* man = G4AnalysisManager::Instance();
    std::cout << "[DEBUG RunAction] Instance d’analyse @ " << man << "\n";

    // 3. On ouvre le fichier
    std::cout << "[DEBUG RunAction] -> OpenFile(\"output.root\")\n";
    man->OpenFile("output.root");

    // 5. Et que le système de fichiers voit bien la création
    std::cout << "[DEBUG RunAction] Après OpenFile, output.root existe ? "
              << std::filesystem::exists("output.root") << "\n";

    // Création du ntuple "momenta"
    man->CreateNtuple("momenta", "Particle Momenta");
    man->CreateNtupleIColumn("eventID");  // colonne 0
    man->CreateNtupleDColumn("px");       // colonne 1
    man->CreateNtupleDColumn("py");       // colonne 2
    man->CreateNtupleDColumn("pz");       // colonne 3
    man->FinishNtuple(0);                 // termine le ntuple d’indice 0
    std::cout << "[RunAction] Ntuple 'momenta' créé\n";
}

void MyRunAction::EndOfRunAction(const G4Run*)
{
    auto* man = G4AnalysisManager::Instance();
    std::cout << "[DEBUG RunAction] Instance d’analyse @ " << man << "\n";

    // 1. Avant écriture/fermeture, le fichier est-il visible ?
    std::cout << "[DEBUG RunAction] Avant Write+Close, output.root existe ? "
              << std::filesystem::exists("output.root") << "\n";

    // 2. On écrit et on ferme
    std::cout << "[DEBUG RunAction] -> Write()\n";
    man->Write();
    std::cout << "[DEBUG RunAction] -> CloseFile()\n";
    man->CloseFile();

    // 3. Vérifs post-fermeture

    std::cout << "[DEBUG RunAction] Après CloseFile, output.root existe ? "
              << std::filesystem::exists("output.root") << "\n";
}
