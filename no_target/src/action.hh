// src/action.hh
#ifndef ACTION_HH
#define ACTION_HH

#include <G4VUserActionInitialization.hh>
#include <string>

class MyActionInitialization : public G4VUserActionInitialization
{
public:
    /**
     * @param dataset    Chemin vers le dossier OpenPMD (ex: "../3D_dataset")
     * @param species    Nom de l’espèce dans OpenPMD (ex: "electrons")
     * @param iteration  Numéro d’itération à lire (ex: 100)
     */
    MyActionInitialization(const std::string& dataset,
                           const std::string& species,
                           int iteration);
    ~MyActionInitialization() override = default;

    /** Enregistre les actionnaires : primary, run, (event) */
    void Build() const override;

private:
    std::string     m_dataset;
    std::string     m_species;
    int             m_iteration;
};

#endif // ACTION_HH
