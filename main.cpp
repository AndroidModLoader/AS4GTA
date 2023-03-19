#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/config.h>
MYMOD(net.rusjj.as4gta, AngelScript for GTA, 0.1, RusJJ)


#include <angelscript.h>
#include <string>
#include <ias4aml.h>
#include <dlfcn.h>
#include <vector>

#include "sharedvars.h"

uintptr_t addr;
GAME_SCRIPT_THREAD* gst;
char ScriptBuf[0xFF]; 
uintptr_t *pdwParamVars[18];
void (*ProcessOneCommand)(GAME_SCRIPT_THREAD*);
CPlayerPed* (*FindPlayerPed)(int);
CPlayerInfo* WorldPlayers;

CPool<CBuilding>* pBuildingPool;
CPool<CDummy>* pDummyPool;
int g_nRemovedBuildings = 0;
BuildingToRemove* g_RemoveBuilding[MAX_BUILDINGS_TO_REMOVE] {0};

enum eGame : unsigned char
{
    GameUnknown = 0,
    GameVC,
    GameSA,
} gLoadedGame;



#include "decl.h"
#define AS_DECL_FULL
#include "decl.h"



inline uint8_t ExecuteScriptBuf()
{
    gst->dwScriptIP = (uintptr_t)ScriptBuf;
    ProcessOneCommand(gst);
    return gst->condResult;
}

void ScriptCommandAS4AML(asIScriptGeneric *gen)
{
    const char* p = ((std::string*)gen->GetArgAddress(1))->c_str();
    memcpy(&ScriptBuf, gen->GetArgAddress(0), 2);
    int buf_pos = 2, argp = 1;
    uint16_t var_pos = 0;

    for(int i = 0; i < 18; ++i) gst->dwLocalVar[i] = 0;

    while(*p)
    {
        switch(*p)
        {
            case 'i':
            case 'd':
            {
                int i = *(int*)gen->GetArgAddress(++argp);
                ScriptBuf[buf_pos] = 0x01;
                memcpy(&ScriptBuf[++buf_pos], &i, 4);
                buf_pos += 4;
                break;
            }
            case 'b': // Handle booleans here
            {
                int i = *(int*)gen->GetArgAddress(++argp)!=0 ? 1 : 0;
                ScriptBuf[buf_pos] = 0x01;
                memcpy(&ScriptBuf[++buf_pos], &i, 4);
                buf_pos += 4;
                break;
            }
            case 'v':
            case 'p':
            {
                Ref_ScriptHandle *v = (Ref_ScriptHandle*)gen->GetArgAddress(++argp);
                ScriptBuf[buf_pos] = 0x03;
                pdwParamVars[var_pos] = &((ScriptHandle*)(v->pValue))->m_nHandleIndex;
                gst->dwLocalVar[var_pos] = ((ScriptHandle*)(v->pValue))->m_nHandleIndex;
                memcpy(&ScriptBuf[++buf_pos], &var_pos, 2);
                buf_pos += 2;
                ++var_pos;
                break;
            }
            case 'f':
            {
                float f = *(float*)gen->GetArgAddress(++argp);
                ScriptBuf[buf_pos] = 0x06;
                memcpy(&ScriptBuf[++buf_pos], &f, 4);
                buf_pos += 4;
                break;
            }
            case 's':
            {
                const char* sz = ((std::string*)gen->GetArgAddress(++argp))->c_str();
                unsigned char aLen = strlen(sz);
                ScriptBuf[buf_pos] = 0x0E;
                ScriptBuf[++buf_pos] = aLen;
                memcpy(&ScriptBuf[++buf_pos], sz, aLen);
                buf_pos += aLen;
                break;
            }
            case 'z':
            {
                ScriptBuf[buf_pos] = 0x00;
                ++buf_pos;
                break;
            }
            default:
            {
                break;
            }
        }
        ++p;
    }

    int result = ExecuteScriptBuf();
    if (var_pos)
    {
        for (int i = 0; i < var_pos; ++i) *pdwParamVars[i] = gst->dwLocalVar[i];
    }
    gen->SetReturnDWord(result);
    //return result;
}

