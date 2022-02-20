// USBRelayDlg.h : header file
//

#pragma once

#include "afxdialogex.h"
#include "resource.h"

// USBRelayDlg dialog

class USBRelayDlg : public CDialogEx
{
public:
  enum { IDD = IDD_USB_RELAY_DIALOG };
  USBRelayDlg(CWnd* pParent = nullptr);

protected:
  virtual BOOL OnInitDialog();
  virtual void DoDataExchange(CDataExchange* pDX);

protected:
  afx_msg void OnPaint();
  afx_msg HCURSOR OnQueryDragIcon();
  DECLARE_MESSAGE_MAP()

protected:
  HICON m_hIcon;
};
