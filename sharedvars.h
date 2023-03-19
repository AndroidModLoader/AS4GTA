#ifndef __SHARED
#define __SHARED

#include "scripting.h"
#include "GTASA_STRUCTS.h"

#define MAX_BUILDINGS_TO_REMOVE 1024

struct BuildingToRemove
{
    int modelId;
    CVector pos;
    float radius;
};
extern int g_nRemovedBuildings;
extern BuildingToRemove* g_RemoveBuilding[MAX_BUILDINGS_TO_REMOVE];

extern uintptr_t addr;
extern GAME_SCRIPT_THREAD* gst;
extern char ScriptBuf[0xFF]; 
extern uintptr_t *pdwParamVars[18];
extern void (*ProcessOneCommand)(GAME_SCRIPT_THREAD*);
extern CPlayerPed* (*FindPlayerPed)(int);
extern CPlayerInfo* WorldPlayers;

inline float Twice(float f)
{
    return f * f;
}
inline float DistanceBetweenPoints(CVector2D& a1, CVector2D& a2)
{
    return sqrtf(Twice(a1.x - a2.x) + Twice(a1.y - a2.y));
}
inline float DistanceBetweenPoints(float x1, float y1, CVector2D& a2)
{
    return sqrtf(Twice(x1 - a2.x) + Twice(y1 - a2.y));
}
inline float DistanceBetweenPoints(CVector2D& a1, float x2, float y2)
{
    return sqrtf(Twice(a1.x - x2) + Twice(a1.y - y2));
}
inline float DistanceBetweenPoints(float x1, float y1, float x2, float y2)
{
    return sqrtf(Twice(x1 - x2) + Twice(y1 - y2));
}
inline float DistanceBetweenPoints(CVector& a1, CVector& a2)
{
    return sqrtf(Twice(a1.x - a2.x) + Twice(a1.y - a2.y) + Twice(a1.z - a2.z));
}
inline float DistanceBetweenPoints(float x1, float y1, float z1, CVector& a2)
{
    return sqrtf(Twice(x1 - a2.x) + Twice(y1 - a2.y) + Twice(z1 - a2.z));
}
inline float DistanceBetweenPoints(CVector& a1, float x2, float y2, float z2)
{
    return sqrtf(Twice(a1.x - x2) + Twice(a1.y - y2) + Twice(a1.z - z2));
}
inline float DistanceBetweenPoints(float x1, float y1, float z1, float x2, float y2, float z2)
{
    return sqrtf(Twice(x1 - x2) + Twice(y1 - y2) + Twice(z1 - z2));
}

#endif // __SHARED
