
// MFCApplication1Dlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "MFCApplication1.h"
#include "MFCApplication1Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCApplication1Dlg 대화 상자



CMFCApplication1Dlg::CMFCApplication1Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCAPPLICATION1_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCApplication1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCApplication1Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CMFCApplication1Dlg 메시지 처리기

BOOL CMFCApplication1Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	m_image.Create(171, 171, 32);
	FILE* p_file;
	// 폭 512, 높이 512 크기의 8비트 이미지 파일을 읽는다.
	if (0 == fopen_s(&p_file, "512_512.raw", "rb")) {
		// 이미지 파일을 모두 읽는다.
		fread(m_OutputImage, m_Re_height * m_Re_width, 1, p_file);
		// 8비트 이미지를 32비트 이미지로 확장한다. 하나의 색상을 R, G, B에 동일하게 
		// 복사하면 회색으로 처리된다. 그리고 이 방식에서는 Alpah 값이 적용되지는 않지만 
		// 불투명도 100%를 의미하는 0xFF값으로 대입한다.
		CMFCApplication1Dlg::OnMeanSub();
		for (int i = 0; i < 171 * 171; i++) {


			*p_dest_pos++ = *p_src_pos;    // Blue
			*p_dest_pos++ = *p_src_pos;    // Green
			*p_dest_pos++ = *p_src_pos++;  // Red
			*p_dest_pos++ = 0xFF;          // Alpha


		}
		fclose(p_file);
		// 32비트로 변환된 이미지 값을 CImage 객체에 적용시킨다.
		::SetBitmapBits(m_image, 171 * 171 * 4, p_dest_image);
	}
	// 이미지 변환에 사용한 메모리를 해제한다.
	delete[] p_dest_image;
	// 8비트 이미지를 읽기 위해 사용한 메모리를 해제한다.
	delete[] p_src_image;
	delete[] m_OutputImage;
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CMFCApplication1Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMFCApplication1Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
		m_image.Draw(dc, 10, 10);
	}
	else
	{
		//CDialogEx::OnPaint();
		CPaintDC dc(this);
		m_image.Draw(dc, 10, 10);
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMFCApplication1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
void CMFCApplication1Dlg::OnMeanSub() {
	int index = 0, i, j, n, m, k, M = 3;
	double* Mask = new double[M * M];
	double Value, Sum = 0.0;
	int m_height, m_width, m_size;
	m_height = (m_Re_height + 1) / M;
	m_width = (m_Re_width + 1) / M;
	m_size = m_height * m_width;
	unsigned char* temp = new unsigned char[513 * 513];
	memset(temp, 0, 513 * 513);
	for (i = 0; i < m_Re_height; i++) {
		for (j = 0; j < m_Re_width; j++) {
			temp[i * 513 + j] = m_OutputImage[i * m_Re_height + j];
		}

	}

	for (i = 0; i < m_Re_height - 1; i = i + M) {
		for (j = 0; j < m_Re_width - 1; j = j + M) {
			for (n = 0; n < M; n++) {
				for (m = 0; m < M; m++) {
					Mask[n * M + m] = temp[(i + n) * 513 + (j + m)];
				}
			}
			for (k = 0; k < M * M; k++)
				Sum = Sum + Mask[k];
			// 마스크에 저장된 값을 누적
			Value = (Sum / (M * M)); // 평균을 계산
			p_src_image[index] = (unsigned char)Value;
			// 평균값을 출력
			index++;
			Sum = 0.0;
		}
	}
}

