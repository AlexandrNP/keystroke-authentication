// WinFormInput.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Enums.h"
#include "WinFormInput.h"
#include "Counter.h"
#include "XmlCredentialsParser.h"
#include "AnomalyDetector.h"


#include <string>
#include <regex>

#define MAX_LOADSTRING 100

// Global Variables:
WNDPROC DefEditProc;
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

bool initTraining = false;
Credentials cred;
Counter counter;
Modes currentMode = E_AUTHENTICATION_MODE;
std::vector<double> keyDownTimes;
std::vector<std::vector<double>> trainingData;
int numberOfRemainingTrainings = 0;

const int ID_OK_BUTTON = 2;
const int ID_TRAINING_MODE_BUTTON = 3;
const int ID_AUTHENTICATION_MODE_BUTTON = 4;
const int ID_LOGIN_INPUT = 5;
const int ID_PASSWORD_INPUT = 6;
const int ID_MODE_TEXT = 8;
const int ID_LOGIN_TEXT = 9;
const int ID_PASSWORD_TEXT = 10;

const int MAX_INPUT_LENGTH = 30;
const int MIN_LOGIN_LENGTH = 4;
const int MIN_PASSWORD_LENGTH = 6;
const int NUMBER_OF_TRAINING_SAMPLES = 10;
const std::wstring FILE_PATH = L"credentials.xml";


// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK NewEditProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

void trainingButtonAction(HWND hWnd);
void authenticationButtonAction(HWND hWnd);
void okButtonTrainingModeAction(HWND hWnd);
void okButtonAuthenticationModeAction(HWND hWnd);
void okButtonAction(HWND hWnd);
void initializeTraining(HWND hWnd, const std::wstring& passwordTextPart1, 
			const std::wstring& passwordTextPart2);
void train(HWND hWnd, const std::wstring& passwordTextPart1, 
			const std::wstring& passwordTextPart2);
bool readCredentialsByLogin(Credentials& cred, const std::wstring& login,
			const std::wstring& filePath);
bool writeCredentials(Credentials& cred, const std::wstring& filePath);
bool checkNewLogin(const std::wstring& login, const std::wstring& filePath);
bool checkPasswordLength(const std::wstring& password);
bool checkFeaturesNumber(unsigned int featuresNumber, unsigned int passwordLength);



int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WINFORMINPUT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINFORMINPUT));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINFORMINPUT));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_WINFORMINPUT);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance;

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
      CW_USEDEFAULT, 0, 400, 270, NULL, NULL, hInstance, NULL);

	 
	 HWND loginInput = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), 0, 
																		WS_CHILD | WS_VISIBLE | WS_BORDER,  50, 70, 150, 22,
																		hWnd,  (HMENU) ID_LOGIN_INPUT,  hInst,  0);


	 HWND passwordInput = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"),  0, 
														WS_CHILD | WS_VISIBLE | WS_BORDER | ES_PASSWORD, 
														50, 130, 150, 22, hWnd, (HMENU) ID_PASSWORD_INPUT, hInst, 0);

	 DefEditProc = (WNDPROC)GetWindowLongPtr(passwordInput,GWLP_WNDPROC);
	 SetWindowLongPtr(passwordInput,GWLP_WNDPROC,(LPARAM)NewEditProc);

	
	 HWND trainingButton = CreateWindowEx(0, TEXT("BUTTON"), TEXT("Training"), 
														WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 250, 100, 120, 30, 
														hWnd, (HMENU) ID_TRAINING_MODE_BUTTON, hInst, 0);

	 HWND authenticationButton = CreateWindowEx(0, TEXT("BUTTON"), TEXT("Authentication"), 
																	WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 250, 60, 120, 30, 
																	hWnd, (HMENU) ID_AUTHENTICATION_MODE_BUTTON, hInst, 0);

	 HWND okButton = CreateWindowEx(0, TEXT("BUTTON"), TEXT("OK"), 
											WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 85, 165, 70, 28, 
											hWnd, (HMENU) ID_OK_BUTTON, hInst, 0);


	 HWND modeText = CreateWindowEx(0, TEXT("STATIC"), TEXT("Authentication mode"), 
												WS_CHILD | WS_VISIBLE, 50, 10, 150, 20, 
												hWnd, (HMENU) ID_MODE_TEXT, hInst, NULL);


	 HWND loginText = CreateWindowEx(0, TEXT("STATIC"), TEXT("Enter login:"), 
												WS_CHILD | WS_VISIBLE, 50, 50, 150, 20, 
												hWnd, (HMENU) ID_LOGIN_TEXT, hInst, NULL);


	 HWND passwordText = CreateWindowEx(0, TEXT("STATIC"), TEXT("Enter password:"), 
													WS_CHILD | WS_VISIBLE, 50, 110, 150, 20, 
													hWnd, (HMENU) ID_PASSWORD_TEXT, hInst, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}



