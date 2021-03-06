// USB-Relay.h : Defines the initialization routines for the DLL.
//

#pragma once

#ifdef USB_RELAY_EXPORTS
#define USB_RELAY_API extern "C" __declspec(dllexport)
#else
#define USB_RELAY_API __declspec(dllimport)
#endif

#include <VExt_DLL>

/**
 * VExt::Window
 */

class Window : public VExt::UI::Window
{
public:
  virtual void on_display();
  virtual void on_setting();
  virtual std::string on_menu_define();
  virtual std::pair<bool, bool> on_menu_update(UINT idx, LPARAM lp);
  virtual void on_menu_execute(UINT idx, LPARAM lp);
};

/**
 * Others
 */

ULONG CALLBACK fnNotifyCallbackRoutine(PVOID Context, ULONG Type, PVOID Setting);
