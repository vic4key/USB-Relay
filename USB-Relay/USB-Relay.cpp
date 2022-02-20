// USB-Relay.cpp : Defines the initialization routines for the DLL.
//

#include "pch.h"
#include "framework.h"

#include <Vu>
#include "USB-Relay.h"
#include "USBRelayDlg.h"

USB_RELAY_API char __name__[]     = "USB Relay";
USB_RELAY_API char __author__[]   = "Vic P.";
USB_RELAY_API char __version__[]  = "1.0";
USB_RELAY_API char __summary__[]  = "The USB Relay extension";
USB_RELAY_API char __url__[]      = "https://github.com/vic4key/USB-Relay.git";

USB_RELAY_API void VExt_Load()
{
  bool succeed = RelayManager::instance().initialize();
  VExt::API::log(vu::format_A("Initialize USB Relay %s", succeed ? "succeed" : "failed"));
}

USB_RELAY_API void VExt_Unload()
{
  OutputDebugStringA("USB_Relay_Unload(...)");
  bool succeed = RelayManager::instance().destroy();
  VExt::API::log(vu::format_A("Uninitialize USB Relay %s", succeed ? "succeed" : "failed"));
}

VExt_Expose_Object(Window)

void Window::on_display_ui()
{
  OutputDebugStringA("USB_Relay_on_display_ui(...)");

  USBRelayDlg dlg;
  dlg.DoModal();
}

void Window::on_settings()
{
  OutputDebugStringA("USB_Relay_on_settings(...)");
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