LRESULT CALLBACK NewEditProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	std::string inputSymbol = "";
	const int ENTER = 13;
	const int BACKSPACE = 8;
	const std::regex restrictions("[a-zA-Z0-9@#$%^&+=-_]");
	switch(message)
	{
	case WM_KEYDOWN:
		inputSymbol.push_back(char(LOWORD(wParam)));
		if(std::regex_match(inputSymbol, restrictions))
		{
			counter.startCounter();
		}
		inputSymbol.clear();
		break;
	case WM_KEYUP:
		if(BACKSPACE == LOWORD(wParam))
		{
			if(!keyDownTimes.empty())
			{
				keyDownTimes.pop_back();
			}
			break;
		}

		inputSymbol.push_back(char(LOWORD(wParam)));
		if(std::regex_match(inputSymbol, restrictions))
		{
			keyDownTimes.push_back(counter.getCounter());
		}
		break;
	case WM_CHAR:
		if(LOWORD(wParam) == ENTER)
		{
				okButtonAction(GetParent(hwnd));
		}
		break;
	}
	return((LRESULT)CallWindowProc((WNDPROC)DefEditProc,hwnd,message,wParam,lParam));
}


//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:

		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_TRAINING_MODE_BUTTON:
			trainingButtonAction(hWnd);
			break;
		case ID_AUTHENTICATION_MODE_BUTTON:
			authenticationButtonAction(hWnd);
			break;
		case ID_OK_BUTTON:
			okButtonAction(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}




//---------------------------------
void trainingButtonAction(HWND hWnd)
{
	SetWindowText(GetDlgItem(hWnd, ID_MODE_TEXT), L"Training mode");
	SetWindowText(GetDlgItem(hWnd, ID_LOGIN_TEXT), L"Select login:");
	SetWindowText(GetDlgItem(hWnd, ID_PASSWORD_TEXT), L"Select password:");
	SetWindowText(GetDlgItem(hWnd, ID_LOGIN_INPUT), L"");
	SetWindowText(GetDlgItem(hWnd, ID_PASSWORD_INPUT), L"");
	trainingData.clear();
	currentMode = E_TRAINING_MODE;
	SetWindowLongPtr(GetDlgItem(hWnd, ID_LOGIN_INPUT), GWL_STYLE, WS_CHILD | WS_VISIBLE | WS_BORDER);
	initTraining = false;
}

//-------------------------------
void authenticationButtonAction(HWND hWnd)
{
	SetWindowText(GetDlgItem(hWnd, ID_MODE_TEXT), L"Authentication mode");
	SetWindowText(GetDlgItem(hWnd, ID_LOGIN_TEXT), L"Enter login:");
	SetWindowText(GetDlgItem(hWnd, ID_PASSWORD_TEXT), L"Enter password:");
	SetWindowText(GetDlgItem(hWnd, ID_LOGIN_INPUT), L"");
	SetWindowText(GetDlgItem(hWnd, ID_PASSWORD_INPUT), L"");
	SetWindowLongPtr(GetDlgItem(hWnd, ID_LOGIN_INPUT), GWL_STYLE, WS_CHILD | WS_VISIBLE | WS_BORDER);
	currentMode = E_AUTHENTICATION_MODE;
}

