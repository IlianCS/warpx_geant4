// src/run.hh
#ifndef RUN_HH
#define RUN_HH

#include <G4UserRunAction.hh>
#include <G4Run.hh>

class MyRunAction : public G4UserRunAction
{
public:
    MyRunAction();
    ~MyRunAction() override;

    void BeginOfRunAction(const G4Run*) override;
    void EndOfRunAction  (const G4Run*) override;
};

#endif // RUN_HH
