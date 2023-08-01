// USB-Relay.cpp : Defines the initialization routines for the DLL.
//

#include "pch.h"
#include "framework.h"

#include <Vu>
#include "USB-Relay.h"
#include "USBRelayDlg.h"

#include <windows.h>
#include <powrprof.h>

USB_RELAY_API char __name__[]     = "USB Relay";
USB_RELAY_API char __author__[]   = "Vic P.";
USB_RELAY_API char __version__[]  = "1.0";
USB_RELAY_API char __summary__[]  = "The USB Relay extension";
USB_RELAY_API char __url__[]      = "https://github.com/vic4key/USB-Relay.git";

static HPOWERNOTIFY g_hpn = nullptr;

/**
 * VExt
 */

USB_RELAY_API void VExt_Load()
{
  DEVICE_NOTIFY_SUBSCRIBE_PARAMETERS dnsp = { 0 };
  dnsp.Callback = fnNotifyCallbackRoutine;
  g_hpn = RegisterSuspendResumeNotification(&dnsp, DEVICE_NOTIFY_CALLBACK);

  bool succeed = RelayManager::instance().initialize();
  auto s = vu::format_A("Initialize USB Relay %s", succeed ? "succeed" : "failed");
  VExt::API::log(s, succeed ? VExt::logging_level_t::info : VExt::logging_level_t::error);

  auto& devices = RelayManager::instance().list_devices();
  if (devices.empty())
  {
    return;
  }

  RelayManager::instance().select_device(devices[0]);

  auto prefs = VExt::API::load_prefs(__name__);
  auto channels_turn_on_on_load = VExt::json_get(prefs, "channels_turn_on_on_load", std::vector<int>());
  for (int channel : channels_turn_on_on_load)
  {
    RelayManager::instance().set_state(channel, state_t::on);
  }
}

USB_RELAY_API void VExt_Unload()
{
  if (g_hpn != nullptr)
  {
    UnregisterSuspendResumeNotification(g_hpn);
  }

  auto prefs = VExt::API::load_prefs(__name__);
  auto channels_turn_on_on_load = VExt::json_get(prefs, "channels_turn_off_on_unload", std::vector<int>());
  for (int channel : channels_turn_on_on_load)
  {
    RelayManager::instance().set_state(channel, state_t::off);
  }

  bool succeed = RelayManager::instance().destroy();
  auto s = vu::format_A("Uninitialize USB Relay %s", succeed ? "succeed" : "failed");
  VExt::API::log(s, succeed ? VExt::logging_level_t::info : VExt::logging_level_t::error);
}

/**
 * VExt::Window
 */

VExt_Expose_Object(Window)

void Window::on_display()
{
  USBRelayDlg dlg;
  dlg.DoModal();
}

void Window::on_setting()
{
  vu::msg_box_A(__FUNCSIG__);
}

std::string Window::on_menu_define()
{
  std::string menus = R"([
  {
    "caption": "Display UI",
    "index": 1,
    "children": []
  },
  {
    "caption": "Information",
    "index": 2,
    "children": []
  }])";

  return menus;
}

std::pair<bool, bool> Window::on_menu_update(UINT idx, LPARAM lp)
{
  return { true, false }; // all menu items are enabled & unchecked
}

void Window::on_menu_execute(UINT idx, LPARAM lp)
{
  if (idx == 1)
  {
    USBRelayDlg dlg;
    dlg.DoModal();
  }
  else if (idx == 2)
  {
    auto s = vu::format_A("%s %s © %s\n\nWritten in C++ on %s", __name__, __version__, __author__, __DATE__);
    VExt::API::message_box(s);
  }
}

/**
 * Others
 */

ULONG CALLBACK fnNotifyCallbackRoutine(PVOID Context, ULONG Type, PVOID Setting)
{
  switch (Type)
  {
  // case PBT_APMRESUMEAUTOMATIC:
  // {
  //   vu::msg_debug_A("Operation resuming automatically after event.");
  // }
  // break;

  case PBT_APMRESUMESUSPEND:
  {
    vu::msg_debug_A("Operation resuming after suspension.");
    RelayManager::instance().set_state(0, state_t::on);
  }
  break;

  case PBT_APMSUSPEND:
  {
    vu::msg_debug_A("System is suspending operation.");
    RelayManager::instance().set_state(0, state_t::off);
  }
  break;

  default:
    break;
  }

  return 0;
}
