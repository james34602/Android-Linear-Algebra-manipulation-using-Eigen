// This file is part of Eigen, a lightweight C++ template library
// for linear algebra.
//
// Copyright (C) 2008-2010 Gael Guennebaud <gael.guennebaud@inria.fr>
// Copyright (C) 2008-2009 Benoit Jacob <jacob.benoit.1@gmail.com>
// Copyright (C) 2009 Kenneth Riddile <kfriddile@yahoo.com>
// Copyright (C) 2010 Hauke Heibel <hauke.heibel@gmail.com>
// Copyright (C) 2010 Thomas Capricelli <orzel@freehackers.org>
//
// Eigen is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
//
// Alternatively, you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// Eigen is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License or the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License and a copy of the GNU General Public License along with
// Eigen. If not, see <http://www.gnu.org/licenses/>.


/*****************************************************************************
*** Platform checks for aligned malloc functions                           ***
*****************************************************************************/

#ifndef EIGEN_MEMORY_H
#define EIGEN_MEMORY_H

// On 64-bit systems, glibc's malloc returns 16-byte-aligned pointers, see:
//   http://www.gnu.org/s/libc/manual/html_node/Aligned-Memory-Blocks.html
// This is true at least since glibc 2.8.
// This leaves the question how to detect 64-bit. According to this document,
//   http://gcc.fyxm.net/summit/2003/Porting%20to%2064%20bit.pdf
// page 114, "[The] LP64 model [...] is used by all 64-bit UNIX ports" so it's indeed
// quite safe, at least within the context of glibc, to equate 64-bit with LP64.
#if defined(__GLIBC__) && ((__GLIBC__>=2 && __GLIBC_MINOR__ >= 8) || __GLIBC__>2) \
 && defined(__LP64__)
  #define EIGEN_GLIBC_MALLOC_ALREADY_ALIGNED 1
#else
  #define EIGEN_GLIBC_MALLOC_ALREADY_ALIGNED 0
#endif

// FreeBSD 6 seems to have 16-byte aligned malloc
//   See http://svn.freebsd.org/viewvc/base/stable/6/lib/libc/stdlib/malloc.c?view=markup
// FreeBSD 7 seems to have 16-byte aligned malloc except on ARM and MIPS architectures
//   See http://svn.freebsd.org/viewvc/base/stable/7/lib/libc/stdlib/malloc.c?view=markup
#if defined(__FreeBSD__) && !defined(__arm__) && !defined(__mips__)
  #define EIGEN_FREEBSD_MALLOC_ALREADY_ALIGNED 1
#else
  #define EIGEN_FREEBSD_MALLOC_ALREADY_ALIGNED 0
#endif

#if defined(__APPLE__) \
 || defined(_WIN64) \
 || EIGEN_GLIBC_MALLOC_ALREADY_ALIGNED \
 || EIGEN_FREEBSD_MALLOC_ALREADY_ALIGNED
  #define EIGEN_MALLOC_ALREADY_ALIGNED 1
#else
  #define EIGEN_MALLOC_ALREADY_ALIGNED 0
#endif

#if ((defined __QNXNTO__) || (defined _GNU_SOURCE) || ((defined _XOPEN_SOURCE) && (_XOPEN_SOURCE >= 600))) \
 && (defined _POSIX_ADVISORY_INFO) && (_POSIX_ADVISORY_INFO > 0)
  #define EIGEN_HAS_POSIX_MEMALIGN 1
#else
  #define EIGEN_HAS_POSIX_MEMALIGN 0
#endif

#ifdef EIGEN_VECTORIZE_SSE
  #define EIGEN_HAS_MM_MALLOC 1
#else
  #define EIGEN_HAS_MM_MALLOC 0
#endif

/*****************************************************************************
*** Implementation of handmade aligned functions                           ***
*****************************************************************************/

/* ----- Hand made implementations of aligned malloc/free and realloc ----- */

/** \internal Like malloc, but the returned pointer is guaranteed to be 16-byte aligned.
  * Fast, but wastes 16 additional bytes of memory. Does not throw any exception.
  */
