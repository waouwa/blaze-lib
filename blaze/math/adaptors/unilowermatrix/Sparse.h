//=================================================================================================
/*!
//  \file blaze/math/adaptors/unilowermatrix/Sparse.h
//  \brief UniLowerMatrix specialization for sparse matrices
//
//  Copyright (C) 2013 Klaus Iglberger - All Rights Reserved
//
//  This file is part of the Blaze library. You can redistribute it and/or modify it under
//  the terms of the New (Revised) BSD License. Redistribution and use in source and binary
//  forms, with or without modification, are permitted provided that the following conditions
//  are met:
//
//  1. Redistributions of source code must retain the above copyright notice, this list of
//     conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice, this list
//     of conditions and the following disclaimer in the documentation and/or other materials
//     provided with the distribution.
//  3. Neither the names of the Blaze development group nor the names of its contributors
//     may be used to endorse or promote products derived from this software without specific
//     prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
//  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
//  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
//  SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
//  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
//  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
//  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
//  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
//  DAMAGE.
*/
//=================================================================================================

#ifndef _BLAZE_MATH_ADAPTORS_UNILOWERMATRIX_SPARSE_H_
#define _BLAZE_MATH_ADAPTORS_UNILOWERMATRIX_SPARSE_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <algorithm>
#include <stdexcept>
#include <vector>
#include <blaze/math/adaptors/Forward.h>
#include <blaze/math/adaptors/unilowermatrix/BaseTemplate.h>
#include <blaze/math/adaptors/unilowermatrix/UniLowerProxy.h>
#include <blaze/math/constraints/Expression.h>
#include <blaze/math/constraints/Lower.h>
#include <blaze/math/constraints/Resizable.h>
#include <blaze/math/constraints/SparseMatrix.h>
#include <blaze/math/constraints/StorageOrder.h>
#include <blaze/math/constraints/Symmetric.h>
#include <blaze/math/constraints/Upper.h>
#include <blaze/math/expressions/SparseMatrix.h>
#include <blaze/math/Functions.h>
#include <blaze/math/shims/Clear.h>
#include <blaze/math/shims/IsDefault.h>
#include <blaze/math/shims/Move.h>
#include <blaze/math/sparse/SparseElement.h>
#include <blaze/math/sparse/SparseMatrix.h>
#include <blaze/math/typetraits/IsComputation.h>
#include <blaze/math/typetraits/IsResizable.h>
#include <blaze/math/typetraits/IsSquare.h>
#include <blaze/math/typetraits/IsStrictlyLower.h>
#include <blaze/math/typetraits/IsUniLower.h>
#include <blaze/math/typetraits/Rows.h>
#include <blaze/util/Assert.h>
#include <blaze/util/constraints/Const.h>
#include <blaze/util/constraints/Numeric.h>
#include <blaze/util/constraints/Pointer.h>
#include <blaze/util/constraints/Reference.h>
#include <blaze/util/constraints/Volatile.h>
#include <blaze/util/DisableIf.h>
#include <blaze/util/EnableIf.h>
#include <blaze/util/typetraits/IsNumeric.h>
#include <blaze/util/Types.h>


