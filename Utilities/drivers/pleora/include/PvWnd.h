// *****************************************************************************
//
//     Copyright (c) 2008, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#ifndef __PV_WND_H__
#define __PV_WND_H__

#include <PvGUILib.h>

#ifdef WIN32
#include <windows.h>
#endif

class Wnd;
class QWidget;


class PV_GUI_API PvWnd
{
public:

	void SetPosition( PvInt32  aPosX, PvInt32  aPosY, PvInt32  aSizeX, PvInt32  aSizeY );
	void GetPosition( PvInt32 &aPosX, PvInt32 &aPosY, PvInt32 &aSizeX, PvInt32 &aSizeY );

    PvResult ShowModal();
    PvResult ShowModal( QWidget* aParentHwnd );

    PvResult ShowModeless();
    PvResult ShowModeless( QWidget* aParentHwnd );

    PvResult Create( QWidget* aHwnd );

#ifdef WIN32
    PvResult ShowModal( HWND aParentHwnd );
    PvResult ShowModeless( HWND aParentHwnd );
	PvResult Create( HWND aHwnd );
#endif // WIN32

    PvString GetTitle() const;
	void SetTitle( const PvString &aTitle );
    QWidget* GetQWidget();

	PvResult Close();

    static void DoEvents();

protected:

    PvWnd();
	virtual ~PvWnd();

    Wnd *mThis;

private:

    // Not implemented
	PvWnd( const PvWnd & );
	const PvWnd &operator=( const PvWnd & );

};


#endif // __PV_WND_H__

