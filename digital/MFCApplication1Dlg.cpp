
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
	m_image.Create(512, 512, 32);
	FILE *p_file;
	// 폭 512, 높이 512 크기의 8비트 이미지 파일을 읽는다.
	if (0 == fopen_s(&p_file, "512_512.raw", "rb")) {
		// 이미지 파일을 모두 읽는다.
		fread(p_src_image, m_Re_height * m_Re_width, 1, p_file);
		// 8비트 이미지를 32비트 이미지로 확장한다. 하나의 색상을 R, G, B에 동일하게 
		// 복사하면 회색으로 처리된다. 그리고 이 방식에서는 Alpah 값이 적용되지는 않지만 
		// 불투명도 100%를 의미하는 0xFF값으로 대입한다.
		//CMFCApplication1Dlg::OnHistoEqual();
		for (int i = 0; i < m_Re_height * m_Re_width; i++) {
			
			
			*p_dest_pos++ = *p_src_pos;    // Blue
			*p_dest_pos++ = *p_src_pos;    // Green
			*p_dest_pos++ = *p_src_pos++;  // Red
			*p_dest_pos++ = 0xFF;          // Alpha
			

		}
		fclose(p_file);
		// 32비트로 변환된 이미지 값을 CImage 객체에 적용시킨다.
		::SetBitmapBits(m_image, m_Re_height * m_Re_width * 4, p_dest_image);
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


void CMFCApplication1Dlg::OnMulConstant() {
	int value = 5;
	for (int i= 0; i < m_Re_size; i++) {
		if (p_src_image[i] * value > 255)
			p_src_image[i] = 255;
		// 곱의 값이 255보다 크면 255를 출력
		else if (p_src_image[i] * value < 0)
			p_src_image[i] = 0;
		// 곱의 값이 0보다 작으면 0을 출력
		else
			p_src_image[i]
			= (unsigned char)(p_src_image[i] * value);
		// 상수 값과 화소 값 곱셈
	}
}

void CMFCApplication1Dlg::OnDivConstant()
{
	int i,value=5;
		for (i = 0; i < m_Re_size; i++) {
			if (p_src_image[i] / value > 255)
				p_src_image[i] = 255;
			// 나눗셈의 값이 255보다 크면 255를 출력
			else if (p_src_image[i] / value < 0)
				p_src_image[i] = 0;
			// 나눗셈의 값이 0보다 작으면 0을 출력
			else
				p_src_image[i]
				= (unsigned char)(p_src_image[i] / value);
			// 상수 값과 화소 값 나눗셈
		}
	
}

void CMFCApplication1Dlg::OnAndOperate()
{
	int i, value = 100;
		for (i = 0; i < m_Re_size; i++) {
			// 비트 단위 AND 연산
			if ((p_src_image[i] & (unsigned char)value) >= 255)
			{
				p_src_image[i] = 255;
			}
			else if ((p_src_image[i] & (unsigned char)value) < 0)
			{
				p_src_image[i] = 0;
			}
			else {
				p_src_image[i] = (p_src_image[i]
					& value);
			}
		}
	
}

void CMFCApplication1Dlg::OnXorOperate()
{
	int i, value = 100;
		for (i = 0; i < m_Re_size; i++) {
			// 비트 단위 XOR 연산
			if ((p_src_image[i] ^ (unsigned char)value) >= 255) {
				p_src_image[i] = 255;
			}
			else if ((p_src_image[i] ^ (unsigned char)value) < 0) {
				p_src_image[i] = 0;
			}
			else {
				p_src_image[i] = (p_src_image[i]
					^ (unsigned char)value);
			}
		}
	}

	void CMFCApplication1Dlg::OnNegaTransform()
	{
		int i;
		for (i = 0; i < m_Re_size; i++)
			p_src_image[i] = 255 - p_src_image[i]; // 영상 반전을 수행
	}

	void CMFCApplication1Dlg::OnGammaCorrection()
	{
		int i;
		double temp,value=0.5;
			for (i = 0; i < m_Re_size; i++) {
				temp = 255 * pow((float)p_src_image[i] / 255, value);
				// 감마 값 계산
				if (temp < 0)
					p_src_image[i] = 0;
				else if (temp > 255)
					p_src_image[i] = 255;
				else
					p_src_image[i] = (unsigned char)temp;
			}
		
	}

	void CMFCApplication1Dlg::OnBinarization()
	{
		int i, value = 90;
			for (i = 0; i < m_Re_size; i++) {
				if (p_src_image[i] >= value)
					p_src_image[i] = 255; // 임계 값보다 크면 255 출력
				else
					p_src_image[i] = 0; // 임계 값보다 작으면 0 출력
			}
		
	}

	void CMFCApplication1Dlg::OnHistoStretch()
	{
		int i;
		unsigned char LOW, HIGH, MAX, MIN;
		LOW = 0;
		HIGH = 255;
		MIN = p_src_image[0]; // 최소값을 찾기 위한 초기값
		MAX = p_src_image[0]; // 최대값을 찾기 위한 초기값
		// 입력 영상의 최소값 찾기
		for (i = 0; i < m_Re_size; i++) {
			if (p_src_image[i] < MIN)
				MIN = p_src_image[i];
		}
		// 입력 영상의 최대값 찾기
		for (i = 0; i < m_Re_size; i++) {
			if (p_src_image[i] > MAX)
				MAX = p_src_image[i];
		}
		m_OutputImage = new unsigned char[m_Re_size];
		// 히스토그램 stretch
		for (i = 0; i < m_Re_size; i++)
			p_src_image[i] = (unsigned char)((p_src_image[i] -
				MIN)*HIGH / (MAX - MIN));
	}

	void CMFCApplication1Dlg::OnHistoEqual()//아무것도 안나옴 웨?
	{
		// 히스토그램의 값은 0~255
		// 히스토그램의 크기 값을 MAX=255로 정규화하여 출력
		// 히스트그램의 크기 : 256*256 지정
		int i,  value;
		unsigned char LOW, HIGH, Temp;
		double SUM = 0.0;
	
		LOW = 0;
		HIGH = 255;
		for (i = 0; i < 256; i++) {
			m_HIST[i] = LOW;
		}//초기화 작업
		for(i=0 ; i<m_Re_size ; i++){ 
			value = (int)p_src_image[i];
			m_HIST[value]++; 
		}//빈도수
		for (i = 0; i < 256; i++) {
			SUM += m_HIST[i];
			m_Sum_Of_HIST[i] = SUM;
		}
		for (i = 0; i < m_Re_size; i++) {
			Temp = p_src_image[i];
			m_OutputImage[i] = (unsigned char)(m_Sum_Of_HIST[Temp] * HIGH / m_Re_size);
		}
	}

	void CMFCApplication1Dlg::OnEmbossing()
	{
		int i, j;
		double EmboMask[3][3] = { {-1., 0., 0.}, {0., 0., 0.}, {0., 0., 1.} };
		// 마스크 선택
		// double EmboMask[3][3] = {{0., 0., 0.}, {0., 1., 0.}, {0., 0., 0.}};
		// double EmboMask[3][3] = {{1., 1., 1.}, {1., -8.,1.}, {1., 1., 1.}};
		m_OutputImage = new unsigned char[m_Re_size];
		OnMaskProcess(p_src_image, EmboMask);
		// OnMaskProcess 함수를 호출하여 회선 처리를 한다.
		for (i = 0; i < m_Re_height; i++) {
			for (j = 0; j < m_Re_width; j++) {
				if (p_src_image[i *  m_Re_height + j] > 255.)
					p_src_image[i *  m_Re_height + j] = 255.;
				if (p_src_image[i *  m_Re_height + j] < 0.)
					p_src_image[i *  m_Re_height + j] = 0.;
			}
		}
	}

	void CMFCApplication1Dlg::OnMaskProcess(unsigned char *Target, double Mask[3][3])
	{ // 회선 처리가 일어나는 함수
		int i, j, n, m;
		double S = 0.0;
		unsigned char *tempInput, *tempOutput;

		// 입력 값을 위한 메모리 할당
		tempInput = new unsigned char[m_Re_height  * m_Re_width];
		memset(tempInput, 0, m_Re_size);
		// 출력 값을 위한 메모리 할당
		// 1차원 입력 영상의 값을 2차원 배열에 할당한다.
		

		//외곽 0으로 처리
		for (i = 1; i < m_Re_height-1; i++) {
			for (j = 1; j < m_Re_width-1; j++) {
					tempInput[i*m_Re_height + j] =Target[i*m_Re_height+j] ;
				
			}
		}
		// 회선연산
		for (i = 1; i < m_Re_height-1; i++) {
			for (j = 1; j < m_Re_width-1; j++) {
				for (n = 0; n < 3; n++) {
					for (m = 0; m < 3; m++) {
						S += Mask[n][m] * tempInput[(i+n) *  m_Re_height + (j+m)];
					}
				} // 회선 마스크의 크기 만큼 이동하면서 값을 누적
				tempInput[i *  m_Re_height + j] = S; // 누적된 값을 출력 메모리에 저장
				S = 0.0; // 다음 블록으로 이동하면 누적 값을 초기화
			}
		}

		for (i = 0; i < m_Re_height; i++) {
			for (j = 0; j < m_Re_width; j++) {
				Target[i*m_Re_height + j] = tempInput[i*m_Re_height + j];
			}
		}

	}

	void CMFCApplication1Dlg::OnBlurr()
	{
		int i, j;
		double BlurrMask[3][3] = { {1. / 9., 1. / 9., 1. / 9.},
		{1. / 9., 1. / 9., 1. / 9.}, {1. / 9., 1. / 9., 1. / 9.} };
	    OnMaskProcess(p_src_image, BlurrMask);
		// 블러링 처리
		// m_tempImage = OnScale(m_tempImage, m_Re_height, m_Re_width);
		// 정규화
		for (i = 0; i < m_Re_height; i++) {
			for (j = 0; j < m_Re_width; j++) {
				if (p_src_image[i *  m_Re_height + j] > 255.)
					p_src_image[i *  m_Re_height + j] = 255.;
				if (p_src_image[i *  m_Re_height + j] < 0.)
					p_src_image[i *  m_Re_height + j] = 0.;
			}
		}
	}

	void CMFCApplication1Dlg::OnSharpening()
	{
		int i, j;
		double SharpeningMask[3][3] = { {-0.25, -0., 0.}, {-0.25, 2.,
		-0.25}, {0., -0.25, 0.} };
		OnMaskProcess(p_src_image, SharpeningMask);
	for (i = 0; i < m_Re_height; i++) {
		for (j = 0; j < m_Re_width; j++) {
			if (p_src_image[i *  m_Re_height + j] > 255.)
				p_src_image[i *  m_Re_height + j] = 255.;
			if (p_src_image[i *  m_Re_height + j] < 0.)
				p_src_image[i *  m_Re_height + j] = 0.;
		}
	}
	
}

	void CMFCApplication1Dlg::OnRoberts()
	{
		int i, j;
		double RobertsMask[3][3] = { {-1., 0., 0.}, {0., 1.,
		0.}, {0., 0., 0.} };
		OnMaskProcess(p_src_image, RobertsMask);
		for (i = 0; i < m_Re_height; i++) {
			for (j = 0; j < m_Re_width; j++) {
				if (p_src_image[i *  m_Re_height + j] > 255.)
					p_src_image[i *  m_Re_height + j] = 255.;
				if (p_src_image[i *  m_Re_height + j] < 0.)
					p_src_image[i *  m_Re_height + j] = 0.;
			}
		}

	}

	void CMFCApplication1Dlg::OnSobel()
	{
		int i, j;
		double SharpeningMask[3][3] = { {-1., -1., -1.}, {0., 0.,
		0.}, {1., 1., 1.} };
		OnMaskProcess(p_src_image, SharpeningMask);
		for (i = 0; i < m_Re_height; i++) {
			for (j = 0; j < m_Re_width; j++) {
				if (p_src_image[i *  m_Re_height + j] > 255.)
					p_src_image[i *  m_Re_height + j] = 255.;
				if (p_src_image[i *  m_Re_height + j] < 0.)
					p_src_image[i *  m_Re_height + j] = 0.;
			}
		}

	}

	void CMFCApplication1Dlg::OnPrewitt()
	{
		int i, j;
		double SharpeningMask[3][3] = { {-1., -2., -1.}, {0., 0.,
		0.}, {1., 2., 1.} };
		OnMaskProcess(p_src_image, SharpeningMask);
		for (i = 0; i < m_Re_height; i++) {
			for (j = 0; j < m_Re_width; j++) {
				if (p_src_image[i *  m_Re_height + j] > 255.)
					p_src_image[i *  m_Re_height + j] = 255.;
				if (p_src_image[i *  m_Re_height + j] < 0.)
					p_src_image[i *  m_Re_height + j] = 0.;
			}
		}

	}

	void CMFCApplication1Dlg::OnLaplacian()
	{
		int i, j;
		double LaplacianMask[3][3] = { {0., 1., 0.}, {1., -4., 1.}, {0., 1., 0.} };
		OnMaskProcess(p_src_image, LaplacianMask);
		// m_tempImage = OnScale(m_tempImage, m_Re_height, m_Re_width);
		for (i = 0; i < m_Re_height; i++) {
			for (j = 0; j < m_Re_width; j++) {
				if (p_src_image[i *  m_Re_height + j] > 255.)
					p_src_image[i *  m_Re_height + j] = 255.;
				if (p_src_image[i *  m_Re_height + j] < 0.)
					p_src_image[i *  m_Re_height + j] = 0.;
			}
		}
	}

	void CMFCApplication1Dlg::OnGaussianFilter()
	{
		int i, j;
		double GaussianMask[3][3] = { {1. / 16., 1. / 8., 1. / 16.},
		{1. / 8., 1. / 4., 1. / 8.}, {1. / 16., 1. / 8., 1. / 16.} };
		OnMaskProcess(p_src_image, GaussianMask);
		// m_tempImage = OnScale(m_tempImage, m_Re_height, m_Re_width);
		for (i = 0; i < m_Re_height; i++) {
			for (j = 0; j < m_Re_width; j++) {
				if (p_src_image[i *  m_Re_height + j] > 255.)
					p_src_image[i *  m_Re_height + j] = 255.;
				if (p_src_image[i *  m_Re_height + j] < 0.)
					p_src_image[i *  m_Re_height + j] = 0.;
			}
		}
	}

	void CMFCApplication1Dlg::OnLog() {
		int i, j;
		double GaussianMask[3][3] = { {1. / 16., 1. / 8., 1. / 16.},
		{1. / 8., 1. / 4., 1. / 8.}, {1. / 16., 1. / 8., 1. / 16.} };
		OnMaskProcess(p_src_image, GaussianMask);
		double LaplacianMask[3][3] = { {0., 1., 0.}, {1., -4., 1.}, {0., 1., 0.} };
		OnMaskProcess(p_src_image, LaplacianMask);
		for (i = 0; i < m_Re_height; i++) {
			for (j = 0; j < m_Re_width; j++) {
				if (p_src_image[i *  m_Re_height + j] > 255.)
					p_src_image[i *  m_Re_height + j] = 255.;
				if (p_src_image[i *  m_Re_height + j] < 0.)
					p_src_image[i *  m_Re_height + j] = 0.;
			}
		}
	}

	void CMFCApplication1Dlg::OnBilinear() {
		int i, j, point, i_H, i_W;
		unsigned char newValue;
		double ZoomRate = 2.0, r_H, r_W, s_H, s_W;
		double C1, C2, C3, C4;
		int m_height = (int)(m_Re_height*ZoomRate);
		int m_width = (int)(m_Re_width*ZoomRate);
		int m_size = m_height * m_width;
		unsigned char *temp = new unsigned char[m_Re_size];
		m_OutputImage = new unsigned char[m_size];
		for (i = 0; i < m_Re_height; i++) {
			for (j = 0; j < m_Re_width; j++) {
				temp[i*m_Re_height + j] = p_src_image[i*m_Re_height + j];
			}
		}
		for (i = 0; i < m_height; i++) {
			for (j = 0; j < m_width; j++) {
				r_H = i / ZoomRate;
				r_W = j / ZoomRate;
				i_H = (int)floor(r_H);
				i_W = (int)floor(r_W);
				s_H = r_H - i_H;
				s_W = r_W - i_W;
				if (i_H < 0 || i_H >= (m_Re_height - 1) || i_W < 0
					|| i_W >= (m_Re_width - 1))
				{
					point = i * m_width + j;
					m_OutputImage[point] = 255;
				}
				else
				{
					C1 = (double)temp[i_H*m_Re_height+i_W];
					C2 = (double)temp[i_H*m_Re_height + (i_W+1)];
					C3 = (double)temp[(i_H+1)*m_Re_height + (i_W+1)];
					C4 = (double)temp[(i_H+1)*m_Re_height + i_W];
					newValue = (unsigned char)(C1*(1 - s_H)*(1 - s_W)
						+ C2 * s_W*(1 - s_H) + C3 * s_W*s_H + C4 * (1 - s_W)*s_H);
					point = i * m_width + j;
					m_OutputImage[point] = newValue;
				}
			}
		}
	
	}

	void CMFCApplication1Dlg::OnMeanSub() {
		int index=0,i, j, n, m, k, M=3;
		double *Mask=new double[M*M];
		double Value, Sum = 0.0;
		int m_height, m_width, m_size;
		m_height = (m_Re_height + 1) / M;
		m_width = (m_Re_width + 1) / M;
		m_size = m_height * m_width;
		unsigned char *temp = new unsigned char[513 * 513];
		m_OutputImage = new unsigned char[m_size];
		for (i = 0; i < m_Re_height; i++) {
			for (j = 0; j < m_Re_width; j++) {
				temp[i*m_Re_height + j] = p_src_image[i*m_Re_height + j];
			}
		}
		for (i = 0; i < m_Re_height - 1; i = i + M) {
			for (j = 0; j < m_Re_width - 1; j = j + M) {
				for (n = 0; n < M; n++) {
					for (m = 0; m < M; m++) {
						Mask[n*M + m] = p_src_image[(i+n)*m_Re_height + (j+m)];
					}
				}
				for (k = 0; k < M*M; k++)
					Sum = Sum + Mask[k];
				// 마스크에 저장된 값을 누적
				Value = (Sum / (M*M)); // 평균을 계산
				m_OutputImage[index] = (unsigned char)Value;
				// 평균값을 출력
				index++;
				Sum = 0.0;
			}
		}
	}

	void CMFCApplication1Dlg::OnMirrorHor()
	{
		int i, j;
		for (i = 0; i < m_Re_height; i++) {
			for (j = 0; j < m_Re_width; j++) {
				m_OutputImage[i*m_Re_height + m_Re_width - j - 1] = p_src_image[i*m_Re_height + j];
				// 입력 영상의 배열 값을 출력 영상을 위한
				// 배열의 수평축 뒷자리부터 저장
			}
		}
	}

	void CMFCApplication1Dlg::OnMirrorVer() {
		int i, j;
		for (i = 0; i < m_Re_height; i++) {
			for (j = 0; j < m_Re_width; j++) {
				m_OutputImage[(m_Re_height-i-1)*m_Re_width+j] = p_src_image[i*m_Re_height + j];
				// 입력 영상의 배열 값을 출력 영상을 위한
				// 배열의 수평축 뒷자리부터 저장
			}
		}
	}

	void CMFCApplication1Dlg::OnRotation()
	{
		int i, j, CenterH, CenterW, newH, newW, degree = 45;
		// degree = 회전할 각도
		double Radian, PI, Value;
		unsigned char *tempImage = new unsigned char[m_Re_size];
		unsigned char *tempArray = new unsigned char[m_Re_size];

		PI = 3.14159265358979; // 회전각을 위한 PI 값
		Radian = (double)degree*PI / 180.0;
		// degree 값을 radian으로 변경
		CenterH = m_Re_height / 2; // 영상의 중심 좌표
		CenterW = m_Re_width / 2; // 영상의 중심 좌표
		for (i = 0; i < m_Re_height; i++) {
			for (j = 0; j < m_Re_width; j++) {
				tempImage[i*m_Re_height+j] = p_src_image[i*m_Re_height + j];
			}
		}
		for (i = 0; i < m_Re_height; i++) {
			for (j = 0; j < m_Re_width; j++) {
				// 회전 변환 행렬을 이용하여 회전하게 될 좌표 값 계산
				newH = (int)((i - CenterH)*cos(Radian)
					- (j - CenterW)*sin(Radian) + CenterH);
				newW = (int)((i - CenterH)*sin(Radian)
					+ (j - CenterW)*sin(Radian) + CenterW);
				if (newH < 0 || newH >= m_Re_height) {
					// 회전된 좌표가 출력 영상을 위한 배열 값을 넘어갈 때
					Value = 0;
				}
				else if (newW < 0 || newW >= m_Re_width) {
					// 회전된 좌표가 출력 영상을 위한 배열 값을 넘어갈 때
					Value = 0;
				}
				else {
					Value = tempImage[newH*m_Re_height+newW];
				}
				tempArray[i*m_Re_height + j] = Value;
			}
		}
		for (i = 0; i < m_Re_height; i++) {
			for (j = 0; j < m_Re_width; j++) {
				m_OutputImage[i* m_Re_width + j]
					= tempArray[i*m_Re_height + j];
			}
		}
		delete[] tempImage;
		delete[] tempArray;
	}

	void CMFCApplication1Dlg::OnBinaryErosion() {
		int i, j, n, m,m_size=514*514;
		double Mask[3][3] = { {255.,255.,255.},{255.,255.,255.},{255.,255.,255.} };
		double S = 0.0;
		unsigned char *temp;
		temp= new unsigned char[m_size];
		memset(temp, 0, m_size);
		for (i = 0; i < m_Re_height; i++) {
			for (j = 0; m_Re_width; j++) {
				temp[(i+1)*m_Re_height + (j+1)] = p_src_image[i*m_Re_height + j];
			}
		}
		for (i = 0; i < m_Re_height; i++) {
			for (j = 0; j < m_Re_width; j++) {
				for (n = 0; n < 3; n++) {
					for (m = 0; m < 3; m++) {
						if (Mask[n][m] == temp[(i + n)*m_Re_height+(j + m)]) {
							// 마스크와 같은 값이 있는지 조사
							S += 1.0;
						}
					}
				}
				if (S == 9.0)
					m_OutputImage[i * m_Re_width + j] = (unsigned char)255.0;
				// 값이 모두 일치하면 출력 값은 255
				else
					m_OutputImage[i * m_Re_width + j] = (unsigned char)0.0;
				// 모두 일치하지 않으면 출력 값은 0
				S = 0.0; // reset
			}
		}
		delete[] temp;

	}

	void CMFCApplication1Dlg::OnBinaryDilation()
	{
		int i, j, n, m;
		double Mask[3][3] = { {0., 0., 0.},{0., 0., 0.},{0., 0., 0.} };
		// 팽창 처리를 위한 마스크
		double S = 0.0;
		unsigned char *temp = new unsigned char[514 * 514];
		memset(temp, 0, 514 * 514);
		for (i = 0; i < m_Re_height; i++) {
			for (j = 0; j < m_Re_width; j++) {
				temp[(i + 1)*m_Re_height + (j + 1)] = p_src_image[i*m_Re_height + j];
			}
		}

		for (i = 0; i < m_Re_height; i++) {
			for (j = 0; j < m_Re_width; j++) {
				for (n = 0; n < 3; n++) {
					for (m = 0; m < 3; m++) {
						if (Mask[n][m] == temp[(i + n)*m_Re_height + (j + m)]) {
							// 마스크와 같은 값이 있는지 조사
							S += 1.0;
						}
					}
				}
				if (S == 9.0)
					m_OutputImage[i * m_Re_width + j]
					= (unsigned char)0.0;
				// 모두 일치하면 출력 값은 0
				else
					m_OutputImage[i * m_Re_width + j]
					= (unsigned char)255.0;
				// 모두 일치하지 않으면 출력 값은 255
				S = 0.0;
			}
		}
		delete[] temp;
	}

	void CMFCApplication1Dlg::OnBinaryOpening() {
		CMFCApplication1Dlg::OnBinaryErosion();
		CMFCApplication1Dlg::OnBinaryDilation();
	}

	void CMFCApplication1Dlg::OnBinaryClosing() {
		CMFCApplication1Dlg::OnBinaryDilation();
		CMFCApplication1Dlg::OnBinaryErosion();
	}

	void CMFCApplication1Dlg::OnLowPassFilter() {
		int i, j;
		double LPF[3][3] = { {1. / 9., 1. / 9., 1. / 9.},
		{1. / 9., 1. / 9., 1. / 9.},
		{1. / 9., 1. / 9., 1. / 9.} };
		OnMaskProcess(p_src_image, LPF);
		for (i = 0; i < m_Re_height; i++) {
			for (j = 0; j < m_Re_width; j++) {
				if (p_src_image[i *  m_Re_height + j] > 255.)
					p_src_image[i *  m_Re_height + j] = 255.;
				if (p_src_image[i *  m_Re_height + j] < 0.)
					p_src_image[i *  m_Re_height + j] = 0.;
			}
		}

	}

	void CMFCApplication1Dlg::OnHighPassFilter()
	{
		int i, j;
		double HPF[3][3] = { {-1. / 9., -1. / 9., -1. / 9.},
		{-1. / 9., 8 / 9., -1. / 9.},
		{-1. / 9., -1. / 9., -1. / 9.} };
	// 고주파 필터 마스크
		OnMaskProcess(p_src_image, HPF);
		for (i = 0; i < m_Re_height; i++) {
			for (j = 0; j < m_Re_width; j++) {
				if (p_src_image[i *  m_Re_height + j] > 255.)
					p_src_image[i *  m_Re_height + j] = 255.;
				if (p_src_image[i *  m_Re_height + j] < 0.)
					p_src_image[i *  m_Re_height + j] = 0.;
			}
		}
}

	void CMFCApplication1Dlg::OnMedianFilter()
	{
		int i, j, n, m, index = 0;
		double  Mask[9];
		unsigned char *temp = new unsigned char[514 * 514];
		memset(temp, 0, 514 * 514);
		for (i = 0; i < m_Re_height; i++) {
			for (j = 0; j < m_Re_width; j++) {
				temp[(i + 1)*m_Re_height + (j + 1)] = p_src_image[i*m_Re_height + j];
			}
		}
		for (i = 0; i < m_Re_height; i++) {
			for (j = 0; j < m_Re_width; j++) {
				for (n = 0; n < 3; n++) {
					for (m = 0; m < 3; m++) {
						Mask[n * 3 + m] = temp[(i + n)*m_Re_height + (j + m)];
						// 3*3 크기 배열 값을 마스크 배열에 할당
					}
				}
				OnBubleSort(Mask, 9); // 마스크 값을 크기순으로 정렬
				p_src_image[index] = (unsigned char)Mask[4];
				// 중간 값 출력
				index++; // 출력 배열의 좌표
			}
		}
	}

	void CMFCApplication1Dlg::OnBubleSort(double * A, int MAX)
	{ // 데이터의 정렬을 처리하는 함수
		int i, j;
		for (i = 0; i < MAX; i++) {
			for (j = 0; j < MAX - 1; j++) {
				if (A[j] > A[j + 1]) {
					OnSwap(&A[j], &A[j + 1]);
				}
			}
		}
	}

	void CMFCApplication1Dlg::OnSwap(double *a, double *b)
	{ // 데이터 교환 함수
		double temp;
		temp = *a;
		*a = *b;
		*b = temp;
	}