void RemoveBuilding(int modelId, float x, float y, float z, float radius)
{
    BuildingToRemove* b = new BuildingToRemove;
    b->modelId = modelId;
    b->pos.x = x;
    b->pos.y = y;
    b->pos.z = z;
    b->radius = radius;
    g_RemoveBuilding[g_nRemovedBuildings] = b;
    ++g_nRemovedBuildings;

    CEntity* foundEnt;
    tPoolObjectFlags* buildingFlags = pBuildingPool->m_byteMap;
    tPoolObjectFlags* dummyFlags = pDummyPool->m_byteMap;
    int i, buildsC = pBuildingPool->m_nSize, dummiesC = pDummyPool->m_nSize;
    if(modelId == -1)
    {
        for(i = 0; i < buildsC; ++i)
        {
            if(buildingFlags[i].bEmpty) continue;
            foundEnt = &pBuildingPool->m_pObjects[i];
            if(foundEnt->GetPosition().z > -500 && DistanceBetweenPoints(x, y, z, foundEnt->GetPosition()) <= radius)
            {
                foundEnt->GetPosition().z = -3500;
            }
        }
        for(i = 0; i < dummiesC; ++i)
        {
            if(dummyFlags[i].bEmpty) continue;
            foundEnt = &pDummyPool->m_pObjects[i];
            if(foundEnt->GetPosition().z > -500 && DistanceBetweenPoints(x, y, z, foundEnt->GetPosition()) <= radius)
            {
                foundEnt->GetPosition().z = -3500;
            }
        }
    }
    else
    {
        for(i = 0; i < buildsC; ++i)
        {
            if(buildingFlags[i].bEmpty) continue;
            foundEnt = &pBuildingPool->m_pObjects[i];
            if(foundEnt->m_nModelIndex == modelId && foundEnt->GetPosition().z > -500 && DistanceBetweenPoints(x, y, z, foundEnt->GetPosition()) <= radius)
            {
                foundEnt->GetPosition().z = -3500;
            }
        }
        for(i = 0; i < dummiesC; ++i)
        {
            if(dummyFlags[i].bEmpty) continue;
            foundEnt = &pDummyPool->m_pObjects[i];
            if(foundEnt->m_nModelIndex == modelId && foundEnt->GetPosition().z > -500 && DistanceBetweenPoints(x, y, z, foundEnt->GetPosition()) <= radius)
            {
                foundEnt->GetPosition().z = -3500;
            }
        }
    }
}

bool IsBuildingRemoved(int modelId, float x, float y, float z)
{
    if(g_nRemovedBuildings > 0)
    {
        int i;
        BuildingToRemove* b;
        for(i = 0; i < g_nRemovedBuildings; ++i)
        {
            b = g_RemoveBuilding[i];
            if((b->modelId == -1 || b->modelId == modelId) && DistanceBetweenPoints(x, y, z, b->pos) <= b->radius)
            {
                return true;
            }
        }
    }
    return false;
}

asIScriptEngine *engine;
IAS4AML* as4aml;
void *lib;
asIScriptContext *ctxCB;

std::vector<asIScriptFunction*> g_vecFunc_OnScreenTouch;
std::vector<asIScriptFunction*> g_vecFunc_PlayerEnteredVehicle;
std::vector<asIScriptFunction*> g_vecFunc_PickupPickedUp;
std::vector<asIScriptFunction*> g_vecFunc_PickupCreated;
std::vector<asIScriptFunction*> g_vecFunc_LocalPlayerUpdated;
DECL_HOOKv(AND_TouchEvent, int type, int finger, int x, int y)
{
    int count = g_vecFunc_OnScreenTouch.size();
    for(int i = 0; i < count; ++i)
    {
        ctxCB->Prepare(g_vecFunc_OnScreenTouch[i]);
        ctxCB->SetArgDWord(0, type);
        ctxCB->SetArgDWord(1, finger);
        ctxCB->SetArgDWord(2, x);
        ctxCB->SetArgDWord(3, y);
        int r = ctxCB->Execute();
        if(r == asEXECUTION_EXCEPTION) logger->Error("An exception occurred:\n%s", ctxCB->GetExceptionString());
        ctxCB->Unprepare();
    }
    AND_TouchEvent(type, finger, x, y);
}

