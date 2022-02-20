#pragma once

#include <vu>
#include <vector>

#include <usb_relay_device.h>

typedef std::vector<pusb_relay_device_info_t> devices_t;

enum class state_t
{
  none,
  on,
  off,
};

static const wchar_t* state_string_t[] =
{
  L"---",
  L"ON ",
  L"OFF",
};

const int max_num_channels = 8;

class RelayManager : public vu::SingletonT<RelayManager>
{
public:
  bool initialize();
  bool destroy();

  devices_t& list_devices(bool refresh = false);

  intptr_t select_device(usb_relay_device_info* ptr_device);
  intptr_t current_device() const;

  bool set_state(int channel, state_t state, intptr_t device = NULL);

private:
  devices_t m_devices;
  intptr_t  m_device = NULL;
  bool m_initialized = false;
  pusb_relay_device_info_t m_ptr_devices = nullptr;
};
