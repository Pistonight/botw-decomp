#pragma once

#include <heap/seadDisposer.h>
#include <prim/seadSafeString.h>
#include <prim/seadBitFlag.h>
#include <prim/seadStringBuilder.h>
#include <math/seadMatrix.h>
#include <container/seadSafeArray.h>
#include "KingSystem/ActorSystem/actBaseProcHandle.h"
#include "KingSystem/ActorSystem/actBaseProcLink.h"
#include "KingSystem/Utils/Types.h"
#include "math/seadVectorFwd.h"

namespace ksys::act {
class InstParamPack;
class Actor;
}

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

inline bool isArmorCreateEquipmentSlot(s32 slot) {
    return slot == (s32)CreateEquipmentSlot::ArmorHead || slot == (s32)CreateEquipmentSlot::ArmorUpper ||
           slot == (s32)CreateEquipmentSlot::ArmorLower;
}

class PlayerCreateTracker {
public:
    PlayerCreateTracker() {
        u = 0;
    }
    // TODO 71010C2A18 probably
    ~PlayerCreateTracker();

    // TODO 71010C2A10;
    static PlayerCreateTracker make(const sead::SafeString& name, u64 x);

    u64 u = 0;

    class Scope {
    public:
        // 71010C2A1C
        Scope(u64 tracker, const sead::SafeString& name, const sead::SafeString& caller, s32 level);
        // 71010C2A34, 71010C2A48
        virtual ~Scope();


    private:
        u64 mTracker;
    };
    KSYS_CHECK_SIZE_NX150(Scope, 0x10);

};
KSYS_CHECK_SIZE_NX150(PlayerCreateTracker, 0x8);


class CreatePlayerEquipActorMgr {
    SEAD_SINGLETON_DISPOSER(CreatePlayerEquipActorMgr)
    CreatePlayerEquipActorMgr();
    virtual ~CreatePlayerEquipActorMgr();

public:
    void init();
    void postCalc();
    void resetAll();
    void reset(s32 slot_idx);
    bool areAllEquipmentActorsReady() const;
    bool areEquipmentActorsReady(s32 slot_idx) const;
    bool isEquipmentProcReady(s32 slot_idx) const;
    ksys::act::Actor* tryGetEquipmentActor(s32 slot_idx);

    void doRequestCreateWeapon(s32 slot_idx, 
                               const sead::SafeString& name, 
                               int value,
                               const WeaponModifierInfo* modifier, 
                               const sead::SafeString& caller);

    void requestCreateWeapon(const sead::SafeString& name, 
                               int value,
                               const WeaponModifierInfo* modifier, 
                               const sead::SafeString& caller);
void doRequestCreateArmorByRawLife(
        s32 slot_idx, 
        const sead::SafeString& actor_name, const sead::Vector3f& pos, 
        /* s32 dye_color, */
    ksys::act::InstParamPack* params,
        s32 raw_life
    );

    void doRequestCreateArmor(s32 slot_idx, const sead::SafeString& name, int dye_color,
                              const sead::SafeString& caller);
    void requestCreateArmor(const sead::SafeString& name, 
                               int dye_color,
                               const sead::SafeString& caller);

    void requestCreateDefaultArmor(s32 slot_idx, const sead::SafeString& caller);
    void requestCreateArmorHeadB(const sead::SafeString& name, int dye_color,
                                 const sead::SafeString& caller);

private:
    enum class EntryStatus : s32 {
        Idle = 0,
        Loading = 1,
        Ready = 2
    };
    class Entry {
        friend class CreatePlayerEquipActorMgr;
        sead::FixedStringBuilder<64> mActorName;
        EntryStatus mStatus = EntryStatus::Idle;
        ksys::act::BaseProcLink mProcLink;

        void reset() {
            mActorName.clear();
            mStatus = EntryStatus::Idle;
            mProcLink.reset();
        }
    };
    KSYS_CHECK_SIZE_NX150(Entry, 0x68);

    class StringBoard_Temp {
    public:
        virtual ~StringBoard_Temp() = default;
    };

    bool isSlotLoading(s32 slot_idx) const {
        return mIsLoading.isOnBit(slot_idx) && mEntries[slot_idx].mStatus == EntryStatus::Loading;
    }

    sead::SafeArray<ksys::act::BaseProcHandle, 6> mProcHandles;
    sead::BitFlag8 mIsLoading;
    PlayerCreateTracker mTracker;
    sead::SafeArray<Entry, (u64)CreateEquipmentSlot::Length> mEntries;

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

class CreatePlayerTrachActorMgr {
};

bool needsArmorHeadB(const sead::SafeString& armor_head_name,
                     const sead::SafeString& armor_upper_name);
bool isArmorHeadMantleType2(const sead::SafeString& armor_head_name);
bool isArmorUpperNotUseMantleType0(const sead::SafeString& armor_upper_name);
CreateEquipmentSlot getCreateEquipmentSlot(ui::PouchItemType type);
ui::EquipmentSlot getEquipmentSlot(CreateEquipmentSlot slot);
bool createEquipmentFromItem(const ui::PouchItem* item, const sead::SafeString& caller);

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
);

}
