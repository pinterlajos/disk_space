import 'dart:async';
import 'dart:io';

import 'package:flutter/services.dart';

class DiskSpace {
  static const MethodChannel _channel = const MethodChannel('disk_space');

  static Future<String?> get platformVersion async {
    final String? version = await _channel.invokeMethod('getPlatformVersion');
    return version;
  }

  /// Returns the free disk space, measured in mebibytes (2^20 bytes), of
  /// the disk. On platforms that don't have a concept of a single disk,
  /// it returns the free disk space on the partition containing the user's
  /// home/profile directory or its approximate. If for some reason the disk
  //   /// space could not be determined, returns null or an error.
  static Future<double?> get getFreeDiskSpace async {
    final double? freeDiskSpace =
        await _channel.invokeMethod('getFreeDiskSpace');
    return freeDiskSpace;
  }

  /// Returns the total disk space, measured in mebibytes (2^20 bytes), of
  /// the disk. On platforms that don't have a concept of a single disk,
  /// it returns the total disk space on the partition containing the user's
  /// home/profile directory or its approximate. If for some reason the disk
  //   /// space could not be determined, returns null or an error.
  static Future<double?> get getTotalDiskSpace async {
    final double? totalDiskSpace =
        await _channel.invokeMethod('getTotalDiskSpace');
    return totalDiskSpace;
  }

  /// Returns the free disk space, measured in mebibytes (2^20 bytes), of
  /// the disk/partition that contains [path]. If for some reason the disk
  //   /// space could not be determined, returns null or an error.
  static Future<double?> getFreeDiskSpaceForPath(String path) async {
    if (!Directory(path).existsSync()) {
      throw Exception("Specified path does not exist");
    }
    final double? freeDiskSpace =
        await _channel.invokeMethod('getFreeDiskSpaceForPath', {"path": path});
    return freeDiskSpace;
  }

  /// Returns the total disk space, measured in mebibytes (2^20 bytes), of
  /// the disk/partition that contains [path]. If for some reason the disk
  /// space could not be determined, returns null or an error.
  static Future<double?> getTotalDiskSpaceForPath(String path) async {
    if (!Directory(path).existsSync()) {
      throw Exception("Specified path does not exist");
    }
    final double? totalDiskSpace =
        await _channel.invokeMethod('getTotalDiskSpaceForPath', {"path": path});
    return totalDiskSpace;
  }
}
