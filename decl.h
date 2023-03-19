#ifdef __ASDECL_H
#undef __ASDECL_H
#undef AS_DECL
#undef AS_DECL_END
#undef AS_SIMPLE_DECL
#undef AS_REF_DECL_CONSTRUCT
#undef AS_REF_DECL
#undef AS_PROP_DECL
#undef AS_PROP_DECL_NOREFBIND
#undef AS_PROP_DECLREF
#undef AS_PROP_DECL2
#undef AS_PROP_DECL_SETNAKED
#undef AS_SIMPLE_REG
#undef AS_SIMPLE_REG2
#undef AS_REF_REG
#undef AS_PROP_REG
#undef AS_PROP_REGGET
#undef AS_PROP_REGGET_NOREFBIND
#undef AS_PROP_REGSET
#endif

#include "asdecl.h"
#include "sharedvars.h"

AS_DECL(CVehicle);
AS_DECL(CompressedVector);

AS_REF_DECL_CONSTRUCT(ScriptHandle, ScriptHandle)
    AS_PROP_DECL(ScriptHandle, uint16_t, id, m_nId)
    AS_PROP_DECL(ScriptHandle, uint16_t, counter, m_nCounter)
    AS_PROP_DECL(ScriptHandle, int32_t, asInt, m_nValueInt)
    AS_PROP_DECL(ScriptHandle, uint32_t, asUInt, m_nHandleIndex)
    AS_PROP_DECL(ScriptHandle, float, asFloat, m_nValueFloat)
AS_DECL_END()
    
AS_REF_DECL_CONSTRUCT(CVector2D, Vector2D)
    AS_PROP_DECL(CVector2D, float, x, x)
    AS_PROP_DECL(CVector2D, float, y, y)
AS_DECL_END()

AS_REF_DECL_CONSTRUCT_FROM(CVector, Vector3D, CVector2D)
    AS_PROP_DECL(CVector, float, z, z)
    #ifdef AS_DECL_FULL
    Ref_CompressedVector &Ref_CVector::GetProp_compressed()
    {
        compressed = CompressVector(*(CVector*)pValue);
        return *(new Ref_CompressedVector(compressed));
    }
    #else
    Ref_CompressedVector &GetProp_compressed();
    CompressedVector compressed;
    #endif
AS_DECL_END()

AS_REF_DECL_CONSTRUCT(CompressedVector, CompressedVector3D)
    AS_PROP_DECL(CompressedVector, int16_t, x, x)
    AS_PROP_DECL(CompressedVector, int16_t, y, y)
    AS_PROP_DECL(CompressedVector, int16_t, z, z)
    #ifdef AS_DECL_FULL
    Ref_CVector &Ref_CompressedVector::GetProp_normal()
    {
        normal = UncompressVector(*(CompressedVector*)pValue);
        return *(new Ref_CVector(normal));
    }
    #else
    CVector normal;
    Ref_CVector &GetProp_normal();
    #endif
AS_DECL_END()

AS_REF_DECL(CWeapon, WeaponData)
    
AS_DECL_END()

AS_REF_DECL(CPlayerInfo, PlayerInfo)
    AS_PROP_DECL(CPlayerInfo, int32_t, money, m_nMoney)
AS_DECL_END()

AS_REF_DECL(CPedFlags, PedFlags)
    
AS_DECL_END()

AS_REF_DECL(CPlaceable, Placeable)
    AS_PROP_DECLREF(CPlaceable, CVector, pos, GetPosition())
AS_DECL_END()

AS_REF_DECL_FROM(CEntity, Entity, CPlaceable)
    AS_PROP_DECL(CEntity, uint16_t, modelindex, m_nModelIndex)
    AS_PROP_DECL(CEntity, uint8_t, interior, m_nInterior)
    AS_PROP_DECL(CEntity, uint32_t, entflags, m_nFlags)
    AS_PROP_DECL_NOREFBIND(CEntity, eEntityType, enttype, m_nType)
    AS_PROP_DECL_NOREFBIND(CEntity, eEntityStatus, entstatus, m_nStatus)
AS_DECL_END()

AS_REF_DECL_FROM(CPhysical, Physical, CEntity)
    AS_PROP_DECL(CPhysical, uint32_t, phyflags, m_nPhysicalFlags)
    AS_PROP_DECL(CPhysical, float, mass, m_fMass)
    AS_PROP_DECL(CPhysical, uint8_t, surfaceid, m_nContactSurface)
    AS_PROP_DECL(CPhysical, float, speed, m_fMovingSpeed)
    AS_PROP_DECLREF(CPhysical, CVector, movevec, m_vecMoveSpeed) //
    AS_PROP_DECLREF(CPhysical, CVector, turnvec, m_vecTurnSpeed)
    AS_PROP_DECLREF(CPhysical, CVector, forcevec, m_vecForce)
    AS_PROP_DECLREFPTR(CPhysical, CPhysical, attached, m_pAttachedTo)