inline void* ei_handmade_aligned_malloc(size_t size)
{
  void *original = std::malloc(size+16);
  if (original == 0) return 0;
  void *aligned = reinterpret_cast<void*>((reinterpret_cast<size_t>(original) & ~(size_t(15))) + 16);
  *(reinterpret_cast<void**>(aligned) - 1) = original;
  return aligned;
}

/** \internal Frees memory allocated with ei_handmade_aligned_malloc */
inline void ei_handmade_aligned_free(void *ptr)
{
  if (ptr) std::free(*(reinterpret_cast<void**>(ptr) - 1));
}

/** \internal
  * \brief Reallocates aligned memory.
  * Since we know that our handmade version is based on std::realloc
  * we can use std::realloc to implement efficient reallocation.
  */
inline void* ei_handmade_aligned_realloc(void* ptr, size_t size, size_t = 0)
{
  if (ptr == 0) return ei_handmade_aligned_malloc(size);
  void *original = *(reinterpret_cast<void**>(ptr) - 1);
  original = std::realloc(original,size+16);
  if (original == 0) return 0;
  void *aligned = reinterpret_cast<void*>((reinterpret_cast<size_t>(original) & ~(size_t(15))) + 16);
  *(reinterpret_cast<void**>(aligned) - 1) = original;
  return aligned;
}

/*****************************************************************************
*** Implementation of generic aligned realloc (when no realloc can be used)***
*****************************************************************************/

void* ei_aligned_malloc(size_t size);
void  ei_aligned_free(void *ptr);

/** \internal
  * \brief Reallocates aligned memory.
  * Allows reallocation with aligned ptr types. This implementation will
  * always create a new memory chunk and copy the old data.
  */
inline void* ei_generic_aligned_realloc(void* ptr, size_t size, size_t old_size)
{
  if (ptr==0)
    return ei_aligned_malloc(size);

  if (size==0)
  {
    ei_aligned_free(ptr);
    return 0;
  }

  void* newptr = ei_aligned_malloc(size);
  if (newptr == 0)
  {
    #ifdef EIGEN_HAS_ERRNO
    errno = ENOMEM; // according to the standard
    #endif
    return 0;
  }

  if (ptr != 0)
  {
    std::memcpy(newptr, ptr, std::min(size,old_size));
    ei_aligned_free(ptr);
  }

  return newptr;
}

/*****************************************************************************
*** Implementation of portable aligned versions of malloc/free/realloc     ***
*****************************************************************************/

/** \internal Allocates \a size bytes. The returned pointer is guaranteed to have 16 bytes alignment.
  * On allocation error, the returned pointer is null, and if exceptions are enabled then a std::bad_alloc is thrown.
  */
inline void* ei_aligned_malloc(size_t size)
{
  #ifdef EIGEN_NO_MALLOC
    ei_assert(false && "heap allocation is forbidden (EIGEN_NO_MALLOC is defined)");
  #endif

  void *result;
  #if !EIGEN_ALIGN
    result = std::malloc(size);
  #elif EIGEN_MALLOC_ALREADY_ALIGNED
    result = std::malloc(size);
  #elif EIGEN_HAS_POSIX_MEMALIGN
    if(posix_memalign(&result, 16, size)) result = 0;
  #elif EIGEN_HAS_MM_MALLOC
    result = _mm_malloc(size, 16);
  #elif (defined _MSC_VER)
    result = _aligned_malloc(size, 16);
  #else
    result = ei_handmade_aligned_malloc(size);
  #endif

  #ifdef EIGEN_EXCEPTIONS
    if(result == 0)
      throw std::bad_alloc();
  #endif
  return result;
}

/** \internal Frees memory allocated with ei_aligned_malloc. */
inline void ei_aligned_free(void *ptr)
{
  #if !EIGEN_ALIGN
    std::free(ptr);
  #elif EIGEN_MALLOC_ALREADY_ALIGNED
    std::free(ptr);
  #elif EIGEN_HAS_POSIX_MEMALIGN
    std::free(ptr);
  #elif EIGEN_HAS_MM_MALLOC
    _mm_free(ptr);
  #elif defined(_MSC_VER)
    _aligned_free(ptr);
  #else
    ei_handmade_aligned_free(ptr);
  #endif
}

