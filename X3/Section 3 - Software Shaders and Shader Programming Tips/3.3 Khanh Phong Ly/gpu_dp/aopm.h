#ifndef __AOMP_H__
#define __AOMP_H__

class ActOutcomeProbMap			// action outcome probability map.
{
	int						*m_pNearestNeighbours;
	float					*m_pAOPMDat;
	float					*m_pDistribProbs;	// probabilites of outcomes.
	int						m_NumActions;
	int						m_NumOutcomes;

public:

	ActOutcomeProbMap();
	~ActOutcomeProbMap();

	void Init(float *pToClientAlloc, int numActions, int numOutcomes);
	float *GetAOPMDataPtr(){ return m_pAOPMDat; }
	int	GetOutcomeActionID( int intendedActionID );

};




#endif //__AOMP_H__
