#include <GarrysMod/Lua/Interface.h>
#include <vphysics_interface.h>
#include "../include/vphysics_interfaceV32.h"

#include "PhysObj.h"
#include "PhysCollision.h"
#include "PhysConstraint.h"
#include "PhysEnv.h"
#include "PhysSoftBody.h"
#include "PhysVehicle.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace GarrysMod::Lua;

#define LINIT_CHECKRET(fn, state) { if ((fn)(state) != 0) { Warning("lua vphysics init: %s failed\n", #fn); return 1; } }

IPhysics32 *g_pPhysics = NULL;
IPhysicsSurfaceProps *g_pSurfProps = NULL;

int lPhysStats(lua_State *state) {
	for (int i = 0; i < g_pPhysics->GetActiveEnvironmentCount(); i++) {
		IPhysicsEnvironment *pEnv = g_pPhysics->GetActiveEnvironmentByIndex(i);
		if (!pEnv)
			break;

		Msg("Environment %d active\n", i);
		Msg("\t%d active objects\n", pEnv->GetActiveObjectCount());
	}

	return 0;
}

GMOD_MODULE_OPEN() {
	CreateInterfaceFn physFactory = Sys_GetFactory("vphysics");
	if (physFactory) {
		g_pPhysics = (IPhysics32 *)physFactory("VPhysics032", NULL); // This specific interface is defined in the newer vphysics
		g_pSurfProps = (IPhysicsSurfaceProps *)physFactory(VPHYSICS_SURFACEPROPS_INTERFACE_VERSION, NULL);
	}

	// Check if the interface was created (if it wasn't, we're not running the newer vphysics)
	if (!g_pPhysics) {
		Warning("VPhysics lua init failed (vphysics module is too old!)");
		return 1;
	}

#ifdef _DEBUG
	if (g_pPhysics) {
		Msg("Found physics interface!\n");
	}
#endif

	// Let's setup our table of functions
	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
		LUA->CreateTable();
			LUA->PushCFunction(lPhysStats); LUA->SetField(-2, "printstats");
		LUA->SetField(-2, "vphysics");
	LUA->Pop();

	LINIT_CHECKRET(Init_PhysObj, state);
	LINIT_CHECKRET(Init_PhysCollision, state);
	LINIT_CHECKRET(Init_PhysConstraint, state);
	LINIT_CHECKRET(Init_PhysEnv, state);
	LINIT_CHECKRET(Init_PhysSoftBody, state);
	LINIT_CHECKRET(Init_PhysVehicle, state);

	return 0;
}

GMOD_MODULE_CLOSE() {
	return 0;
}