/**
* \internal
* \brief Reallocates an aligned block of memory.
* \throws std::bad_alloc if EIGEN_EXCEPTIONS are defined.
**/
inline void* ei_aligned_realloc(void *ptr, size_t new_size, size_t old_size)
{
  EIGEN_UNUSED_VARIABLE(old_size);

  void *result;
#if !EIGEN_ALIGN
  result = std::realloc(ptr,new_size);
#elif EIGEN_MALLOC_ALREADY_ALIGNED
  result = std::realloc(ptr,new_size);
#elif EIGEN_HAS_POSIX_MEMALIGN
  result = ei_generic_aligned_realloc(ptr,new_size,old_size);
#elif EIGEN_HAS_MM_MALLOC
  // The defined(_mm_free) is just here to verify that this MSVC version
  // implements _mm_malloc/_mm_free based on the corresponding _aligned_
  // functions. This may not always be the case and we just try to be safe.
  #if defined(_MSC_VER) && defined(_mm_free)
    result = _aligned_realloc(ptr,new_size,16);
  #else
    result = ei_generic_aligned_realloc(ptr,new_size,old_size);
  #endif
#elif defined(_MSC_VER)
  result = _aligned_realloc(ptr,new_size,16);
#else
  result = ei_handmade_aligned_realloc(ptr,new_size,old_size);
#endif

#ifdef EIGEN_EXCEPTIONS
  if (result==0 && new_size!=0)
    throw std::bad_alloc();
#endif
  return result;
}

/*****************************************************************************
*** Implementation of conditionally aligned functions                      ***
*****************************************************************************/

/** \internal Allocates \a size bytes. If Align is true, then the returned ptr is 16-byte-aligned.
  * On allocation error, the returned pointer is null, and if exceptions are enabled then a std::bad_alloc is thrown.
  */
template<bool Align> inline void* ei_conditional_aligned_malloc(size_t size)
{
  return ei_aligned_malloc(size);
}

template<> inline void* ei_conditional_aligned_malloc<false>(size_t size)
{
  #ifdef EIGEN_NO_MALLOC
    ei_assert(false && "heap allocation is forbidden (EIGEN_NO_MALLOC is defined)");
  #endif

  void *result = std::malloc(size);
  #ifdef EIGEN_EXCEPTIONS
    if(!result) throw std::bad_alloc();
  #endif
  return result;
}

/** \internal Frees memory allocated with ei_conditional_aligned_malloc */
template<bool Align> inline void ei_conditional_aligned_free(void *ptr)
{
  ei_aligned_free(ptr);
}

template<> inline void ei_conditional_aligned_free<false>(void *ptr)
{
  std::free(ptr);
}

template<bool Align> inline void* ei_conditional_aligned_realloc(void* ptr, size_t new_size, size_t old_size)
{
  return ei_aligned_realloc(ptr, new_size, old_size);
}

template<> inline void* ei_conditional_aligned_realloc<false>(void* ptr, size_t new_size, size_t)
{
  return std::realloc(ptr, new_size);
}

/*****************************************************************************
*** Construction/destruction of array elements                             ***
*****************************************************************************/

/** \internal Constructs the elements of an array.
  * The \a size parameter tells on how many objects to call the constructor of T.
  */
template<typename T> inline T* ei_construct_elements_of_array(T *ptr, size_t size)
{
  for (size_t i=0; i < size; ++i) ::new (ptr + i) T;
  return ptr;
}

/** \internal Destructs the elements of an array.
  * The \a size parameters tells on how many objects to call the destructor of T.
  */
template<typename T> inline void ei_destruct_elements_of_array(T *ptr, size_t size)
{
  // always destruct an array starting from the end.
  if(ptr)
    while(size) ptr[--size].~T();
}

