#include "main_model.h"

using namespace modelpkg;

MainModel::MainModel()
    :trj_count(2)
{

}

void MainModel::setTrajectory(int trj_num, const Trajectory &trj)
{
    getTrajectory(trj_num) = trj;
}

Trajectory& MainModel::getTrajectory(int trj_num)
{
    if (trj_num > trj_count)
    {
        throw Exception("Error: index out of array bounds trj_num >= trj_count");
    }
    return trj_num == 0? first_trj: second_trj;
}

const Trajectory &MainModel::getTrajectory(int trj_num) const
{
    if (trj_num > trj_count)
    {
        throw Exception("Error: index out of array bounds trj_num >= trj_count");
    }
    return trj_num == 0? first_trj: second_trj;
}
