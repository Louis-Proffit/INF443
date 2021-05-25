#pragma once
#include <iostream>
#include <map>

class LSytem
{
public:
    LSytem();                           // Class constructor
    std::map<char, std::string> axioms; //Liste des axioms de la class (pour faire des arbres differents )

    void AddAxiom(char key, std::string rule); // Ajouter un Axiome

    void ClearAxioms();

    std::string ApplyAxioms(std::string system); // Transformer une fois le system avec des axioms, genre F -> F+F

    std::string ApplyAxioms(std::string system, int iterations); // Le faire beaucoup de fois
};