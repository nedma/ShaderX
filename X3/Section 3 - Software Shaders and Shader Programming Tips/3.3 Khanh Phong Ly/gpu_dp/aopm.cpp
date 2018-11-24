#include <assert.h>
#include <d3dx9.h>
#include <tchar.h>
#include "aopm.h"



ActOutcomeProbMap::ActOutcomeProbMap() : m_pNearestNeighbours(0), 
m_pAOPMDat(0), m_pDistribProbs(0), m_NumActions(0), m_NumOutcomes(0)
{
}

ActOutcomeProbMap::~ActOutcomeProbMap()
{
	// client is responsible for clearing of m_pAOPMDat;
	delete [] m_pNearestNeighbours;
	delete [] m_pDistribProbs;
}

void ActOutcomeProbMap::Init(float *pToClientAlloc, int numActions, int numOutcomes)
{
	m_NumActions = numActions;
	m_NumOutcomes= numOutcomes;

	float		*prb	= pToClientAlloc;// new float[numOutcomes];
	m_pAOPMDat			= prb;

	m_pDistribProbs		= new float[numOutcomes];

	// set probabilites of intended action and probable outcomes.
	// we choose a simple distribution whereby the chance of moving to a box 
	// has small probability of landing in the boxes next to the dest box.

	//  0  1  2
	//  3     4
	//  5  6  7

	m_pNearestNeighbours= new int[numActions*numOutcomes]; // including the cell itself. Right cells have priority over left cells.

	int i=0;
	m_pNearestNeighbours[i*numOutcomes+ 0]= 0;
	m_pNearestNeighbours[i*numOutcomes+ 1]= 1;
	m_pNearestNeighbours[i*numOutcomes+ 2]= 3;
	m_pNearestNeighbours[i*numOutcomes+ 3]= 2;
	m_pNearestNeighbours[i*numOutcomes+ 4]= 5;
	m_pNearestNeighbours[i*numOutcomes+ 5]= 4;
	m_pNearestNeighbours[i*numOutcomes+ 6]= 6;
	m_pNearestNeighbours[i*numOutcomes+ 7]= 7;
	++i;
	m_pNearestNeighbours[i*numOutcomes+ 0]= 1;
	m_pNearestNeighbours[i*numOutcomes+ 1]= 2;
	m_pNearestNeighbours[i*numOutcomes+ 2]= 0;
	m_pNearestNeighbours[i*numOutcomes+ 3]= 4;
	m_pNearestNeighbours[i*numOutcomes+ 4]= 3;
	m_pNearestNeighbours[i*numOutcomes+ 5]= 7;
	m_pNearestNeighbours[i*numOutcomes+ 6]= 5;
	m_pNearestNeighbours[i*numOutcomes+ 7]= 6;
	++i;
	m_pNearestNeighbours[i*numOutcomes+ 0]= 2;
	m_pNearestNeighbours[i*numOutcomes+ 1]= 4;
	m_pNearestNeighbours[i*numOutcomes+ 2]= 1;
	m_pNearestNeighbours[i*numOutcomes+ 3]= 7;
	m_pNearestNeighbours[i*numOutcomes+ 4]= 0;
	m_pNearestNeighbours[i*numOutcomes+ 5]= 6;
	m_pNearestNeighbours[i*numOutcomes+ 6]= 3;
	m_pNearestNeighbours[i*numOutcomes+ 7]= 5;
	++i;
	m_pNearestNeighbours[i*numOutcomes+ 0]= 3;
	m_pNearestNeighbours[i*numOutcomes+ 1]= 0;
	m_pNearestNeighbours[i*numOutcomes+ 2]= 5;
	m_pNearestNeighbours[i*numOutcomes+ 3]= 1;
	m_pNearestNeighbours[i*numOutcomes+ 4]= 6;
	m_pNearestNeighbours[i*numOutcomes+ 5]= 2;
	m_pNearestNeighbours[i*numOutcomes+ 6]= 7;
	m_pNearestNeighbours[i*numOutcomes+ 7]= 4;
	++i;
	m_pNearestNeighbours[i*numOutcomes+ 0]= 4;
	m_pNearestNeighbours[i*numOutcomes+ 1]= 7;
	m_pNearestNeighbours[i*numOutcomes+ 2]= 2;
	m_pNearestNeighbours[i*numOutcomes+ 3]= 6;
	m_pNearestNeighbours[i*numOutcomes+ 4]= 1;
	m_pNearestNeighbours[i*numOutcomes+ 5]= 5;
	m_pNearestNeighbours[i*numOutcomes+ 6]= 0;
	m_pNearestNeighbours[i*numOutcomes+ 7]= 3;
	++i;
	m_pNearestNeighbours[i*numOutcomes+ 0]= 5;
	m_pNearestNeighbours[i*numOutcomes+ 1]= 3;
	m_pNearestNeighbours[i*numOutcomes+ 2]= 6;
	m_pNearestNeighbours[i*numOutcomes+ 3]= 0;
	m_pNearestNeighbours[i*numOutcomes+ 4]= 7;
	m_pNearestNeighbours[i*numOutcomes+ 5]= 1;
	m_pNearestNeighbours[i*numOutcomes+ 6]= 4;
	m_pNearestNeighbours[i*numOutcomes+ 7]= 2;
	++i;
	m_pNearestNeighbours[i*numOutcomes+ 0]= 6;
	m_pNearestNeighbours[i*numOutcomes+ 1]= 5;
	m_pNearestNeighbours[i*numOutcomes+ 2]= 7;
	m_pNearestNeighbours[i*numOutcomes+ 3]= 3;
	m_pNearestNeighbours[i*numOutcomes+ 4]= 4;
	m_pNearestNeighbours[i*numOutcomes+ 5]= 0;
	m_pNearestNeighbours[i*numOutcomes+ 6]= 2;
	m_pNearestNeighbours[i*numOutcomes+ 7]= 1;
	++i;
	m_pNearestNeighbours[i*numOutcomes+ 0]= 7;
	m_pNearestNeighbours[i*numOutcomes+ 1]= 6;
	m_pNearestNeighbours[i*numOutcomes+ 2]= 4;
	m_pNearestNeighbours[i*numOutcomes+ 3]= 5;
	m_pNearestNeighbours[i*numOutcomes+ 4]= 2;
	m_pNearestNeighbours[i*numOutcomes+ 5]= 3;
	m_pNearestNeighbours[i*numOutcomes+ 6]= 1;
	m_pNearestNeighbours[i*numOutcomes+ 7]= 0;
	++i;

	// probability distribution. [0] is the chance of ending up where you intended.
	// [1]. is the chance of ending up at next nearest neighbours in the direction intended.
	m_pDistribProbs[0] = 0.9f;
	m_pDistribProbs[1] = 0.05f;
	m_pDistribProbs[2] = 0.05f;
	m_pDistribProbs[3] = 0.0f;
	m_pDistribProbs[4] = 0.0f;
	m_pDistribProbs[5] = 0.0f;
	m_pDistribProbs[6] = 0.0f;
	m_pDistribProbs[7] = 0.0f;


//	float checksum = (m_pDistribProbs[0] + 2*(m_pDistribProbs[1]+m_pDistribProbs[2]+m_pDistribProbs[3]) + m_pDistribProbs[4]);
	float checksum =0;

	for ( i=0; i<numOutcomes; i++ )
	{ checksum+= m_pDistribProbs[i]; }

	float eps = 1e-6f;
	assert( checksum <= 1+ eps );
	assert( checksum >= 1- eps );



	for ( i=0; i<numActions; i++ )
	{
		memset( prb, 0, numOutcomes*sizeof(float) );
		prb[m_pNearestNeighbours[i*numOutcomes+ 0]] = m_pDistribProbs[0];
		prb[m_pNearestNeighbours[i*numOutcomes+ 1]] = m_pDistribProbs[1];
		prb[m_pNearestNeighbours[i*numOutcomes+ 2]] = m_pDistribProbs[2];
		prb[m_pNearestNeighbours[i*numOutcomes+ 3]] = m_pDistribProbs[3];
		prb[m_pNearestNeighbours[i*numOutcomes+ 4]] = m_pDistribProbs[4];
		prb[m_pNearestNeighbours[i*numOutcomes+ 5]] = m_pDistribProbs[5];
		prb[m_pNearestNeighbours[i*numOutcomes+ 6]] = m_pDistribProbs[6];
		prb[m_pNearestNeighbours[i*numOutcomes+ 7]] = m_pDistribProbs[7];
		prb+=numOutcomes;
	}

}


int	ActOutcomeProbMap::GetOutcomeActionID( int intendedActionID )
{
	int ir = rand();
	float p = float(ir)/float(RAND_MAX);

	float	threshold= m_pDistribProbs[0];
	int i;
	for ( i=0; i< m_NumOutcomes; i++ )
	{
		if ( p < threshold )
		{
			// get outcomeid and return
			return m_pNearestNeighbours[intendedActionID*m_NumOutcomes+ i];
		}
		threshold+= m_pDistribProbs[i+1]; // should have returned by i=m_NumOutcomes-1;
		assert( threshold <=1.0f ); 
	}

	return intendedActionID;


}

