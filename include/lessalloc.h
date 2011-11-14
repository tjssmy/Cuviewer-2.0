/*
This utility creates a dynamic memory allocation scheme in which
requested allocation size grows exponentially, reducing <new> calls.

You'll have to be up to date with C++ to use this effectively though.
Look at your favourite C++ book for the usual info. You'll definately
need to know about classes, a little about template classes, and
probably a little about exceptions (unless you're sure you'll always
have enough memory).

Memory management is all contained inside the template class, so no
need to worry about allocating or freeing. Pointers returned from this
need no alteration.

So, now to go over the functions. Of course, there's the constructor
and destructor, but there's really not too much to say about them.
(More info about the parameters of the constructor later, in case
you want to tweek the performance from the defaults a little.) Next is
"grab()", and it's used as follows:
>>> ob.grab();
"grab()" returns a pointer to a free memory block at the end of all
the allocated objects. So, you could do something (using int as the
type) like:
>>> *ob.grab() = 24;
to add 24 to the end of the "array". Keep on doing this to fill the
array with data. The address to a certain element in the array will
never change. In the case that the last element that was added should
need to be removed, "remove()" is what you want. It simply takes off
the last element, returning nothing. It is used as:
>>> ob.remove();
"kill()" on the other hand kills all of the data withing the template
class. It's a reset button in a certain way I suppose. The use:
>>> ob.kill();
You do NOT need to specify this before the template class is deleted.

Exceptions can be thrown from "grab()" A bad_alloc exceptionwill be
thrown by <new> when it cannot allocate the requested memory.To
catch everything, you can write:
>>> try
>>> {
>>>     ob.grab();
>>> }
>>> catch(...)
>>> {
>>>     cerr << "Memory problem" << endl;
>>> }

The next thing on the list is "operator[](unsigned long long
index)". This works just like any other array would, with a few
differences. The first difference is that because of the nature of the
design, the index has to be first found by a binary search, and then
pointer arithmatic arithmatic. You don't need to worry about that, but
you should realize that it will take an extra little time to find, so
use a quicker method if possible. The other main difference is that
bounds checking is preformed (only on the upper, since using
unsigned). If the index is past the end, then the pointer to the last
element is returned. This could cause a problem with debugging, but it
makes getting to the last element a snap. An example really isn't
needed, but here's one anyway (it sets int element 9 to 99):
>>> *ob[9] = 99;

The next section is on the pre/post (inc/dec)rement operators. These
operate much quicker than the [] operator, but can only go one element
at a time forward or backward. Typically, only pointer math is needed
to get to the correct element. All the operators preform as expected,
except they will not go out of bounds (they'll just return the pointer
on that extreme). As an example, to output the next index of the
array, write:
>>> cout << *(++ob);

The template class remembers the last pointer and index it was using,
so that is how it knows how to (inc/dec)rement. If you want to get the
last pointer used, use "operator()()". This can be very useful with
structures, when multiple sub-items must be added. The use is again
incredibly simple, and as an example, the current index will be output
(assuming that it can be...):
>>> cout << *ob();
The pointer that the () operator gives is changed by grab() and
Remove() (and obviously set to 0 by kill()), the [] operator, and all
the (inc/dec)rement operators. So, if you wanted to go through the
array from 0, you could use ob[0], then ++ through it.

As mentionned before, the template class remembers the index it's on
too. To access it, use the "index()" function. So,
>>> cout << ob.index();
will output the current index.

The size of the array is also kept (makes life easier when "for"ing
through the array). To get that, use the "size()" function. This
function returns the SIZE, not the last element in the array (the size
is one bigger than the last element's index). As an example, to output
everything in the array to the screen:
>>> ob[0];
>>> for (int i=0 ; i < ob.size() ; i++)
>>>     cout << *(ob++);

If you don't like how the template class is preforming for you, you
can always tinker with the parameters the constructor takes. maxAllocs
is the maximum amout of allocs internal data has to be restructured.
Restructuring takes a while, so if possible, plan ahead and avoid the
slow-down. startAllocSize is the size of the first allocation. Again,
adjust it to your taste; just remember to make sure when you int
divide it by growthDivisor, you get something bigger than 0, or things
won't be happy. growthDivisor is the number that the total amount of
objects allocated is divided by to yield the amount to allocate this
time. The reciprocal of this number is the fraction of the computer's
memory that will be allocated, but unused in the worst-case senario.

That should be about all that needs to be known. If not, then you can
always email me at cliff_dugal@bigfoot.com.

Cliff Dugal
*/

#ifndef lessalloc_h
#define lessalloc_h

#include <new>

#ifdef __GNUC__
#	define __lessa_uint64 unsigned long long
#	define __lessa_int64 long long
#else
#	if defined(__BORLANDC__) || defined(MSC)
#		define __lessa_uint64 unsigned __int64
#		define __lessa_int64 __int64
#	else
#		define __lessa_uint64 unsigned long long
#		define __lessa_int64 long long
#	endif
#endif

using namespace std;

