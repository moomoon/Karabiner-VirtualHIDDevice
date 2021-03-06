#include "karabiner_virtual_hid_device_methods.hpp"
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/hid/IOHIDDevice.h>
#include <IOKit/hid/IOHIDElement.h>
#include <IOKit/hid/IOHIDManager.h>
#include <IOKit/hid/IOHIDQueue.h>
#include <IOKit/hid/IOHIDValue.h>
#include <IOKit/hidsystem/IOHIDShared.h>
#include <IOKit/hidsystem/ev_keymap.h>
#include <cmath>
#include <iostream>
#include <thread>

int main(int argc, const char* argv[]) {
  if (getuid() != 0) {
    std::cerr << "dispatch_keyboard_event_example requires root privilege." << std::endl;
  }

  kern_return_t kr;
  io_connect_t connect = IO_OBJECT_NULL;
  auto service = IOServiceGetMatchingService(kIOMasterPortDefault, IOServiceNameMatching(pqrs::karabiner_virtual_hid_device::get_virtual_hid_root_name()));
  if (!service) {
    std::cerr << "IOServiceGetMatchingService error" << std::endl;
    goto finish;
  }

  kr = IOServiceOpen(service, mach_task_self(), kIOHIDServerConnectType, &connect);
  if (kr != KERN_SUCCESS) {
    std::cerr << "IOServiceOpen error" << std::endl;
    goto finish;
  }

  kr = pqrs::karabiner_virtual_hid_device_methods::initialize_virtual_hid_keyboard(connect);
  if (kr != KERN_SUCCESS) {
    std::cerr << "initialize_virtual_hid_keyboard error" << std::endl;
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  // ----------------------------------------

  for (int i = 0; i < 20; ++i) {
    pqrs::karabiner_virtual_hid_device::hid_event_service::keyboard_event keyboard_event;

    switch (i % 10) {
    case 0:
      keyboard_event.usage = pqrs::karabiner_virtual_hid_device::usage(kHIDUsage_KeyboardA);
      keyboard_event.value = 1;
      break;
    case 1:
      keyboard_event.usage = pqrs::karabiner_virtual_hid_device::usage(kHIDUsage_KeyboardA);
      keyboard_event.value = 0;
      break;
    case 2:
      keyboard_event.usage = pqrs::karabiner_virtual_hid_device::usage(kHIDUsage_KeyboardB);
      keyboard_event.value = 1;
      break;
    case 3:
      keyboard_event.usage = pqrs::karabiner_virtual_hid_device::usage(kHIDUsage_KeyboardB);
      keyboard_event.value = 0;
      break;
    case 4:
      keyboard_event.usage = pqrs::karabiner_virtual_hid_device::usage(kHIDUsage_KeyboardRightControl);
      keyboard_event.value = 1;
      break;
    case 5:
      keyboard_event.usage = pqrs::karabiner_virtual_hid_device::usage(kHIDUsage_KeyboardRightControl);
      keyboard_event.value = 0;
      break;
    case 6:
      keyboard_event.usage_page = pqrs::karabiner_virtual_hid_device::usage_page::apple_vendor_top_case;
      keyboard_event.usage = pqrs::karabiner_virtual_hid_device::usage::av_top_case_keyboard_fn;
      keyboard_event.value = 1;
      break;
    case 7:
      keyboard_event.usage = pqrs::karabiner_virtual_hid_device::usage(kHIDUsage_KeyboardQ);
      keyboard_event.value = 1;
      break;
    case 8:
      keyboard_event.usage = pqrs::karabiner_virtual_hid_device::usage(kHIDUsage_KeyboardQ);
      keyboard_event.value = 0;
      break;
    case 9:
      keyboard_event.usage_page = pqrs::karabiner_virtual_hid_device::usage_page::apple_vendor_top_case;
      keyboard_event.usage = pqrs::karabiner_virtual_hid_device::usage::av_top_case_keyboard_fn;
      keyboard_event.value = 0;
      break;
    }

    kr = pqrs::karabiner_virtual_hid_device_methods::dispatch_keyboard_event(connect, keyboard_event);
    if (kr != KERN_SUCCESS) {
      std::cerr << "dispatch_keyboard_event error" << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  // ----------------------------------------
  // key repeat
  {
    pqrs::karabiner_virtual_hid_device::hid_event_service::keyboard_event keyboard_event;
    keyboard_event.usage = pqrs::karabiner_virtual_hid_device::usage(kHIDUsage_KeyboardC);
    keyboard_event.value = 1;
    kr = pqrs::karabiner_virtual_hid_device_methods::dispatch_keyboard_event(connect, keyboard_event);
    if (kr != KERN_SUCCESS) {
      std::cerr << "dispatch_keyboard_event error" << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  kr = pqrs::karabiner_virtual_hid_device_methods::reset_virtual_hid_keyboard(connect);
  if (kr != KERN_SUCCESS) {
    std::cerr << "reset_virtual_hid_keyboard error" << std::endl;
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  std::cout << std::endl;

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  // ----------------------------------------
  // apple_vendor_keyboard

  {
    pqrs::karabiner_virtual_hid_device::hid_event_service::keyboard_event keyboard_event;
    keyboard_event.usage_page = pqrs::karabiner_virtual_hid_device::usage_page::apple_vendor_keyboard;
    keyboard_event.usage = pqrs::karabiner_virtual_hid_device::usage::apple_vendor_keyboard_expose_all;
    keyboard_event.value = 1;

    kr = pqrs::karabiner_virtual_hid_device_methods::dispatch_keyboard_event(connect, keyboard_event);
    if (kr != KERN_SUCCESS) {
      std::cerr << "dispatch_keyboard_event error" << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    keyboard_event.value = 0;
    kr = pqrs::karabiner_virtual_hid_device_methods::dispatch_keyboard_event(connect, keyboard_event);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    keyboard_event.value = 1;
    kr = pqrs::karabiner_virtual_hid_device_methods::dispatch_keyboard_event(connect, keyboard_event);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    keyboard_event.value = 0;
    kr = pqrs::karabiner_virtual_hid_device_methods::dispatch_keyboard_event(connect, keyboard_event);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

finish:
  if (connect) {
    IOServiceClose(connect);
  }
  if (service) {
    IOObjectRelease(service);
  }

  return 0;
}
