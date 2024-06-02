
// ChromosDlg.h: файл заголовка
//

#pragma once

#include <afxwin.h>
#include <afxext.h>
#include <afxcmn.h>
#include <vector>
#include <numeric>



// Диалоговое окно CChromosDlg
class CChromosDlg : public CDialogEx
{
// Создание
public:
	CChromosDlg(CWnd* pParent = nullptr);	// стандартный конструктор

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHROMOS_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV


// Реализация
protected:
	HICON m_hIcon;

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_listCtrl;
	CButton m_btnOpen;
	CButton m_btnClose;
	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnBnClickedButtonClose();
private:
	void ReadSTGFile(const CString& filePath);
	void NormalizeMolarConcentrations(std::vector<double>& concentrations);
	void CalculateMassConcentrations(const std::vector<double>& molarConcentrations,
		const std::vector<double>& molecularMasses,
		std::vector<double>& massConcentrations);
};
