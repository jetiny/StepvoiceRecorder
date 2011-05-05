/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_PAGECOM_H__189B5FC7_2E15_4E20_BA3C_82EADCA86D62__INCLUDED_)
#define AFX_PAGECOM_H__189B5FC7_2E15_4E20_BA3C_82EADCA86D62__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "struct.h"

/////////////////////////////////////////////////////////////////////////////
class CPageCom : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageCom)

private:
	CONF_DIAL_GEN* pconfig;
	int m_default_play_id;
	int m_default_rec_id;

public:
	CPageCom();
	~CPageCom();

	void SetConfig(CONF_DIAL_GEN* pconf_dial_gen);

// Dialog Data
	//{{AFX_DATA(CPageCom)
	enum { IDD = IDD_COMMON };
	BOOL	m_Loader;
	BOOL	m_AutoRun;
	BOOL	m_Minimized;
	BOOL	m_ToolTipEnable;
	BOOL	m_TrayIcon;
	BOOL	m_TrayMin;
	BOOL	m_MInstances;
	int		m_playbackDeviceID;
	int		m_recordingDeviceID;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageCom)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CPageCom)
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_PAGECOM_H__189B5FC7_2E15_4E20_BA3C_82EADCA86D62__INCLUDED_)
