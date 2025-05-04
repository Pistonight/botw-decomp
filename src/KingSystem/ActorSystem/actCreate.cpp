#include "KingSystem/ActorSystem/actCreate.h"
#include "KingSystem/ActorSystem/actActorCreator.h"
#include "KingSystem/ActorSystem/actInstParamPack.h"

namespace ksys::act {

void requestCreateActor(
    const char* actor_class,
    const sead::Matrix34f& matrix,
    f32 scale,
    sead::Heap* heap,
    BaseProcHandle* handle,
    s32 life,
    InstParamPack* params_in,
    s32 task_lane_id
) {


    InstParamPack params;
    if (params_in) {
        params = *params_in;
        /* params.setProc(params_in->getProc()); */
        /* params.getBuffer() = params_in->getBuffer(); */
    }
    ActorCreator::addScale(params, scale);
    params->add(life, "Life");
    params->add(matrix, "@M");
    ActorCreator::instance()->requestCreateActor(
        actor_class, heap, handle, &params, nullptr, task_lane_id);
}

}
