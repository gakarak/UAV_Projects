#ifndef MAINMODEL_H
#define MAINMODEL_H

#include "entities/trajectory.h"

namespace modelpkg
{
    class MainModel
    {
    public:
        MainModel();

        Trajectory  first_trj;
        Trajectory  second_trj;
        Map         main_map;

        int trj_count;
    };
}

#endif // MAINMODEL_H
