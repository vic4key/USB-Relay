// USBRelayDlg.h : header file
//

#pragma once

#include "afxdialogex.h"
#include "resource.h"
#include "RelayManager.h"

// USBRelayDlg dialog

class USBRelayDlg : public CDialogEx
{
public:
  enum { IDD = IDD_USB_RELAY_DIALOG };
  USBRelayDlg(CWnd* pParent = nullptr);

protected:
  virtual BOOL OnInitDialog();
  virtual void DoDataExchange(CDataExchange* pDX);
  virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
  afx_msg void OnPaint();
  afx_msg HCURSOR OnQueryDragIcon();
  afx_msg void OnBnClicked_Refresh();
  afx_msg void OnCbnSelchange_Devices();
  afx_msg void OnNMDblclk_States(NMHDR* pNMHDR, LRESULT* pResult);
  DECLARE_MESSAGE_MAP()

private:
  void Initialize();
  void Update();
  void SetDisplayState(int channel, state_t state);

private:
  HICON m_hIcon;
  CComboBox m_Devices;
  CListCtrl m_States;
};
