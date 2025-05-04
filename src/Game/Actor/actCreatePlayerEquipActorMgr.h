#pragma once

#include <heap/seadDisposer.h>
#include <prim/seadSafeString.h>
#include <container/seadObjArray.h>
#include "KingSystem/ActorSystem/actBaseProcHandle.h"
#include "KingSystem/ActorSystem/actBaseProcLink.h"
#include "KingSystem/Utils/Types.h"
#include "prim/seadStringBuilder.h"

namespace uking::ui {

class PouchItem;
enum class PouchItemType;
enum class EquipmentSlot;

}  // namespace uking::ui

namespace uking::act {
struct WeaponModifierInfo;

enum class CreateEquipmentSlot : u8 {
    WeaponSword = 0,
    WeaponShield = 1,
    WeaponBow = 2,
    ArmorHead = 3,
    ArmorUpper = 4,
    ArmorLower = 5,
    Length = 6,
};

class CreatePlayerEquipActorMgr {
    SEAD_SINGLETON_DISPOSER(CreatePlayerEquipActorMgr)
    CreatePlayerEquipActorMgr();
    virtual ~CreatePlayerEquipActorMgr();

public:
    void init();
    // postCalc();
    // resetAllEquipmentLoading
    // resetEquipmentLoading
    // plEquipmentActorsReady
    bool isEquipmentProcReady(s32 slot_idx) const;
    // wakeEquipment

    // TODO 0x71006669F8
    void doRequestCreateWeapon(int slot_idx, const sead::SafeString& name, int value,
                               const WeaponModifierInfo* modifier, const sead::SafeString& caller);
    // requestCreateWeapon

    // TODO 0x7100666CF8
    void doRequestCreateArmor(int slot_idx, const sead::SafeString& name, int dye_color,
                              const sead::SafeString& caller);
    // requestCreateArmor
    void requestCreateDefaultArmor(s32 slot_idx, const sead::SafeString& caller);
    void requestCreateArmorHeadB(const sead::SafeString& name, int dye_color,
                                 const sead::SafeString& caller);

private:
    enum class EntryStatus : u32 {
        _0 = 0,
        Ready = 2
    };
    class Entry {
        friend class CreatePlayerEquipActorMgr;
        sead::FixedStringBuilder<64> mS;
        EntryStatus mStatus = EntryStatus::_0;
        ksys::act::BaseProcLink mProcLink;
    };
    KSYS_CHECK_SIZE_NX150(Entry, 0x68);

    class StringBoard_Temp {
    public:
        virtual ~StringBoard_Temp() = default;
    };

    sead::SafeArray<ksys::act::BaseProcHandle, 6> mProcHandles{};
    u32 mIsLoadingFlags = 0;
    void* mNullptr = nullptr;
    sead::SafeArray<Entry, (u64)CreateEquipmentSlot::Length> mEntries{};
    StringBoard_Temp mStringBoard;
    void* _310 = nullptr;
    s32 _318 = 3; // some enum
    sead::SafeString mDebugStatus;
    f32 _330 = 1;

    u32 _334 = 0;

    f32 _338 = 0;
    f32 _33C = 0;
    u32 _340 = 0;
    f32 _344 = 1;

    f32 _348 = 0;
    f32 _34C = 0;
    f32 _350 = 0;

    f32 _354 = 1;
    f32 _358 = 0;
    u32 _35C = 0;

    f32 _360 = 1;
    f32 _364 = 1;
    u32 _368 = 0;

    f32 _36C = 0;
    f32 _370 = 1;
    f32 _374 = 1;

    u64 _378 = 0;

    u64 _380 = 0;
    u64 _388 = 0;
    f32 _390 = 0;
    f32 _394 = 0;
    u32 _398 = 0;
    u32 _39C = 0;
    u32 _3A0 = 0;
    u32 _3A4 = 0;
    f32 _3A8 = 1;

};
KSYS_CHECK_SIZE_NX150(CreatePlayerEquipActorMgr, 0x3B0);

bool needsArmorHeadB(const sead::SafeString& armor_head_name,
                     const sead::SafeString& armor_upper_name);
bool isArmorHeadMantleType2(const sead::SafeString& armor_head_name);
bool isArmorUpperNotUseMantleType0(const sead::SafeString& armor_upper_name);
CreateEquipmentSlot getCreateEquipmentSlot(ui::PouchItemType type);
ui::EquipmentSlot getEquipmentSlot(CreateEquipmentSlot slot);
bool createEquipmentFromItem(const ui::PouchItem* item, const sead::SafeString& caller);

}  // namespace uking::act
