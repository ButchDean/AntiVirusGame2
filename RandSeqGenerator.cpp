#include <stdio.h>
#include "RandSeqGenerator.h"

const long	IA		= 16807;
const long	IM		= 2147483647;
const float AM		= 1.0f / (float) IM;
const long	IQ		= 127773;
const long	IR		= 2836;
const int	NTAB	= 32;
const long	NDIV	= 1 + (IM - 1) / NTAB;
const double EPS		= 1.2e-7;
const double RNMX	= 1.0f - EPS;

void PositionPowerUps(int *powerUpOrder)
{
	static long initSeed = -IM;
	float randFloat;
	int randNum;
	bool zeroSelected = false, oneSelected = false, twoSelected = false, threeSelected = false, fourSelected = false;
	static bool indexStored;				// If value is going to be stored.
	int i = 0;								// Position index.

	while(!zeroSelected || !oneSelected || !twoSelected || !threeSelected || !fourSelected)
	{
		randFloat = RandGen(&initSeed);
		randNum = (int)(1e6 * randFloat) % 5;

		switch(randNum)
		{
			case 0:
				if(!zeroSelected)
				{
					powerUpOrder[i] = randNum;
					indexStored = true;
				}
				else indexStored = false;	// Not stored, so go through iteration again.
				zeroSelected = true;		// We know that this value has been selected and either stored or not.
				break;
			case 1:
				if(!oneSelected)
				{
					powerUpOrder[i] = randNum;
					indexStored = true;
				}
				else indexStored = false;
				oneSelected = true;
				break;
			case 2:
				if(!twoSelected)
				{
					powerUpOrder[i] = randNum;
					indexStored = true;
				}
				else indexStored = false;
				twoSelected = true;
				break;
			case 3:
				if(!threeSelected)
				{
					powerUpOrder[i] = randNum;
					indexStored = true;
				}
				else indexStored = false;
				threeSelected = true;
				break;
			case 4:
				if(!fourSelected)
				{
					powerUpOrder[i] = randNum;
					indexStored = true;
				}
				else indexStored = false;
				fourSelected = true;
				break;
		}

		if(indexStored)
			i++;						// Increment the index if successful.
		else
			indexStored = true;			// If unsuccessful storage, let index remain at current value and set indexStored to receive next value.
	}
}

float RandGen(long *seed)
// Function borrowed from Numerical Recipes in C, Chapter 7.
{
	int j;
	long k;
	static long iy = 0;
	static long iv[NTAB];
	float temp;

	if(*seed <= 0 || !iy)
	{
		if(-*seed < 1)
			*seed = 1;
		else
			*seed = -*seed;

		for(j = NTAB + 7; j >= 0; j--)
		{
			k = *seed / IQ;
			*seed = IA * (*seed - k * IQ) - IR * k;

			if(*seed < 0)
				*seed += IM;

			if(j < NTAB)
				iv[j] = *seed;

		}
		iy = iv[0];
	}

	k = *seed / IQ;
	*seed = IA * (*seed  - k * IQ) - IR * k;

	if(*seed < 0)
		*seed += IM;

	j = iy / NDIV;

	iy = iv[j];

	iv[j] = *seed;

	if((temp = AM * iy) > RNMX)
		return (float) RNMX;
	else
		return temp;
}