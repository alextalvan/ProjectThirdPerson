#pragma once
#include <fstream>
#include <mge/core/World.hpp>

namespace LevelEditor
{
    void LoadLevel(std::string fileName, World* world, GUI * window);

    bool ParseAbstractObject(ifstream& f, GameObject* parent);

    void ParseGameObject(ifstream& f, GameObject* parent);

    void ParseComponents(ifstream& f, GameObject* owner);

    void ParseChildren(ifstream& f, GameObject* parent);

    void ParseCamera(ifstream& f, GameObject* parent);

    void ParseParticleSystem(ifstream& f, GameObject* parent);

    void ParsePointLight(ifstream& f, GameObject* parent);

    void ProcessModelMatrix(ifstream& f, GameObject* parent, GameObject* child);

}
