/**
    @file cuvvector.h
    @brief This is a collection of vector operators.

    Operations include
     - copying the contents of one vector to another
     - checking for equality
     - vector addition
     - vector subtraction
     - vector scaling
     - dot multiplication
     - performing cross multiplication
     - getting the length of a vector
     - vector normalization
     - vector rotation

     Note that the vectors used are contained in arrays.
  */

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

#ifndef CUVVECTOR_H
#define CUVVECTOR_H

#include <math.h>
#include "cuvtags.h"

/**
    @brief Copys all 4 components of a vector to another 4-component vector.

    The vcopy4 function copys the components of from to the respective components of to.
  */
inline void vcopy4(cuv_float to[4], const cuv_float from[4])
{
	to[0]=from[0];
	to[1]=from[1];
	to[2]=from[2];
	to[3]=from[3];
	return;
}

/**
    @brief Copys a 3-component vector to a 4-component vector.

    The vcopy3to4 function copys the 3 components to the first 3 components of the
    4-component vector and sets the last component to 1.0.

    This function should be used primarily for colours.
  */
inline void vcopy3to4(cuv_float to[4], const cuv_float from[3]) // used for colours
{
	to[0]=from[0];
	to[1]=from[1];
	to[2]=from[2];
	to[3]=1.0;
	return;
}

/**
    @brief Copys a 3-component vector to another 3-component vector.

    The vcopy3 function copys the components of from to the respective components of to.
  */
inline void vcopy3(cuv_float to[3], const cuv_float from[3])
{
	to[0]=from[0];
	to[1]=from[1];
	to[2]=from[2];
	return;
}

/**
    @brief Checks two 3-component vectors for equality.

    The vequal3 function checks if each component in both vectors are equivalent.

    @return true - if all components of both vectors are equivalent
    @return false - otherwise
  */
inline bool vequal3(const cuv_float vect1[3], const cuv_float vect2[3])
{
	return ( (vect1[0]==vect2[0]) && (vect1[1]==vect2[1]) && (vect1[2]==vect2[2]) ) ? true : false;
}

/**
    @brief Checks if two 3-component vectors are not equal.

    The vunequal3 function checks if one of the
    corresponding components in both vectors are not equivalent.

    @return true - if one of the components are unequal
    @return false - otherwise
  */
inline bool vunequal3(const cuv_float vect1[3], const cuv_float vect2[3])
{
	return ( (vect1[0]!=vect2[0]) || (vect1[1]!=vect2[1]) || (vect1[2]!=vect2[2]) ) ? true : false;
}

/**
    @brief Checks two 4-component vectors for equality.

    @return true - if all components of both vectors are equal
    @return false - otherwise
  */
inline bool vequal4(const cuv_float vect1[4], const cuv_float vect2[4])
{
	return ( (vect1[0]==vect2[0]) && (vect1[1]==vect2[1]) && (vect1[2]==vect2[2]) && (vect1[3]==vect2[3]) ) ? true : false;
}

/**
    @brief Adds the components of two vectors

    The vadd function adds the components of vect1 and
    vect2 and stores the results in ans.

    In other words, ans:= vect1 + vect2.
  */
inline void vadd(cuv_float ans[3], const cuv_float vect1[3], const cuv_float vect2[3])
{
	ans[0] = vect1[0] + vect2[0];
	ans[1] = vect1[1] + vect2[1];
	ans[2] = vect1[2] + vect2[2];
	return;
}

/**
    @brief Subtracts the components of two vectors

    The vsub function subtracts the components of vect1 and vect2 and stores the results in ans.

    In other words, ans := vect1 - vect2.
  */
inline void vsub(cuv_float ans[3], const cuv_float vect1[3], const cuv_float vect2[3])
{
	ans[0] = vect1[0] - vect2[0];
	ans[1] = vect1[1] - vect2[1];
	ans[2] = vect1[2] - vect2[2];
	return;
}

/**
    @brief Scales the vector.

    The vscale function multiplies vect by the scaleFactor. The result is stored in vect.
  */
inline void vscale(cuv_float vect[3], cuv_float scaleFactor)
{
  vect[0] *= scaleFactor;
  vect[1] *= scaleFactor;
  vect[2] *= scaleFactor;
	return;
}

/**
    @brief Performs dot product operation on two vectors

    The vdot function takes the dot product of vect1 and vect2 and returns the result.

    @return The dot product of vect1 and vect2
  */
inline cuv_float vdot(const cuv_float vect1[3], const cuv_float vect2[3])
{
	return (vect1[0] * vect2[0] + vect1[1] * vect2[1] + vect1[2] * vect2[2]);
}

/**
    @brief vcross Takes the cross product of two vectors

    The vcross function takes the cross product of vect1 and vect2 and stores the results in ans.

    @return true - If one of the components in ans are non-zero.
    @return false - If all components in ans are zero.
  */
inline bool vcross(cuv_float ans[3], const cuv_float vect1[3], const cuv_float vect2[3]) 
{
	ans[0] = vect1[1] * vect2[2] - vect1[2] * vect2[1];
	ans[1] = vect1[2] * vect2[0] - vect1[0] * vect2[2];
	ans[2] = vect1[0] * vect2[1] - vect1[1] * vect2[0];

	return (ans[0] || ans[1] || ans[2]) ? true : false;
}

/**
    @brief Returns the length of the vector.

    The vlength function calculates the length of vect and returns it.

    @return Length of vector.
  */
inline cuv_float vlength(const cuv_float vect[3])
{
	return sqrt(vdot(vect, vect));
}

/**
    @brief Normalizes the vector.

    The vnormalize function takes vect and normalizes it
    (i.e. change it to have a length of 1).

    @return true - If normalizing vect was successful.
    @return false - If normalizing vect failed (likely
                    due to vect having a length of 0).
  */
inline bool vnormalize(cuv_float vect[3])
{
	cuv_float length;
	
	length = vlength(vect);

	if (length)
	{
		vect[0] /= length;
		vect[1] /= length;
		vect[2] /= length;
		return true;
	}
	else
	{
		return false;
	}
}

/**
    @brief Rotates the vector about an axis by a number of degrees.

    The vrotate function rotates vect a given amount of degrees
    about an axis given by rot and stores the result in ans.

    The rot vector contains the angle of rotation in degrees and the
    axis direction which vect will rotate about.

    The first component of rot (rot[0]) contains the angle while the
    other three components (rot[1], rot[2], rot[3]) contain the
    x, y, and z components of the vector that vect will rotate about.
  */
void vrotate(cuv_float ans[3], const cuv_float vect[3], const cuv_float rot[4]);

#endif
