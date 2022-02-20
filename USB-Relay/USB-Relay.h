// USB-Relay.h : Defines the initialization routines for the DLL.
//

#pragma once

#ifdef USB_RELAY_EXPORTS
#define USB_RELAY_API extern "C" __declspec(dllexport)
#else
#define USB_RELAY_API __declspec(dllimport)
#endif
