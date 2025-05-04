#pragma once

#include "KingSystem/ActorSystem/actActor.h"

namespace ksys::act {

// TODO
class WeaponBase : public Actor {
public:
    bool areExtraActorsReady() const;
};

}  // namespace ksys::act