/*****************************************************************************
*** Implementation of aligned new/delete-like functions                    ***
*****************************************************************************/

/** \internal Allocates \a size objects of type T. The returned pointer is guaranteed to have 16 bytes alignment.
  * On allocation error, the returned pointer is undefined, but if exceptions are enabled then a std::bad_alloc is thrown.
  * The default constructor of T is called.
  */
template<typename T> inline T* ei_aligned_new(size_t size)
{
  T *result = reinterpret_cast<T*>(ei_aligned_malloc(sizeof(T)*size));
  return ei_construct_elements_of_array(result, size);
}

template<typename T, bool Align> inline T* ei_conditional_aligned_new(size_t size)
{
  T *result = reinterpret_cast<T*>(ei_conditional_aligned_malloc<Align>(sizeof(T)*size));
  return ei_construct_elements_of_array(result, size);
}

/** \internal Deletes objects constructed with ei_aligned_new
  * The \a size parameters tells on how many objects to call the destructor of T.
  */
template<typename T> inline void ei_aligned_delete(T *ptr, size_t size)
{
  ei_destruct_elements_of_array<T>(ptr, size);
  ei_aligned_free(ptr);
}

/** \internal Deletes objects constructed with ei_conditional_aligned_new
  * The \a size parameters tells on how many objects to call the destructor of T.
  */
template<typename T, bool Align> inline void ei_conditional_aligned_delete(T *ptr, size_t size)
{
  ei_destruct_elements_of_array<T>(ptr, size);
  ei_conditional_aligned_free<Align>(ptr);
}

template<typename T, bool Align> inline T* ei_conditional_aligned_realloc_new(T* pts, size_t new_size, size_t old_size)
{
  T *result = reinterpret_cast<T*>(ei_conditional_aligned_realloc<Align>(reinterpret_cast<void*>(pts), sizeof(T)*new_size, sizeof(T)*old_size));
  if (new_size > old_size)
    ei_construct_elements_of_array(result+old_size, new_size-old_size);
  return result;
}

/****************************************************************************/

/** \internal Returns the index of the first element of the array that is well aligned for vectorization.
  *
  * \param array the address of the start of the array
  * \param size the size of the array
  *
  * \note If no element of the array is well aligned, the size of the array is returned. Typically,
  * for example with SSE, "well aligned" means 16-byte-aligned. If vectorization is disabled or if the
  * packet size for the given scalar type is 1, then everything is considered well-aligned.
  *
  * \note If the scalar type is vectorizable, we rely on the following assumptions: sizeof(Scalar) is a
  * power of 2, the packet size in bytes is also a power of 2, and is a multiple of sizeof(Scalar). On the
  * other hand, we do not assume that the array address is a multiple of sizeof(Scalar), as that fails for
  * example with Scalar=double on certain 32-bit platforms, see bug #79.
  *
  * There is also the variant ei_first_aligned(const MatrixBase&) defined in DenseCoeffsBase.h.
  */
template<typename Scalar, typename Index>
inline static Index ei_first_aligned(const Scalar* array, Index size)
{
  typedef typename ei_packet_traits<Scalar>::type Packet;
  enum { PacketSize = ei_packet_traits<Scalar>::size,
         PacketAlignedMask = PacketSize-1
  };

  if(PacketSize==1)
  {
    // Either there is no vectorization, or a packet consists of exactly 1 scalar so that all elements
    // of the array have the same alignment.
    return 0;
  }
  else if(size_t(array) & (sizeof(Scalar)-1))
  {
    // There is vectorization for this scalar type, but the array is not aligned to the size of a single scalar.
    // Consequently, no element of the array is well aligned.
    return size;
  }
  else
  {
    return std::min<Index>( (PacketSize - (Index((size_t(array)/sizeof(Scalar))) & PacketAlignedMask))
                           & PacketAlignedMask, size);
  }
}

/*****************************************************************************
*** Implementation of runtime stack allocation (falling back to malloc)    ***
*****************************************************************************/

