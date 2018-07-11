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

#ifndef CUVDATAUTILS_H
#define CUVDATAUTILS_H

#include <qstring.h>

inline void mirrorBytes(void *toMirror, const unsigned int byteSize)
{
	char temp;

	for (unsigned int i = 0 ; i < (byteSize / 2) ; i++)
	{
		temp = *( (char*)toMirror + i);
		*( (char *)toMirror + i) = *( (char *)toMirror + byteSize - 1 - i);
		*( (char *)toMirror + byteSize - 1 - i) = temp;
	}
	return;
}

inline bool getCUVUByte(QIODevice *ioDeviceIn, cuv_ubyte &toFill)
{
  if ( ioDeviceIn->read((char *)(&toFill), (uint) sizeof(toFill))
		 == /*(int)*/sizeof(toFill) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

inline bool getCUVUBytes(QIODevice *ioDeviceIn, cuv_ubyte *toFill,
						 const unsigned int count)
{
  if ( ((unsigned int) ioDeviceIn->read( (char*) toFill, (uint) count) )
		== (int)count )
	{
		return true;
	}
	else
	{
		return false;
	}
}

// WARNING: assumes that a tag is only 1 char in size, and
// therefore no byte mirrioring is needed.
inline bool getCUVTag(QIODevice *ioDeviceIn, cuv_tag &toFill)
{
  if ( ioDeviceIn->read((char *)(&toFill), (uint) sizeof(toFill))
		 == (int)sizeof(toFill) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

inline bool getCUVUInt(QIODevice *ioDeviceIn, cuv_uint &toFill, const bool bytesMirrored)
{
  if ( ioDeviceIn->read((char *)(&toFill), (uint) sizeof(toFill))
		 == (int)sizeof(toFill) )
	{
		if (bytesMirrored)
		{
			mirrorBytes(&toFill, sizeof(cuv_uint));
		}
		
		return true;
	}
	else
	{
		return false;
	}
}
	
inline bool getCUVFloats(QIODevice *ioDeviceIn, cuv_float *toFill,
						 const unsigned int count, const bool bytesMirrored)
{
  if ( ((unsigned int) ioDeviceIn->read( (char*) toFill,
												(uint) (sizeof(cuv_float) * count) ))
		 == (sizeof(cuv_float) * count) )
	{
		if (bytesMirrored)
		{
			for (unsigned int i=0 ; i < count ; i++)
				mirrorBytes(toFill+i, sizeof(cuv_float));
		}
		
		return true;
	}
	else
	{
		return false;
	}
}

inline bool getRawUTF16String(QIODevice *ioDeviceIn, QString &toFill,
			      const unsigned int charCount, const bool bytesMirrored)
{
#ifndef WIN32
  cuv_ushort tempData[charCount];
  
  if ( ((unsigned int) ioDeviceIn->read( (char*) tempData,
					      (uint) (sizeof(tempData[0]) * charCount) ))
       == (sizeof(tempData[0]) * charCount) )
    {
      QChar tempQData[charCount];
      
      if (bytesMirrored)
	{
	  for (unsigned int i=0 ; i < charCount ; i++)
	    mirrorBytes(tempData+i, sizeof(tempData[0]));
	}
		
      for (unsigned int i=0 ; i < charCount ; i++)
	{
	  tempQData[i].unicode() = (ushort) tempData[i];
	}
		
      toFill = QString(tempQData, charCount);
		
      return true;
    }
  else
    {
      return false;
    }
#endif
  return false;
}

#endif
