// src/generator.hh
#ifndef GENERATOR_HH
#define GENERATOR_HH

#include <G4VUserPrimaryGeneratorAction.hh>
#include <G4ParticleGun.hh>
#include <G4SystemOfUnits.hh>
#include <G4ThreeVector.hh>
#include <random>
#include <string>

// Interface de lecture OpenPMD
#include "read.hh"

class MyPrimaryGenerator : public G4VUserPrimaryGeneratorAction
{
public:
    /**
     * @param dataset   Chemin vers le dossier OpenPMD (ex: "../3D_dataset")
     * @param species   Nom de l’espèce dans OpenPMD (ex: "electrons")
     * @param iteration Numéro d’itération à lire (ex: 100)
     */
    MyPrimaryGenerator(const std::string& dataset,
                       const std::string& species,
                       int iteration);
    ~MyPrimaryGenerator() override;

    void GeneratePrimaries(G4Event* anEvent) override;

private:
    G4ParticleGun*                         fParticleGun{nullptr};
    wxg4::ParticleData                     fPData;      // px,py,pz et ws
    std::mt19937                           fGen;        // moteur RNG
    std::uniform_real_distribution<double> fDist{0.0, 1.0};
};

#endif // GENERATOR_HH