#include "Game/Actor/actCreateUtils.h"
#include "Game/Actor/actPlayerCreate.h"
#include "Game/Actor/actWeapon.h"
#include "Game/UI/uiPauseMenuDataMgr.h"
#include "KingSystem/ActorSystem/actActorCreator.h"

namespace uking::act {

CreateEquipmentSlot getCreateEquipmentSlot(ui::PouchItemType type) {
    switch (type) {
    case ui::PouchItemType::Sword:
        return CreateEquipmentSlot::WeaponSword;
    case ui::PouchItemType::Bow:
        return CreateEquipmentSlot::WeaponBow;
    case ui::PouchItemType::Shield:
        return CreateEquipmentSlot::WeaponShield;
    case ui::PouchItemType::ArmorHead:
        return CreateEquipmentSlot::ArmorHead;
    case ui::PouchItemType::ArmorUpper:
        return CreateEquipmentSlot::ArmorUpper;
    case ui::PouchItemType::ArmorLower:
        return CreateEquipmentSlot::ArmorLower;
    default:
        return CreateEquipmentSlot::Length;
    }
}

bool createEquipmentFromItem(const ui::PouchItem* item, const sead::SafeString& caller) {
    if (!item) {
        return false;
    }

    auto* mgr = CreatePlayerEquipActorMgr::instance();
    if (!mgr) {
        return false;
    }

    auto type = item->getType();
    auto slot = getCreateEquipmentSlot(type);
    int slot_idx = (int)slot;

    if (slot <= CreateEquipmentSlot::WeaponBow) {
        WeaponModifierInfo modifier(*item);
        mgr->doRequestCreateWeapon(slot_idx, item->getName(), item->getValue(), &modifier, caller);
        return true;
    }

    if (slot <= CreateEquipmentSlot::ArmorLower) {
        mgr->doRequestCreateArmor(slot_idx, item->getName(), item->getValue(), caller);
        return true;
    }

    return false;
}

void requestCreateWeaponByRawLife(const char* actor_class, const sead::Matrix34f& matrix, f32 scale,
                                  sead::Heap* heap, ksys::act::BaseProcHandle* handle, s32 life,
                                  bool is_player_put, const WeaponModifierInfo* modifier,
                                  s32 task_lane_id, s32 raw_life) {
    ksys::act::InstParamPack params;
    params->add(is_player_put, "IsPlayerPut");

#ifdef MATCHING_HACK_NX_CLANG
    // stack reordering
    const auto f = [&]() {
#endif
        if (modifier) {
            modifier->addModifierParams(params);
        }
        params->add(true, "IsWeaponCreateByRawLife");
        params->add(raw_life, "@RL");

        uking::act::requestCreateActor(actor_class, matrix, scale, heap, handle, life, &params,
                                       task_lane_id);

#ifdef MATCHING_HACK_NX_CLANG
    };
    f();
#endif
}

void requestCreateActor(const char* actor_class, const sead::Matrix34f& matrix, f32 scale,
                        sead::Heap* heap, ksys::act::BaseProcHandle* handle, s32 life,
                        ksys::act::InstParamPack* params_ptr, s32 task_lane_id) {
    ksys::act::InstParamPack params;
    if (params_ptr) {
        params = *params_ptr;
    }

#ifdef MATCHING_HACK_NX_CLANG
    // stack reordering
    const auto f = [&]() {
#endif
        ksys::act::ActorCreator::addScale(params, scale);
        params->add(life, sead::SafeString("Life"));
        params->add(matrix, sead::SafeString("@M"));
        ksys::act::ActorCreator::instance()->requestCreateActor(actor_class, heap, handle, &params,
                                                                nullptr, task_lane_id);

#ifdef MATCHING_HACK_NX_CLANG
    };
    f();
#endif
}

}  // namespace uking::act
