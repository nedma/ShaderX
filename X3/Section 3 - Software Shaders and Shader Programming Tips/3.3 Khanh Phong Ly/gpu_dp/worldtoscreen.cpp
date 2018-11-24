#include "worldtoscreen.h"
#include <assert.h>


//-----------------------------------------------------------------------------

HRESULT WorldScreenTransform::InitDeviceObjects( float scrOffX, float scrOffY, float scrDeltaX, float scrDeltaY,
												int arraySizeRows, int arraySizeCols)
{
	m_scrOffsetX	= scrOffX;
	m_scrOffsetY	= scrOffY;
	m_scrDeltaX		= scrDeltaX;
	m_scrDeltaY		= scrDeltaY;
	m_ArraySizeRows = arraySizeRows;
	m_ArraySizeCols = arraySizeCols;

	m_left = scrOffX;// - 0.5f*scrDeltaX;
	m_right = m_left + arraySizeCols*scrDeltaX;
	m_top	= scrOffY;// -0.5f*scrDeltaY;
	m_bottom  = m_top + arraySizeRows*scrDeltaY;

	return S_OK;
}

HRESULT WorldScreenTransform::DeleteDeviceObjects()
{	return S_OK; }


HRESULT WorldScreenTransform::WorldToScreen( int row, int col, int& outScrX, int& outScrY )
{
	if ( (col >= m_ArraySizeCols) || (col<0) || (row>= m_ArraySizeRows) || (row<0) )
		return E_FAIL;
	outScrX = (int)( (0.5f+float(col))*m_scrDeltaX + m_scrOffsetX);
	outScrY = (int)( (0.5f+float(row))*m_scrDeltaY + m_scrOffsetY);
	return S_OK;
}

HRESULT WorldScreenTransform::ScreenToWorld( int scrX, int scrY, int& outRow, int& outCol )
{
	// find row,col cell closest to point.
	// return error if point is outside of screen area of data map.
	//	float top = m_scrOffsetX
	if ( (scrX > m_right) || (scrX<m_left) || ( scrY < m_top ) || (scrY> m_bottom) )
		return E_FAIL;

	float col = (float)(scrX - m_left)/(float)m_scrDeltaX;
	outCol = int(col);

	float row = (float)(scrY - m_top)/(float)m_scrDeltaY;
	outRow = int(row);

	assert ( !(col >= m_ArraySizeCols) && !(col<0) && !(row>= m_ArraySizeRows) && !(row<0) );
	return S_OK;
}
