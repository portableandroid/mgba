/* Copyright (c) 2013-2016 Jeffrey Pfau
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#include "engine.h"

#include <mgba/core/scripting.h>
#include <mgba/debugger/debugger.h>
#include <mgba-util/string.h>
#include <mgba-util/vfs.h>

#include "lib.h"

static const char* mPythonScriptEngineName(struct mScriptEngine*);
static bool mPythonScriptEngineInit(struct mScriptEngine*, struct mScriptBridge*);
static void mPythonScriptEngineDeinit(struct mScriptEngine*);
static bool mPythonScriptEngineIsScript(struct mScriptEngine*, const char* name, struct VFile* vf);
static bool mPythonScriptEngineLoadScript(struct mScriptEngine*, const char* name, struct VFile* vf);
static void mPythonScriptEngineRun(struct mScriptEngine*);

struct mPythonScriptEngine {
	struct mScriptEngine d;
	struct mScriptBridge* sb;
};

struct mPythonScriptEngine* mPythonCreateScriptEngine(void) {
	struct mPythonScriptEngine* engine = malloc(sizeof(*engine));
	engine->d.name = mPythonScriptEngineName;
	engine->d.init = mPythonScriptEngineInit;
	engine->d.deinit = mPythonScriptEngineDeinit;
	engine->d.isScript = mPythonScriptEngineIsScript;
	engine->d.loadScript = mPythonScriptEngineLoadScript;
	engine->d.run = mPythonScriptEngineRun;
	engine->sb = NULL;
	return engine;
}

void mPythonSetup(struct mScriptBridge* sb) {
	struct mPythonScriptEngine* se = mPythonCreateScriptEngine();
	mScriptBridgeInstallEngine(sb, &se->d);
}

const char* mPythonScriptEngineName(struct mScriptEngine* se) {
	UNUSED(se);
	return "python";
}

bool mPythonScriptEngineInit(struct mScriptEngine* se, struct mScriptBridge* sb) {
	struct mPythonScriptEngine* engine = (struct mPythonScriptEngine*) se;
	engine->sb = sb;
	return true;
}

void mPythonScriptEngineDeinit(struct mScriptEngine* se) {
	struct mPythonScriptEngine* engine = (struct mPythonScriptEngine*) se;
	free(se);
}

bool mPythonScriptEngineIsScript(struct mScriptEngine* se, const char* name, struct VFile* vf) {
	UNUSED(se);
	UNUSED(vf);
	return endswith(name, ".py");
}

bool mPythonScriptEngineLoadScript(struct mScriptEngine* se, const char* name, struct VFile* vf) {
	struct mPythonScriptEngine* engine = (struct mPythonScriptEngine*) se;
	return mPythonLoadScript(name, vf);
}

void mPythonScriptEngineRun(struct mScriptEngine* se) {
	struct mPythonScriptEngine* engine = (struct mPythonScriptEngine*) se;

	struct mDebugger* debugger = mScriptBridgeGetDebugger(engine->sb);
	if (debugger) {
		mPythonSetDebugger(debugger);
	}

	mPythonRunPending();
}