DECL_HOOKv(GameLogicUpdate, void* self)
{
    GameLogicUpdate(self);
    as4aml->Scripts_Update();
}

bool bStarted = false;
DECL_HOOKv(GameLogicStart, void* self)
{
    GameLogicStart(self);
    bStarted = true;
    as4aml->Scripts_GameStart();
}

bool bPaused = false;
DECL_HOOKv(StartTimerPause, void* self)
{
    if(bStarted)
    {
        bPaused = true;
        as4aml->Scripts_GamePause();
    }
    StartTimerPause(self);
}

DECL_HOOKv(StopTimerPause, void* self)
{
    StopTimerPause(self);
    if(bStarted && bPaused)
    {
        as4aml->Scripts_GameResume();
        bPaused = false;
    }
}

DECL_HOOK(bool, TaskGetInCarProcess, CTaskSimpleCarGetIn* self, CPed* ped)
{
    if(self->m_bIsFinished && ped->IsPlayer())
    {
        asIScriptContext *ctx = engine->CreateContext();
        int count = g_vecFunc_PlayerEnteredVehicle.size();
        if(count) for(int i = 0; i < count; ++i)
        {
            ctx->Prepare(g_vecFunc_PlayerEnteredVehicle[i]);
            ctx->SetArgObject(0, new Ref_CPlayerPed((CPlayerPed*)ped));
            ctx->SetArgObject(1, new Ref_CVehicle(self->m_pTargetVehicle));
            int r = ctx->Execute();
            if(r == asEXECUTION_EXCEPTION) logger->Error("An exception occurred:\n%s", ctx->GetExceptionString());
            ctx->Unprepare();
        }
        ctx->Release();
    }
    return TaskGetInCarProcess(self, ped);
}

DECL_HOOK(bool, PickupUpdate, CPickup* self, CPlayerPed* ped, CVehicle* veh, int playerId)
{
    bool picked = PickupUpdate(self, ped, veh, playerId);
    if(picked)
    {
        asIScriptContext *ctx;
        int count = g_vecFunc_PlayerEnteredVehicle.size();
        if(count) for(int i = 0; i < count; ++i)
        {
            ctx = engine->CreateContext();
            ctx->Prepare(g_vecFunc_PickupPickedUp[i]);
            ctx->SetArgObject(0, new Ref_CPickup(self));
            ctx->SetArgObject(1, new Ref_CPlayerPed(ped));
            ctx->SetArgObject(2, new Ref_CVehicle(veh));
            int r = ctx->Execute();
            if(r == asEXECUTION_EXCEPTION) logger->Error("An exception occurred:\n%s", ctx->GetExceptionString());
            ctx->Release();
        }
    }
    return picked;
}

CPickup* aPickUps;
DECL_HOOK(ScriptHandle, GenerateNewPickup, CVector coors, uint32_t modelId, ePickupType pickupType, uint32_t ammo, uint32_t moneyPerDay, bool isEmpty, char* message)
{
    ScriptHandle pkp = GenerateNewPickup(coors, modelId, pickupType, ammo, moneyPerDay, isEmpty, message);
    if(pkp.m_nHandleIndex != 0)
    {
        int count = g_vecFunc_PickupCreated.size();
        if(count) for(int i = 0; i < count; ++i)
        {
            ctxCB->Prepare(g_vecFunc_PickupCreated[i]);
            ctxCB->SetArgObject(0, new Ref_CPickup(&aPickUps[pkp.m_nId]));
            int r = ctxCB->Execute();
            if(r == asEXECUTION_EXCEPTION) logger->Error("An exception occurred:\n%s", ctxCB->GetExceptionString());
            ctxCB->Unprepare();
        }
    }
    return pkp;
}