/** \internal
  * Allocates an aligned buffer of SIZE bytes on the stack if SIZE is smaller than
  * EIGEN_STACK_ALLOCATION_LIMIT, and if stack allocation is supported by the platform
  * (currently, this is Linux only). Otherwise the memory is allocated on the heap.
  * Data allocated with ei_aligned_stack_alloc \b must be freed by calling
  * ei_aligned_stack_free(PTR,SIZE).
  * \code
  * float * data = ei_aligned_stack_alloc(float,array.size());
  * // ...
  * ei_aligned_stack_free(data,float,array.size());
  * \endcode
  */
#if (defined __linux__)
  #define ei_aligned_stack_alloc(SIZE) (SIZE<=EIGEN_STACK_ALLOCATION_LIMIT) \
                                    ? alloca(SIZE) \
                                    : ei_aligned_malloc(SIZE)
  #define ei_aligned_stack_free(PTR,SIZE) if(SIZE>EIGEN_STACK_ALLOCATION_LIMIT) ei_aligned_free(PTR)
#elif defined(_MSC_VER)
  #define ei_aligned_stack_alloc(SIZE) (SIZE<=EIGEN_STACK_ALLOCATION_LIMIT) \
                                    ? _alloca(SIZE) \
                                    : ei_aligned_malloc(SIZE)
  #define ei_aligned_stack_free(PTR,SIZE) if(SIZE>EIGEN_STACK_ALLOCATION_LIMIT) ei_aligned_free(PTR)
#else
  #define ei_aligned_stack_alloc(SIZE) ei_aligned_malloc(SIZE)
  #define ei_aligned_stack_free(PTR,SIZE) ei_aligned_free(PTR)
#endif

#define ei_aligned_stack_new(TYPE,SIZE) ei_construct_elements_of_array(reinterpret_cast<TYPE*>(ei_aligned_stack_alloc(sizeof(TYPE)*SIZE)), SIZE)
#define ei_aligned_stack_delete(TYPE,PTR,SIZE) do {ei_destruct_elements_of_array<TYPE>(PTR, SIZE); \
                                                   ei_aligned_stack_free(PTR,sizeof(TYPE)*SIZE);} while(0)


/*****************************************************************************
*** Implementation of EIGEN_MAKE_ALIGNED_OPERATOR_NEW [_IF]                ***
*****************************************************************************/

#if EIGEN_ALIGN
  #ifdef EIGEN_EXCEPTIONS
    #define EIGEN_MAKE_ALIGNED_OPERATOR_NEW_NOTHROW(NeedsToAlign) \
      void* operator new(size_t size, const std::nothrow_t&) throw() { \
        try { return Eigen::ei_conditional_aligned_malloc<NeedsToAlign>(size); } \
        catch (...) { return 0; } \
        return 0; \
      }
  #else
    #define EIGEN_MAKE_ALIGNED_OPERATOR_NEW_NOTHROW(NeedsToAlign) \
      void* operator new(size_t size, const std::nothrow_t&) throw() { \
        return Eigen::ei_conditional_aligned_malloc<NeedsToAlign>(size); \
      }
  #endif

  #define EIGEN_MAKE_ALIGNED_OPERATOR_NEW_IF(NeedsToAlign) \
      void *operator new(size_t size) { \
        return Eigen::ei_conditional_aligned_malloc<NeedsToAlign>(size); \
      } \
      void *operator new[](size_t size) { \
        return Eigen::ei_conditional_aligned_malloc<NeedsToAlign>(size); \
      } \
      void operator delete(void * ptr) throw() { Eigen::ei_conditional_aligned_free<NeedsToAlign>(ptr); } \
      void operator delete[](void * ptr) throw() { Eigen::ei_conditional_aligned_free<NeedsToAlign>(ptr); } \
      /* in-place new and delete. since (at least afaik) there is no actual   */ \
      /* memory allocated we can safely let the default implementation handle */ \
      /* this particular case. */ \
      static void *operator new(size_t size, void *ptr) { return ::operator new(size,ptr); } \
      void operator delete(void * memory, void *ptr) throw() { return ::operator delete(memory,ptr); } \
      /* nothrow-new (returns zero instead of std::bad_alloc) */ \
      EIGEN_MAKE_ALIGNED_OPERATOR_NEW_NOTHROW(NeedsToAlign) \
      void operator delete(void *ptr, const std::nothrow_t&) throw() { \
        Eigen::ei_conditional_aligned_free<NeedsToAlign>(ptr); \
      } \
      typedef void ei_operator_new_marker_type;
