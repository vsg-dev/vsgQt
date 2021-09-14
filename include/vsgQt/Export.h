
#ifndef VSGQT_DECLSPEC_H
#define VSGQT_DECLSPEC_H

#ifdef VSGQT_STATIC_DEFINE
#  define VSGQT_DECLSPEC
#  define VSGQT_NO_EXPORT
#else
#  ifndef VSGQT_DECLSPEC
#    ifdef vsgQt_EXPORTS
        /* We are building this library */
#      define VSGQT_DECLSPEC 
#    else
        /* We are using this library */
#      define VSGQT_DECLSPEC 
#    endif
#  endif

#  ifndef VSGQT_NO_EXPORT
#    define VSGQT_NO_EXPORT 
#  endif
#endif

#ifndef VSGQT_DEPRECATED
#  define VSGQT_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef VSGQT_DEPRECATED_EXPORT
#  define VSGQT_DEPRECATED_EXPORT VSGQT_DECLSPEC VSGQT_DEPRECATED
#endif

#ifndef VSGQT_DEPRECATED_NO_EXPORT
#  define VSGQT_DEPRECATED_NO_EXPORT VSGQT_NO_EXPORT VSGQT_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef VSGQT_NO_DEPRECATED
#    define VSGQT_NO_DEPRECATED
#  endif
#endif

#endif /* VSGQT_DECLSPEC_H */
