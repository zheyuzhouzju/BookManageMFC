#include "pch.h"
#include "BookManageMFC.h"
#include "LoginDlg.h"

CBookManageApp theApp;

CBookManageApp::CBookManageApp() {}

BOOL CBookManageApp::InitInstance()
{
    CWinApp::InitInstance();

    SetRegistryKey(_T("BookManageMFC"));

    LoginDlg loginDlg;
    INT_PTR nResponse = loginDlg.DoModal();

    if (nResponse != IDOK)
        return FALSE;

    return FALSE;
}
