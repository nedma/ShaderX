#ifndef __WORLDTOSCREEN_H__
#define __WORLDTOSCREEN_H__

#include <d3dx9.h>
#include <tchar.h>

// WorldScreenTransform maps cell coords( row, col ) to screenX,screenY 
class WorldScreenTransform
{
	float		m_scrOffsetX;	// the centre of the row=0,col=0 cell on the screen.
	float		m_scrOffsetY;
	float		m_scrDeltaX;	// width of a cell on screen
	float		m_scrDeltaY;	
	int			m_ArraySizeRows;	// num of elems in data.
	int			m_ArraySizeCols;
	float		m_top,m_bottom,m_left,m_right;

public:
	virtual HRESULT InitDeviceObjects( float scrOffX, float scrOffY, float scrDeltaX, float scrDeltaY,
		int arraySizeRows, int arraySizeCols);
	virtual HRESULT DeleteDeviceObjects();

	HRESULT WorldToScreen( int row, int col, int& outScrX, int& outScrY );
	HRESULT ScreenToWorld( int scrX, int scrY, int& outRow, int& outCol );
	float	Get_ScrDeltaX(){return m_scrDeltaX;}
};

#endif __WORLDTOSCREEN_H__