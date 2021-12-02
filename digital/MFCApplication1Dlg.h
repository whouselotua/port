
// MFCApplication1Dlg.h: 헤더 파일
//

#pragma once


// CMFCApplication1Dlg 대화 상자
class CMFCApplication1Dlg : public CDialogEx
{
// 생성입니다.
public:
	CMFCApplication1Dlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.
	void CMFCApplication1Dlg::OnMulConstant();
	void CMFCApplication1Dlg::OnDivConstant();
	void CMFCApplication1Dlg::OnAndOperate();
	void CMFCApplication1Dlg::OnXorOperate();
	void CMFCApplication1Dlg::OnNegaTransform();
	void CMFCApplication1Dlg::OnGammaCorrection();
	void CMFCApplication1Dlg::OnBinarization();
	void CMFCApplication1Dlg::OnHistoStretch();
	void CMFCApplication1Dlg::OnHistoEqual();
	void CMFCApplication1Dlg::OnEmbossing();
	void CMFCApplication1Dlg::OnMaskProcess(unsigned char *Target, double Mask[3][3]);
	void CMFCApplication1Dlg::OnBlurr();
	void CMFCApplication1Dlg::OnSharpening();
	void CMFCApplication1Dlg::OnRoberts();
	void CMFCApplication1Dlg::OnSobel();
	void CMFCApplication1Dlg::OnPrewitt();
	void CMFCApplication1Dlg::OnLaplacian();
	void CMFCApplication1Dlg::OnGaussianFilter();
	void CMFCApplication1Dlg::OnLog();
	void CMFCApplication1Dlg::OnBilinear();
	void CMFCApplication1Dlg::OnMeanSub();
	void CMFCApplication1Dlg::OnMirrorHor();
	void CMFCApplication1Dlg::OnMirrorVer();
	void CMFCApplication1Dlg::OnRotation();
	void CMFCApplication1Dlg::OnBinaryErosion();
	void CMFCApplication1Dlg::OnBinaryDilation();
	void CMFCApplication1Dlg::OnBinaryOpening();
	void CMFCApplication1Dlg::OnBinaryClosing();
	void CMFCApplication1Dlg::OnLowPassFilter();
	void CMFCApplication1Dlg::OnHighPassFilter();
	void CMFCApplication1Dlg::OnMedianFilter();
	void CMFCApplication1Dlg::OnBubleSort(double * A, int MAX);
	void CMFCApplication1Dlg::OnSwap(double *a, double *b);
// 대화 상자 데이터입니다.1
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCAPPLICATION1_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;
	CImage m_image;
	double m_HIST[256];
	double m_Sum_Of_HIST[256];
	unsigned char m_Scale_HIST[256];
	int m_Re_height = 512;
	int m_Re_width = 512;
	int m_Re_size=m_Re_height*m_Re_width;
	unsigned char *m_OutputImage= new unsigned char[m_Re_height * m_Re_width ];

	// 이미지가 폭 512, 높이 512 크기인 8비트 이미지이기 때문에 512*512 크기의 메모리를 할당한다.
	unsigned char *p_src_image = new unsigned char[m_Re_height * m_Re_width ];
	// 출력할때 32비트 이미지로 출력하기 위해서 8비트 이미지를 32비트 이미지로 바꾸기 위해서 
	// 512*512*4 크기의 메모리를 할당한다.
	unsigned char *p_dest_image = new unsigned char[m_Re_height * m_Re_width * 4];

	// p_dest_image, p_src_image는 동적 할당된 메모리의 시작 주소를 기억하고 있어야지
	// 작업이 끝난 후에 동적 할당된 메모리의 주소를 해제할 수 있다.
	unsigned char *p_dest_pos = p_dest_image;
	unsigned char *p_src_pos = p_src_image;
	unsigned char *m_OutputImage_pos = m_OutputImage;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};
