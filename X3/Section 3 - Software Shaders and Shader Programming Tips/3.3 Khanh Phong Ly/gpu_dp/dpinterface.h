#ifndef __DPINTERFACE_H__
#define __DPINTERFACE_H__

#include <d3dx9.h>
#include <tchar.h>
#include "aopm.h"

class DPParams	// simply an interface to be inherited.
{
};



class DPBase
{
protected:

	int					m_DataSize;
	int					m_DataWidth;
	int					m_DataHeight;

	static const int	NUM_ACTIONS = 8;
	static const int	NUM_OUTCOMES = 8;

public:

	ActOutcomeProbMap		m_AOPM;

	static const char BOUNDVAL= 0;

	DPBase() : m_DataSize(0), m_DataWidth(0), m_DataHeight(0) 
	{}
	virtual ~DPBase(){}

	virtual HRESULT OneTimeSceneInit(DPParams* p)= 0;
	virtual HRESULT InitDeviceObjects(DPParams* p)= 0;
	virtual HRESULT RestoreDeviceObjects(DPParams* p)= 0;
	virtual HRESULT InvalidateDeviceObjects()= 0;
	virtual HRESULT DeleteDeviceObjects()= 0;
	virtual HRESULT FinalCleanup()= 0;

	virtual int IterateVals(int numIterations, bool bRestartFromInitialData) = 0;
	virtual int CreateActionMap() = 0;

	virtual int	GetAction( int row, int col )= 0;
	virtual float GetCurrValue( int row, int col )= 0;
	virtual bool IsBoundary(int row, int col)= 0;

	int			GetDataWidth(){return m_DataWidth;}
	int			GetDataHeight(){return m_DataHeight;}



};


#endif //__DPINTERFACE_H__