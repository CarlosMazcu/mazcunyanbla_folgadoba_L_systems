#ifndef __LSYSTEM_HPP__
#define __LSYSTEM_HPP__ 1

#include <string>
#include <unordered_map>
#include <vector>
#include <random>

class LSystem {
public:
    LSystem();

    void setAxiom(const std::string& axiom);
    void addRule(char predecessor, const std::string& successor);
    void generate(int iterations);
    void reset();
    std::string getCurrent() const;
    void iterate();
    void generateGuidedRules(int action);
    void generateRandomRules(); 

    void nextGuidedStep(int choice); // Nueva función para modo guiado

    int getGuidedStep() const { return guidedStep; }
    void resetGuided();
private:
    std::string originalAxiom;
    std::string current;
    std::unordered_map<char, std::string> rules;
    int guidedStep = 0;
};

#endif