template <typename StoreType> class LessAlloc
{
	void defaultConfig();
	void fillAllocs(__lessa_uint64 allocVect[], unsigned int vectSize);

	unsigned int       maxAllocs; // max allocs in the array to allow for
	__lessa_uint64 startAllocSize; // initial alloc item count
	__lessa_uint64 maxAllocSize; // max items to ever alloc
	__lessa_uint64 growthDivisor; // array exponential growth property
	
	StoreType **memPointer; // an array of starting pointer addresses
	StoreType *freePointer; // can be uninitialized when it is pointing past the end
	StoreType *currentPointer;
	StoreType *tempPointer;
	__lessa_uint64 currentIndex; // individual data index
	unsigned int currentIndexInside; // which memPointer is being used (to speed up pre/post fix)
	__lessa_uint64 *allocAmount; // array for alloc sizes (note: [0] = 0; +1 size)

	unsigned int grabIndexNext; // next index to grab
	__lessa_uint64 amtFree; // how much room is left in the current index
	__lessa_uint64 amtFreeTry; // temp
	__lessa_uint64 amtAlloc; // how many items are currently allocated (for bound checking)

	unsigned int low, high, mid; // for the [] operator's binary search

  public:
	LessAlloc(unsigned int maxAllocs=2049, __lessa_uint64 startAllocSize=128,
	__lessa_uint64 maxAllocSize=32768, __lessa_uint64 growthDivisor=8)
		throw(bad_alloc);
	~LessAlloc();

	StoreType *grab() throw(bad_alloc);
	// ***Remember to use "try...catch" just in case for new
	// ***Will also throw "End of memory index array"
	void remove();
	void kill();

	StoreType *operator[](__lessa_uint64 theIndex);
	StoreType *operator++();    // prefix
	StoreType *operator++(int); // postfix
	StoreType *operator--();    // prefix
	StoreType *operator--(int); // postfix
	StoreType *operator()() { return currentPointer; }

	__lessa_uint64 index() { return currentIndex; } 
	__lessa_uint64 size() { return amtAlloc; }
};

template <typename StoreType>
void LessAlloc<StoreType>::fillAllocs(__lessa_uint64 allocVect[],
									  unsigned int vectSize)
{
	bool keepGrowing = true;
	__lessa_uint64 toAdd = 0; // stop compiler from complaining
	
	if (vectSize > 0)
	{
		allocVect[0] = 0;
		
		if (vectSize > 1)
		{
			allocVect[1] = startAllocSize;
			
			for (unsigned int i = 2; i < vectSize ; i++)
			{
				if (keepGrowing)
					toAdd = allocVect[i-1] / growthDivisor;
				// else use the previous value
				
				if (toAdd > maxAllocSize)
				{
					toAdd = maxAllocSize;
					keepGrowing = false;
				}
				
				allocVect[i] = allocVect[i-1] + toAdd;
			}
		}

	}
}

template <typename StoreType> inline void LessAlloc<StoreType>::defaultConfig()
{
	grabIndexNext = 0;
	amtFree = 0;
	amtAlloc = 0;
	currentPointer = NULL;
	currentIndex = 0;
	currentIndexInside = 0;
}

template <typename StoreType>
LessAlloc<StoreType>::LessAlloc(unsigned int maxAllocs,
								__lessa_uint64 startAllocSize,
								__lessa_uint64 maxAllocSize,
								__lessa_uint64 growthDivisor) throw(bad_alloc)
{
	this->maxAllocs = maxAllocs;
	this->startAllocSize = startAllocSize;
	this->maxAllocSize = maxAllocSize;
	this->growthDivisor = growthDivisor;
	
	memPointer = new StoreType *[maxAllocs];
	allocAmount = new __lessa_uint64[maxAllocs+1];

	// initialize the allocation size array
	fillAllocs(allocAmount, maxAllocs+1);
	
	defaultConfig();
}

template <typename StoreType> inline LessAlloc<StoreType>::~LessAlloc()
{
	kill();
	
	delete[] allocAmount;
	delete[] memPointer;
}

template <typename StoreType> StoreType* LessAlloc<StoreType>::grab() throw(bad_alloc)
{
	if (!amtFree) // If the current memory is exhausted, grab more
	{
		if (grabIndexNext >= maxAllocs) // end of array
		{
			StoreType **memPointerTry;
			__lessa_uint64 *allocAmountTry;
			
			unsigned int maxAllocsTry = maxAllocs * 2;
			
			memPointerTry = new StoreType *[maxAllocsTry]; // exits here if cannot alloc
			try
			{
				allocAmountTry = new __lessa_uint64[maxAllocsTry+1];
			}
			catch (bad_alloc bad)
			{
				delete[] memPointerTry;
				throw; // rethrow the exception
			}
			
			// will only be here if allocation went well.
			
			// initialize the allocation size array
			fillAllocs(allocAmountTry, maxAllocsTry+1);
			
			// finally copy the old alloc positions
			for (unsigned int i = 0 ; i < maxAllocs ; i++)
			{
				memPointerTry[i] = memPointer[i];
			}
			
			// make changes permanent
			delete[] allocAmount;
			allocAmount = allocAmountTry;
			delete[] memPointer;
			memPointer = memPointerTry;
			maxAllocs = maxAllocsTry;
		}
		
		amtFreeTry = allocAmount[grabIndexNext + 1] - allocAmount[grabIndexNext];
		freePointer = memPointer[grabIndexNext] = new StoreType[amtFreeTry];
		
		grabIndexNext++; // not set above in case of a <new> throw
		amtFree = amtFreeTry;
	}
	currentPointer = freePointer++;
	currentIndexInside = grabIndexNext - 1;
	currentIndex = amtAlloc;

	amtFree--;
	amtAlloc++;

	return currentPointer;
}

