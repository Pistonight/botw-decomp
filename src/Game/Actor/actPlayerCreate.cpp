#include "Game/Actor/actPlayerCreate.h"
#include <prim/seadStringBuilder.h>
#include <prim/seadRuntimeTypeInfo.h>
#include "Game/Actor/actWeapon.h"
#include "Game/UI/uiPauseMenuDataMgr.h"
#include "KingSystem/ActorSystem/actActorConstDataAccess.h"
#include "KingSystem/ActorSystem/actActorHeapUtil.h"
#include "KingSystem/ActorSystem/actActorLinkConstDataAccess.h"
#include "KingSystem/ActorSystem/actInfoCommon.h"
#include "KingSystem/ActorSystem/actInfoData.h"
#include "KingSystem/ActorSystem/actInstParamPack.h"
#include "KingSystem/ActorSystem/actActorCreator.h"
#include "KingSystem/ActorSystem/actPlayerInfo.h"
#include "KingSystem/ActorSystem/Profiles/actWeaponBase.h"
#include "KingSystem/ActorSystem/Profiles/actPlayerBase.h"
#include "KingSystem/GameData/gdtCommonFlagsUtils.h"

namespace uking::act {

SEAD_SINGLETON_DISPOSER_IMPL(CreatePlayerEquipActorMgr)

CreatePlayerEquipActorMgr::CreatePlayerEquipActorMgr() {
    mTracker = PlayerCreateTracker::make("CreatePlayerEquipActorMgr", 0);
    /* memset((char*)&mProcHandles.mBuffer, 0, sizeof(mProcHandles)); */
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

// NON_MATCHING loop variable
void CreatePlayerEquipActorMgr::postCalc() {
    if (mIsLoading.isZero()) {
        // nothing is loading
        return;
    }
    for (s32 i = 0; i < mEntries.size(); i++) {
        if (!isSlotLoading(i)) {
            continue;
        }
        auto& handle = mProcHandles[i];
        if (handle.hasProcCreationFailed()) {
            handle.isProcCreationCancelled();
            handle.deleteProc();
            mEntries[i].reset();
            mIsLoading.resetBit(i);
        } else if (handle.isProcReady()) {
            if (isArmorCreateEquipmentSlot(i)) {
                auto* actor = sead::DynamicCast<ksys::act::Actor>(handle.releaseAndWakeProc());
                mEntries[i].mProcLink.acquire(actor, false);
            }
            mEntries[i].mStatus = EntryStatus::Ready;
        }
    }
}

void CreatePlayerEquipActorMgr::resetAll() {
    for (s32 i = 0; i < (s32)CreateEquipmentSlot::Length; ++i) {
        reset(i);
    }
    mIsLoading.makeAllZero();
}

void CreatePlayerEquipActorMgr::reset(s32 slot_idx) {
    auto& handle = mProcHandles[slot_idx];
    if (handle.isSettled()) {
        handle.deleteProc();
    }
    mIsLoading.resetBit(slot_idx);
    mEntries[slot_idx].reset();
}

// NON_MATCHING register renaming and order
bool CreatePlayerEquipActorMgr::areAllEquipmentActorsReady() const {
    if (mIsLoading.isZero()) {
        return true;
    }
    for (u32 i = 0; i< (u32)CreateEquipmentSlot::Length; ++i) {
        auto& handle = mProcHandles[(s32)i];
        if (isSlotLoading((s32)i)) {
            return false;
        }
        auto* proc = handle.getProc();
        auto* weapon = sead::DynamicCast<ksys::act::WeaponBase>(proc);
        if(weapon && !weapon->areExtraActorsReady()) {
            return false;
        }
    }
    return true;
}

bool CreatePlayerEquipActorMgr::areEquipmentActorsReady(s32 slot_idx) const {
    if (mIsLoading.isOnBit(slot_idx) && mEntries[slot_idx].mStatus == EntryStatus::Loading) {
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
    if (handle.isSettled()) {
        return handle.isProcReady();
    }
    return false;
}

ksys::act::Actor* CreatePlayerEquipActorMgr::tryGetEquipmentActor(s32 slot_idx) {
    if (mEntries[slot_idx].mStatus != EntryStatus::Ready) {
        return nullptr;
    }
    auto& handle = mProcHandles[slot_idx];
    if (handle.isSettled()) {
        if (handle.isProcReady()) {
            auto* actor = sead::DynamicCast<ksys::act::Actor>(handle.releaseAndWakeProc());
            mEntries[slot_idx].mProcLink.acquire(actor, false);
        }
    }

    auto* actor = 
        sead::DynamicCast<ksys::act::Actor>(
        mEntries[slot_idx].mProcLink.getProc(nullptr, nullptr));

    mEntries[slot_idx].reset();

    return actor;
}

void CreatePlayerEquipActorMgr::doRequestCreateWeapon(s32 slot_idx, 
                               const sead::SafeString& name, 
                               int value,
                               const WeaponModifierInfo* modifier, 
                               const sead::SafeString& caller) {
    auto scope = 
        PlayerCreateTracker::Scope(mTracker.u, "requestCreateWeapon", caller, 1);

    if (mIsLoading.isOnBit(slot_idx)) {
        mProcHandles[slot_idx].deleteProc();
    }
    auto* player = ksys::act::PlayerInfo::instance()->getPlayer();

    // TODO: probably better way to write this?
    sead::Matrix34f pos = {
        1.0, 0.0, 0.0, player->getMtx().m[0][3], 
        0.0, 1.0 , 0.0, player->getMtx().m[1][3], 
        0.0 , 0.0, 1.0, player->getMtx().m[2][3] ,
    };


    requestCreateWeaponByRawLife(
        name.cstr(),
        pos,
        1.0,
        ksys::act::ActorHeapUtil::instance()->getBaseProcHeap(),
        &mProcHandles[slot_idx],
        value,
        false,
        modifier,
        2,
        2
    );

    if (mProcHandles[slot_idx].isSettled()) {
        auto& entry = mEntries[slot_idx];
        entry.mActorName.copy(name.cstr());
        mIsLoading.setBit(slot_idx);
        entry.mStatus = EntryStatus::Loading;
    }

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

// TODO: initialized in sInitArmorStrings (0x7100E2D0F0)
// find a better place to put these
sead::SafeString ArmorDyeColor; // "ArmorDyeColor"
sead::SafeString EnableDynamicColorChange; // "EnableDynamicColorChange"


void CreatePlayerEquipActorMgr::doRequestCreateArmorByRawLife(
    s32 slot_idx,
        const sead::SafeString& actor_name, const sead::Vector3f& pos, 
    /* s32 dye_color, */
    ksys::act::InstParamPack* params,
    s32 raw_life
    ) {
        params->getBuffer().add(pos, "@P");
        params->getBuffer().add(raw_life, "@RL");

        ksys::act::ActorCreator::instance()->requestCreateActor(
            actor_name.cstr(),
            ksys::act::ActorHeapUtil::instance()->getBaseProcHeap(),
            &mProcHandles[slot_idx],
            params,
            nullptr,
            2
        );
}

void CreatePlayerEquipActorMgr::doRequestCreateArmor(s32 slot_idx, const sead::SafeString& name, int dye_color,
                              const sead::SafeString& caller) {
    auto scope = 
        PlayerCreateTracker::Scope(mTracker.u, "requestCreateArmor", caller, 1);

    if (mIsLoading.isOnBit(slot_idx)) {
        mProcHandles[slot_idx].deleteProc();
    }

    ksys::act::InstParamPack params;
    bool is_dye = ksys::act::InfoData::instance()->hasTag(name.cstr(), ksys::act::tags::ArmorDye);
    if (is_dye) {
        /* params->getBuffer().add(dye_color, DyeParamNames[1]); */
        /* if (ksys::gdt::getFlag_ColorChange_EnablePreview()) { */
        /*     params->getBuffer().add(true, ColorChangeParamName); */
        /* } */
        /* setArmorDyeColorParam(dye_color, &params); */
    params->add(dye_color, ArmorDyeColor);
        if (ksys::gdt::getFlag_ColorChange_EnablePreview()) {
            const auto& name = EnableDynamicColorChange;
            params->add(true, name);
        }
    }

    {
        ksys::act::ActorConstDataAccess access;
        ksys::act::acquireActor(
            &ksys::act::PlayerInfo::instance()->getPlayerLink(), &access
        );
        sead::Vector3f pos;
        access.getActorMtx().getBase(pos, 3);
        doRequestCreateArmorByRawLife(
            slot_idx,
            name,
            pos,
            &params,
            2
        );
        /* params->add(pos, "@P"); */
        /* params->add(2, "@RL"); */
        /*  */
        /* ksys::act::ActorCreator::instance()->requestCreateActor( */
        /*     name.cstr(), */
        /*     ksys::act::ActorHeapUtil::instance()->getBaseProcHeap(), */
        /*     &mProcHandles[slot_idx], */
        /*     &params, */
        /*     nullptr, */
        /*     2 */
        /* ); */
    }
    if (mProcHandles[slot_idx].isSettled()) {
        auto& entry = mEntries[slot_idx];
        entry.mActorName.copy(name.cstr());
        mIsLoading.setBit(slot_idx);
        entry.mStatus = EntryStatus::Loading;
    }
}

void CreatePlayerEquipActorMgr::requestCreateArmor( const sead::SafeString& name, 
                               int dye_color,
                               const sead::SafeString& caller) {

    auto scope = 
        PlayerCreateTracker::Scope(mTracker.u, "requestCreateArmor", caller, 1);
    auto* info_data = ksys::act::InfoData::instance();
    if (!info_data) {
        return;
    }
    const char* profile_raw;
    if (!info_data->getActorProfile(&profile_raw, name.cstr())) {
        return;
    }

    sead::SafeString profile = profile_raw;
    s32 slot_idx = (s32)CreateEquipmentSlot::ArmorHead;
    if (profile == "Armor_Upper") {
        slot_idx = (s32)CreateEquipmentSlot::ArmorUpper;
    } else if (profile == "Armor_Lower") {
        slot_idx = (s32)CreateEquipmentSlot::ArmorLower;
    }

    doRequestCreateArmor(slot_idx, name, dye_color, caller);
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

void requestCreateWeaponByRawLife(
    const char* actor_class,
    const sead::Matrix34f& matrix,
    f32 scale,
    sead::Heap* heap,
    ksys::act::BaseProcHandle* handle,
    s32 life,
    bool is_player_put,
    const WeaponModifierInfo* modifier,
    s32 task_lane_id,
    s32 raw_life
) {
    ksys::act::InstParamPack params;
    params->add(is_player_put, "IsPlayerPut");
    if (modifier) {
        modifier->addModifierParams(params);
    }
    params->add(true, "IsWeaponCreateByRawLife");
    params->add(raw_life, "@RL");

    ksys::act::requestCreateActor(
        actor_class, matrix, scale, heap, handle, life, &params, task_lane_id);
}


}  // namespace uking::act
