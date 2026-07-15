#pragma once

class CBookManageApp : public CWinApp
{
public:
    CBookManageApp();
    virtual ~CBookManageApp() = default;

    virtual BOOL InitInstance();
};

extern CBookManageApp theApp;
