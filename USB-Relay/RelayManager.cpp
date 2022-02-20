#include "pch.h"
#include "RelayManager.h"

bool RelayManager::initialize()
{
  if (!m_initialized)
  {
    m_initialized = usb_relay_init() == 0;
  }

  return m_initialized;
}

bool RelayManager::destroy()
{
  if (m_ptr_devices != nullptr)
  {
    usb_relay_device_free_enumerate(m_ptr_devices);
    m_ptr_devices = nullptr;
  }

  m_initialized = false;

  return usb_relay_exit() == 0;
}

devices_t& RelayManager::list_devices(bool refresh)
{
  refresh |= m_devices.empty();

  if (refresh)
  {
    m_devices.clear();

    if (m_ptr_devices != nullptr)
    {
      usb_relay_device_free_enumerate(m_ptr_devices);
      m_ptr_devices = nullptr;
    }

    m_ptr_devices = usb_relay_device_enumerate();

    for (auto ptr = m_ptr_devices; ptr != nullptr; ptr = ptr->next)
    {
      m_devices.push_back(ptr);
    }
  }

  return m_devices;
}

intptr_t RelayManager::select_device(usb_relay_device_info* ptr_device)
{
  if (m_device != NULL)
  {
    usb_relay_device_close(m_device);
  }

  m_device = NULL;

  if (ptr_device != nullptr)
  {
    m_device = usb_relay_device_open(ptr_device);
  }

  return m_device;
}

intptr_t RelayManager::current_device() const
{
  return m_device;
}

bool RelayManager::set_state(int channel, state_t state, intptr_t device)
{
  if (channel >= max_num_channels || state == state_t::none)
  {
    return false;
  }

  if (device == NULL)
  {
    device = m_device;
  }

  channel += 1; // bit 0/1/2/3/4/5/6/7/8 indicate channel 1/2/3/4/5/6/7/8 status

  bool result = false;

  switch (state)
  {
  case state_t::on:
    result = usb_relay_device_open_one_relay_channel(device, channel) == 0;
    break;

  case state_t::off:
    result = usb_relay_device_close_one_relay_channel(device, channel) == 0;
    break;

  default:
    break;
  }

  return result;
}
