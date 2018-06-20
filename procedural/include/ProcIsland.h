#pragma once

#include <vector>

class Model;

struct ProcState
{
    ProcState() = default;
    // TODO
};

void GenerateProceduralIsland(std::vector<Model*>& models, ProcState params);