#else
  #define EIGEN_MAKE_ALIGNED_OPERATOR_NEW_IF(NeedsToAlign)
#endif

#define EIGEN_MAKE_ALIGNED_OPERATOR_NEW EIGEN_MAKE_ALIGNED_OPERATOR_NEW_IF(true)
#define EIGEN_MAKE_ALIGNED_OPERATOR_NEW_IF_VECTORIZABLE_FIXED_SIZE(Scalar,Size) \
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW_IF(((Size)!=Eigen::Dynamic) && ((sizeof(Scalar)*(Size))%16==0))

/****************************************************************************/

/** \class aligned_allocator
* \ingroup Core_Module
*
* \brief STL compatible allocator to use with with 16 byte aligned types
*
* Example:
* \code
* // Matrix4f requires 16 bytes alignment:
* std::map< int, Matrix4f, std::less<int>, aligned_allocator<Matrix4f> > my_map_mat4;
* // Vector3f does not require 16 bytes alignment, no need to use Eigen's allocator:
* std::map< int, Vector3f > my_map_vec3;
* \endcode
*
*/
template<class T>
class aligned_allocator
{
public:
    typedef size_t    size_type;
    typedef std::ptrdiff_t difference_type;
    typedef T*        pointer;
    typedef const T*  const_pointer;
    typedef T&        reference;
    typedef const T&  const_reference;
    typedef T         value_type;

    template<class U>
    struct rebind
    {
        typedef aligned_allocator<U> other;
    };

    pointer address( reference value ) const
    {
        return &value;
    }

    const_pointer address( const_reference value ) const
    {
        return &value;
    }

    aligned_allocator() throw()
    {
    }

    aligned_allocator( const aligned_allocator& ) throw()
    {
    }

    template<class U>
    aligned_allocator( const aligned_allocator<U>& ) throw()
    {
    }

    ~aligned_allocator() throw()
    {
    }

    size_type max_size() const throw()
    {
        return std::numeric_limits<size_type>::max();
    }

    pointer allocate( size_type num, const_pointer* hint = 0 )
    {
        static_cast<void>( hint ); // suppress unused variable warning
        return static_cast<pointer>( ei_aligned_malloc( num * sizeof(T) ) );
    }

    void construct( pointer p, const T& value )
    {
        ::new( p ) T( value );
    }

    void destroy( pointer p )
    {
        p->~T();
    }

    void deallocate( pointer p, size_type /*num*/ )
    {
        ei_aligned_free( p );
    }

    bool operator!=(const aligned_allocator<T>& ) const
    { return false; }

    bool operator==(const aligned_allocator<T>& ) const
    { return true; }
};

//---------- Cache sizes ----------
/** \internal
 * Queries and returns the cache sizes in Bytes of the L1, L2, and L3 data caches respectively */
inline void ei_queryCacheSizes(int& l1, int& l2, int& l3)
{
  l1 = l2 = l3 = -1;
}

/** \internal
 * \returns the size in Bytes of the L1 data cache */
inline int ei_queryL1CacheSize()
{
  int l1(-1), l2, l3;
  ei_queryCacheSizes(l1,l2,l3);
  return l1;
}

/** \internal
 * \returns the size in Bytes of the L2 or L3 cache if this later is present */
inline int ei_queryTopLevelCacheSize()
{
  int l1, l2(-1), l3(-1);
  ei_queryCacheSizes(l1,l2,l3);
  return std::max(l2,l3);
}

#endif // EIGEN_MEMORY_H
