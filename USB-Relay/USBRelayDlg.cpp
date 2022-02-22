// USBRelayDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "USBRelayDlg.h"
#include "RelayManager.h"

// USBRelayDlg dialog

USBRelayDlg::USBRelayDlg(CWnd* pParent) : CDialogEx(USBRelayDlg::IDD, pParent)
{
  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void USBRelayDlg::DoDataExchange(CDataExchange* pDX)
{
  __super::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_DEVICES, m_Devices);
  DDX_Control(pDX, IDC_STATES, m_States);
}

BEGIN_MESSAGE_MAP(USBRelayDlg, CDialogEx)
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
  ON_BN_CLICKED(IDC_REFRESH, OnBnClicked_Refresh)
  ON_CBN_SELCHANGE(IDC_DEVICES, OnCbnSelchange_Devices)
  ON_NOTIFY(NM_DBLCLK, IDC_STATES, OnNMDblclk_States)
END_MESSAGE_MAP()

// USBRelayDlg message handlers

BOOL USBRelayDlg::OnInitDialog()
{
  __super::OnInitDialog();

  // Set the icon for this dialog.  The framework does this automatically
  //  when the application's main window is not a dialog
  SetIcon(m_hIcon, TRUE);			// Set big icon
  SetIcon(m_hIcon, FALSE);		// Set small icon

  // TODO: Add extra initialization here

  this->Initialize();

  return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void USBRelayDlg::OnPaint()
{
  if (IsIconic())
  {
    CPaintDC dc(this); // device context for painting

    SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

    // Center icon in client rectangle
    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    // Draw the icon
    dc.DrawIcon(x, y, m_hIcon);
  }
  else
  {
    __super::OnPaint();
  }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR USBRelayDlg::OnQueryDragIcon()
{
  return static_cast<HCURSOR>(m_hIcon);
}

void USBRelayDlg::OnBnClicked_Refresh()
{
  m_Devices.ResetContent();

  for (auto& ptr : RelayManager::instance().list_devices())
  {
    auto idx = m_Devices.AddString(CString(ptr->serial_number));
    m_Devices.SetItemDataPtr(idx, ptr);
  }

  m_Devices.SetCurSel(0);

  this->OnCbnSelchange_Devices();
}

void USBRelayDlg::OnCbnSelchange_Devices()
{
  auto idx = m_Devices.GetCurSel();
  auto ptr = reinterpret_cast<usb_relay_device_info*>(m_Devices.GetItemDataPtr(idx));
  RelayManager::instance().select_device(ptr);

  this->Update();
}

void USBRelayDlg::Initialize()
{
  m_States.SetExtendedStyle(m_States.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

  auto ptr_header = m_States.GetHeaderCtrl();
  assert(ptr_header != nullptr);

  static CImageList image_list;
  image_list.DeleteImageList();
  image_list.Create(IDB_LOG_SMALL, 16, 1, 0xFFFFFF);
  m_States.SetImageList(&image_list, LVSIL_SMALL);

  LVCOLUMN lvc = { 0 };

  lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
  lvc.fmt = LVCFMT_LEFT;
  lvc.cx = 70;
  lvc.pszText = LPWSTR(L"State");
  m_States.InsertColumn(0, &lvc);

  lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
  lvc.fmt = LVCFMT_LEFT;
  lvc.cx = 200;
  lvc.pszText = LPWSTR(L"Channel");
  m_States.InsertColumn(1, &lvc);

  this->OnBnClicked_Refresh();
}

void USBRelayDlg::Update()
{
  m_States.DeleteAllItems();

  auto device = RelayManager::instance().current_device();
  auto ptr = reinterpret_cast<usb_relay_device_info*>(device);

  auto states = 0u;
  usb_relay_device_get_status(device, &states);

  for (int i = 0; i < max_num_channels; i++)
  {
    int channel = -1;
    state_t state = state_t::none;

    if (i < ptr->type)
    {
      channel = i;
      state = states & (1 << i) ? state_t::on : state_t::off;
    }

    this->SetDisplayState(channel, state);
  }
}

void USBRelayDlg::SetDisplayState(int channel, state_t state)
{
  std::wstring line = channel == -1 ? L"---" : vu::format_W(L"Channel %d", channel);
  LVITEM lvi = { 0 };
  lvi.mask = LVIF_IMAGE;
  lvi.iItem = m_States.GetItemCount();
  lvi.iSubItem = 0;
  lvi.iImage = int(state);
  int index = m_States.InsertItem(&lvi);
  m_States.SetItemText(index, 0, state_string_t[int(state)]);
  m_States.SetItemText(index, 1, line.c_str());
  m_States.SetItemData(index, DWORD_PTR(state));
  m_States.EnsureVisible(index, TRUE);
};

void USBRelayDlg::OnNMDblclk_States(NMHDR* pNMHDR, LRESULT* pResult)
{
  LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
  assert(pNMItemActivate != nullptr);

  static int index;
  index = pNMItemActivate->iItem;
  if (index == -1)
  {
    return;
  }

  int channel = index;

  state_t state = state_t(m_States.GetItemData(channel));

  if (state == state_t::on)
  {
    state = state_t::off;
  }
  else if (state == state_t::off)
  {
    state = state_t::on;
  }

  RelayManager::instance().set_state(channel, state);

  *pResult = 0;

  this->Update();

  m_States.SetItemState(index, LVIS_SELECTED, LVIS_SELECTED);
}