//-----------------------------------
void okButtonAction(HWND hWnd)
{
	DWORD sleepTime = 1500;
	switch(currentMode)
	{
	case E_TRAINING_MODE:
		SetWindowLongPtr(GetDlgItem(hWnd, ID_PASSWORD_INPUT), GWL_STYLE, WS_CHILD | WS_DISABLED | WS_VISIBLE | WS_BORDER);
		Sleep(sleepTime);
		okButtonTrainingModeAction(hWnd);
		SetWindowLongPtr(GetDlgItem(hWnd, ID_PASSWORD_INPUT), GWL_STYLE, WS_CHILD | WS_VISIBLE | WS_BORDER);
		break;
	case E_AUTHENTICATION_MODE:
		okButtonAuthenticationModeAction(hWnd);
		break;
	}
	keyDownTimes.clear();
	SetWindowText(GetDlgItem(hWnd, ID_PASSWORD_INPUT), L"");
}


//-----------------------------------
void okButtonTrainingModeAction(HWND hWnd)
{
	const std::wstring passwordTextPart1 = L"Remained ",
		passwordTextPart2 = L" reenters";

	if(false == initTraining)
	{
		initializeTraining(hWnd, passwordTextPart1, passwordTextPart2);
	}
	else
	{
		train(hWnd, passwordTextPart1, passwordTextPart2);
	}

	if(0 == numberOfRemainingTrainings)
	{
		try
		{
			AnomalyDetector detector(trainingData);
			cred.means = detector.getMeans();
			cred.deviation = detector.getDeviation();
			cred.threshold = detector.getThreshold();
		}
		catch(DataProcessigException err)
		{
			MessageBox(0, err.getReason().c_str(), L"Result", MB_OK);
			return;
		}

		if(!writeCredentials(cred, FILE_PATH))
		{
			return;
		}

		MessageBox(0, L"Training is successfully finished!", L"Result", MB_OK);
		authenticationButtonAction(hWnd);
	}
	keyDownTimes.clear();
	SetFocus(GetDlgItem(hWnd, ID_PASSWORD_INPUT));
}


//-----------------------------
void okButtonAuthenticationModeAction(HWND hWnd)
{
	wchar_t login[MAX_INPUT_LENGTH];
	wchar_t password[MAX_INPUT_LENGTH];

	GetDlgItemText(hWnd, ID_LOGIN_INPUT, login, MAX_INPUT_LENGTH);
	GetDlgItemText(hWnd, ID_PASSWORD_INPUT, password, MAX_INPUT_LENGTH);	

	if(!readCredentialsByLogin(cred, login, FILE_PATH))
	{
		return;
	}

	if(!cred.isPasswordCorrect(password))
	{
		MessageBox(0, L"Incorrect login or password", L"Result", MB_OK);
		SetWindowText(GetDlgItem(hWnd, ID_PASSWORD_INPUT), L"");
		return;
	}

	AnomalyDetector detector;
	detector.setMeans(cred.means);
	detector.setDeviation(cred.deviation);
	detector.setThreshold(cred.threshold);
	if(detector.isNormal(keyDownTimes))
	{
		MessageBox(0, L"Authentication succeeded!", L"Result", MB_OK);
	}
	else
	{
		MessageBox(0, L"Authentication failed!", L"Result", MB_OK);
	}
	SetFocus(GetDlgItem(hWnd, ID_PASSWORD_INPUT));
}