DECL_HOOKv(InitPools)
{
    InitPools();
    SET_TO(pBuildingPool, *(void**)aml->GetSym(lib, "_ZN6CPools16ms_pBuildingPoolE"));
    SET_TO(pDummyPool, *(void**)aml->GetSym(lib, "_ZN6CPools13ms_pDummyPoolE"));
}

DECL_HOOK(CEntity*, LoadObjectInstance, CFileObjectInstance* obj, const char* mdlName)
{
    if(IsBuildingRemoved(obj->instanceIndex, obj->m_vecPos.x, obj->m_vecPos.y, obj->m_vecPos.z))
    {
        obj->m_vecPos.z = -3500; // Actually a working way to do that without using "empty-body" models
    }
    return LoadObjectInstance(obj, mdlName);
}

DECL_HOOKv(ProcessPlayerInfo, CPlayerInfo* self, int playaNum)
{
    ProcessPlayerInfo(self, playaNum);
    if(playaNum != 0) return;
    
    int count = g_vecFunc_LocalPlayerUpdated.size();
    for(int i = 0; i < count; ++i)
    {
        ctxCB->Prepare(g_vecFunc_LocalPlayerUpdated[i]);
        ctxCB->SetArgObject(0, new Ref_CPlayerPed(WorldPlayers[0].m_pPed));
        int r = ctxCB->Execute();
        if(r == asEXECUTION_EXCEPTION) logger->Error("An exception occurred:\n%s", ctxCB->GetExceptionString());
        ctxCB->Unprepare();
    }
}

class CPolyBunch;
struct CPtrList
{
    void* m_node;
};
DECL_HOOK(void, CastShadowSectorList, CPtrList* a1, float a2, float a3, float a4, float a5, CVector* a6, float a7, float a8, float a9, float a10, short a11, unsigned char a12, unsigned char a13, unsigned char a14, float a15, float a16, CPolyBunch** a17, CRealTimeShadow* a18, unsigned char* a19)
{
    if(a1 && a1->m_node)
    {
        uintptr_t vtable_off = ***(uintptr_t***)a1;
        if(*(uintptr_t*)(vtable_off + 0x28) != 0)
            CastShadowSectorList(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19);
    }
}


void OnNewASScript(AS_MODULE_TYPE* module)
{
    asIScriptFunction* f;
    f = module->GetFunctionByDecl("void OnScreenTouch(int,int,int,int)");
    if(f) g_vecFunc_OnScreenTouch.push_back(f);
    f = module->GetFunctionByName("PlayerEnteredVehicle");
    if(f) g_vecFunc_PlayerEnteredVehicle.push_back(f);
    f = module->GetFunctionByName("PickupPickedUp");
    if(f) g_vecFunc_PickupPickedUp.push_back(f);
    f = module->GetFunctionByName("PickupCreated");
    if(f) g_vecFunc_PickupCreated.push_back(f);
    f = module->GetFunctionByName("LocalPlayerUpdated");
    if(f) g_vecFunc_LocalPlayerUpdated.push_back(f);
}
    


