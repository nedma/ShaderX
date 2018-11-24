#ifndef __DPSOFT_H__
#define __DPSOFT_H__

#include "dpinterface.h"

class DPGPU;
struct DPSoft_Params : public DPParams
{
	DPSoft_Params(): m_pDPGPU(0)
	{}
	
	DPGPU*				m_pDPGPU;
};


class DPSoft : public DPBase
{
	struct NeighbourMask
	{
		unsigned char	m_mask[NUM_ACTIONS];
	};

	float				m_AOPMData[NUM_ACTIONS*NUM_OUTCOMES];
	int					m_CurrValBuffID;
	int					m_NextValBuffID;
	float				*m_pCurrValueMap[2];	// double buffer to update new vals to.
	float				*m_pValueMap;
	NeighbourMask		*m_pNeighbourhoodMask;
	unsigned char		*m_pActionMap;
	unsigned char		*m_pBoundaryMap;

	HRESULT InitValueMap(DPParams* p);
	HRESULT InitNeighbourhoodMask(DPParams* p);

	inline float MinActionCost( float actionCost[] );
	void AddCost( int row, int col, float cost );
	void ComputeActionCosts( int row, int col, float actionCost[]  );
//	inline float GetCurrValue( int row, int col );
	inline void SetNextCurrValue( int row, int col, float val );
	inline void SetAction( int row, int col, unsigned char val );
	inline unsigned char GetMinCostActionID( float actionCost[] );

	inline NeighbourMask* GetNeighbourMask( int row, int col );

	void GetNeighbourVals( int row, int col, float outArray[] );
	float ComputeCostOfAction( int row, int col, float cachedNeighbourVals[], int actionID );

public:

	DPSoft();
	virtual ~DPSoft(){}

	virtual HRESULT OneTimeSceneInit(DPParams* p);
	virtual HRESULT InitDeviceObjects(DPParams* p);
	virtual HRESULT RestoreDeviceObjects(DPParams* p);
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
	virtual HRESULT FinalCleanup();

	virtual int IterateVals(int numIterations, bool bRestartFromInitialData);
	virtual int CreateActionMap();

	virtual int	GetAction( int row, int col );
	virtual float GetCurrValue( int row, int col );
	virtual bool IsBoundary(int row, int col);
	inline float GetOrigValue( int row, int col );
};




#endif // __DPSOFT_H__