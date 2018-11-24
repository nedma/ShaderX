#include <windows.h>
#include <stdio.h>
#include "resource.h"
#include "dialogs.h"
#include "light.h"

extern LightStruct Lights[3];
extern int NumberOfPhongLights;

/*****************************************************************************
 * Function Name  : DlgProcAbout
 * Inputs		  : HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam
 * Returns		  : BOOL
 * Description    : Displays the About dialog box
 *****************************************************************************/
BOOL CALLBACK DlgProcAbout(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char	buf[256];

	switch(uMsg) {
	case WM_INITDIALOG:
		/* Dialog box title */
		sprintf(buf, "About %s", PROGRAM_TITLE);
		SetWindowText(hWndDlg, buf);

		/* Program name */
		SetDlgItemText(hWndDlg, IDC_STATIC1, PROGRAM_TITLE);

		/* Program version */
		sprintf(buf, "Product Version: %s", PROGRAM_VERSION_STRING);
		SetDlgItemText(hWndDlg, IDC_STATIC2, buf);

		/* Compilation date */
		sprintf(buf, "%s compilation: %s (%s)",
#ifdef _DEBUG
			"Debug",
#else
			"Release",
#endif
			__DATE__, __TIME__);

		SetDlgItemText(hWndDlg, IDC_STATIC3, buf);

		/* Program author */
		sprintf(buf, "Author: %s", PROGRAM_AUTHOR_STRING);
		SetDlgItemText(hWndDlg, IDC_STATIC4, buf);

		return 1;

	case WM_ACTIVATEAPP:
		if(!(BOOL)wParam) {
			EndDialog(hWndDlg, 0);
			return 0;
		}
		return 1;
	case WM_COMMAND:
		if(LOWORD(wParam) == IDOK) 
		{
			EndDialog(hWndDlg, 0);
			return 1;
		}

	}

	return 0;
}

/*****************************************************************************
 * Function Name  : DlgProcPhongLights
 * Inputs		  : HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam
 * Returns		  : BOOL
 * Description    : Displays the light information dialog box
 *****************************************************************************/
