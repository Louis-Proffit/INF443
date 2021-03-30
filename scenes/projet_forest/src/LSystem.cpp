#include "LSystem.hpp"
#include "string"

typedef std::map<char, string>::iterator iter;

LSytem::LSytem()
{
}

void LSytem::AddAxiom(char key, string rule) //Une Key se transforme en string; par ex F - > F+F
{
    axioms[key] = rule;
}

void LSytem::ClearAxioms()
{
    axioms.clear();
}

string LSytem::ApplyAxioms(string system)
{
    string applied = "";

    for (int c = 0; c < system.size(); c++)
    {
        bool passThrough = true;
        for (iter iterator = axioms.begin(); iterator != axioms.end(); iterator++)
        {
            if (system[c] == iterator->first) // On identifie le
            {
                passThrough = false;
                applied += iterator->second;
                break;
            }
        }
        if (passThrough)
            applied += system[c]; // c'est bien viable par on passe sur les variables dans l'ordre vu l'ordre des boucles, on passe dans chacun des char à la suite.
    }

    return applied;
}

string LSytem::ApplyAxioms(string system, int iterations)
{
    if (iterations <= 0)
        return system;
    string applied = system;
    for (int i = 0; i < iterations; i++)
    {
        applied = ApplyAxioms(applied);
    }

    return applied;
}