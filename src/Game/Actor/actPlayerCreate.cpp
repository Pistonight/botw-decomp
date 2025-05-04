#include "Game/Actor/actPlayerCreate.h"
#include <prim/seadStringBuilder.h>
#include <prim/seadRuntimeTypeInfo.h>
#include "Game/Actor/actWeapon.h"
#include "Game/UI/uiPauseMenuDataMgr.h"
#include "KingSystem/ActorSystem/actInfoCommon.h"
#include "KingSystem/ActorSystem/actInfoData.h"
#include "KingSystem/ActorSystem/Profiles/actWeaponBase.h"

namespace uking::act {

SEAD_SINGLETON_DISPOSER_IMPL(CreatePlayerEquipActorMgr)

CreatePlayerEquipActorMgr::CreatePlayerEquipActorMgr() {
    /* mNullptr = stubbedInitWithName("CreatePlayerEquipActorMgr", 0); */
}
CreatePlayerEquipActorMgr::~CreatePlayerEquipActorMgr() {
    /* stubbedDestroy(&mNullptr); */
}

void CreatePlayerEquipActorMgr::init() {
    sead::SafeString x;
    x = "読み込み中デバッグボード"; // Loading Debug Board
    _318 = 3;
    mDebugStatus = "◎読み込み中◎"; // Loading
    _330 = 0.9;

    _334 = 0;

    _338 = 1;
    _33C = 1;
    _340 = 1;
    _344 = 1;

    _348 = 0.2;
    _34C = 0.2;
    _350 = 0.2;
    _354 = 1;
    _358 = 1.8;
    _35C = 0;
    _364 = 34.742115; // 0.6063 radian?
    _360 = 198.638428;
    _368 = 0;
    _36C = 0.2;
    _370 = 0.2;
    _374 = 0.3;

    _378 = 0;
    _380 = 0;
    _388 = 0;
    _390 = -506.679992;
    _394 = -256.519989;

    _3A0 = 0;
    _3A4 = 0;
    _3A8 = 1;
}

void CreatePlayerEquipActorMgr::deleteAllEquipments() {
    for (s32 i = 0; i < (s32)CreateEquipmentSlot::Length; ++i) {
        deleteEquipment(i);
    }
    mFlag.makeAllZero();
}

void CreatePlayerEquipActorMgr::deleteEquipment(s32 slot_idx) {
    auto& handle = mProcHandles[slot_idx];
    if (handle.getUnit() || handle.hasFailed()) {
        handle.deleteProc();
    }
    mFlag.resetBit(slot_idx);
    mEntries[slot_idx].reset();
}

bool CreatePlayerEquipActorMgr::areAllEquipmentActorsReady() const {
    if (mFlag.isZero()) {
        return true;
    }
    for (u32 i = 0; i< (u32)CreateEquipmentSlot::Length; ++i) {
        if (mFlag.isOnBit((s32)i) && mEntries[(s32)i].mStatus == EntryStatus::Loading) {
            return false;
        }
        const auto& handle = mProcHandles[(s32)i];
        auto* proc = handle.getProc();
        auto* weapon = sead::DynamicCast<ksys::act::WeaponBase>(proc);
        if(weapon && !weapon->areExtraActorsReady()) {
            return false;
        }
    }
    return true;
}

bool CreatePlayerEquipActorMgr::areEquipmentActorsReady(s32 slot_idx) const {
    if (mFlag.isOnBit(slot_idx) && mEntries[slot_idx].mStatus == EntryStatus::Loading) {
        return false;
    }
    const auto& handle = mProcHandles[slot_idx];
    auto* proc = handle.getProc();
    auto* weapon = sead::DynamicCast<ksys::act::WeaponBase>(proc);
    return !weapon || weapon->areExtraActorsReady();
}

bool CreatePlayerEquipActorMgr::isEquipmentProcReady(s32 slot_idx) const {
    const auto& entry = mEntries[slot_idx];
    if (entry.mStatus != EntryStatus::Ready) {
        return false;
    }
    const auto& handle = mProcHandles[slot_idx];
    if (handle.getUnit() || handle.hasFailed()) {
        return handle.isProcReady();
    }
    return false;
}

void CreatePlayerEquipActorMgr::requestCreateWeapon( const sead::SafeString& name, 
                               int value,
                               const WeaponModifierInfo* modifier, 
                               const sead::SafeString& caller) {
    auto scope = 
        PlayerCreateTracker::Scope(mTracker.u, "requestCreateWeapon", caller, 1);
    auto* info_data = ksys::act::InfoData::instance();
    if (!info_data) {
        return;
    }
    const char* profile_raw;
    if (!info_data->getActorProfile(&profile_raw, name.cstr())) {
        return;
    }
    sead::SafeString profile = profile_raw;
    s32 slot_idx = (s32)CreateEquipmentSlot::WeaponSword;
    if (profile == "WeaponBow") {
        slot_idx = (s32)CreateEquipmentSlot::WeaponBow;
    } else if (profile == "WeaponShield") {
        slot_idx = (s32)CreateEquipmentSlot::WeaponShield;
    }

    doRequestCreateWeapon(slot_idx, name, value, modifier, caller);
}


void CreatePlayerEquipActorMgr::requestCreateDefaultArmor(s32 slot,
                                                          const sead::SafeString& caller) {
    switch (slot) {
    case (int)CreateEquipmentSlot::ArmorHead:
        doRequestCreateArmor(3, "Armor_Default_Head", -1, caller);
        break;
    case (int)CreateEquipmentSlot::ArmorUpper:
        doRequestCreateArmor(4, "Armor_Default_Upper", -1, caller);
        break;
    case (int)CreateEquipmentSlot::ArmorLower:
        doRequestCreateArmor(5, "Armor_Default_Lower", -1, caller);
        break;
    default:
        break;
    }
}

void CreatePlayerEquipActorMgr::requestCreateArmorHeadB(const sead::SafeString& name, int dye_color,
                                                        const sead::SafeString& caller) {
    sead::FixedStringBuilder<0x40> s;
    s.copy(name.cstr());
    s.append("_B", -1);
    doRequestCreateArmor((int)CreateEquipmentSlot::ArmorHead, s, dye_color, caller);
}

bool needsArmorHeadB(const sead::SafeString& armor_head_name,
                     const sead::SafeString& armor_upper_name) {
    if (armor_upper_name.isEmpty()) {
        return false;
    }
    return isArmorHeadMantleType2(armor_head_name) &&
           isArmorUpperNotUseMantleType0(armor_upper_name);
}

bool isArmorHeadMantleType2(const sead::SafeString& armor_head_name) {
    if (!ksys::act::InfoData::instance()) {
        return false;
    }
    return ksys::act::getArmorHeadMantleType(ksys::act::InfoData::instance(),
                                            armor_head_name.cstr()) == 2;
}

bool isArmorUpperNotUseMantleType0(const sead::SafeString& armor_upper_name) {
    if (!ksys::act::InfoData::instance()) {
        return false;
    }
    return ksys::act::getArmorUpperUseMantleType(ksys::act::InfoData::instance(),
                                               armor_upper_name.cstr()) != 0;
}

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

ui::EquipmentSlot getEquipmentSlot(CreateEquipmentSlot slot) {
    switch (slot) {
    case CreateEquipmentSlot::WeaponSword:
        return ui::EquipmentSlot::WeaponRight;
    case CreateEquipmentSlot::WeaponShield:
        return ui::EquipmentSlot::WeaponLeft;
    case CreateEquipmentSlot::WeaponBow:
        return ui::EquipmentSlot::WeaponBow;
    case CreateEquipmentSlot::ArmorHead:
        return ui::EquipmentSlot::ArmorHead;
    case CreateEquipmentSlot::ArmorUpper:
        return ui::EquipmentSlot::ArmorUpper;
    case CreateEquipmentSlot::ArmorLower:
        return ui::EquipmentSlot::ArmorLower;
    default:
        return ui::EquipmentSlot::Invalid;
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

}  // namespace uking::act
