/*
	GUIDO Library
	Copyright (C) 2002  Holger Hoos, Juergen Kilian, Kai Renz

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

#include "GraphTools.h"
#include "NVPoint.h"


// -----------------------------------------------------------------------------
/** \brief Calculate a Bezier curve.
 
	\param nSegs The number of segments used to build the curve. Use a small value (about 10 or 20) 
				for fast calculation and a large value (about 50-200) for better quality.
				Good software should use 1 segment to cover 2 or 3 screen pixels.

	Takes 4 control points x1,y1,x2,y2,x3,y3,x4,y4 and build a
	bezier curve using these points.

*/
void makeCurve( float x1, float y1, float x2, float y2, float x3, float y3,
				float x4, float y4, int nSegs, NVPoint point_array[], int * index )
{
	const float inc = float(1.0) / (float)nSegs;
	float u, b03, b13, b23, b33;
	float x, y;

	int tmpIndex = *index;
	point_array[ tmpIndex ].x = x1;
	point_array[ tmpIndex ].y = y1;
	++ tmpIndex;

	float uPow2;
	float minU;
	float minUPow2;
	for (u = 0; u <= 1; u += inc) 	// (JB) replaced all pow() by multiplications
	{
		uPow2 = u * u;
		minU = (1 - u);
		minUPow2 = minU * minU;
		
		b03 = minUPow2 * minU;					// was pow(1 - u, 3);
		b13 = 3 * u * minUPow2;					// was 3 * u * pow(1 - u, 2);
		b23 = 3 * uPow2 * minU;					// was 3 * pow(u, 2) * (1 - u);
		b33 = uPow2 * u;						// was pow(u, 3);

		x = (x1 * b03 +
			x2 * b13 +
			x3 * b23 +
			x4 * b33);
		
		y = (y1 * b03 +
			y2 * b13 +
			y3 * b23 +
			y4 * b33);

		point_array[ tmpIndex ].x = (float)x;
		point_array[ tmpIndex ].y = (float)y;
		++ tmpIndex;
	}

	point_array[ tmpIndex ].x = (float)x4;
	point_array[ tmpIndex ].y = (float)y4;
	++ tmpIndex;
	* index = tmpIndex;
}

