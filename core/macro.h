#ifndef _CORE_MACRO_H
#define _CORE_MACRO_H

// core/macro.h
// 1/20/2006

// - CORE_VA_COUNT -

#define CORE_LP (
#define CORE_RP )

#ifdef _MSC_VER
  #define CORE_VA_COUNT(...)    ARGCOUNT(__VA_ARGS__)
#elif defined(__GNUC__)
  #define CORE_VA_COUNT(...)    PP_NARG(__VA_ARGS__)
#else
  #define CORE_VA_COUNT         __static_assert_0__CORE_VA_COUNT_not_supported__
#endif // _MSC_VER || __GNUC__

// See: http://www.gamedev.net/topic/567686-vc-macro-fail /
#ifdef _MSC_VER
  #define LASTARG(_1, _2, _3, _4, _5, _6, _7, _8, _9, N, ...) N
  #define ARGCOUNT(...) LASTARG CORE_LP __VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 CORE_RP
#elif defined(__GNUC__)
  #define LASTARG(_1, _2, _3, _4, _5, _6, _7, _8, _9, N, ...) N
  #define ARGCOUNT(...) LASTARG(__VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#endif // __MSC_VER || __GNUC__

#ifdef __GNUC__

// The PP_NARG macro returns the number of arguments that have been passed to it.
// Note: GCC only
// Note: using PP_NARG() without arguments would violate 6.10.3p4 of ISO C99.
// See: http://groups.google.com/group/comp.std.c/browse_thread/thread/77ee8c8f92e4a3fb/346fc464319b1ee5?pli=1


#define PP_NARG(...) \
         PP_NARG_(__VA_ARGS__, PP_RSEQ_N())
#define PP_NARG_(...) \
         PP_ARG_N(__VA_ARGS__)
#define PP_ARG_N( \
          _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
         _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
         _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
         _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
         _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
         _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
         _61,_62,_63,N,...) N
#define PP_RSEQ_N() \
         63,62,61,60,                   \
         59,58,57,56,55,54,53,52,51,50, \
         49,48,47,46,45,44,43,42,41,40, \
         39,38,37,36,35,34,33,32,31,30, \
         29,28,27,26,25,24,23,22,21,20, \
         19,18,17,16,15,14,13,12,11,10, \
         9,8,7,6,5,4,3,2,1,0

// Some test cases
/*
PP_NARG(A) -> 1
PP_NARG(A,B) -> 2
PP_NARG(A,B,C) -> 3
PP_NARG(A,B,C,D) -> 4
PP_NARG(A,B,C,D,E) -> 5
PP_NARG(1,2,3,4,5,6,7,8,9,0,
         1,2,3,4,5,6,7,8,9,0,
         1,2,3,4,5,6,7,8,9,0,
         1,2,3,4,5,6,7,8,9,0,
         1,2,3,4,5,6,7,8,9,0,
         1,2,3,4,5,6,7,8,9,0,
*/

#endif // __GNUC__

// - CORE_VA_EMPTY -

#define CORE_VA_EMPTY(...)    ISEMPTY(__VA_ARGS__)

// http://gustedt.wordpress.com/2010/06/08/detect-empty-macro-arguments/
#define _ARG16(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, ...) _15
#define HAS_COMMA(...) _ARG16(__VA_ARGS__, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0)
#define _TRIGGER_PARENTHESIS_(...) ,

#define ISEMPTY(...)                                                    \
_ISEMPTY(                                                               \
          /* test if there is just one argument, eventually an empty    \
             one */                                                     \
          HAS_COMMA(__VA_ARGS__),                                       \
          /* test if _TRIGGER_PARENTHESIS_ together with the argument   \
             adds a comma */                                            \
          HAS_COMMA(_TRIGGER_PARENTHESIS_ __VA_ARGS__),                 \
          /* test if the argument together with a parenthesis           \
             adds a comma */                                            \
          HAS_COMMA(__VA_ARGS__ (~)),                                   \
          /* test if placing it between _TRIGGER_PARENTHESIS_ and the   \
             parenthesis adds a comma */                                \
          HAS_COMMA(_TRIGGER_PARENTHESIS_ __VA_ARGS__ (~))              \
          )

#define PASTE5(_0, _1, _2, _3, _4) _0 ## _1 ## _2 ## _3 ## _4
#define   _ISEMPTY(_0, _1, _2, _3) HAS_COMMA(PASTE5(_IS_EMPTY_CASE_, _0, _1, _2, _3))

/*
#define _IS_EMPTY_CASE_0001 ,
#define EATER0(...)
#define EATER1(...) ,
#define EATER2(...) (~)
#define EATER3(...) (~),
#define EATER4(...) EATER1
#define EATER5(...) EATER2
ISEMPTY()
ISEMPTY(//comment)
ISEMPTY(a)
ISEMPTY(a, b)
ISEMPTY(a, b, c)
ISEMPTY(a, b, c, d)
ISEMPTY(a, b, c, d, e)
ISEMPTY((void))
ISEMPTY((void), b, c, d)
ISEMPTY(_TRIGGER_PARENTHESIS_)
ISEMPTY(EATER0)
ISEMPTY(EATER1)
ISEMPTY(EATER2)
ISEMPTY(EATER3)
ISEMPTY(EATER4)
*/

// TODO
// http://gustedt.wordpress.com/2010/06/03/default-arguments-for-c99/
/*
#define _ARG2(_0, _1, _2, ...) _2
#define NARG2(...) _ARG2(__VA_ARGS__, 2, 1, 0)
#define _ONE_OR_TWO_ARGS_1(NAME, a) a, NAME ## _default_arg_1()
#define _ONE_OR_TWO_ARGS_2(NAME, a, b) a, b
#define __ONE_OR_TWO_ARGS(NAME, N, ...) _ONE_OR_TWO_ARGS_ ## N (NAME, __VA_ARGS__)
#define _ONE_OR_TWO_ARGS(NAME, N, ...) __ONE_OR_TWO_ARGS(NAME, N, __VA_ARGS__)
#define ONE_OR_TWO_ARGS(NAME, ...) NAME(_ONE_OR_TWO_ARGS(NAME, NARG2(__VA_ARGS__), __VA_ARGS__))
#define one_or_two(...) ONE_OR_TWO_ARGS(one_or_two, __VA_ARGS__)
*/

#endif // _CORE_MACRO_H

