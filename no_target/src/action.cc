#include "action.hh"
#include "generator.hh"
#include "run.hh"

MyActionInitialization::MyActionInitialization(
    const std::string& dataset,
    const std::string& species,
    int iteration
)
: G4VUserActionInitialization()
, m_dataset(dataset)
, m_species(species)
, m_iteration(iteration)
{}

void MyActionInitialization::Build() const
{
    std::cout << "[ActionInit] Enregistrement du PrimaryGenerator\n";
    // Register primary generator
    SetUserAction(new MyPrimaryGenerator(
        m_dataset,
        m_species,
        m_iteration
    ));
    // Register run action
    std::cout << "[ActionInit] Enregistrement du RunAction\n";
    SetUserAction(new MyRunAction());
}