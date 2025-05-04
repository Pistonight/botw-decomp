#pragma once
#include <prim/seadSafeString.h>

namespace uking::act {

// used in CreatePlayerEquipActorMgr and CreatePlayerTrashActorMgr
void* stubbedInitWithName(const sead::SafeString& name, u64);
void stubbedDestroy(void** ptr);

}
