
// -*- C++ -*-
// Definition for Win32 Export directives.
// This file is generated automatically by generate_export_file.pl topic_annotations_test_lib
// ------------------------------
#ifndef TOPIC_ANNOTATIONS_TEST_LIB_EXPORT_H
#define TOPIC_ANNOTATIONS_TEST_LIB_EXPORT_H

#include "ace/config-all.h"

#if defined (ACE_AS_STATIC_LIBS) && !defined (TOPIC_ANNOTATIONS_TEST_LIB_HAS_DLL)
#  define TOPIC_ANNOTATIONS_TEST_LIB_HAS_DLL 0
#endif /* ACE_AS_STATIC_LIBS && TOPIC_ANNOTATIONS_TEST_LIB_HAS_DLL */

#if !defined (TOPIC_ANNOTATIONS_TEST_LIB_HAS_DLL)
#  define TOPIC_ANNOTATIONS_TEST_LIB_HAS_DLL 1
#endif /* ! TOPIC_ANNOTATIONS_TEST_LIB_HAS_DLL */

#if defined (TOPIC_ANNOTATIONS_TEST_LIB_HAS_DLL) && (TOPIC_ANNOTATIONS_TEST_LIB_HAS_DLL == 1)
#  if defined (TOPIC_ANNOTATIONS_TEST_LIB_BUILD_DLL)
#    define topic_annotations_test_lib_Export ACE_Proper_Export_Flag
#    define TOPIC_ANNOTATIONS_TEST_LIB_SINGLETON_DECLARATION(T) ACE_EXPORT_SINGLETON_DECLARATION (T)
#    define TOPIC_ANNOTATIONS_TEST_LIB_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_EXPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  else /* TOPIC_ANNOTATIONS_TEST_LIB_BUILD_DLL */
#    define topic_annotations_test_lib_Export ACE_Proper_Import_Flag
#    define TOPIC_ANNOTATIONS_TEST_LIB_SINGLETON_DECLARATION(T) ACE_IMPORT_SINGLETON_DECLARATION (T)
#    define TOPIC_ANNOTATIONS_TEST_LIB_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_IMPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  endif /* TOPIC_ANNOTATIONS_TEST_LIB_BUILD_DLL */
#else /* TOPIC_ANNOTATIONS_TEST_LIB_HAS_DLL == 1 */
#  define topic_annotations_test_lib_Export
#  define TOPIC_ANNOTATIONS_TEST_LIB_SINGLETON_DECLARATION(T)
#  define TOPIC_ANNOTATIONS_TEST_LIB_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#endif /* TOPIC_ANNOTATIONS_TEST_LIB_HAS_DLL == 1 */

// Set TOPIC_ANNOTATIONS_TEST_LIB_NTRACE = 0 to turn on library specific tracing even if
// tracing is turned off for ACE.
#if !defined (TOPIC_ANNOTATIONS_TEST_LIB_NTRACE)
#  if (ACE_NTRACE == 1)
#    define TOPIC_ANNOTATIONS_TEST_LIB_NTRACE 1
#  else /* (ACE_NTRACE == 1) */
#    define TOPIC_ANNOTATIONS_TEST_LIB_NTRACE 0
#  endif /* (ACE_NTRACE == 1) */
#endif /* !TOPIC_ANNOTATIONS_TEST_LIB_NTRACE */

#if (TOPIC_ANNOTATIONS_TEST_LIB_NTRACE == 1)
#  define TOPIC_ANNOTATIONS_TEST_LIB_TRACE(X)
#else /* (TOPIC_ANNOTATIONS_TEST_LIB_NTRACE == 1) */
#  if !defined (ACE_HAS_TRACE)
#    define ACE_HAS_TRACE
#  endif /* ACE_HAS_TRACE */
#  define TOPIC_ANNOTATIONS_TEST_LIB_TRACE(X) ACE_TRACE_IMPL(X)
#  include "ace/Trace.h"
#endif /* (TOPIC_ANNOTATIONS_TEST_LIB_NTRACE == 1) */

#endif /* TOPIC_ANNOTATIONS_TEST_LIB_EXPORT_H */

// End of auto generated file.