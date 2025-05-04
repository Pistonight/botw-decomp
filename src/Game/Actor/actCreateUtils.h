#pragma once

#include <math/seadMatrix.h>
#include <prim/seadSafeString.h>

namespace sead {
class Heap;
}

namespace ksys::act {
class BaseProcHandle;
class InstParamPack;
}  // namespace ksys::act

namespace uking::ui {
class PouchItem;
enum class PouchItemType;
}  // namespace uking::ui

namespace uking::act {

enum class CreateEquipmentSlot : u8;
struct WeaponModifierInfo;

CreateEquipmentSlot getCreateEquipmentSlot(ui::PouchItemType type);

bool createEquipmentFromItem(const ui::PouchItem* item, const sead::SafeString& caller);

void requestCreateWeaponByRawLife(const char* actor_class, const sead::Matrix34f& matrix, f32 scale,
                                  sead::Heap* heap, ksys::act::BaseProcHandle* handle, s32 life,
                                  bool is_player_put, const WeaponModifierInfo* modifier,
                                  s32 task_lane_id, s32 raw_life);

void requestCreateActor(const char* actor_class, const sead::Matrix34f& matrix, f32 scale,
                        sead::Heap* heap, ksys::act::BaseProcHandle* handle, s32 life,
                        ksys::act::InstParamPack* params_ptr, s32 task_lane_id);

}  // namespace uking::act