namespace blaze {

//=================================================================================================
//
//  CLASS TEMPLATE SPECIALIZATION FOR SPARSE MATRICES
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Specialization of UniLowerMatrix for sparse matrices.
// \ingroup unilower_matrix
//
// This specialization of UniLowerMatrix adapts the class template to the requirements of sparse
// matrices.
*/
template< typename MT  // Type of the adapted sparse matrix
        , bool SO >    // Storage order of the adapted sparse matrix
class UniLowerMatrix<MT,SO,false>
   : public SparseMatrix< UniLowerMatrix<MT,SO,false>, SO >
{
 private:
   //**Type definitions****************************************************************************
   typedef typename MT::OppositeType   OT;  //!< Opposite type of the sparse matrix.
   typedef typename MT::TransposeType  TT;  //!< Transpose type of the sparse matrix.
   typedef typename MT::ElementType    ET;  //!< Element type of the sparse matrix.
   //**********************************************************************************************

 public:
   //**Type definitions****************************************************************************
   typedef UniLowerMatrix<MT,SO,false>  This;            //!< Type of this UniLowerMatrix instance.
   typedef This                         ResultType;      //!< Result type for expression template evaluations.
   typedef UniLowerMatrix<OT,!SO,false> OppositeType;    //!< Result type with opposite storage order for expression template evaluations.
   typedef UniUpperMatrix<TT,!SO,false> TransposeType;   //!< Transpose type for expression template evaluations.
   typedef ET                           ElementType;     //!< Type of the matrix elements.
   typedef typename MT::ReturnType      ReturnType;      //!< Return type for expression template evaluations.
   typedef const This&                  CompositeType;   //!< Data type for composite expression templates.
   typedef UniLowerProxy<MT>            Reference;       //!< Reference to a non-constant matrix value.
   typedef typename MT::ConstReference  ConstReference;  //!< Reference to a constant matrix value.
   typedef typename MT::ConstIterator   ConstIterator;   //!< Iterator over constant elements.
   //**********************************************************************************************

   //**Rebind struct definition********************************************************************
   /*!\brief Rebind mechanism to obtain a UniLowerMatrix with different data/element type.
   */
   template< typename ET >  // Data type of the other matrix
   struct Rebind {
      //! The type of the other UniLowerMatrix.
      typedef UniLowerMatrix< typename MT::template Rebind<ET>::Other >  Other;
   };
   //**********************************************************************************************

   //**UniLowerValue class definition*************************************************************
   /*!\brief Representation of a value within the sparse lower unitriangular matrix.
   */
   class UniLowerValue
   {
    private:
      //**struct BuiltinType***********************************************************************
      /*!\brief Auxiliary struct to determine the value type of the represented complex element.
      */
      template< typename T >
      struct BuiltinType { typedef INVALID_TYPE  Type; };
      //*******************************************************************************************

      //**struct ComplexType***********************************************************************
      /*!\brief Auxiliary struct to determine the value type of the represented complex element.
      */
      template< typename T >
      struct ComplexType { typedef typename T::value_type  Type; };
      //*******************************************************************************************

    public:
      //**Type definitions*************************************************************************
      //! Value type of the represented complex element.
      typedef typename If< IsComplex<ElementType>
                         , ComplexType<ElementType>
                         , BuiltinType<ElementType> >::Type::Type  ValueType;

      typedef ValueType  value_type;  //!< Value type of the represented complex element.
      //*******************************************************************************************

      //**Constructor******************************************************************************
      /*!\brief Constructor for the UniLowerValue class.
      //
      // \param value Reference to the represented value.
      // \param diagonal \a true in case the element is on the diagonal, \a false if not.
      */
      inline UniLowerValue( ElementType& value, bool diagonal )
         : value_   ( &value   )  // The represented value.
         , diagonal_( diagonal )  // true in case the element is on the diagonal, false if not
      {}
      //*******************************************************************************************

      //*******************************************************************************************
      /*!\brief Copy assignment operator for UniLowerValue.
      //
      // \param uv The unilower value to be copied.
      // \return Reference to the assigned unilower value.
      // \exception std::invalid_argument Invalid assignment to diagonal matrix element.
      */
      inline UniLowerValue& operator=( const UniLowerValue& uv ) {
         if( diagonal_ )
            throw std::invalid_argument( "Invalid assignment to diagonal matrix element" );
         *value_ = *uv.value;
         return *this;
      }
      //*******************************************************************************************

      //*******************************************************************************************
      /*!\brief Assignment to the unilower value.
      //
      // \param v The new value of the unilower value.
      // \return Reference to the assigned unilower value.
      // \exception std::invalid_argument Invalid assignment to diagonal matrix element.
      */
      template< typename T > inline UniLowerValue& operator=( const T& v ) {
         if( diagonal_ )
            throw std::invalid_argument( "Invalid assignment to diagonal matrix element" );
         *value_ = v;
         return *this;
      }
      //*******************************************************************************************

      //*******************************************************************************************
      /*!\brief Addition assignment to the unilower value.
      //
      // \param v The right-hand side value to be added to the unilower value.
      // \return Reference to the assigned unilower value.
      // \exception std::invalid_argument Invalid assignment to diagonal matrix element.
      */
      template< typename T > inline UniLowerValue& operator+=( const T& v ) {
         if( diagonal_ )
            throw std::invalid_argument( "Invalid assignment to diagonal matrix element" );
         *value_ += v;
         return *this;
      }
      //*******************************************************************************************

      //*******************************************************************************************
      /*!\brief Subtraction assignment to the unilower value.
      //
      // \param v The right-hand side value to be subtracted from the unilower value.
      // \return Reference to the assigned unilower value.
      // \exception std::invalid_argument Invalid assignment to diagonal matrix element.
      */
      template< typename T > inline UniLowerValue& operator-=( const T& v ) {
         if( diagonal_ )
            throw std::invalid_argument( "Invalid assignment to diagonal matrix element" );
         *value_ -= v;
         return *this;
      }
      //*******************************************************************************************

      //*******************************************************************************************
      /*!\brief Multiplication assignment to the unilower value.
      //
      // \param v The right-hand side value for the multiplication.
      // \return Reference to the assigned unilowe value.
      // \exception std::invalid_argument Invalid assignment to diagonal matrix element.
      */
      template< typename T > inline UniLowerValue& operator*=( const T& v ) {
         if( diagonal_ )
            throw std::invalid_argument( "Invalid assignment to diagonal matrix element" );
         *value_ *= v;
         return *this;
      }
      //*******************************************************************************************

      //*******************************************************************************************
      /*!\brief Division assignment to the unilower value.
      //
      // \param v The right-hand side value for the division.
      // \return Reference to the assigned unilower value.
      // \exception std::invalid_argument Invalid assignment to diagonal matrix element.
      */
      template< typename T > inline UniLowerValue& operator/=( const T& v ) {
         if( diagonal_ )
            throw std::invalid_argument( "Invalid assignment to diagonal matrix element" );
         *value_ /= v;
         return *this;
      }
      //*******************************************************************************************

      //*******************************************************************************************
      /*!\brief Conversion to the represented value.
      //
      // \return Copy of the represented value.
      */
      inline operator ElementType() const {
         return *value_;
      }
      //*******************************************************************************************

      //*******************************************************************************************
      /*!\brief Returns the real part of the represented complex number.
      //
      // \return The current real part of the represented complex number.
      //
      // In case the proxy represents a complex number, this function returns the current value
      // of its real part.
      */
      inline ValueType real() const {
         return value_->real();
      }
      //*******************************************************************************************

      //*******************************************************************************************
      /*!\brief Setting the real part of the represented complex number.
      //
      // \param value The new value for the real part.
      // \return void
      // \exception std::invalid_argument Invalid assignment to diagonal matrix element.
      //
      // In case the proxy represents a complex number, this function sets a new value to its
      // real part.
      */
      inline void real( ValueType value ) const {
         if( diagonal_ )
            throw std::invalid_argument( "Invalid assignment to diagonal matrix element" );
         value_->real( value );
      }
      //*******************************************************************************************

      //*******************************************************************************************
      /*!\brief Returns the imaginary part of the represented complex number.
      //
      // \return The current imaginary part of the represented complex number.
      //
      // In case the proxy represents a complex number, this function returns the current value of its
      // imaginary part.
      */
      inline ValueType imag() const {
         return value_->imag();
      }
      //*******************************************************************************************

      //*******************************************************************************************
      /*!\brief Setting the imaginary part of the represented complex number.
      //
      // \param value The new value for the imaginary part.
      // \return void
      //
      // In case the proxy represents a complex number, this function sets a new value to its
      // imaginary part.
      // \exception std::invalid_argument Invalid assignment to diagonal matrix element.
      */
      inline void imag( ValueType value ) const {
         if( diagonal_ )
            throw std::invalid_argument( "Invalid assignment to diagonal matrix element" );
         value_->imag( value );
      }
      //*******************************************************************************************

    private:
      //**Member variables*************************************************************************
      ElementType* value_;     //!< The represented value.
      bool         diagonal_;  //!< \a true in case the element is on the diagonal, \a false if not.
      //*******************************************************************************************
   };
   //**********************************************************************************************

   //**UniLowerElement class definition************************************************************
   /*!\brief Representation of an element within the sparse lower unitriangular matrix.
   */
   class UniLowerElement : private SparseElement
   {
    private:
      //**Type definitions*************************************************************************
      typedef typename MT::Iterator  IteratorType;  //!< Type of the underlying sparse matrix iterators.
      //*******************************************************************************************

    public:
      //**Type definitions*************************************************************************
      typedef UniLowerValue        ValueType;       //!< The value type of the value-index-pair.
      typedef size_t               IndexType;       //!< The index type of the value-index-pair.
      typedef UniLowerValue        Reference;       //!< Reference return type.
      typedef const UniLowerValue  ConstReference;  //!< Reference-to-const return type.
      typedef UniLowerElement*     Pointer;         //!< Pointer return type.
      //*******************************************************************************************

      //**Constructor******************************************************************************
      /*!\brief Constructor for the UniLowerElement class.
      //
      // \param pos Iterator to the current position with the sparse lower unitriangular matrix.
      // \param diagonal \a true in case the element is on the diagonal, \a false if not.
      */
      inline UniLowerElement( IteratorType pos, bool diagonal )
         : pos_     ( pos      )  // Iterator to the current lower unitriangular matrix element
         , diagonal_( diagonal )  // true in case the element is on the diagonal, false if not
      {}
      //*******************************************************************************************

      //**Assignment operator**********************************************************************
      /*!\brief Assignment to the unilower element.
      //
      // \param v The new v of the unilower element.
      // \return Reference to the assigned unilower element.
      // \exception std::invalid_argument Invalid assignment to diagonal matrix element.
      */
      template< typename T > inline UniLowerElement& operator=( const T& v ) {
         if( diagonal_ )
            throw std::invalid_argument( "Invalid assignment to diagonal matrix element" );
         *pos_ = v;
         return *this;
      }
      //*******************************************************************************************

      //**Addition assignment operator*************************************************************
      /*!\brief Addition assignment to the unilower element.
      //
      // \param v The right-hand side value for the addition.
      // \return Reference to the assigned unilower element.
      // \exception std::invalid_argument Invalid assignment to diagonal matrix element.
      */
      template< typename T > inline UniLowerElement& operator+=( const T& v ) {
         if( diagonal_ )
            throw std::invalid_argument( "Invalid assignment to diagonal matrix element" );
         *pos_ += v;
         return *this;
      }
      //*******************************************************************************************

      //**Subtraction assignment operator**********************************************************
      /*!\brief Subtraction assignment to the unilower element.
      //
      // \param v The right-hand side value for the subtraction.
      // \return Reference to the assigned unilower element.
      // \exception std::invalid_argument Invalid assignment to diagonal matrix element.
      */
      template< typename T > inline UniLowerElement& operator-=( const T& v ) {
         if( diagonal_ )
            throw std::invalid_argument( "Invalid assignment to diagonal matrix element" );
         *pos_ -= v;
         return *this;
      }
      //*******************************************************************************************

      //**Multiplication assignment operator*******************************************************
      /*!\brief Multiplication assignment to the unilower element.
      //
      // \param v The right-hand side value for the multiplication.
      // \return Reference to the assigned unilower element.
      // \exception std::invalid_argument Invalid assignment to diagonal matrix element.
      */
      template< typename T > inline UniLowerElement& operator*=( const T& v ) {
         if( diagonal_ )
            throw std::invalid_argument( "Invalid assignment to diagonal matrix element" );
         *pos_ *= v;
         return *this;
      }
      //*******************************************************************************************

      //**Division assignment operator*************************************************************
      /*!\brief Division assignment to the unilower element.
      //
      // \param v The right-hand side value for the division.
      // \return Reference to the assigned unilower element.
      // \exception std::invalid_argument Invalid assignment to diagonal matrix element.
      */
      template< typename T > inline UniLowerElement& operator/=( const T& v ) {
         if( diagonal_ )
            throw std::invalid_argument( "Invalid assignment to diagonal matrix element" );
         *pos_ /= v;
         return *this;
      }
      //*******************************************************************************************

      //**Element access operator******************************************************************
      /*!\brief Direct access to the unilower element.
      //
      // \return Reference to the value of the unilower element.
      */
      inline Pointer operator->() {
         return this;
      }
      //*******************************************************************************************

      //**Value function***************************************************************************
      /*!\brief Access to the current value of the unilower element.
      //
      // \return The current value of the unilower element.
      */
      inline Reference value() const {
         return Reference( pos_->value(), diagonal_ );
      }
      //*******************************************************************************************

      //**Index function***************************************************************************
      /*!\brief Access to the current index of the unilower element.
      //
      // \return The current index of the unilower element.
      */
      inline IndexType index() const {
         return pos_->index();
      }
      //*******************************************************************************************

    private:
      //**Member variables*************************************************************************
      IteratorType pos_;  //!< Iterator to the current lower unitriangular matrix element.
      bool    diagonal_;  //!< \a true in case the element is on the diagonal, \a false if not.
      //*******************************************************************************************
   };
   //**********************************************************************************************

   //**Iterator class definition*******************************************************************
   /*!\brief Iterator over the elements of the lower unitriangular matrix.
   */
   class Iterator
   {
    public:
      //**Type definitions*************************************************************************
      typedef typename MT::Iterator  IteratorType;  //!< Type of the underlying sparse matrix iterators.

      typedef std::forward_iterator_tag  IteratorCategory;  //!< The iterator category.
      typedef UniLowerElement            ValueType;         //!< Type of the underlying elements.
      typedef ValueType                  PointerType;       //!< Pointer return type.
      typedef ValueType                  ReferenceType;     //!< Reference return type.
      typedef ptrdiff_t                  DifferenceType;    //!< Difference between two iterators.

      // STL iterator requirements
      typedef IteratorCategory  iterator_category;  //!< The iterator category.
      typedef ValueType         value_type;         //!< Type of the underlying elements.
      typedef PointerType       pointer;            //!< Pointer return type.
      typedef ReferenceType     reference;          //!< Reference return type.
      typedef DifferenceType    difference_type;    //!< Difference between two iterators.
      //*******************************************************************************************

      //**Default constructor**********************************************************************
      /*!\brief Default constructor for the Iterator class.
      */
      inline Iterator()
         : pos_   (     )  // Iterator to the current lower unitriangular matrix element
         , index_ ( 0UL )  // The row/column index of the iterator
      {}
      //*******************************************************************************************

      //**Constructor******************************************************************************
      /*!\brief Constructor for the Iterator class.
      //
      // \param pos The initial position of the iterator.
      // \param index The row/column index of the iterator.
      */
      inline Iterator( IteratorType pos, size_t index )
         : pos_  ( pos   )  // Iterator to the current lower unitriangular matrix element
         , index_( index )  // The row/column index of the iterator
      {}
      //*******************************************************************************************

      //**Prefix increment operator****************************************************************
      /*!\brief Pre-increment operator.
      //
      // \return Reference to the incremented iterator.
      */
      inline Iterator& operator++() {
         ++pos_;
         return *this;
      }
      //*******************************************************************************************

      //**Postfix increment operator***************************************************************
      /*!\brief Post-increment operator.
      //
      // \return The previous position of the iterator.
      */
      inline const Iterator operator++( int ) {
         const Iterator tmp( *this );
         ++(*this);
         return tmp;
      }
      //*******************************************************************************************

      //**Element access operator******************************************************************
      /*!\brief Direct access to the current sparse matrix element.
      //
      // \return Reference to the current sparse matrix element.
      */
      inline ReferenceType operator*() const {
         return ReferenceType( pos_, pos_->index() == index_ );
      }
      //*******************************************************************************************

      //**Element access operator******************************************************************
      /*!\brief Direct access to the current sparse matrix element.
      //
      // \return Pointer to the current sparse matrix element.
      */
      inline PointerType operator->() const {
         return PointerType( pos_, pos_->index() == index_ );
      }
      //*******************************************************************************************

      //**Conversion operator**********************************************************************
      /*!\brief Conversion to an iterator over constant elements.
      //
      // \return An iterator over constant elements.
      */
      inline operator ConstIterator() const {
         return pos_;
      }
      //*******************************************************************************************

      //**Equality operator************************************************************************
      /*!\brief Equality comparison between two Iterator objects.
      //
      // \param rhs The right-hand side matrix iterator.
      // \return \a true if the iterators refer to the same element, \a false if not.
      */
      inline bool operator==( const Iterator& rhs ) const {
         return pos_ == rhs.pos_;
      }
      //*******************************************************************************************

      //**Inequality operator**********************************************************************
      /*!\brief Inequality comparison between two Iterator objects.
      //
      // \param rhs The right-hand side matrix iterator.
      // \return \a true if the iterators don't refer to the same element, \a false if they do.
      */
      inline bool operator!=( const Iterator& rhs ) const {
         return !( *this == rhs );
      }
      //*******************************************************************************************

      //**Subtraction operator*********************************************************************
      /*!\brief Calculating the number of elements between two matrix iterators.
      //
      // \param rhs The right-hand side matrix iterator.
      // \return The number of elements between the two matrix iterators.
      */
      inline DifferenceType operator-( const Iterator& rhs ) const {
         return pos_ - rhs.pos_;
      }
      //*******************************************************************************************

      //**Base function****************************************************************************
      /*!\brief Access to the current position of the matrix iterator.
      //
      // \return The current position of the matrix iterator.
      */
      inline IteratorType base() const {
         return pos_;
      }
      //*******************************************************************************************

    private:
      //**Member variables*************************************************************************
      IteratorType pos_;    //!< Iterator to the current lower unitriangular matrix element.
      size_t       index_;  //!< The row/column index of the iterator.
      //*******************************************************************************************
   };
   //**********************************************************************************************

   //**Compilation flags***************************************************************************
   //! Compilation switch for the expression template assignment strategy.
   enum { smpAssignable = 0 };
   //**********************************************************************************************

   //**Constructors********************************************************************************
   /*!\name Constructors */
   //@{
   explicit inline UniLowerMatrix();
   explicit inline UniLowerMatrix( size_t n );
   explicit inline UniLowerMatrix( size_t n, size_t nonzeros );
   explicit inline UniLowerMatrix( size_t n, const std::vector<size_t>& nonzeros );

                                      inline UniLowerMatrix( const UniLowerMatrix& m );
   template< typename MT2, bool SO2 > inline UniLowerMatrix( const Matrix<MT2,SO2>& m );
   //@}
   //**********************************************************************************************

   //**Destructor**********************************************************************************
   // No explicitly declared destructor.
   //**********************************************************************************************

   //**Data access functions***********************************************************************
   /*!\name Data access functions */
   //@{
   inline Reference      operator()( size_t i, size_t j );
   inline ConstReference operator()( size_t i, size_t j ) const;
   inline Iterator       begin ( size_t i );
   inline ConstIterator  begin ( size_t i ) const;
   inline ConstIterator  cbegin( size_t i ) const;
   inline Iterator       end   ( size_t i );
   inline ConstIterator  end   ( size_t i ) const;
   inline ConstIterator  cend  ( size_t i ) const;
   //@}
   //**********************************************************************************************

   //**Assignment operators************************************************************************
   /*!\name Assignment operators */
   //@{
   inline UniLowerMatrix& operator=( const UniLowerMatrix& rhs );

   template< typename MT2, bool SO2 >
   inline typename DisableIf< IsComputation<MT2>, UniLowerMatrix& >::Type
      operator=( const Matrix<MT2,SO2>& rhs );

   template< typename MT2, bool SO2 >
   inline typename EnableIf< IsComputation<MT2>, UniLowerMatrix& >::Type
      operator=( const Matrix<MT2,SO2>& rhs );

   template< typename MT2, bool SO2 >
   inline typename DisableIf< IsComputation<MT2>, UniLowerMatrix& >::Type
      operator+=( const Matrix<MT2,SO2>& rhs );

   template< typename MT2, bool SO2 >
   inline typename EnableIf< IsComputation<MT2>, UniLowerMatrix& >::Type
      operator+=( const Matrix<MT2,SO2>& rhs );

   template< typename MT2, bool SO2 >
   inline typename DisableIf< IsComputation<MT2>, UniLowerMatrix& >::Type
      operator-=( const Matrix<MT2,SO2>& rhs );

   template< typename MT2, bool SO2 >
   inline typename EnableIf< IsComputation<MT2>, UniLowerMatrix& >::Type
      operator-=( const Matrix<MT2,SO2>& rhs );

   template< typename MT2, bool SO2 >
   inline UniLowerMatrix& operator*=( const Matrix<MT2,SO2>& rhs );
   //@}
   //**********************************************************************************************

   //**Utility functions***************************************************************************
   /*!\name Utility functions */
   //@{
   inline size_t   rows() const;
   inline size_t   columns() const;
   inline size_t   capacity() const;
   inline size_t   capacity( size_t i ) const;
   inline size_t   nonZeros() const;
   inline size_t   nonZeros( size_t i ) const;
   inline void     reset();
   inline void     reset( size_t i );
   inline void     clear();
   inline Iterator set( size_t i, size_t j, const ElementType& value );
   inline Iterator insert( size_t i, size_t j, const ElementType& value );
   inline void     erase( size_t i, size_t j );
   inline Iterator erase( size_t i, Iterator pos );
   inline Iterator erase( size_t i, Iterator first, Iterator last );
   inline void     resize ( size_t n, bool preserve=true );
   inline void     reserve( size_t nonzeros );
   inline void     reserve( size_t i, size_t nonzeros );
   inline void     trim();
   inline void     trim( size_t i );
   inline void     swap( UniLowerMatrix& m ) /* throw() */;

   static inline size_t maxNonZeros();
   static inline size_t maxNonZeros( size_t n );
   //@}
   //**********************************************************************************************

   //**Lookup functions****************************************************************************
   /*!\name Lookup functions */
   //@{
   inline Iterator      find      ( size_t i, size_t j );
   inline ConstIterator find      ( size_t i, size_t j ) const;
   inline Iterator      lowerBound( size_t i, size_t j );
   inline ConstIterator lowerBound( size_t i, size_t j ) const;
   inline Iterator      upperBound( size_t i, size_t j );
   inline ConstIterator upperBound( size_t i, size_t j ) const;
   //@}
   //**********************************************************************************************

   //**Low-level utility functions*****************************************************************
   /*!\name Low-level utility functions */
   //@{
   inline void append  ( size_t i, size_t j, const ElementType& value, bool check=false );
   inline void finalize( size_t i );
   //@}
   //**********************************************************************************************

   //**Expression template evaluation functions****************************************************
   /*!\name Expression template evaluation functions */
   //@{
   template< typename Other > inline bool canAlias ( const Other* alias ) const;
   template< typename Other > inline bool isAliased( const Other* alias ) const;

   inline bool canSMPAssign() const;
   //@}
   //**********************************************************************************************

 private:
   //**Utility functions***************************************************************************
   /*!\name Utility functions */
   //@{
   inline void resetUpper();
   //@}
   //**********************************************************************************************

   //**Member variables****************************************************************************
   /*!\name Member variables */
   //@{
   MT matrix_;  //!< The adapted sparse matrix.
   //@}
   //**********************************************************************************************

   //**Friend declarations*************************************************************************
   template< typename MT2, bool SO2, bool DF2 >
   friend MT2& derestrict( UniLowerMatrix<MT2,SO2,DF2>& m );
   //**********************************************************************************************

   //**Compile time checks*************************************************************************
   BLAZE_CONSTRAINT_MUST_BE_SPARSE_MATRIX_TYPE       ( MT );
   BLAZE_CONSTRAINT_MUST_NOT_BE_REFERENCE_TYPE       ( MT );
   BLAZE_CONSTRAINT_MUST_NOT_BE_POINTER_TYPE         ( MT );
   BLAZE_CONSTRAINT_MUST_NOT_BE_CONST                ( MT );
   BLAZE_CONSTRAINT_MUST_NOT_BE_VOLATILE             ( MT );
   BLAZE_CONSTRAINT_MUST_NOT_BE_EXPRESSION_TYPE      ( MT );
   BLAZE_CONSTRAINT_MUST_NOT_BE_SYMMETRIC_MATRIX_TYPE( MT );
   BLAZE_CONSTRAINT_MUST_NOT_BE_LOWER_MATRIX_TYPE    ( MT );
   BLAZE_CONSTRAINT_MUST_NOT_BE_UPPER_MATRIX_TYPE    ( MT );
   BLAZE_CONSTRAINT_MUST_BE_MATRIX_WITH_STORAGE_ORDER( OT, !SO );
   BLAZE_CONSTRAINT_MUST_BE_MATRIX_WITH_STORAGE_ORDER( TT, !SO );
   BLAZE_CONSTRAINT_MUST_BE_NUMERIC_TYPE( ElementType );
   BLAZE_STATIC_ASSERT( IsResizable<MT>::value || IsSquare<MT>::value );
   //**********************************************************************************************
};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  CONSTRUCTORS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief The default constructor for UniLowerMatrix.
*/
template< typename MT  // Type of the adapted sparse matrix
        , bool SO >    // Storage order of the adapted sparse matrix
inline UniLowerMatrix<MT,SO,false>::UniLowerMatrix()
   : matrix_()  // The adapted sparse matrix
{
   BLAZE_INTERNAL_ASSERT( isSquare( matrix_ ), "Non-square unilower matrix detected" );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Constructor for a matrix of size \f$ n \times n \f$.
//
// \param n The number of rows and columns of the matrix.
//
// The matrix is initialized as identity matrix and has no additional free capacity.
*/
template< typename MT  // Type of the adapted sparse matrix
        , bool SO >    // Storage order of the adapted sparse matrix
inline UniLowerMatrix<MT,SO,false>::UniLowerMatrix( size_t n )
   : matrix_( n, n, n )  // The adapted sparse matrix
{
   BLAZE_CONSTRAINT_MUST_BE_RESIZABLE( MT );

   for( size_t i=0UL; i<n; ++i ) {
      matrix_.append( i, i, ElementType(1) );
      matrix_.finalize( i );
   }

   BLAZE_INTERNAL_ASSERT( isSquare( matrix_ ), "Non-square unilower matrix detected" );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Constructor for a matrix of size \f$ n \times n \f$.
//
// \param n The number of rows and columns of the matrix.
// \param nonzeros The number of expected non-zero elements.
//
// The matrix is initialized as identity matrix and will have at least the capacity for
// \a nonzeros non-zero elements.
*/
template< typename MT  // Type of the adapted sparse matrix
        , bool SO >    // Storage order of the adapted sparse matrix
inline UniLowerMatrix<MT,SO,false>::UniLowerMatrix( size_t n, size_t nonzeros )
   : matrix_( n, n, max( nonzeros, n ) )  // The adapted sparse matrix
{
   BLAZE_CONSTRAINT_MUST_BE_RESIZABLE( MT );

   for( size_t i=0UL; i<n; ++i ) {
      matrix_.append( i, i, ElementType(1) );
      matrix_.finalize( i );
   }

   BLAZE_INTERNAL_ASSERT( isSquare( matrix_ ), "Non-square unilower matrix detected" );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Constructor for a matrix of size \f$ n \times n \f$.
//
// \param n The number of rows and columns of the matrix.
// \param nonzeros The expected number of non-zero elements in each row/column.
// \exception std::invalid_argument Invalid capacity specification.
//
// The matrix is initialized as identity matrix and will have the specified capacity in each
// row/column. Note that since the matrix is initialized as \f$ n \times n \f$ identity matrix
// the given vector must have at least \a n elements, all of which must not be 0. If the number
// of non-zero elements of any row/column is specified as 0, a \a std::invalid_argument exception
// is thrown.
*/
template< typename MT  // Type of the adapted sparse matrix
        , bool SO >    // Storage order of the adapted sparse matrix
inline UniLowerMatrix<MT,SO,false>::UniLowerMatrix( size_t n, const std::vector<size_t>& nonzeros )
   : matrix_( n, n, nonzeros )  // The adapted sparse matrix
{
   BLAZE_CONSTRAINT_MUST_BE_RESIZABLE( MT );

   for( size_t i=0UL; i<n; ++i )
   {
      if( nonzeros[i] == 0UL )
         throw std::invalid_argument( "Invalid capacity specification" );

      matrix_.append( i, i, ElementType(1) );
      matrix_.finalize( i );
   }

   BLAZE_INTERNAL_ASSERT( isSquare( matrix_ ), "Non-square unilower matrix detected" );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief The copy constructor for UniLowerMatrix.
//
// \param m The unilower matrix to be copied.
*/
template< typename MT  // Type of the adapted sparse matrix
        , bool SO >    // Storage order of the adapted sparse matrix
inline UniLowerMatrix<MT,SO,false>::UniLowerMatrix( const UniLowerMatrix& m )
   : matrix_( m.matrix_ )  // The adapted sparse matrix
{
   BLAZE_INTERNAL_ASSERT( isSquare( matrix_ ), "Non-square unilower matrix detected" );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Conversion constructor from different matrices.
//
// \param m Matrix to be copied.
// \exception std::invalid_argument Invalid setup of unilower matrix.
//
// This constructor initializes the unilower matrix as a copy of the given matrix. In case the
// given matrix is not an unilower matrix, a \a std::invalid_argument exception is thrown.
*/
template< typename MT   // Type of the adapted sparse matrix
        , bool SO >     // Storage order of the adapted sparse matrix
template< typename MT2  // Type of the foreign matrix
        , bool SO2 >    // Storage order of the foreign matrix
inline UniLowerMatrix<MT,SO,false>::UniLowerMatrix( const Matrix<MT2,SO2>& m )
   : matrix_( ~m )  // The adapted sparse matrix
{
   if( !IsUniLower<MT2>::value && !isUniLower( matrix_ ) )
      throw std::invalid_argument( "Invalid setup of unilower matrix" );

   if( !IsUniLower<MT2>::value )
      resetUpper();

   BLAZE_INTERNAL_ASSERT( isSquare( matrix_ ), "Non-square unilower matrix detected" );
}
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  DATA ACCESS FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief 2D-access to the matrix elements.
//
// \param i Access index for the row. The index has to be in the range \f$[0..N-1]\f$.
// \param j Access index for the column. The index has to be in the range \f$[0..N-1]\f$.
// \return Reference to the accessed value.
//
// The function call operator provides access to the elements at position (i,j). The attempt
// to assign to an element on the diagonal or in the upper part of the matrix (i.e. above the
// diagonal) will result in a \a std::invalid_argument exception.
*/
template< typename MT  // Type of the adapted sparse matrix
        , bool SO >    // Storage order of the adapted sparse matrix
inline typename UniLowerMatrix<MT,SO,false>::Reference
   UniLowerMatrix<MT,SO,false>::operator()( size_t i, size_t j )
{
   BLAZE_USER_ASSERT( i<rows()   , "Invalid row access index"    );
   BLAZE_USER_ASSERT( j<columns(), "Invalid column access index" );

   return Reference( matrix_, i, j );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief 2D-access to the matrix elements.
//
// \param i Access index for the row. The index has to be in the range \f$[0..N-1]\f$.
// \param j Access index for the column. The index has to be in the range \f$[0..N-1]\f$.
// \return Reference to the accessed value.
//
// The function call operator provides access to the elements at position (i,j). The attempt
// to assign to an element on the diagonal or in the upper part of the matrix (i.e. above the
// diagonal) will result in a \a std::invalid_argument exception.
*/
template< typename MT  // Type of the adapted sparse matrix
        , bool SO >    // Storage order of the adapted sparse matrix
inline typename UniLowerMatrix<MT,SO,false>::ConstReference
   UniLowerMatrix<MT,SO,false>::operator()( size_t i, size_t j ) const
{
   BLAZE_USER_ASSERT( i<rows()   , "Invalid row access index"    );
   BLAZE_USER_ASSERT( j<columns(), "Invalid column access index" );

   return matrix_(i,j);
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns an iterator to the first element of row/column \a i.
//
// \param i The row/column index.
// \return Iterator to the first element of row/column \a i.
//
// This function returns a row/column iterator to the first element of row/column \a i. In case
// the unilower matrix adapts a \a rowMajor sparse matrix the function returns an iterator to
// the first element of row \a i, in case it adapts a \a columnMajor sparse matrix the function
// returns an iterator to the first element of column \a i.
*/
template< typename MT  // Type of the adapted sparse matrix
        , bool SO >    // Storage order of the adapted sparse matrix
inline typename UniLowerMatrix<MT,SO,false>::Iterator
   UniLowerMatrix<MT,SO,false>::begin( size_t i )
{
   return Iterator( matrix_.begin(i), i );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns an iterator to the first element of row/column \a i.
//
// \param i The row/column index.
// \return Iterator to the first element of row/column \a i.
//
// This function returns a row/column iterator to the first element of row/column \a i. In case
// the unilower matrix adapts a \a rowMajor sparse matrix the function returns an iterator to
// the first element of row \a i, in case it adapts a \a columnMajor sparse matrix the function
// returns an iterator to the first element of column \a i.
*/
template< typename MT  // Type of the adapted sparse matrix
        , bool SO >    // Storage order of the adapted sparse matrix
inline typename UniLowerMatrix<MT,SO,false>::ConstIterator
   UniLowerMatrix<MT,SO,false>::begin( size_t i ) const
{
   return matrix_.begin(i);
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns an iterator to the first element of row/column \a i.
//
// \param i The row/column index.
// \return Iterator to the first element of row/column \a i.
//
// This function returns a row/column iterator to the first element of row/column \a i. In case
// the unilower matrix adapts a \a rowMajor sparse matrix the function returns an iterator to
// the first element of row \a i, in case it adapts a \a columnMajor sparse matrix the function
// returns an iterator to the first element of column \a i.
*/
template< typename MT  // Type of the adapted sparse matrix
        , bool SO >    // Storage order of the adapted sparse matrix
inline typename UniLowerMatrix<MT,SO,false>::ConstIterator
   UniLowerMatrix<MT,SO,false>::cbegin( size_t i ) const
{
   return matrix_.cbegin(i);
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns an iterator just past the last element of row/column \a i.
//
// \param i The row/column index.
// \return Iterator just past the last element of row/column \a i.
//
// This function returns an row/column iterator just past the last element of row/column \a i.
// In case the unilower matrix adapts a \a rowMajor sparse matrix the function returns an iterator
// just past the last element of row \a i, in case it adapts a \a columnMajor sparse matrix the
// function returns an iterator just past the last element of column \a i.
*/
template< typename MT  // Type of the adapted sparse matrix
        , bool SO >    // Storage order of the adapted sparse matrix
inline typename UniLowerMatrix<MT,SO,false>::Iterator
   UniLowerMatrix<MT,SO,false>::end( size_t i )
{
   return Iterator( matrix_.end(i), i );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns an iterator just past the last element of row/column \a i.
//
// \param i The row/column index.
// \return Iterator just past the last element of row/column \a i.
//
// This function returns an row/column iterator just past the last element of row/column \a i.
// In case the unilower matrix adapts a \a rowMajor sparse matrix the function returns an iterator
// just past the last element of row \a i, in case it adapts a \a columnMajor sparse matrix the
// function returns an iterator just past the last element of column \a i.
*/
template< typename MT  // Type of the adapted sparse matrix
        , bool SO >    // Storage order of the adapted sparse matrix
inline typename UniLowerMatrix<MT,SO,false>::ConstIterator
   UniLowerMatrix<MT,SO,false>::end( size_t i ) const
{
   return matrix_.end(i);
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns an iterator just past the last element of row/column \a i.
//
// \param i The row/column index.
// \return Iterator just past the last element of row/column \a i.
//
// This function returns an row/column iterator just past the last element of row/column \a i.
// In case the unilower matrix adapts a \a rowMajor sparse matrix the function returns an iterator
// just past the last element of row \a i, in case it adapts a \a columnMajor sparse matrix the
// function returns an iterator just past the last element of column \a i.
*/
template< typename MT  // Type of the adapted sparse matrix
        , bool SO >    // Storage order of the adapted sparse matrix
inline typename UniLowerMatrix<MT,SO,false>::ConstIterator
   UniLowerMatrix<MT,SO,false>::cend( size_t i ) const
{
   return matrix_.cend(i);
}
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ASSIGNMENT OPERATORS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Copy assignment operator for UniLowerMatrix.
//
// \param rhs Matrix to be copied.
// \return Reference to the assigned matrix.
//
// If possible and necessary, the matrix is resized according to the given \f$ N \times N \f$
// matrix and initialized as a copy of this matrix.
*/
template< typename MT  // Type of the adapted sparse matrix
        , bool SO >    // Storage order of the adapted sparse matrix
inline UniLowerMatrix<MT,SO,false>&
   UniLowerMatrix<MT,SO,false>::operator=( const UniLowerMatrix& rhs )
{
   matrix_ = rhs.matrix_;

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Assignment operator for general matrices.
//
// \param rhs The general matrix to be copied.
// \return Reference to the assigned matrix.
// \exception std::invalid_argument Invalid assignment to unilower matrix.
//
// If possible and necessary, the matrix is resized according to the given \f$ N \times N \f$
// matrix and initialized as a copy of this matrix. If the matrix cannot be resized accordingly,
// a \a std::invalid_argument exception is thrown. Also note that the given matrix must be a
// unilower matrix. Otherwise, a \a std::invalid_argument exception is thrown.
*/
template< typename MT   // Type of the adapted sparse matrix
        , bool SO >     // Storage order of the adapted sparse matrix
template< typename MT2  // Type of the right-hand side matrix
        , bool SO2 >    // Storage order of the right-hand side matrix
inline typename DisableIf< IsComputation<MT2>, UniLowerMatrix<MT,SO,false>& >::Type
   UniLowerMatrix<MT,SO,false>::operator=( const Matrix<MT2,SO2>& rhs )
{
   if( !IsUniLower<MT2>::value && !isUniLower( ~rhs ) )
      throw std::invalid_argument( "Invalid assignment to unilower matrix" );

   matrix_ = ~rhs;

   if( !IsUniLower<MT2>::value )
      resetUpper();

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Assignment operator for matrix computations.
//
// \param rhs The matrix computation to be copied.
// \return Reference to the assigned matrix.
// \exception std::invalid_argument Invalid assignment to unilower matrix.
//
// If possible and necessary, the matrix is resized according to the given \f$ N \times N \f$
// matrix and initialized as a copy of this matrix. If the matrix cannot be resized accordingly,
// a \a std::invalid_argument exception is thrown. Also note that the given matrix must be a
// unilower matrix. Otherwise, a \a std::invalid_argument exception is thrown.
*/
template< typename MT   // Type of the adapted sparse matrix
        , bool SO >     // Storage order of the adapted sparse matrix
template< typename MT2  // Type of the right-hand side matrix
        , bool SO2 >    // Storage order of the right-hand side matrix
inline typename EnableIf< IsComputation<MT2>, UniLowerMatrix<MT,SO,false>& >::Type
   UniLowerMatrix<MT,SO,false>::operator=( const Matrix<MT2,SO2>& rhs )
{
   if( !IsSquare<MT2>::value && !isSquare( ~rhs ) )
      throw std::invalid_argument( "Invalid assignment to unilower matrix" );

   if( IsUniLower<MT2>::value ) {
      matrix_ = ~rhs;
   }
   else {
      MT tmp( ~rhs );

      if( !isUniLower( tmp ) )
         throw std::invalid_argument( "Invalid assignment to unilower matrix" );

      move( matrix_, tmp );
   }

   if( !IsUniLower<MT2>::value )
      resetUpper();

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Addition assignment operator for the addition of a general matrix (\f$ A+=B \f$).
//
// \param rhs The right-hand side general matrix to be added.
// \return Reference to the matrix.
// \exception std::invalid_argument Invalid assignment to unilower matrix.
//
// In case the current sizes of the two matrices don't match, a \a std::invalid_argument
// exception is thrown. Also note that the result of the addition operation must be an unilower
// matrix, i.e. the given matrix must be a strictly lower matrix. In case the result is not an
// unilower matrix, a \a std::invalid_argument exception is thrown.
*/
template< typename MT   // Type of the adapted sparse matrix
        , bool SO >     // Storage order of the adapted sparse matrix
template< typename MT2  // Type of the right-hand side matrix
        , bool SO2 >    // Storage order of the right-hand side matrix
inline typename DisableIf< IsComputation<MT2>, UniLowerMatrix<MT,SO,false>& >::Type
   UniLowerMatrix<MT,SO,false>::operator+=( const Matrix<MT2,SO2>& rhs )
{
   if( !IsStrictlyLower<MT2>::value && !isStrictlyLower( ~rhs ) )
      throw std::invalid_argument( "Invalid assignment to unilower matrix" );

   matrix_ += ~rhs;

   if( !IsStrictlyLower<MT2>::value )
      resetUpper();

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Addition assignment operator for the addition of a matrix computation (\f$ A+=B \f$).
//
// \param rhs The right-hand side matrix computation to be added.
// \return Reference to the matrix.
// \exception std::invalid_argument Invalid assignment to unilower matrix.
//
// In case the current sizes of the two matrices don't match, a \a std::invalid_argument
// exception is thrown. Also note that the result of the addition operation must be an unilower
// matrix, i.e. the given matrix must be a strictly lower matrix. In case the result is not an
// unilower matrix, a \a std::invalid_argument exception is thrown.
*/
template< typename MT   // Type of the adapted sparse matrix
        , bool SO >     // Storage order of the adapted sparse matrix
template< typename MT2  // Type of the right-hand side matrix
        , bool SO2 >    // Storage order of the right-hand side matrix
inline typename EnableIf< IsComputation<MT2>, UniLowerMatrix<MT,SO,false>& >::Type
   UniLowerMatrix<MT,SO,false>::operator+=( const Matrix<MT2,SO2>& rhs )
{
   if( !IsSquare<MT2>::value && !isSquare( ~rhs ) )
      throw std::invalid_argument( "Invalid assignment to unilower matrix" );

   if( IsStrictlyLower<MT2>::value ) {
      matrix_ += ~rhs;
   }
   else {
      typename MT2::ResultType tmp( ~rhs );

      if( !isStrictlyLower( tmp ) )
         throw std::invalid_argument( "Invalid assignment to unilower matrix" );

      matrix_ += tmp;
   }

   if( !IsStrictlyLower<MT2>::value )
      resetUpper();

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Subtraction assignment operator for the subtraction of a general matrix (\f$ A-=B \f$).
//
// \param rhs The right-hand side general matrix to be subtracted.
// \return Reference to the matrix.
// \exception std::invalid_argument Invalid assignment to unilower matrix.
//
// In case the current sizes of the two matrices don't match, a \a std::invalid_argument
// exception is thrown. Also note that the result of the subtraction operation must be an
// unilower matrix, i.e. the given matrix must be a strictly lower matrix. In case the
// result is not an unilower matrix, a \a std::invalid_argument exception is thrown.
*/
template< typename MT   // Type of the adapted sparse matrix
        , bool SO >     // Storage order of the adapted sparse matrix
template< typename MT2  // Type of the right-hand side matrix
        , bool SO2 >    // Storage order of the right-hand side matrix
inline typename DisableIf< IsComputation<MT2>, UniLowerMatrix<MT,SO,false>& >::Type
   UniLowerMatrix<MT,SO,false>::operator-=( const Matrix<MT2,SO2>& rhs )
{
   if( !IsStrictlyLower<MT2>::value && !isStrictlyLower( ~rhs ) )
      throw std::invalid_argument( "Invalid assignment to unilower matrix" );

   matrix_ -= ~rhs;

   if( !IsStrictlyLower<MT2>::value )
      resetUpper();

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Subtraction assignment operator for the subtraction of a matrix computation (\f$ A-=B \f$).
//
// \param rhs The right-hand side matrix computation to be subtracted.
// \return Reference to the matrix.
// \exception std::invalid_argument Invalid assignment to unilower matrix.
//
// In case the current sizes of the two matrices don't match, a \a std::invalid_argument
// exception is thrown. Also note that the result of the subtraction operation must be an
// unilower matrix, i.e. the given matrix must be a strictly lower matrix. In case the
// result is not an unilower matrix, a \a std::invalid_argument exception is thrown.
*/
template< typename MT   // Type of the adapted sparse matrix
        , bool SO >     // Storage order of the adapted sparse matrix
template< typename MT2  // Type of the right-hand side matrix
        , bool SO2 >    // Storage order of the right-hand side matrix
inline typename EnableIf< IsComputation<MT2>, UniLowerMatrix<MT,SO,false>& >::Type
   UniLowerMatrix<MT,SO,false>::operator-=( const Matrix<MT2,SO2>& rhs )
{
   if( !IsSquare<MT2>::value && !isSquare( ~rhs ) )
      throw std::invalid_argument( "Invalid assignment to unilower matrix" );

   if( IsStrictlyLower<MT2>::value ) {
      matrix_ -= ~rhs;
   }
   else {
      typename MT2::ResultType tmp( ~rhs );

      if( !isStrictlyLower( tmp ) )
         throw std::invalid_argument( "Invalid assignment to unilower matrix" );

      matrix_ -= tmp;
   }

   if( !IsStrictlyLower<MT2>::value )
      resetUpper();

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Multiplication assignment operator for the multiplication of a matrix (\f$ A*=B \f$).
//
// \param rhs The right-hand side matrix for the multiplication.
// \return Reference to the matrix.
// \exception std::invalid_argument Matrix sizes do not match.
//
// In case the current sizes of the two matrices don't match, a \a std::invalid_argument exception
// is thrown. Also note that the result of the multiplication operation must be an unilower matrix.
// In case it is not, a \a std::invalid_argument exception is thrown.
*/
template< typename MT   // Type of the adapted sparse matrix
        , bool SO >     // Storage order of the adapted sparse matrix
template< typename MT2  // Type of the right-hand side matrix
        , bool SO2 >    // Storage order of the right-hand side matrix
inline UniLowerMatrix<MT,SO,false>&
   UniLowerMatrix<MT,SO,false>::operator*=( const Matrix<MT2,SO2>& rhs )
{
   if( matrix_.rows() != (~rhs).columns() )
      throw std::invalid_argument( "Invalid assignment to unilower matrix" );

   MT tmp( matrix_ * ~rhs );

   if( !isUniLower( tmp ) )
      throw std::invalid_argument( "Invalid assignment to unilower matrix" );

   move( matrix_, tmp );

   if( !IsUniLower<MT2>::value )
      resetUpper();

   return *this;
}
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  UTILITY FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns the current number of rows of the matrix.
//
// \return The number of rows of the matrix.
*/
template< typename MT  // Type of the adapted sparse matrix
        , bool SO >    // Storage order of the adapted sparse matrix
inline size_t UniLowerMatrix<MT,SO,false>::rows() const
{
   return matrix_.rows();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns the current number of columns of the matrix.
//
// \return The number of columns of the matrix.
*/
template< typename MT  // Type of the adapted sparse matrix
        , bool SO >    // Storage order of the adapted sparse matrix
inline size_t UniLowerMatrix<MT,SO,false>::columns() const
{
   return matrix_.columns();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns the maximum capacity of the matrix.
//
// \return The capacity of the matrix.
*/
template< typename MT  // Type of the adapted sparse matrix
        , bool SO >    // Storage order of the adapted sparse matrix
inline size_t UniLowerMatrix<MT,SO,false>::capacity() const
{
   return matrix_.capacity();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns the current capacity of the specified row/column.
//
// \param i The index of the row/column.
// \return The current capacity of row/column \a i.
//
// This function returns the current capacity of the specified row/column. In case the unilower
// matrix adapts a \a rowMajor sparse matrix the function returns the capacity of row \a i, in
// case it adapts a \a columnMajor sparse matrix the function returns the capacity of column
// \a i.
*/
template< typename MT  // Type of the adapted sparse matrix
        , bool SO >    // Storage order of the adapted sparse matrix
inline size_t UniLowerMatrix<MT,SO,false>::capacity( size_t i ) const
{
   return matrix_.capacity(i);
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns the total number of non-zero elements in the matrix
//
// \return The number of non-zero elements in the unilower matrix.
*/
template< typename MT  // Type of the adapted sparse matrix
        , bool SO >    // Storage order of the adapted sparse matrix
inline size_t UniLowerMatrix<MT,SO,false>::nonZeros() const
{
   return matrix_.nonZeros();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns the number of non-zero elements in the specified row/column.
//
// \param i The index of the row/column.
// \return The number of non-zero elements of row/column \a i.
//
// This function returns the current number of non-zero elements in the specified row/column.
// In case the unilower matrix adapts a \a rowMajor sparse matrix the function returns the
// number of non-zero elements in row \a i, in case it adapts a to \a columnMajor sparse
// matrix the function returns the number of non-zero elements in column \a i.
*/
template< typename MT  // Type of the adapted sparse matrix
        , bool SO >    // Storage order of the adapted sparse matrix
inline size_t UniLowerMatrix<MT,SO,false>::nonZeros( size_t i ) const
{
   return matrix_.nonZeros(i);
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Reset to the default initial values.
//
// \return void
*/
template< typename MT  // Type of the adapted sparse matrix
        , bool SO >    // Storage order of the adapted sparse matrix
inline void UniLowerMatrix<MT,SO,false>::reset()
{
   if( SO ) {
      for( size_t j=0UL; j<columns(); ++j ) {
         matrix_.erase( j, matrix_.lowerBound(j+1UL,j), matrix_.end(j) );
      }
   }
   else {
      for( size_t i=1UL; i<rows(); ++i ) {
         matrix_.erase( i, matrix_.begin(i), matrix_.lowerBound(i,i) );
      }
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Reset the specified row/column to the default initial values.
//
// \param i The index of the row/column.
// \return void
// \exception std::invalid_argument Invalid row/column access index.
//
// This function resets the values in the specified row/column to their default value. In case
// the storage order is set to \a rowMajor the function resets the values in row \a i, in case
// the storage order is set to \a columnMajor the function resets the values in column \a i.
// Note that the reset() function has no impact on the capacity of the matrix or row/column.
*/
template< typename MT  // Type of the adapted sparse matrix
        , bool SO >    // Storage order of the adapted sparse matrix
inline void UniLowerMatrix<MT,SO,false>::reset( size_t i )
{
   if( SO ) {
      matrix_.erase( i, matrix_.lowerBound(i+1UL,i), matrix_.end(i) );
   }
   else {
      matrix_.erase( i, matrix_.begin(i), matrix_.lowerBound(i,i) );
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Clearing the unilower matrix.
//
// \return void
//
// This function clears the unilower matrix and returns it to its default state.
*/
template< typename MT  // Type of the adapted sparse matrix
        , bool SO >    // Storage order of the adapted sparse matrix
inline void UniLowerMatrix<MT,SO,false>::clear()
{
   using blaze::clear;

   clear( matrix_ );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Setting elements of the unilower matrix.
//
// \param i The row index of the new element. The index has to be in the range \f$[0..N-1]\f$.
// \param j The column index of the new element. The index has to be in the range \f$[0..N-1]\f$.
// \param value The value of the element to be set.
// \return Iterator to the set element.
// \exception std::invalid_argument Invalid access to diagonal or upper matrix element.
//
// This function sets the value of an element of the unilower matrix. In case the unilower matrix
// already contains an element with row index \a i and column index \a j its value is modified,
// else a new element with the given \a value is inserted. The attempt to set an element on the
// diagonal or in the upper part of the matrix (i.e. above the diagonal) will result in a
// \a std::invalid_argument exception.
*/
template< typename MT  // Type of the adapted sparse matrix
        , bool SO >    // Storage order of the adapted sparse matrix
inline typename UniLowerMatrix<MT,SO,false>::Iterator
   UniLowerMatrix<MT,SO,false>::set( size_t i, size_t j, const ElementType& value )
{
   if( i <= j )
      throw std::invalid_argument( "Invalid access to diagonal or upper matrix element" );

   return Iterator( matrix_.set( i, j, value ), ( SO ? j : i ) );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Inserting elements into the unilower matrix.
//
// \param i The row index of the new element. The index has to be in the range \f$[0..N-1]\f$.
// \param j The column index of the new element. The index has to be in the range \f$[0..N-1]\f$.
// \param value The value of the element to be inserted.
// \return Iterator to the newly inserted element.
// \exception std::invalid_argument Invalid sparse matrix access index.
// \exception std::invalid_argument Invalid access to diagonal or upper matrix element.
//
// This function inserts a new element into the unilower matrix. However, duplicate elements are
// not allowed. In case the unilower matrix already contains an element with row index \a i and
// column index \a j, a \a std::invalid_argument exception is thrown. Also, the attempt to insert
// an element on the diagonal or in the upper part of the matrix (i.e. above the diagonal) will
// result in a \a std::invalid_argument exception.
*/
template< typename MT  // Type of the adapted sparse matrix
        , bool SO >    // Storage order of the adapted sparse matrix
inline typename UniLowerMatrix<MT,SO,false>::Iterator
   UniLowerMatrix<MT,SO,false>::insert( size_t i, size_t j, const ElementType& value )
{
   if( i <= j )
      throw std::invalid_argument( "Invalid access to diagonal or upper matrix element" );

   return Iterator( matrix_.insert( i, j, value ), ( SO ? j : i ) );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Erasing elements from the unilower matrix.
//
// \param i The row index of the element to be erased. The index has to be in the range \f$[0..N-1]\f$.
// \param j The column index of the element to be erased. The index has to be in the range \f$[0..N-1]\f$.
// \return void
// \exception std::invalid_argument Invalid access to diagonal matrix element.
//
// This function erases a non-diagonal element from the unilower matrix. The attempt to erase a
// diagonal element will result in a \a std::invalid_argument exception.
*/
template< typename MT  // Type of the adapted sparse matrix
        , bool SO >    // Storage order of the adapted sparse matrix
inline void UniLowerMatrix<MT,SO,false>::erase( size_t i, size_t j )
{
   if( i == j )
      throw std::invalid_argument( "Invalid access to diagonal matrix element" );

   matrix_.erase( i, j );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Erasing elements from the unilower matrix.
//
// \param i The row/column index of the element to be erased. The index has to be in the range \f$[0..N-1]\f$.
// \param pos Iterator to the element to be erased.
// \return Iterator to the element after the erased element.
// \exception std::invalid_argument Invalid access to diagonal matrix element.
//
// This function erases a non-diagonal element from the unilower matrix. In case the unilower
// matrix adapts a \a rowMajor sparse matrix the function erases an element from row \a i, in
// case it adapts a \a columnMajor sparse matrix the function erases an element from column \a i.
// The attempt to erase a diagonal element will result in a \a std::invalid_argument exception.
*/
template< typename MT  // Type of the adapted sparse matrix
        , bool SO >    // Storage order of the adapted sparse matrix
inline typename UniLowerMatrix<MT,SO,false>::Iterator
   UniLowerMatrix<MT,SO,false>::erase( size_t i, Iterator pos )
{
   if( pos != matrix_.end(i) && pos->index() == i )
      throw std::invalid_argument( "Invalid access to diagonal matrix element" );

   return Iterator( matrix_.erase( i, pos.base() ), i );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Erasing a range of elements from the unilower matrix.
//
// \param i The row/column index of the element to be erased. The index has to be in the range \f$[0..N-1]\f$.
// \param first Iterator to first element to be erased.
// \param last Iterator just past the last element to be erased.
// \return Iterator to the element after the erased element.
// \exception std::invalid_argument Invalid access to diagonal matrix element.
//
// This function erases a range of elements from the unilower matrix. In case the unilower matrix
// adapts a \a rowMajor sparse matrix the function erases a range of elements from row \a i, in
// case it adapts a \a columnMajor matrix the function erases a range of elements from column \a i.
// The attempt to erase a diagonal element will result in a \a std::invalid_argument exception.
*/
template< typename MT  // Type of the adapted sparse matrix
        , bool SO >    // Storage order of the adapted sparse matrix
inline typename UniLowerMatrix<MT,SO,false>::Iterator
   UniLowerMatrix<MT,SO,false>::erase( size_t i, Iterator first, Iterator last )
{
   for( Iterator element=first; element!=last; ++element ) {
      if( element->index() == i )
         throw std::invalid_argument( "Invalid access to diagonal matrix element" );
   }

   return Iterator( matrix_.erase( i, first.base(), last.base() ), i );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Changing the size of the unilower matrix.
//
// \param n The new number of rows and columns of the matrix.
// \param preserve \a true if the old values of the matrix should be preserved, \a false if not.
// \return void
//
// This function resizes the matrix using the given size to \f$ m \times n \f$. During this
// operation, new dynamic memory may be allocated in case the capacity of the matrix is too
// small. Note that this function may invalidate all existing views (submatrices, rows, columns,
// ...) on the matrix if it is used to shrink the matrix. Additionally, the resize operation
// potentially changes all matrix elements. In order to preserve the old matrix values, the
// \a preserve flag can be set to \a true.
*/
template< typename MT  // Type of the adapted sparse matrix
        , bool SO >    // Storage order of the adapted sparse matrix
void UniLowerMatrix<MT,SO,false>::resize( size_t n, bool preserve )
{
   BLAZE_CONSTRAINT_MUST_BE_RESIZABLE( MT );

   BLAZE_INTERNAL_ASSERT( isSquare( matrix_ ), "Non-square unilower matrix detected" );

   const size_t oldsize( matrix_.rows() );

   matrix_.resize( n, n, preserve );

   if( n > oldsize ) {
      for( size_t i=oldsize; i<n; ++i )
         matrix_.insert( i, i, ElementType(1) );
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Setting the minimum capacity of the unilower matrix.
//
// \param nonzeros The new minimum capacity of the unilower matrix.
// \return void
//
// This function increases the capacity of the unilower matrix to at least \a nonzeros elements.
// The current values of the matrix elements and the individual capacities of the matrix rows
// are preserved.
*/
template< typename MT  // Type of the adapted sparse matrix
        , bool SO >    // Storage order of the adapted sparse matrix
inline void UniLowerMatrix<MT,SO,false>::reserve( size_t nonzeros )
{
   matrix_.reserve( nonzeros );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Setting the minimum capacity of a specific row/column of the unilower matrix.
//
// \param i The row/column index \f$[0..N-1]\f$.
// \param nonzeros The new minimum capacity of the specified row/column.
// \return void
//
// This function increases the capacity of row/column \a i of the unilower matrix to at least
// \a nonzeros elements. The current values of the unilower matrix and all other individual
// row/column capacities are preserved. In case the unilower matrix adapts a \rowMajor sparse
// matrix the function reserves capacity for row \a i. In case it adapts a \a columnMajor the
// function reserves capacity for column \a i. The index has to be in the range \f$[0..N-1]\f$.
*/
template< typename MT  // Type of the adapted sparse matrix
        , bool SO >    // Storage order of the adapted sparse matrix
inline void UniLowerMatrix<MT,SO,false>::reserve( size_t i, size_t nonzeros )
{
   matrix_.reserve( i, nonzeros );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Removing all excessive capacity from all rows/columns.
//
// \return void
//
// The trim() function can be used to reverse the effect of all row/column-specific reserve()
// calls. The function removes all excessive capacity from all rows (in case of a rowMajor
// matrix) or columns (in case of a columnMajor matrix). Note that this function does not
// remove the overall capacity but only reduces the capacity per row/column.
*/
template< typename MT  // Type of the adapted sparse matrix
        , bool SO >    // Storage order of the adapted sparse matrix
inline void UniLowerMatrix<MT,SO,false>::trim()
{
   matrix_.trim();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Removing all excessive capacity of a specific row/column of the unilower matrix.
//
// \param i The index of the row/column to be trimmed \f$[0..N-1]\f$.
// \return void
//
// This function can be used to reverse the effect of a row/column-specific reserve() call.
// It removes all excessive capacity from the specified row (in case of a rowMajor matrix)
// or column (in case of a columnMajor matrix). The excessive capacity is assigned to the
// subsequent row/column.
*/
template< typename MT  // Type of the adapted sparse matrix
        , bool SO >    // Storage order of the adapted sparse matrix
inline void UniLowerMatrix<MT,SO,false>::trim( size_t i )
{
   matrix_.trim( i );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Swapping the contents of two matrices.
//
// \param m The matrix to be swapped.
// \return void
// \exception no-throw guarantee.
*/
template< typename MT  // Type of the adapted sparse matrix
        , bool SO >    // Storage order of the adapted sparse matrix
inline void UniLowerMatrix<MT,SO,false>::swap( UniLowerMatrix& m ) /* throw() */
{
   using std::swap;

   swap( matrix_, m.matrix_ );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns the maximum number of non-zero values for a lower unitriangular matrix.
//
// \return The maximum number of non-zero values.
//
// This function returns the maximum possible number of non-zero values for a lower unitriangular
// matrix with fixed-size adapted matrix of type \a MT. Note that this function can only be
// called in case the adapted dense matrix is a fixed-size matrix. The attempt to call this
// function in case the adapted matrix is resizable matrix will result in a compile time error.
*/
template< typename MT  // Type of the adapted dense matrix
        , bool SO >    // Storage order of the adapted dense matrix
inline size_t UniLowerMatrix<MT,SO,false>::maxNonZeros()
{
   BLAZE_CONSTRAINT_MUST_NOT_BE_RESIZABLE( MT );

   return maxNonZeros( Rows<MT>::value );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns the maximum number of non-zero values for a lower unitriangular matrix.
//
// \param n The number of rows and columns of the matrix.
// \return The maximum number of non-zero values.
//
// This function returns the maximum possible number of non-zero values for a lower unitriangular
// matrix of the given number of rows and columns.
*/
template< typename MT  // Type of the adapted dense matrix
        , bool SO >    // Storage order of the adapted dense matrix
inline size_t UniLowerMatrix<MT,SO,false>::maxNonZeros( size_t n )
{
   return ( ( n + 1UL ) * n ) / 2UL;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Reset the complete upper part of the matrix to the default initial values.
//
// \return void
*/
template< typename MT  // Type of the adapted dense matrix
        , bool SO >    // Storage order of the adapted dense matrix
inline void UniLowerMatrix<MT,SO,false>::resetUpper()
{
   if( SO ) {
      for( size_t j=1UL; j<columns(); ++j )
         matrix_.erase( j, matrix_.begin( j ), matrix_.lowerBound( j, j ) );
   }
   else {
      for( size_t i=0UL; i<rows(); ++i )
         matrix_.erase( i, matrix_.upperBound( i, i ), matrix_.end( i ) );
   }
}
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  LOOKUP FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Searches for a specific matrix element.
//
// \param i The row index of the search element. The index has to be in the range \f$[0..N-1]\f$.
// \param j The column index of the search element. The index has to be in the range \f$[0..N-1]\f$.
// \return Iterator to the element in case the index is found, end() iterator otherwise.
//
// This function can be used to check whether a specific element is contained in the unilower
// matrix. It specifically searches for the element with row index \a i and column index \a j.
// In case the element is found, the function returns an row/column iterator to the element.
// Otherwise an iterator just past the last non-zero element of row \a i or column \a j (the
// end() iterator) is returned. Note that the returned unilower matrix iterator is subject to
// invalidation due to inserting operations via the function call operator or the insert()
// function!
*/
template< typename MT  // Type of the adapted sparse matrix
        , bool SO >    // Storage order of the adapted sparse matrix
inline typename UniLowerMatrix<MT,SO,false>::Iterator
   UniLowerMatrix<MT,SO,false>::find( size_t i, size_t j )
{
   return Iterator( matrix_.find( i, j ), ( SO ? j : i ) );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Searches for a specific matrix element.
//
// \param i The row index of the search element. The index has to be in the range \f$[0..N-1]\f$.
// \param j The column index of the search element. The index has to be in the range \f$[0..N-1]\f$.
// \return Iterator to the element in case the index is found, end() iterator otherwise.
//
// This function can be used to check whether a specific element is contained in the unilower
// matrix. It specifically searches for the element with row index \a i and column index \a j.
// In case the element is found, the function returns an row/column iterator to the element.
// Otherwise an iterator just past the last non-zero element of row \a i or column \a j (the
// end() iterator) is returned. Note that the returned unilower matrix iterator is subject to
// invalidation due to inserting operations via the function call operator or the insert()
// function!
*/
template< typename MT  // Type of the adapted sparse matrix
        , bool SO >    // Storage order of the adapted sparse matrix
inline typename UniLowerMatrix<MT,SO,false>::ConstIterator
   UniLowerMatrix<MT,SO,false>::find( size_t i, size_t j ) const
{
   return matrix_.find( i, j );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns an iterator to the first index not less then the given index.
//
// \param i The row index of the search element. The index has to be in the range \f$[0..N-1]\f$.
// \param j The column index of the search element. The index has to be in the range \f$[0..N-1]\f$.
// \return Iterator to the first index not less then the given index, end() iterator otherwise.
//
// In case of a row-major matrix, this function returns a row iterator to the first element with
// an index not less then the given column index. In case of a column-major matrix, the function
// returns a column iterator to the first element with an index not less then the given row
// index. In combination with the upperBound() function this function can be used to create
// a pair of iterators specifying a range of indices. Note that the returned unilower matrix
// iterator is subject to invalidation due to inserting operations via the function call
// operator or the insert() function!
*/
template< typename MT  // Type of the adapted sparse matrix
        , bool SO >    // Storage order of the adapted sparse matrix
inline typename UniLowerMatrix<MT,SO,false>::Iterator
   UniLowerMatrix<MT,SO,false>::lowerBound( size_t i, size_t j )
{
   return Iterator( matrix_.lowerBound( i, j ), ( SO ? j : i ) );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns an iterator to the first index not less then the given index.
//
// \param i The row index of the search element. The index has to be in the range \f$[0..N-1]\f$.
// \param j The column index of the search element. The index has to be in the range \f$[0..N-1]\f$.
// \return Iterator to the first index not less then the given index, end() iterator otherwise.
//
// In case of a row-major matrix, this function returns a row iterator to the first element with
// an index not less then the given column index. In case of a column-major matrix, the function
// returns a column iterator to the first element with an index not less then the given row
// index. In combination with the upperBound() function this function can be used to create
// a pair of iterators specifying a range of indices. Note that the returned unilower matrix
// iterator is subject to invalidation due to inserting operations via the function call
// operator or the insert() function!
*/
template< typename MT  // Type of the adapted sparse matrix
        , bool SO >    // Storage order of the adapted sparse matrix
inline typename UniLowerMatrix<MT,SO,false>::ConstIterator
   UniLowerMatrix<MT,SO,false>::lowerBound( size_t i, size_t j ) const
{
   return matrix_.lowerBound( i, j );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns an iterator to the first index greater then the given index.
//
// \param i The row index of the search element. The index has to be in the range \f$[0..N-1]\f$.
// \param j The column index of the search element. The index has to be in the range \f$[0..N-1]\f$.
// \return Iterator to the first index greater then the given index, end() iterator otherwise.
//
// In case of a row-major matrix, this function returns a row iterator to the first element with
// an index greater then the given column index. In case of a column-major matrix, the function
// returns a column iterator to the first element with an index greater then the given row
// index. In combination with the upperBound() function this function can be used to create
// a pair of iterators specifying a range of indices. Note that the returned unilower matrix
// iterator is subject to invalidation due to inserting operations via the function call
// operator or the insert() function!
*/
template< typename MT  // Type of the adapted sparse matrix
        , bool SO >    // Storage order of the adapted sparse matrix
inline typename UniLowerMatrix<MT,SO,false>::Iterator
   UniLowerMatrix<MT,SO,false>::upperBound( size_t i, size_t j )
{
   return Iterator( matrix_.upperBound( i, j ), ( SO ? j : i ) );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns an iterator to the first index greater then the given index.
//
// \param i The row index of the search element. The index has to be in the range \f$[0..N-1]\f$.
// \param j The column index of the search element. The index has to be in the range \f$[0..N-1]\f$.
// \return Iterator to the first index greater then the given index, end() iterator otherwise.
//
// In case of a row-major matrix, this function returns a row iterator to the first element with
// an index greater then the given column index. In case of a column-major matrix, the function
// returns a column iterator to the first element with an index greater then the given row
// index. In combination with the upperBound() function this function can be used to create
// a pair of iterators specifying a range of indices. Note that the returned unilower matrix
// iterator is subject to invalidation due to inserting operations via the function call
// operator or the insert() function!
*/
template< typename MT  // Type of the adapted sparse matrix
        , bool SO >    // Storage order of the adapted sparse matrix
inline typename UniLowerMatrix<MT,SO,false>::ConstIterator
   UniLowerMatrix<MT,SO,false>::upperBound( size_t i, size_t j ) const
{
   return matrix_.upperBound( i, j );
}
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  LOW-LEVEL UTILITY FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Appending elements to the specified row/column of the unilower matrix.
//
// \param i The row index of the new element. The index has to be in the range \f$[0..N-1]\f$.
// \param j The column index of the new element. The index has to be in the range \f$[0..N-1]\f$.
// \param value The value of the element to be appended.
// \param check \a true if the new value should be checked for default values, \a false if not.
// \return void
// \exception std::invalid_argument Invalid access to diagonal or upper matrix element.
//
// This function provides a very efficient way to fill a unilower sparse matrix with elements.
// It appends a new element to the end of the specified row/column without any additional memory
// allocation. Therefore it is strictly necessary to keep the following preconditions in mind:
//
//  - the index of the new element must be strictly larger than the largest index of non-zero
//    elements in the specified row/column of the sparse matrix
//  - the current number of non-zero elements in the matrix must be smaller than the capacity
//    of the matrix
//
// Ignoring these preconditions might result in undefined behavior! The optional \a check
// parameter specifies whether the new value should be tested for a default value. If the new
// value is a default value (for instance 0 in case of an integral element type) the value is
// not appended. Per default the values are not tested.
//
// In combination with the reserve() and the finalize() function, append() provides the most
// efficient way to add new elements to a (newly created) sparse matrix:

   \code
   using blaze::CompressedMatrix;
   using blaze::UniLowerMatrix;
   using blaze::columnMajor;

   UniLowerMatrix< CompressedMatrix<double,columnMajor> > A( 4 );

   A.reserve( 3 );         // Reserving enough space for 3 non-zero elements
   A.append( 1, 0, 1.0 );  // Appending the value 1 in column 0 with row index 1
   A.finalize( 0 );        // Finalizing column 0
   A.append( 2, 1, 2.0 );  // Appending the value 2 in column 1 with row index 2
   A.finalize( 1 );        // Finalizing column 1
   A.append( 3, 2, 3.0 );  // Appending the value 3 in column 2 with row index 3
   A.finalize( 2 );        // Finalizing column 2
   \endcode

// Note that although append() does not allocate new memory it still invalidates all iterators
// returned by the end() functions! Also note that the attempt to append an element within the
// upper part of the matrix (i.e. above the diagonal) will result in a \a std::invalid_argument
// exception.
*/
template< typename MT  // Type of the adapted sparse matrix
        , bool SO >    // Storage order of the adapted sparse matrix
inline void UniLowerMatrix<MT,SO,false>::append( size_t i, size_t j, const ElementType& value, bool check )
{
   if( i <= j )
      throw std::invalid_argument( "Invalid access to diagonal or upper matrix element" );

   if( !check || !isDefault( value ) )
      matrix_.insert( i, j, value );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Finalizing the element insertion of a row/column.
//
// \param i The index of the row/column to be finalized \f$[0..N-1]\f$.
// \return void
//
// This function is part of the low-level interface to efficiently fill a matrix with elements.
// After completion of row/column \a i via the append() function, this function can be called to
// finalize row/column \a i and prepare the next row/column for insertion process via append().
//
// \b Note: Although finalize() does not allocate new memory, it still invalidates all iterators
// returned by the end() functions!
*/
template< typename MT  // Type of the adapted sparse matrix
        , bool SO >    // Storage order of the adapted sparse matrix
inline void UniLowerMatrix<MT,SO,false>::finalize( size_t i )
{
   matrix_.trim( i );
}
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  EXPRESSION TEMPLATE EVALUATION FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns whether the matrix can alias with the given address \a alias.
//
// \param alias The alias to be checked.
// \return \a true in case the alias corresponds to this matrix, \a false if not.
//
// This function returns whether the given address can alias with the matrix. In contrast
// to the isAliased() function this function is allowed to use compile time expressions
// to optimize the evaluation.
*/
template< typename MT       // Type of the adapted sparse matrix
        , bool SO >         // Storage order of the adapted sparse matrix
template< typename Other >  // Data type of the foreign expression
inline bool UniLowerMatrix<MT,SO,false>::canAlias( const Other* alias ) const
{
   return matrix_.canAlias( alias );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns whether the matrix is aliased with the given address \a alias.
//
// \param alias The alias to be checked.
// \return \a true in case the alias corresponds to this matrix, \a false if not.
//
// This function returns whether the given address is aliased with the matrix. In contrast
// to the canAlias() function this function is not allowed to use compile time expressions
// to optimize the evaluation.
*/
template< typename MT       // Type of the adapted sparse matrix
        , bool SO >         // Storage order of the adapted sparse matrix
template< typename Other >  // Data type of the foreign expression
inline bool UniLowerMatrix<MT,SO,false>::isAliased( const Other* alias ) const
{
   return matrix_.isAliased( alias );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns whether the matrix can be used in SMP assignments.
//
// \return \a true in case the matrix can be used in SMP assignments, \a false if not.
//
// This function returns whether the matrix can be used in SMP assignments. In contrast to the
// \a smpAssignable member enumeration, which is based solely on compile time information, this
// function additionally provides runtime information (as for instance the current number of
// rows and/or columns of the matrix).
*/
template< typename MT  // Type of the adapted sparse matrix
        , bool SO >    // Storage order of the adapted sparse matrix
inline bool UniLowerMatrix<MT,SO,false>::canSMPAssign() const
{
   return matrix_.canSMPAssign();
}
/*! \endcond */
//*************************************************************************************************

} // namespace blaze

#endif