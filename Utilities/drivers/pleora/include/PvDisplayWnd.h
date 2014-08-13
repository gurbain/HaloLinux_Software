// *****************************************************************************
//
//     Copyright (c) 2007, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#ifndef __PVDISPLAY_H__
#define __PVDISPLAY_H__


#include <PvGUILib.h>
#include <PvWnd.h>
#include <PvBuffer.h>


class DisplayWnd;


typedef enum
{
	PvRendererModeDefault = 0,
	PvRendererModeAccelerated = 1,
	PvRendererModeHardwareAccelerated = 2,
	PvRendererModeUndefined = 999,

} PvRendererMode;


class PV_GUI_API PvDisplayWnd : public PvWnd
{
public:

    PvDisplayWnd();
    virtual ~PvDisplayWnd();

    PvResult Display( const PvBuffer &aBuffer, bool aVSync = false );
    PvResult SetBackgroundColor( PvUInt8 aR, PvUInt8 aG, PvUInt8 aB );
    PvBufferConverter &GetConverter();

    PvRendererMode GetRendererMode();
    PvResult SetRendererMode( PvRendererMode aMode );

	void Clear();

    static PvRendererMode GetHighestRendererMode();
    static PvString GetRenderer();
    static PvString GetRendererVersion();

    void SetTextOverlay( const PvString &aText );
    void SetTextOverlayColor( PvUInt8 aR, PvUInt8 aG, PvUInt8 aB );
    PvResult SetTextOverlaySize( PvInt32 aSize );
    void SetTextOverlayOffsetX( PvUInt32 aX );
    void SetTextOverlayOffsetY( PvUInt32 aY );

    PvString GetTextOverlay() const;
    void GetTextOverlayColor( PvUInt8 &aR, PvUInt8 &aG, PvUInt8 &aB ) const;
    PvInt32 GetTextOverlaySize() const;
    PvInt32 GetTextOverlayOffsetX() const;
    PvInt32 GetTextOverlayOffsetY() const;

protected:

private:

    // Not implemented
    PvDisplayWnd( const PvDisplayWnd & );
    const PvDisplayWnd &operator=( const PvDisplayWnd & );

};


#endif // __PVDISPLAY_H__
