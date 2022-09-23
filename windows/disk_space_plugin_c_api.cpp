#include "include/disk_space/disk_space_plugin.h"
#include "include/disk_space/disk_space_plugin_c_api.h"

#include <flutter/plugin_registrar_windows.h>

#include "disk_space_plugin.h"

void DiskSpacePluginRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  disk_space::DiskSpacePlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}

void DiskSpacePluginCApiRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  disk_space::DiskSpacePlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
