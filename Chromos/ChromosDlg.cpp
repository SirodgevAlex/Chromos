#include "pch.h"
#include "framework.h"
#include "Chromos.h"
#include "ChromosDlg.h"
#include "afxdialogex.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <numeric>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CChromosDlg::CChromosDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_CHROMOS_DIALOG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CChromosDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_PEAKS, m_listCtrl);
}

BEGIN_MESSAGE_MAP(CChromosDlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON_OPEN, &CChromosDlg::OnBnClickedButtonOpen)
    ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CChromosDlg::OnBnClickedButtonClose)
END_MESSAGE_MAP()

BOOL CChromosDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);

    m_listCtrl.InsertColumn(0, _T("#"), LVCFMT_LEFT, 50);
    m_listCtrl.InsertColumn(1, _T("Время(мин)"), LVCFMT_LEFT, 100);
    m_listCtrl.InsertColumn(2, _T("Высота(мВ)"), LVCFMT_LEFT, 100);
    m_listCtrl.InsertColumn(3, _T("Площадь(мВ·мин)"), LVCFMT_LEFT, 100);
    m_listCtrl.InsertColumn(4, _T("Концентрация(мол%)"), LVCFMT_LEFT, 150);
    m_listCtrl.InsertColumn(5, _T("Концентрация(масс%)"), LVCFMT_LEFT, 150);
    m_listCtrl.InsertColumn(6, _T("Компонент"), LVCFMT_LEFT, 150);

    return TRUE;
}

void CChromosDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this);

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialogEx::OnPaint();
    }
}

HCURSOR CChromosDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CChromosDlg::OnBnClickedButtonOpen()
{
    CFileDialog fileDlg(TRUE, _T("stg"), NULL, 0, _T("STG Files (*.stg)|*.stg|All Files (*.*)|*.*||"));
    if (fileDlg.DoModal() == IDOK) {
        CString filePath = fileDlg.GetPathName();
        ReadSTGFile(filePath);
    }
}

void CChromosDlg::OnBnClickedButtonClose()
{
    EndDialog(0);
}

void CChromosDlg::ReadSTGFile(const CString& filePath)
{
    std::ifstream file((LPCTSTR)filePath);
    if (!file.is_open()) {
        AfxMessageBox(_T("Не удалось открыть файл!"));
        return;
    }

    std::vector<double> molarConcentrations;
    std::vector<double> molecularMasses; // заполняйте соответствующими данными
    std::vector<double> massConcentrations;
    std::string line;
    int index = 0;

    while (std::getline(file, line)) {
        // пропускаем заголовки
        if (index++ < 5) continue;

        std::istringstream iss(line);
        std::string token;
        std::vector<std::string> tokens;
        while (std::getline(iss, token, '\t')) {
            tokens.push_back(token);
        }

        if (tokens.size() < 7) continue;

        int n = std::stoi(tokens[0]);
        double time = std::stod(tokens[1]);
        double height = std::stod(tokens[2]);
        double area = std::stod(tokens[3]);
        double molConc = std::stod(tokens[4]);
        molarConcentrations.push_back(molConc);
        // используйте правильные молекулярные массы для каждого компонента
        double molMass = 0; // установите соответствующую молекулярную массу
        molecularMasses.push_back(molMass);

        CString name = CString(tokens[6].c_str());

        int row = m_listCtrl.InsertItem(n, std::to_wstring(n).c_str());
        m_listCtrl.SetItemText(row, 1, std::to_wstring(time).c_str());
        m_listCtrl.SetItemText(row, 2, std::to_wstring(height).c_str());
        m_listCtrl.SetItemText(row, 3, std::to_wstring(area).c_str());
        m_listCtrl.SetItemText(row, 6, name);
    }

    NormalizeMolarConcentrations(molarConcentrations);
    CalculateMassConcentrations(molarConcentrations, molecularMasses, massConcentrations);

    for (int i = 0; i < molarConcentrations.size(); ++i) {
        m_listCtrl.SetItemText(i, 4, std::to_wstring(molarConcentrations[i]).c_str());
        m_listCtrl.SetItemText(i, 5, std::to_wstring(massConcentrations[i]).c_str());
    }
}

void CChromosDlg::NormalizeMolarConcentrations(std::vector<double>& concentrations)
{
    double sum = std::accumulate(concentrations.begin(), concentrations.end(), 0.0);
    for (auto& concentration : concentrations)
    {
        concentration = (concentration / sum) * 100;
    }
}

void CChromosDlg::CalculateMassConcentrations(const std::vector<double>& molarConcentrations,
    const std::vector<double>& molecularMasses,
    std::vector<double>& massConcentrations)
{
    double totalMass = 0.0;
    for (size_t i = 0; i < molarConcentrations.size(); ++i)
    {
        totalMass += molarConcentrations[i] * molecularMasses[i];
    }

    massConcentrations.resize(molarConcentrations.size());
    for (size_t i = 0; i < molarConcentrations.size(); ++i)
    {
        massConcentrations[i] = (molarConcentrations[i] * molecularMasses[i] / totalMass) * 100.0;
    }
}
