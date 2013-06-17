//=================================================================================================
/*!
//  \file blaze/math/typetraits/IsAbsExpr.h
//  \brief Header file for the IsAbsExpr type trait class
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

#ifndef _BLAZE_MATH_TYPETRAITS_ISABSEXPR_H_
#define _BLAZE_MATH_TYPETRAITS_ISABSEXPR_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <boost/type_traits/is_base_of.hpp>
#include <blaze/util/FalseType.h>
#include <blaze/util/SelectType.h>
#include <blaze/util/TrueType.h>


namespace blaze {

//=================================================================================================
//
//  ::blaze NAMESPACE FORWARD DECLARATIONS
//
//=================================================================================================

struct AbsExpr;




//=================================================================================================
//
//  CLASS DEFINITION
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Auxiliary helper struct for the IsAbsExpr type trait.
// \ingroup math_type_traits
*/
template< typename T >
struct IsAbsExprHelper
{
   //**********************************************************************************************
   enum { value = boost::is_base_of<AbsExpr,T>::value && !boost::is_base_of<T,AbsExpr>::value };
   typedef typename SelectType<value,TrueType,FalseType>::Type  Type;
   //**********************************************************************************************
};
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Compile time check whether the given type is an absolute value expression template.
// \ingroup math_type_traits
//
// This type trait class tests whether or not the given type \a Type is an absolute value
// expression template. In order to qualify as a valid absolute value expression template,
// the given type has to derive (publicly or privately) from the AbsExpr base class. In
// case the given type is a valid absolute value expression template, the \a value member
// enumeration is set to 1, the nested type definition \a Type is \a TrueType, and the class
// derives from \a TrueType. Otherwise \a value is set to 0, \a Type is \a FalseType, and
// the class derives from \a FalseType.
*/
template< typename T >
struct IsAbsExpr : public IsAbsExprHelper<T>::Type
{
 public:
   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   enum { value = IsAbsExprHelper<T>::value };
   typedef typename IsAbsExprHelper<T>::Type  Type;
   /*! \endcond */
   //**********************************************************************************************
};
//*************************************************************************************************

} // namespace blaze

#endif