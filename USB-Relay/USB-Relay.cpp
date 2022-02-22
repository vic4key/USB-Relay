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
  VExt::API::log(vu::format_A("Initialize USB Relay %s", succeed ? "succeed" : "failed"));
}

USB_RELAY_API void VExt_Unload()
{
  if (g_hpn != nullptr)
  {
    UnregisterSuspendResumeNotification(g_hpn);
  }

  bool succeed = RelayManager::instance().destroy();
  VExt::API::log(vu::format_A("Uninitialize USB Relay %s", succeed ? "succeed" : "failed"));
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
  OutputDebugStringA("USB_Relay_on_setting(...)");
}

std::string Window::on_menu_define()
{
  std::string menus = R"([
  {
    "caption": "Hello World",
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
    VExt::API::msg_box("I am written in C++", "Hello World");
  }
  else if (idx == 2)
  {
    VExt::API::msg_box(__name__);
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
