#pragma once
#include "../GameConstants.h"

#if LAWN_WIDESCREEN

#include "GameObject.h"

namespace Sexy
{
    class Graphics;
}
using namespace Sexy;

class Bush : public GameObject
{
public:
    int             mPosX;
    int             mPosY;
    int             mID;
    int             mBushIndex;
    ReanimationID   mReanimID;

public:
    Bush();

    void    BushInitialize(int theX, int theY, int theRow, bool theNight);
    void    Update();
    void    Draw(Graphics* g);
    void    AnimateBush();
};

#endif // LAWN_WIDESCREEN
