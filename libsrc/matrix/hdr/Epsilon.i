//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// calculate a minimum precission for a data type

namespace ombt {

template <class T>
T
calcEpsilon(T)
{
	T f1 = 1.0;
	T f2 = 1.0;
	T oldf2 = 0.0;

	for (f2 /= 2.0; ((f1-f2) != f1); oldf2=f2, f2 /= 2.0) ;
	return(T(2.0)*oldf2);
}

}
