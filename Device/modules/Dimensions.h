#ifndef __DIMENSIONS_H__
#define __DIMENSIONS_H__

struct Dimensions
{
    float tubeRadius;
    float funnelRadius;
    float outTubeRadius;
    float outTubeWall;
    float pressureSensorTubeRadius;
};

struct Calibrate
{
    uint16_t fullAdc;
    uint16_t emptyAdc;
    float fullHeight;
};

#endif