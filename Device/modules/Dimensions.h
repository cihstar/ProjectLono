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

struct Timing
{
    uint16_t tx; //Time between sending readings in ms
    uint16_t samp; //How often to sample the sensor in ms
    uint16_t reads; //how many reads of the sensor to average per sample
};

#endif