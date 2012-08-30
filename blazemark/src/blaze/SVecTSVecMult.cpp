//=================================================================================================
/*!
//  \file src/blaze/SVecTSVecMult.cpp
//  \brief Source file for the Blaze sparse vector/sparse vector outer product kernel
//
//  Copyright (C) 2011 Klaus Iglberger - All Rights Reserved
//
//  This file is part of the Blaze library. This library is free software; you can redistribute
//  it and/or modify it under the terms of the GNU General Public License as published by the
//  Free Software Foundation; either version 3, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along with a special
//  exception for linking and compiling against the Blaze library, the so-called "runtime
//  exception"; see the file COPYING. If not, see http://www.gnu.org/licenses/.
*/
//=================================================================================================


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <iostream>
#include <blaze/math/CompressedMatrix.h>
#include <blaze/math/CompressedVector.h>
#include <blaze/util/Timing.h>
#include <blazemark/blaze/init/CompressedVector.h>
#include <blazemark/blaze/SVecTSVecMult.h>
#include <blazemark/system/Config.h>


namespace blazemark {

namespace blaze {

//=================================================================================================
//
//  KERNEL FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Blaze sparse vector/sparse vector outer product kernel.
//
// \param N The size of the vectors for the outer product.
// \param F The number of non-zero elements for the sparse vectors.
// \param steps The number of iteration steps to perform.
// \return Minimum runtime of the kernel function.
//
// This kernel function implements the sparse vector/sparse vector outer product by means of
// the Blaze functionality.
*/
double svectsvecmult( size_t N, size_t F, size_t steps )
{
   using ::blazemark::element_t;
   using ::blaze::rowVector;
   using ::blaze::columnVector;
   using ::blaze::rowMajor;

   ::blaze::setSeed( seed );

   ::blaze::CompressedVector<element_t,columnVector> a( N );
   ::blaze::CompressedVector<element_t,rowVector> b( N );
   ::blaze::CompressedMatrix<element_t,rowMajor> A( N, N );
   ::blaze::timing::WcTimer timer;

   init( a, F );
   init( b, F );

   A = a * b;

   for( size_t rep=0UL; rep<reps; ++rep )
   {
      timer.start();
      for( size_t step=0UL; step<steps; ++step ) {
         A = a * b;
      }
      timer.end();

      if( A.rows() != N )
         std::cerr << " Line " << __LINE__ << ": ERROR detected!!!\n";

      if( timer.last() > maxtime )
         break;
   }

   const double minTime( timer.min()     );
   const double avgTime( timer.average() );

   if( minTime * ( 1.0 + deviation*0.01 ) < avgTime )
      std::cerr << " Blaze kernel 'svectsvecmult': Time deviation too large!!!\n";

   return minTime;
}
//*************************************************************************************************

} // namespace blaze

} // namespace blazemark
