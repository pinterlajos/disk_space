#include "disk_space_plugin.h"

// This must be included before many other Windows headers.
#include <windows.h>

// For getPlatformVersion; remove unless needed for your plugin implementation.
#include <VersionHelpers.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>
#include <flutter_windows.h>
#include <KnownFolders.h>
#include <shlobj_core.h>

#include <codecvt>
#include <memory>
#include <sstream>

namespace disk_space {

constexpr char kBadArgumentsError[] = "Bad Arguments";
constexpr char kNoMemory[] = "Out of memory";
constexpr char PATH_KEY[] = "path";
constexpr double ONE_MEGABYTE = 1024 * 1024;

enum class WhichMethod {
  UNKNOWN,
  FREE_SPACE_PATH,
  TOTAL_SPACE_PATH,
  FREE_SPACE,
  TOTAL_SPACE,
  PLATFORM_VER,
};

// static
void DiskSpacePlugin::RegisterWithRegistrar(
    flutter::PluginRegistrarWindows *registrar) {
  auto channel =
      std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
          registrar->messenger(), "disk_space",
          &flutter::StandardMethodCodec::GetInstance());

  auto plugin = std::make_unique<DiskSpacePlugin>();

  channel->SetMethodCallHandler(
      [plugin_pointer = plugin.get()](const auto &call, auto result) {
        plugin_pointer->HandleMethodCall(call, std::move(result));
      });

  registrar->AddPlugin(std::move(plugin));
}

DiskSpacePlugin::DiskSpacePlugin() {}

DiskSpacePlugin::~DiskSpacePlugin() {}

void DiskSpacePlugin::HandleMethodCall(
    const flutter::MethodCall<flutter::EncodableValue> &method_call,
    std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  auto whichMethod = WhichMethod::UNKNOWN;
  if (method_call.method_name().compare("getFreeDiskSpaceForPath") == 0) {
    whichMethod = WhichMethod::FREE_SPACE_PATH;
  } else if (method_call.method_name().compare("getTotalDiskSpaceForPath") == 0) {
    whichMethod = WhichMethod::TOTAL_SPACE_PATH;
  } else if (method_call.method_name().compare("getFreeDiskSpace") == 0) {
    whichMethod = WhichMethod::FREE_SPACE;
  } else if (method_call.method_name().compare("getTotalDiskSpace") == 0) {
    whichMethod = WhichMethod::TOTAL_SPACE;
  } else if (method_call.method_name().compare("getPlatformVersion") == 0) {
    whichMethod = WhichMethod::PLATFORM_VER;
  }

  switch (whichMethod) {
    case WhichMethod::FREE_SPACE_PATH:
    case WhichMethod::TOTAL_SPACE_PATH: {
      const auto* args = std::get_if<flutter::EncodableMap>(method_call.arguments());
      if (!args) {
        result->Error(kBadArgumentsError, "Expected string");
        return;
      }
      auto path_it = args->find(flutter::EncodableValue(PATH_KEY));
      if (path_it == args->end()) {
        result->Error(kBadArgumentsError, "Expected 'path' argument");
        return;
      }
      // key and value should be an EncodableValue
      if (!(std::holds_alternative<std::string>(path_it->second))) {
        result->Error(kBadArgumentsError, "Expected string in Map entry");
        return;
      }
      auto path = std::get<std::string>(path_it->second);
      std::wstring pathStr = std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t>{}.from_bytes(path);
      // Retrieve free disk space
      ULARGE_INTEGER lFreeBytesAvailableToCaller;
      ULARGE_INTEGER lTotalNumberOfBytes;
      ULARGE_INTEGER lTotalNumberOfFreeBytes;
      if (!GetDiskFreeSpaceEx(pathStr.c_str(), &lFreeBytesAvailableToCaller, &lTotalNumberOfBytes, &lTotalNumberOfFreeBytes)) {
        // Failed
        DWORD error = GetLastError();
        std::ostringstream err_stream;
        err_stream << "Win32 error " << error;
        result->Error("Error", err_stream.str());
        return;
      }
      if (whichMethod == WhichMethod::FREE_SPACE_PATH)
        result->Success(flutter::EncodableValue(static_cast<double>(lFreeBytesAvailableToCaller.QuadPart) / ONE_MEGABYTE));
      else
        result->Success(flutter::EncodableValue(static_cast<double>(lTotalNumberOfBytes.QuadPart) / ONE_MEGABYTE));
      break;
    }
    case WhichMethod::FREE_SPACE:
    case WhichMethod::TOTAL_SPACE: {
      // Unlike Apple or Android phones, Windows does not have a single "disk" that we can
      // query the properties of. Assume the user wants the properties of the partition
      // their user profile is stored on; retrieve the Desktop folder's location
      HRESULT hr;
      PWSTR folderPath = NULL; // << ensure this is always freed w/ CoTaskMemFree
      hr = SHGetKnownFolderPath(FOLDERID_Desktop, KF_FLAG_DEFAULT, nullptr, &folderPath);
      if (S_OK != hr) {
        // Failed
        result->Error("Error", "Failed to get Desktop folder location");
        CoTaskMemFree(folderPath);
        return;
      }
      // Retrieve free disk space
      ULARGE_INTEGER lFreeBytesAvailableToCaller;
      ULARGE_INTEGER lTotalNumberOfBytes;
      ULARGE_INTEGER lTotalNumberOfFreeBytes;
      if (!GetDiskFreeSpaceEx(folderPath, &lFreeBytesAvailableToCaller, &lTotalNumberOfBytes, &lTotalNumberOfFreeBytes)) {
        // Failed
        DWORD error = GetLastError();
        std::ostringstream err_stream;
        err_stream << "Win32 error " << error;
        result->Error("Error", err_stream.str());
        CoTaskMemFree(folderPath);
        return;
      }
      CoTaskMemFree(folderPath);
      if (whichMethod == WhichMethod::FREE_SPACE)
        result->Success(flutter::EncodableValue(static_cast<double>(lFreeBytesAvailableToCaller.QuadPart) / ONE_MEGABYTE));
      else
        result->Success(flutter::EncodableValue(static_cast<double>(lTotalNumberOfBytes.QuadPart) / ONE_MEGABYTE));
      break;
    }
    case WhichMethod::PLATFORM_VER: {
      std::ostringstream version_stream;
      version_stream << "Windows ";
      if (IsWindows10OrGreater()) {
        version_stream << "10+";
      } else if (IsWindows8OrGreater()) {
        version_stream << "8";
      } else if (IsWindows7OrGreater()) {
        version_stream << "7";
      }
      result->Success(flutter::EncodableValue(version_stream.str()));
      break;
    }
    default: {
      result->NotImplemented();
      break;
    }
  }
}

}  // namespace disk_space
