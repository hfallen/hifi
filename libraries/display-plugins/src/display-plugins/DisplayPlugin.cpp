//
//  Created by Bradley Austin Davis on 2015/05/29
//  Copyright 2015 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#include "DisplayPlugin.h"

#include <plugins/PluginManager.h>

#include "NullDisplayPlugin.h"
#include "stereo/SideBySideStereoDisplayPlugin.h"
#include "stereo/InterleavedStereoDisplayPlugin.h"
#include "hmd/DebugHmdDisplayPlugin.h"
#include "Basic2DWindowOpenGLDisplayPlugin.h"
#include "hmd/DaydreamDisplayPlugin.h"
#include "stereo/StereoDisplayPlugin.h"
#include "hmd/DebugHmdDisplayPlugin.h"


const QString& DisplayPlugin::MENU_PATH() {
    static const QString value = "Display";
    return value;
}
#if defined(ANDROID)
gvr_context* __gvr_context;

LibInstance::LibInstance(){
    static std::once_flag once;
    std::call_once(once, [&] {
        qDebug() << __FILE__ << "has been initialized";
        DisplayPlugin* PLUGIN_POOL[] = {
            new DaydreamDisplayPlugin(),
//            new Basic2DWindowOpenGLDisplayPlugin(),
//            new SideBySideStereoDisplayPlugin(),
//            new DebugHmdDisplayPlugin(),
            nullptr
        };
        PluginManager::getInstance()->loadDisplayPlugins(PLUGIN_POOL);
    });
  }

extern "C" {

JNIEXPORT void Java_io_highfidelity_hifiinterface_InterfaceActivity_nativeOnCreate(JNIEnv* env, jobject obj, jobject asset_mgr, jlong gvr_context_ptr) {
    //qDebug() << "nativeOnCreate" << gvr_context_ptr << " On thread " << QThread::currentThreadId();
    __gvr_context = reinterpret_cast<gvr_context*>(gvr_context_ptr);
}

}

#endif

// TODO migrate to a DLL model where plugins are discovered and loaded at runtime by the PluginManager class
DisplayPluginList getDisplayPlugins() {
    DisplayPlugin* PLUGIN_POOL[] = {
        new Basic2DWindowOpenGLDisplayPlugin(),
        new DebugHmdDisplayPlugin(),
#ifdef DEBUG
        new NullDisplayPlugin(),
#endif
        // Stereo modes
        // SBS left/right
        new SideBySideStereoDisplayPlugin(),
        // Interleaved left/right
        new InterleavedStereoDisplayPlugin(),
        nullptr
    };

    DisplayPluginList result;
    for (int i = 0; PLUGIN_POOL[i]; ++i) {
        DisplayPlugin * plugin = PLUGIN_POOL[i];
        if (plugin->isSupported()) {
            plugin->init();
            result.push_back(DisplayPluginPointer(plugin));
        }
    }
    return result;
}