BOOL CALLBACK DlgProcPhongLights(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndComboBox = 0;
	static HWND hwndCheckBox = 0;
	static HWND hwndLightRed = 0;
	static HWND hwndLightGreen = 0;
	static HWND hwndLightBlue = 0;
	static HWND hwndLightX = 0;
	static HWND hwndLightY = 0;
	static HWND hwndLightZ = 0;
	static HWND hwndLightAnimated = 0;
	static HWND hwndNumberPhongLights = 0;
	char text[8][8] = {"Light 1", "Light 2", "Light 3", "Light 4", "Light 5", "Light 6", "Light 7", "Light 8"};
	int i;
	static int currentLight = 0;
	char buf[4];
	UINT status;

	switch(uMsg) 
	{
	case WM_INITDIALOG:
		hwndComboBox = GetDlgItem(hWndDlg, IDC_COMBO1);
		hwndCheckBox = GetDlgItem(hWndDlg, IDC_LIGHTENABLED);
		hwndLightAnimated = GetDlgItem(hWndDlg, IDC_LIGHTANIMATED);
		hwndLightRed = GetDlgItem(hWndDlg, IDC_LIGHTRED);
		hwndLightGreen = GetDlgItem(hWndDlg, IDC_LIGHTGREEN);
		hwndLightBlue = GetDlgItem(hWndDlg, IDC_LIGHTBLUE);
		hwndLightX = GetDlgItem(hWndDlg, IDC_LIGHTX);
		hwndLightY = GetDlgItem(hWndDlg, IDC_LIGHTY);
		hwndLightZ = GetDlgItem(hWndDlg, IDC_LIGHTZ);
		hwndNumberPhongLights = GetDlgItem(hWndDlg, IDC_EDIT1);

		sprintf(buf, "%d", NumberOfPhongLights);
		SendMessage(hwndNumberPhongLights, WM_SETTEXT, 0, (LPARAM) buf); 
		SendMessage(hwndComboBox, CB_RESETCONTENT, 0, 0); 
		for(i=0; i<8; i++)
			SendMessage(hwndComboBox, CB_ADDSTRING,  0, (LPARAM) text[i]); 
		SendMessage(hwndComboBox, CB_SETCURSEL, 0, 0);
		if(Lights[0].bEnabled)
		{
			SendMessage(hwndCheckBox, BM_SETCHECK, (WPARAM)BST_CHECKED, 0);
		}
		else
		{
			SendMessage(hwndCheckBox, BM_SETCHECK, (WPARAM)BST_UNCHECKED, 0);
		}
		if(Lights[currentLight].bDoAnimate)
		{
			SendMessage(hwndLightAnimated, BM_SETCHECK, (WPARAM)BST_CHECKED, 0);
		}
		else
		{
			SendMessage(hwndLightAnimated, BM_SETCHECK, (WPARAM)BST_UNCHECKED, 0);
		}
		sprintf(buf, "%d", (int)(Lights[0].colour[0] * 255));
		SendMessage(hwndLightRed, WM_SETTEXT, 0, (LPARAM) buf); 
		sprintf(buf, "%d", (int)(Lights[0].colour[1] * 255));
		SendMessage(hwndLightGreen, WM_SETTEXT, 0, (LPARAM) buf); 
		sprintf(buf, "%d", (int)(Lights[0].colour[2] * 255));
		SendMessage(hwndLightBlue, WM_SETTEXT, 0, (LPARAM) buf); 
		sprintf(buf, "%d", (int)(Lights[0].position[0]));
		SendMessage(hwndLightX, WM_SETTEXT, 0, (LPARAM) buf); 
		sprintf(buf, "%d", (int)(Lights[0].position[1]));
		SendMessage(hwndLightY, WM_SETTEXT, 0, (LPARAM) buf); 
		sprintf(buf, "%d", (int)(Lights[0].position[2]));
		SendMessage(hwndLightZ, WM_SETTEXT, 0, (LPARAM) buf); 
		return 1;

	case WM_ACTIVATEAPP:
		if(!(BOOL)wParam) {
			EndDialog(hWndDlg, 0);
			return 0;
		}
		return 1;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_COMBO1:
			switch(HIWORD(wParam))
			{
			case CBN_SELCHANGE:
				currentLight = (int)SendMessage(hwndComboBox, CB_GETCURSEL, 0, 0);
				if(Lights[currentLight].bEnabled)
				{
					SendMessage(hwndCheckBox, BM_SETCHECK, (WPARAM)BST_CHECKED, 0);
				}
				else
				{
					SendMessage(hwndCheckBox, BM_SETCHECK, (WPARAM)BST_UNCHECKED, 0);
				}
				if(Lights[currentLight].bDoAnimate)
				{
					SendMessage(hwndLightAnimated, BM_SETCHECK, (WPARAM)BST_CHECKED, 0);
				}
				else
				{
					SendMessage(hwndLightAnimated, BM_SETCHECK, (WPARAM)BST_UNCHECKED, 0);
				}
				sprintf(buf, "%d", (int)(Lights[currentLight].colour[0] * 255));
				SendMessage(hwndLightRed, WM_SETTEXT, 0, (LPARAM) buf); 
				sprintf(buf, "%d", (int)(Lights[currentLight].colour[1] * 255));
				SendMessage(hwndLightGreen, WM_SETTEXT, 0, (LPARAM) buf); 
				sprintf(buf, "%d", (int)(Lights[currentLight].colour[2] * 255));
				SendMessage(hwndLightBlue, WM_SETTEXT, 0, (LPARAM) buf); 
				sprintf(buf, "%d", (int)(Lights[currentLight].position[0]));
				SendMessage(hwndLightX, WM_SETTEXT, 0, (LPARAM) buf); 
				sprintf(buf, "%d", (int)(Lights[currentLight].position[1]));
				SendMessage(hwndLightY, WM_SETTEXT, 0, (LPARAM) buf); 
				sprintf(buf, "%d", (int)(Lights[currentLight].position[2]));
				SendMessage(hwndLightZ, WM_SETTEXT, 0, (LPARAM) buf); 
				break;
			}
			break;
		case IDOK:
			status = SendMessage(hwndLightAnimated, BM_GETCHECK, (WPARAM)0, (LPARAM)0);   
			if (status == BST_CHECKED)
				Lights[currentLight].bDoAnimate = TRUE;
			else
				Lights[currentLight].bDoAnimate = FALSE;

			/* get back the colours from the dialog box */
			SendMessage(hwndLightRed, WM_GETTEXT, 10, (LPARAM) buf); 
			Lights[currentLight].colour[0] = atoi(buf) / 255.0f;
			SendMessage(hwndLightGreen, WM_GETTEXT, 10, (LPARAM) buf); 
			Lights[currentLight].colour[1] = atoi(buf) / 255.0f;
			SendMessage(hwndLightBlue, WM_GETTEXT, 10, (LPARAM) buf); 
			Lights[currentLight].colour[2] = atoi(buf) / 255.0f;

			EndDialog(hWndDlg, 0);
			return 1;
		case IDCANCEL:
			EndDialog(hWndDlg, 0);
			return 1;
		}
		break;
	}

	return 0;
}
