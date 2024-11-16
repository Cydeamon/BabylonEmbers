#ifndef FLAMEPARTICLE_H
#define FLAMEPARTICLE_H

#include <Engine/GameObjects/PhysicsRectangle.h>

class FlameParticle : public PhysicsRectangle
{
public:
    FlameParticle(Vector2 pos, float angle);
    ~FlameParticle();
    void Update() override;

protected:
    double nextSmokeTime = 0;
    float smokeIntervalMin = 0.25;
    float smokeIntervalMax = 1;
    bool prevIsColliding = true;
    bool isColliding = false;

    void processCollisions();
    void handleAudio();
};


#endif