template <typename StoreType> void LessAlloc<StoreType>::remove()
{
	if (amtAlloc == 1) // if removing the last element
	{
		kill();
	}
	else if (amtAlloc) // if there is stuff in the array
	{
		amtFree++;
		amtAlloc--;
		currentIndex = amtAlloc - 1;

		if ( amtFree == (allocAmount[grabIndexNext] - allocAmount[grabIndexNext-1]) ) // If re-sizing is needed
		{
			grabIndexNext--;
			delete[] memPointer[grabIndexNext];

			amtFree = 0;
			currentPointer = memPointer[grabIndexNext-1] + (currentIndex - allocAmount[grabIndexNext-1]);
			freePointer = currentPointer + 1; // freePointer is always one ahead
		}
		else // no re-sizing is needed
		{
			freePointer--;
			currentPointer = freePointer - 1;
		}
			currentIndexInside = grabIndexNext - 1;
	}
}

template <typename StoreType> void LessAlloc<StoreType>::kill()
{
	if (grabIndexNext > 0) // if not empty
	{
		for (unsigned int i = 0 ; i < grabIndexNext ; i++)
			delete[] memPointer[i];
		
		defaultConfig();
	}
}

template <typename StoreType> StoreType *LessAlloc<StoreType>::operator[](__lessa_uint64 theIndex)
{
	// binary search
	if (theIndex < amtAlloc) // check the range
	{
		low = 0; 
		high = grabIndexNext;

		while (low <= high)
		{
			mid = (low + high) / 2;

			if (theIndex < allocAmount[mid])
				high = mid - 1;
			else if (theIndex > allocAmount[mid])
				low = mid + 1;
			else
			{
				currentIndex = theIndex;
				currentIndexInside = mid;
				currentPointer = memPointer[mid];
				return currentPointer; // because of the array realationships
			}
		}

		// If the program is here, the index you are looking for is in the middle of an alloc.
		// This is what will normally happen. <high> will now contain the index to add to.
		// e.g. if [5] = 26 and [6] = 37, and you are looking for 30, <high> will be [5].

		currentIndex = theIndex;
		currentIndexInside = high;
		currentPointer = memPointer[high] + (currentIndex - allocAmount[high]);
		return currentPointer;
	}
	else if (amtAlloc) // return the last pointer
	{
		currentIndex = amtAlloc - 1;
		currentIndexInside = grabIndexNext - 1;
		currentPointer = memPointer[grabIndexNext-1] + (currentIndex - allocAmount[grabIndexNext-1]);
		return currentPointer;
	}
	else // There's nothing in the array to return
		return NULL;
}

template <typename StoreType> inline StoreType *LessAlloc<StoreType>::operator++() // prefix
{
	if (amtAlloc) // if there is stuff in the array
	{
		if (currentIndex < (amtAlloc - 1)) // if it's not already at the end
		{
			currentIndex++;

			if (currentIndex == allocAmount[currentIndexInside+1]) // if it's time to jump up an index
			{
				currentPointer = memPointer[++currentIndexInside];
			}
			else // nothing too hard; just increment the pointer
			{
				currentPointer++;
			}
		}

		return currentPointer;
	}
	else // There's nothing in the array
		return NULL;
}

template <typename StoreType> inline StoreType *LessAlloc<StoreType>::operator++(int) // postfix
{
	tempPointer = currentPointer;
	operator++();
	return tempPointer;
}

template <typename StoreType> inline StoreType *LessAlloc<StoreType>::operator--() // prefix
{
	if (amtAlloc) // if there is stuff in the array
	{
		if (currentIndex) // if it's not already 0
		{
			currentIndex--;

			if (currentIndex < allocAmount[currentIndexInside]) // if it's time to jump down an index
			{
				currentIndexInside--;
				currentPointer = memPointer[currentIndexInside] +
					(allocAmount[currentIndexInside+1] - allocAmount[currentIndexInside] - 1);
			}
			else // nothing too hard; just decrement the pointer
			{
				currentPointer--;
			}
		}

		return currentPointer;
	}
	else // There's nothing in the array
		return NULL;
}

template <typename StoreType> inline StoreType *LessAlloc<StoreType>::operator--(int) // postfix
{
	tempPointer = currentPointer;
	operator--();
	return tempPointer;
}

#endif