AS_DECL_END()

AS_REF_DECL_FROM(CObject, Object, CPhysical)
    AS_PROP_DECL(CObject, eObjectType, objtype, m_nObjectType)
    AS_PROP_DECL(CObject, uint32_t, objflags, m_nObjectFlags)
    AS_PROP_DECL(CObject, float, health, m_fHealth)
    AS_PROP_DECL(CObject, float, scale, m_fScale)
AS_DECL_END()

AS_REF_DECL_FROM(CPed, Ped, CPhysical)
    AS_PROP_DECL(CPed, float, armour, m_fArmour)
    AS_PROP_DECL(CPed, float, health, m_fHealth)
    AS_PROP_DECL(CPed, float, maxhealth, m_fMaxHealth)
    AS_PROP_DECL(CPed, ePedState, state, m_nPedState)
    AS_PROP_DECL(CPed, eMoveState, movestate, m_nMoveState)
    AS_PROP_DECLREF(CPed, CPedFlags, pedflags, m_PedFlags)
    AS_PROP_DECLREFPTR(CPed, CVehicle, vehicle, m_pVehicle)
    AS_PROP_DECL(CPed, float, rotation1, m_fRotation1)
    AS_PROP_DECL(CPed, float, rotation2, m_fRotation2)
    AS_PROP_DECL(CPed, eFightingStyle, fightstyle, m_byteFightingStyle)
    AS_PROP_DECL(CPed, float, lookingdir, m_fLookingDirection)
AS_DECL_END()

AS_REF_DECL_FROM(CPlayerPed, Player, CPed)
    AS_PROP_DECL(CPlayerPed, uint8_t, weaponid, m_pPlayerData->m_nChosenWeapon)
    AS_PROP_DECL(CPlayerPed, uint32_t, flags, m_pPlayerData->m_nPlayerFlags)
    AS_PROP_DECL(CPlayerPed, uint8_t, drunk, m_pPlayerData->m_nDrunkenness)
    AS_PROP_DECL(CPlayerPed, float, oxygen, m_pPlayerData->m_fBreath)
    AS_PROP_DECL(CPlayerPed, bool, nosprint, m_pPlayerData->m_bPlayerSprintDisabled)
    AS_PROP_DECL(CPlayerPed, bool, noweaponswitch, m_pPlayerData->m_bDontAllowWeaponChange)
    AS_PROP_DECL(CPlayerPed, float, waterheight, m_pPlayerData->m_fWaterHeight)
    
    #ifdef AS_DECL_FULL
    int Ref_CPlayerPed::GetProp_id()
    {
        for(int i = 0; i < 1250; ++i) // A support for patched values, by samp for example
        {
            if(FindPlayerPed(i) == pValue) return i;
        }
        return -1;
    }
    #else
    int GetProp_id();
    #endif
    
    #ifdef AS_DECL_FULL
    Ref_CPlayerInfo* Ref_CPlayerPed::GetProp_info()
    {
        return new Ref_CPlayerInfo(&WorldPlayers[GetProp_id()]);
    }
    #else
    Ref_CPlayerInfo* GetProp_info();
    #endif
AS_DECL_END()

AS_REF_DECL_FROM(CVehicle, Vehicle, CPhysical)
    AS_PROP_DECL(CVehicle, uint64_t, vehflags, m_nVehicleFlags)
    AS_PROP_DECL(CVehicle, float, health, m_fHealth)
AS_DECL_END()

AS_REF_DECL(CPickup, Pickup)
    AS_PROP_DECLREFPTR(CPickup, CObject, obj, m_pObject)
    AS_PROP_DECL(CPickup, uint32_t, ammo, m_nAmmo)
    AS_PROP_DECL(CPickup, uint16_t, dailymoney, m_nMoneyPerDay)
    AS_PROP_DECL(CPickup, int16_t, modelindex, m_nModelIndex)
    AS_PROP_DECL(CPickup, ePickupType, type, m_nPickupType)
    AS_PROP_DECL_NOREFBIND(CPickup, uint8_t, disabled, m_nFlags.bDisabled)
    AS_PROP_DECL_NOREFBIND(CPickup, uint8_t, visible, m_nFlags.bVisible)
    AS_PROP_DECLREF(CPickup, CompressedVector, pos, m_vecPos)
AS_DECL_END()
