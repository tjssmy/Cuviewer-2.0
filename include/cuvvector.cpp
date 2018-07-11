/*
Copyright (C) 1999, 2000 Carleton University Department of Electronics

This file is part of CU-Viewer.

CU-Viewer is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation; either version 2, or (at your option) any later version. 

CU-Viewer is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. 

You should have received a copy of the GNU General Public License along with
program; see the file COPYING. If not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include "cuvvector.h"
#define _USE_MATH_DEFINES
#include <cmath>

/*
  For formula refer to Rodrigues' rotation formula (See Wikipedia article on rotation matrix)
  ans = R * vect
            [x^2(1-cosA)+cosA  xy(1-cosA)-zsinA  xz(1-cosA)+ysinA]
  where R = [xy(1-cosA)+zsinA  y^2(1-cosA)+cosA  yz(1-cosA)-zsinA]
            [xz(1-cosA)-ysinA  yz(1-cosA)+xsinA  z^2(1-cosA)+cosA]
   normlz = transpose of [x y z]
  */
void vrotate(cuv_float ans[3], const cuv_float vect[3], const cuv_float rot[4])
{
	cuv_float normlz[3];
	cuv_float cosA, sinA, oneSubCosA;
	cuv_float xyMultOneSubCosA, xzMultOneSubCosA, yzMultOneSubCosA;
	cuv_float xMultSinA, yMultSinA, zMultSinA;
	
	vcopy3(normlz, (rot+1));
	vnormalize(normlz);
	
	cosA = cos(rot[0] * M_PI / 180.0);
	sinA = sin(rot[0] * M_PI / 180.0);
	
	oneSubCosA = 1.0 - cosA;
	
	xyMultOneSubCosA = normlz[0] * normlz[1] * oneSubCosA;
	xzMultOneSubCosA = normlz[0] * normlz[2] * oneSubCosA;
	yzMultOneSubCosA = normlz[1] * normlz[2] * oneSubCosA;
	
	xMultSinA = normlz[0] * sinA;
	yMultSinA = normlz[1] * sinA;
	zMultSinA = normlz[2] * sinA;
	
	ans[0] = (normlz[0] * normlz[0] * oneSubCosA + cosA) * vect[0] +
			 (xyMultOneSubCosA - zMultSinA) * vect[1] +
			 (xzMultOneSubCosA + yMultSinA) * vect[2];
	
	ans[1] = (xyMultOneSubCosA + zMultSinA) * vect[0] +
			 (normlz[1] * normlz[1] * oneSubCosA + cosA) * vect[1] +
			 (yzMultOneSubCosA - xMultSinA) * vect[2];
	
	ans[2] = (xzMultOneSubCosA - yMultSinA) * vect[0] +
			 (yzMultOneSubCosA + xMultSinA) * vect[1] +
			 (normlz[2] * normlz[2] * oneSubCosA + cosA) * vect[2];
	
	return;
}