//-----------------------------------
void initializeTraining(HWND hWnd, const std::wstring& passwordTextPart1, const std::wstring& passwordTextPart2)
{
	wchar_t login[MAX_INPUT_LENGTH];
	wchar_t password[MAX_INPUT_LENGTH];
	numberOfRemainingTrainings = NUMBER_OF_TRAINING_SAMPLES - 1;

	GetDlgItemText(hWnd, ID_LOGIN_INPUT, login, MAX_INPUT_LENGTH);
	GetDlgItemText(hWnd, ID_PASSWORD_INPUT, password, MAX_INPUT_LENGTH);					

	cred.clear();

	if(!checkNewLogin(login, FILE_PATH) || !checkPasswordLength(password))
	{
		return;
	}

	cred.login = login;
	cred.password = password;

	if(!checkFeaturesNumber(keyDownTimes.size(), cred.password.length()))
	{
		return;
	}
	trainingData.push_back(keyDownTimes);
	SetWindowLongPtr(GetDlgItem(hWnd, ID_LOGIN_INPUT), GWL_STYLE, WS_CHILD | WS_DISABLED |
		WS_VISIBLE | WS_BORDER);
	SetWindowText(GetDlgItem(hWnd, ID_PASSWORD_TEXT), 
		(passwordTextPart1 + 
		std::to_wstring((UINT64) numberOfRemainingTrainings) + 
		passwordTextPart2).c_str());
	initTraining = true;
}

//----------------------------------
void train(HWND hWnd, const std::wstring& passwordTextPart1, const std::wstring& passwordTextPart2)
{
	wchar_t password[MAX_INPUT_LENGTH];

	GetDlgItemText(hWnd, ID_PASSWORD_INPUT, password, MAX_INPUT_LENGTH);
	if(!cred.isPasswordCorrect(password))
	{
		MessageBox(0, L"Passwords does not match", L"Result", MB_OK);
		return;
	}

	if(!checkFeaturesNumber(keyDownTimes.size(), cred.password.length()))
	{
		return;
	}

	--numberOfRemainingTrainings;
	trainingData.push_back(keyDownTimes);
	SetWindowText(GetDlgItem(hWnd, ID_PASSWORD_TEXT), 
		(passwordTextPart1 + std::to_wstring((UINT64) numberOfRemainingTrainings) +
		passwordTextPart2).c_str());
}



//-----------------------------------
bool readCredentialsByLogin(Credentials& userCredentials, const std::wstring& login, const std::wstring& filePath)
{
	try
	{
		XmlCredentialsParser parser;
		parser.setFilePath(filePath);
		userCredentials = parser.getCredentialsByLogin(login);
	}
	catch (XmlParsingException err)
	{
		MessageBox(0, err.getReason().c_str(),	L"Result", MB_OK);
		return false;
	}

	return true;
}


//-----------------------------------
bool writeCredentials(Credentials& userCredentials, const std::wstring& filePath)
{
	try
	{
		XmlCredentialsParser parser;
		parser.setFilePath(filePath);
		parser.writeCredentials(userCredentials);
	}
	catch (XmlParsingException err)
	{
		MessageBox(0, err.getReason().c_str(),	L"Result", MB_OK);
		return false;
	}

	return true;
}


//-----------------------------------
bool checkNewLogin(const std::wstring& login, const std::wstring& filePath)
{
	Credentials fileEntry;
	const std::wstring emptyString = L"";

	if(!readCredentialsByLogin(fileEntry, login, filePath))
	{
		return false;
	}

	if(0 != emptyString.compare(fileEntry.login))
	{
		MessageBox(0, L"Login already exists",	L"Result", MB_OK);
		return false;
	}

	if(login.length() < MIN_LOGIN_LENGTH)
	{
		MessageBox(0, (L"Login should have at least " + std::to_wstring((UINT64) MIN_LOGIN_LENGTH) 
			+ L" characters").c_str(),	L"Result", MB_OK);
		return false;
	}

	return true;
}


//-----------------------------------
bool checkPasswordLength(const std::wstring& password)
{
	if(password.length() < MIN_PASSWORD_LENGTH)
	{
		MessageBox(0, (L"Password should have at least " + std::to_wstring((UINT64) MIN_PASSWORD_LENGTH) 
			+ L" characters").c_str(),	L"Result", MB_OK);
		return false;
	}

	return true;
}


//-----------------------------------
bool checkFeaturesNumber(unsigned int featuresNumber, unsigned int passwordLength)
{
	if(featuresNumber != passwordLength)
	{
		MessageBox(0, L"Error processing password. Have you stroke several keys at once?", 
			L"Result", MB_OK);
		return false;
	}
	return true;
}