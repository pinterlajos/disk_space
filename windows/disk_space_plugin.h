#ifndef FLUTTER_PLUGIN_DISK_SPACE_PLUGIN_H_
#define FLUTTER_PLUGIN_DISK_SPACE_PLUGIN_H_

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>

#include <memory>

namespace disk_space {

class DiskSpacePlugin : public flutter::Plugin {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

  DiskSpacePlugin();

  virtual ~DiskSpacePlugin();

  // Disallow copy and assign.
  DiskSpacePlugin(const DiskSpacePlugin&) = delete;
  DiskSpacePlugin& operator=(const DiskSpacePlugin&) = delete;

 private:
  // Called when a method is called on this plugin's channel from Dart.
  void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
};

}  // namespace disk_space

#endif  // FLUTTER_PLUGIN_DISK_SPACE_PLUGIN_H_