extern "C" void OnModLoad()
{
    logger->SetTag("AngelScript GTA");
    
    engine = (asIScriptEngine*)GetInterface("AngelScript");
    as4aml = (IAS4AML*)GetInterface("AS4AML");
    if(!engine || !as4aml)
    {
        logger->Error("Failed to start AngelScript for GTA!");
        return;
    }
    as4aml->OnScriptRegisteredCallback(OnNewASScript);
    
    gLoadedGame = GameUnknown;
    lib = dlopen("libGTASA.so", RTLD_LAZY);
    if(lib)
    {
        addr = aml->GetLib("libGTASA.so");
        gLoadedGame = GameSA;
        logger->SetTag("AngelScript GTASA");
    }
    else
    {
        lib = dlopen("libGTAVC.so", RTLD_LAZY);
        if(lib)
        {
            addr = aml->GetLib("libGTAVC.so");
            gLoadedGame = GameVC;
            logger->SetTag("AngelScript GTAVC");
        }
    }
    ctxCB = engine->CreateContext();
    
    AS_REF_REG_CONSTRUCT(ScriptHandle);
    AS_REF_REG(CVector2D);
    AS_REF_REG(CVector);
    AS_REF_REG(CompressedVector);
    AS_REF_REG(CEntity);
    AS_REF_REG(CObject);
    AS_REF_REG(CPedFlags);
    AS_REF_REG(CPed);
    AS_REF_REG(CPlayerPed);
    AS_REF_REG(CVehicle);
    AS_REF_REG(CPlayerInfo);
    AS_REF_REG(CPickup);
    
    
    AS_PROP_REG(ScriptHandle, uint16, id);
    AS_PROP_REG(ScriptHandle, uint16, counter);
    AS_PROP_REG(ScriptHandle, int, asInt);
    AS_PROP_REG(ScriptHandle, uint, asUInt);
    AS_PROP_REG(ScriptHandle, float, asFloat);
    
    AS_PROP_REG(CVector2D, float, x);
    AS_PROP_REG(CVector2D, float, y);
    
    AS_PROP_REG(CVector, float, x);
    AS_PROP_REG(CVector, float, y);
    AS_PROP_REG(CVector, float, z);
    AS_PROP_REGGET(CVector, CompressedVector3D, compressed);
    
    AS_PROP_REG(CompressedVector, int16, x);
    AS_PROP_REG(CompressedVector, int16, y);
    AS_PROP_REG(CompressedVector, int16, z);
    AS_PROP_REGGET(CompressedVector, Vector3D, normal);
    
    // Entity
    AS_PROP_REGGET(CEntity, Vector3D, pos); // CEntity
    AS_PROP_REG(CEntity, uint16, modelindex);
    AS_PROP_REG(CEntity, uint8, interior);
    AS_PROP_REG(CEntity, uint, entflags);
    AS_PROP_REGGET_NOREFBIND(CEntity, uint8, enttype);
    AS_PROP_REGGET_NOREFBIND(CEntity, uint8, entstatus);
    AS_PROP_REGSET(CEntity, uint8, enttype);
    AS_PROP_REGSET(CEntity, uint8, entstatus); // end CEntity
    
    // Ped
    AS_PROP_REGGET(CPed, Vector3D, pos); // CEntity
    AS_PROP_REG(CPed, uint16, modelindex);
    AS_PROP_REG(CPed, uint8, interior);
    AS_PROP_REG(CPed, uint, entflags);
    AS_PROP_REGGET_NOREFBIND(CPed, uint8, enttype);
    AS_PROP_REGGET_NOREFBIND(CPed, uint8, entstatus);
    AS_PROP_REGSET(CPed, uint8, enttype);
    AS_PROP_REGSET(CPed, uint8, entstatus); // end CEntity
    AS_PROP_REG(CPed, uint, phyflags); // CPhysical
    AS_PROP_REG(CPed, float, mass);
    AS_PROP_REG(CPed, uint8, surfaceid);
    AS_PROP_REG(CPed, float, speed); // end CPhysical
    AS_PROP_REG(CPed, float, armour); // CPed
    AS_PROP_REG(CPed, float, health);
    AS_PROP_REG(CPed, float, maxhealth);
    AS_PROP_REG(CPed, int, state);
    AS_PROP_REG(CPed, int, movestate);
    AS_PROP_REGGET(CPed, PedFlags, pedflags);
    AS_PROP_REGGET(CPed, Vehicle, vehicle);
    AS_PROP_REG(CPed, float, rotation1);
    AS_PROP_REG(CPed, float, rotation2);
    AS_PROP_REG(CPed, int8, fightstyle);
    AS_PROP_REG(CPed, float, lookingdir); // end CPed
    
    // Player
    AS_PROP_REGGET(CPlayerPed, Vector3D, pos); // CEntity
    AS_PROP_REG(CPlayerPed, uint16, modelindex);
    AS_PROP_REG(CPlayerPed, uint8, interior);
    AS_PROP_REG(CPlayerPed, uint, entflags);
    AS_PROP_REGGET_NOREFBIND(CPlayerPed, uint8, enttype);
    AS_PROP_REGGET_NOREFBIND(CPlayerPed, uint8, entstatus);
    AS_PROP_REGSET(CPlayerPed, uint8, enttype);
    AS_PROP_REGSET(CPlayerPed, uint8, entstatus); // end CEntity
    AS_PROP_REG(CPlayerPed, uint, phyflags); // CPhysical
    AS_PROP_REG(CPlayerPed, float, mass);
    AS_PROP_REG(CPlayerPed, uint8, surfaceid);
    AS_PROP_REG(CPlayerPed, float, speed); // end CPhysical
    AS_PROP_REG(CPlayerPed, float, armour); // CPed
    AS_PROP_REG(CPlayerPed, float, health);
    AS_PROP_REG(CPlayerPed, float, maxhealth);
    AS_PROP_REG(CPlayerPed, int, state);
    AS_PROP_REG(CPlayerPed, int, movestate);
    AS_PROP_REGGET(CPlayerPed, PedFlags, pedflags);
    AS_PROP_REGGET(CPlayerPed, Vehicle, vehicle);
    AS_PROP_REG(CPlayerPed, float, rotation1);
    AS_PROP_REG(CPlayerPed, float, rotation2);
    AS_PROP_REG(CPlayerPed, int8, fightstyle);
    AS_PROP_REG(CPlayerPed, float, lookingdir); // end CPed
    AS_PROP_REG(CPlayerPed, uint8, weaponid); // CPlayerPed
    AS_PROP_REG(CPlayerPed, uint, flags);
    AS_PROP_REG(CPlayerPed, uint8, drunk);
    AS_PROP_REG(CPlayerPed, float, oxygen);
    AS_PROP_REG(CPlayerPed, bool, nosprint);
    AS_PROP_REG(CPlayerPed, bool, noweaponswitch);
    AS_PROP_REG(CPlayerPed, float, waterheight);
    AS_PROP_REGGET_NOREFBIND(CPlayerPed, int, id);
    AS_PROP_REGGET(CPlayerPed, PlayerInfo, info); // end CPlayerPed
    
    // Vehicle
    AS_PROP_REGGET(CVehicle, Vector3D, pos); // CEntity
    AS_PROP_REG(CVehicle, uint16, modelindex);
    AS_PROP_REG(CVehicle, uint8, interior);
    AS_PROP_REG(CVehicle, uint, entflags);
    AS_PROP_REGGET_NOREFBIND(CVehicle, uint8, enttype);
    AS_PROP_REGGET_NOREFBIND(CVehicle, uint8, entstatus);
    AS_PROP_REGSET(CVehicle, uint8, enttype);
    AS_PROP_REGSET(CVehicle, uint8, entstatus); // end CEntity
    AS_PROP_REG(CVehicle, uint, phyflags); // CPhysical
    AS_PROP_REG(CVehicle, float, mass);
    AS_PROP_REG(CVehicle, uint8, surfaceid);
    AS_PROP_REG(CVehicle, float, speed); // end CPhysical
    AS_PROP_REG(CVehicle, float, health);
    
    // PlayerInfo
    AS_PROP_REG(CPlayerInfo, int, money);
    
    // Object
    AS_PROP_REGGET(CObject, Vector3D, pos); // CEntity
    AS_PROP_REG(CObject, uint16, modelindex);
    AS_PROP_REG(CObject, uint8, interior);
    AS_PROP_REG(CObject, uint, entflags);
    AS_PROP_REGGET_NOREFBIND(CObject, uint8, enttype);
    AS_PROP_REGGET_NOREFBIND(CObject, uint8, entstatus);
    AS_PROP_REGSET(CObject, uint8, enttype);
    AS_PROP_REGSET(CObject, uint8, entstatus); // end CEntity
    AS_PROP_REG(CObject, uint, phyflags); // CPhysical
    AS_PROP_REG(CObject, float, mass);
    AS_PROP_REG(CObject, uint8, surfaceid);
    AS_PROP_REG(CObject, float, speed); // end CPhysical
    AS_PROP_REG(CObject, int, objtype); // CObject
    AS_PROP_REG(CObject, uint, objflags);
    AS_PROP_REG(CObject, float, health);
    AS_PROP_REG(CObject, float, scale); // end CObject
    
    // Pickup
    AS_PROP_REGGET(CPickup, Object, obj);
    AS_PROP_REG(CPickup, uint, ammo);
    AS_PROP_REG(CPickup, uint16, dailymoney);
    AS_PROP_REG(CPickup, int16, modelindex);
    AS_PROP_REG(CPickup, uint8, type);
    AS_PROP_REG(CPickup, uint8, disabled);
    AS_PROP_REG(CPickup, uint8, visible);
    AS_PROP_REGGET(CPickup, CompressedVector3D, pos);
    
    
    engine->RegisterGlobalFunction("int GTA_CallOpcode(const uint &in, ?&in var1 = 0, ?&in var2 = 0, ?&in var3 = 0, ?&in var4 = 0, ?&in var5 = 0, ?&in var6 = 0, ?&in var7 = 0, ?&in var8 = 0, ?&in var9 = 0, ?&in var10 = 0, ?&in var11 = 0, ?&in var12 = 0, ?&in var13 = 0, ?&in var14 = 0, ?&in var15 = 0)", asFUNCTION(ScriptCommandAS4AML), asCALL_GENERIC);
    engine->RegisterGlobalFunction("void GTA_RemoveBuilding(int, float, float, float, float = 0.25)", asFUNCTION(RemoveBuilding), asCALL_CDECL);
    
    if(gLoadedGame == GameSA) HOOKPLT(AND_TouchEvent, addr + 0x675DE4);
    //else HOOK(AND_TouchEvent, aml->GetSym(lib, "_Z14AND_TouchEventiiii"));
    HOOK(GameLogicUpdate, aml->GetSym(lib, "_ZN10CGameLogic6UpdateEv"));
    HOOK(GameLogicStart, aml->GetSym(lib, "_ZN10CGameLogic17InitAtStartOfGameEv"));
    HOOK(StartTimerPause, aml->GetSym(lib, "_ZN6CTimer14StartUserPauseEv"));
    HOOK(StopTimerPause, aml->GetSym(lib, "_ZN6CTimer12EndUserPauseEv"));
    HOOK(TaskGetInCarProcess, aml->GetSym(lib, "_ZN19CTaskSimpleCarGetIn10ProcessPedEP4CPed"));
    HOOK(PickupUpdate, aml->GetSym(lib, "_ZN7CPickup6UpdateEP10CPlayerPedP8CVehiclei"));
    HOOK(GenerateNewPickup, aml->GetSym(lib, "_ZN8CPickups14GenerateNewOneE7CVectorjhjjbPc"));
    HOOK(ProcessPlayerInfo, aml->GetSym(lib, "_ZN11CPlayerInfo7ProcessEi"));
    HOOKPLT(InitPools, addr + 0x672468);
    SET_TO(aPickUps, aml->GetSym(lib, "_ZN8CPickups8aPickUpsE"));
    
    gst = new GAME_SCRIPT_THREAD{0};
    SET_TO(ProcessOneCommand, aml->GetSym(lib, "_ZN14CRunningScript17ProcessOneCommandEv"));
    SET_TO(FindPlayerPed, aml->GetSym(lib, "_Z13FindPlayerPedi"));
    if(gLoadedGame == GameSA)
    {
        SET_TO(WorldPlayers,               *(uintptr_t*)(addr + 0x6783C8));
        HOOKPLT(LoadObjectInstance,        addr + 0x675E6C);
        //HOOKPLT(CastShadowSectorList,      addr + 0x66E510);
    }
}
