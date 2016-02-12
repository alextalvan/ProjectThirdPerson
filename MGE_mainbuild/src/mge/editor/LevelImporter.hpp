#pragma once
#include <fstream>
#include <mge/core/World.hpp>

namespace LevelEditor
{
    void LoadLevel(std::string fileName, World* world);

    bool ParseAbstractObject(ifstream& f, GameObject* parent);

    void ParseGameObject(ifstream& f, GameObject* parent);

    void ParseComponents(ifstream& f, GameObject* owner);

    void ParseChildren(ifstream& f, GameObject* parent);

    void ParseCamera(ifstream& f, GameObject* parent);

    void ParseDirLight(ifstream& f, GameObject* parent);

}
