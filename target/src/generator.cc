// src/generator.cc
#include "generator.hh"

#include <G4ParticleTable.hh>
#include <G4Event.hh>
#include <G4SystemOfUnits.hh>
#include <G4ThreeVector.hh>

#include "G4SystemOfUnits.hh"    // pour MeV
#include "G4PhysicalConstants.hh"// pour c_light

// Chargement de l’API OpenPMD via read.hh
#include "read.hh"

MyPrimaryGenerator::MyPrimaryGenerator(const std::string& dataset,
                                       const std::string& species,
                                       int iteration)
: fGen{std::random_device{}()}
{    
    std::cout << "[Generator] Chargement des données OpenPMD : "
              << dataset << ", espèce=" << species
              << ", itération=" << iteration << "\n";
    fPData = wxg4::read_particle_data_3d(dataset, species, iteration);
    std::cout << "[Generator] Données chargées ("
              << fPData.px.size() << " particules)\n";

    // 2) Création du G4ParticleGun
    fParticleGun = new G4ParticleGun(1);
    auto particle = G4ParticleTable::GetParticleTable()->FindParticle("e-");
    fParticleGun->SetParticleDefinition(particle);
    fParticleGun->SetParticlePosition(G4ThreeVector(0,0,0));

// ────────────────────────────────────────────────────────────────
// [NOUVEAU] Pré-filtrage T > 1 MeV (corrigé pour ws, sans CDF)
// ────────────────────────────────────────────────────────────────
constexpr double c_SI   = 299792458.0;            // m/s
constexpr double MeV_J  = 1.602176634e-13;        // 1 MeV en Joules
const    double Tcut_J  = 50.0 * MeV_J;

// masse depuis le gun (en MeV), convertie en kg
const double m_MeV = fParticleGun->GetParticleDefinition()->GetPDGMass() / MeV; // nombre en MeV
const double m_J   = m_MeV * MeV_J;
const double m_kg  = m_J / (c_SI * c_SI);

// si pas de poids dans le fichier, on suppose poids=1
if (fPData.ws.empty()) {
    fPData.ws.assign(fPData.px.size(), 1.0);
}

std::vector<double> px_f, py_f, pz_f, ws_f;
px_f.reserve(fPData.px.size());
py_f.reserve(fPData.py.size());
pz_f.reserve(fPData.pz.size());
ws_f.reserve(fPData.ws.size());

size_t kept = 0;
for (size_t i = 0; i < fPData.px.size(); ++i) {
    const double px = fPData.px[i]; // SI: kg·m/s
    const double py = fPData.py[i];
    const double pz = fPData.pz[i];
    const double p  = std::sqrt(px*px + py*py + pz*pz); // kg·m/s

    const double ratio = p / (m_kg * c_SI);            // p/(m c) (sans unité)
    const double gamma = std::sqrt(1.0 + ratio*ratio); // γ
    const double T_J   = (gamma - 1.0) * m_kg * c_SI * c_SI;

    if (T_J > Tcut_J) {
        px_f.push_back(px);
        py_f.push_back(py);
        pz_f.push_back(pz);
        ws_f.push_back(fPData.ws[i]);
        ++kept;
    }
}

const double Tcut_MeV = Tcut_J / MeV_J;

if (px_f.empty()) {
    G4ExceptionDescription desc;
    desc << "Aucune particule avec T > " << Tcut_MeV
         << " MeV — on conserve l'ensemble original.";
    G4Exception("MyPrimaryGenerator", "HighEnergyFilterEmpty", JustWarning, desc);
} else {
    const size_t oldN = fPData.px.size();
    fPData.px.swap(px_f);
    fPData.py.swap(py_f);
    fPData.pz.swap(pz_f);
    fPData.ws.swap(ws_f);
    std::cout << "[Generator] Filtrage T > " << Tcut_J / MeV_J << " MeV : "
              << kept << " / " << oldN << " particules conservées.\n";
}

}


MyPrimaryGenerator::~MyPrimaryGenerator()
{
    delete fParticleGun;
}

void MyPrimaryGenerator::GeneratePrimaries(G4Event* anEvent)
{
    G4int evtID = anEvent->GetEventID();
    std::cout << "[Generator DEBUG] --- event " << evtID << " ---\n";

    // 1) Tirage pondéré et récupération brute
    double r = fDist(fGen);
    auto pm = wxg4::sample_momentum_3d(fPData, r);
    std::cout << "[Generator DEBUG] raw momentum (from openPMD) = ("
              << pm[0] << ", " << pm[1] << ", " << pm[2]
              << ") [SI: kg·m/s]\n";

    // 2) Construction du vecteur Geant4 et magnitude en SI
    G4ThreeVector vec(pm[0], pm[1], pm[2]);
    G4double p_SI = vec.mag();
    std::cout << "[Generator DEBUG] |p| raw = " << p_SI
              << " kg·m/s\n";

    // 3) Conversion en MeV/c (Geant4 momentum unit)
    G4double MEV_C_CONVERSION = 5.3442859e-22;

    G4double p_MeV = p_SI /MEV_C_CONVERSION * MeV;

    std::cout << "[Generator DEBUG] p_converted = "
              << p_MeV << " MeV/c\n";

    // 4) Direction normalisée
    G4ThreeVector dir = vec.unit();
    std::cout << "[Generator DEBUG] direction = ("
              << dir.x() << ", " << dir.y() << ", " << dir.z()
              << ")\n";

    // 5) Configuration du gun
    fParticleGun->SetParticleMomentumDirection(dir);
    fParticleGun->SetParticleMomentum(p_MeV * MeV);
    std::cout << "[Generator DEBUG] gun configured: p = "
              << p_MeV << " MeV/c, dir = " << dir << "\n";

    // 6) Tir du vertex
    fParticleGun->GeneratePrimaryVertex(anEvent);
}