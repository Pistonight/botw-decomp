#pragma once

#include <math/seadMatrix.h>
#include <heap/seadHeap.h>

namespace ksys::act {

class BaseProcHandle;
class InstParamPack;

void requestCreateWeaponByRawLife(
    const char* actor_class,
    const sead::Matrix34f& matrix,
    f32 scale,
    sead::Heap* heap,

);
void requestCreateActor(
    const char* actor_class,
    const sead::Matrix34f& matrix,
    f32 scale,
    sead::Heap* heap,
    BaseProcHandle* handle,
    s32 life,
    InstParamPack* params,
    s32 task_lane_id
);

}
