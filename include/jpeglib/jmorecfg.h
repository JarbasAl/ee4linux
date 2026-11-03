/*
 * jmorecfg.h
 *
 * Copyright (C) 1991-1997, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains additional configuration options that customize the
 * JPEG software for special applications or support machine-dependent
 * optimizations.  Most users will not need to touch this file.
 *
 * RECONSTRUCTION (IJG libjpeg 6b), declarations only.
 */

/*
 * Define BITS_IN_JSAMPLE as either
 *   8   for 8-bit sample values (the usual setting)
 *   12  for 12-bit sample values
 */

#define BITS_IN_JSAMPLE  8	/* use 8 or 12 */

/*
 * Maximum number of components (color channels) allowed in JPEG image.
 */

#define MAX_COMPONENTS  10	/* maximum number of image components */


/* Representation of a single sample (pixel element value). */

#if BITS_IN_JSAMPLE == 8

#ifdef HAVE_UNSIGNED_CHAR

typedef unsigned char JSAMPLE;
#define GETJSAMPLE(value)  ((int) (value))

#else /* not HAVE_UNSIGNED_CHAR */

typedef char JSAMPLE;
#ifdef CHAR_IS_UNSIGNED
#define GETJSAMPLE(value)  ((int) (value))
#else
#define GETJSAMPLE(value)  ((int) (value) & 0xFF)
#endif /* CHAR_IS_UNSIGNED */

#endif /* HAVE_UNSIGNED_CHAR */

#define MAXJSAMPLE	255
#define CENTERJSAMPLE	128

#endif /* BITS_IN_JSAMPLE == 8 */


#if BITS_IN_JSAMPLE == 12

typedef short JSAMPLE;
#define GETJSAMPLE(value)  ((int) (value))

#define MAXJSAMPLE	4095
#define CENTERJSAMPLE	2048

#endif /* BITS_IN_JSAMPLE == 12 */


/* Representation of a DCT frequency coefficient. */

typedef short JCOEF;


/* Compressed datastreams are represented as arrays of JOCTET. */

#ifdef HAVE_UNSIGNED_CHAR

typedef unsigned char JOCTET;
#define GETJOCTET(value)  (value)

#else /* not HAVE_UNSIGNED_CHAR */

typedef char JOCTET;
#ifdef CHAR_IS_UNSIGNED
#define GETJOCTET(value)  (value)
#else
#define GETJOCTET(value)  ((value) & 0xFF)
#endif /* CHAR_IS_UNSIGNED */

#endif /* HAVE_UNSIGNED_CHAR */


/* These typedefs are used for various table entries and so forth. */

/* UINT8 must hold at least the values 0..255. */

#ifdef HAVE_UNSIGNED_CHAR
typedef unsigned char UINT8;
#else /* not HAVE_UNSIGNED_CHAR */
#ifdef CHAR_IS_UNSIGNED
typedef char UINT8;
#else /* not CHAR_IS_UNSIGNED */
typedef short UINT8;
#endif /* CHAR_IS_UNSIGNED */
#endif /* HAVE_UNSIGNED_CHAR */

/* UINT16 must hold at least the values 0..65535. */

#ifdef HAVE_UNSIGNED_SHORT
typedef unsigned short UINT16;
#else /* not HAVE_UNSIGNED_SHORT */
typedef unsigned int UINT16;
#endif /* HAVE_UNSIGNED_SHORT */

/* INT16 must hold at least the values -32768..32767. */

#ifndef XMD_H			/* X11/xmd.h correctly defines INT16 */
typedef short INT16;
#endif

/* INT32 must hold at least signed 32-bit values. */

#ifndef XMD_H			/* X11/xmd.h correctly defines INT32 */
#ifndef _BASETSD_H_		/* Microsoft defines it in basetsd.h */
typedef long INT32;
#endif
#endif

/* Datatype used for image dimensions. */

typedef unsigned int JDIMENSION;

#define JPEG_MAX_DIMENSION  65500L  /* a tad under 64K to prevent overflows */


/* These macros are used in all function definitions and extern declarations. */

/* a function called through method pointers: */
#define METHODDEF(type)		static type
/* a function used only in its module: */
#define LOCAL(type)		static type
/* a function referenced thru EXTERNs: */
#define GLOBAL(type)		type
/* a reference to a GLOBAL function: */
#define EXTERN(type)		extern type


/* This macro is used to declare a "method", that is, a function pointer. */

#ifdef HAVE_PROTOTYPES
#define JMETHOD(type,methodname,arglist)  type (*methodname) arglist
#else
#define JMETHOD(type,methodname,arglist)  type (*methodname) ()
#endif


/* Here is the pseudo-keyword for declaring pointers that must be "far". */

#ifdef NEED_FAR_POINTERS
#define FAR  far
#else
#ifndef FAR
#define FAR
#endif
#endif


/* boolean */

#ifndef HAVE_BOOLEAN
typedef int boolean;
#endif
#ifndef FALSE			/* in case these macros already exist */
#define FALSE	0		/* values of boolean */
#endif
#ifndef TRUE
#define TRUE	1
#endif


/* The remaining options affect code selection within the JPEG library. */

#ifdef JPEG_INTERNALS
#define JPEG_INTERNAL_OPTIONS
#endif

#ifdef JPEG_INTERNAL_OPTIONS

#define C_ARITH_CODING_SUPPORTED	/* Arithmetic coding back end? */
#undef C_ARITH_CODING_SUPPORTED
#define C_MULTISCAN_FILES_SUPPORTED /* Multiple-scan JPEG files? */
#define C_PROGRESSIVE_SUPPORTED	    /* Progressive JPEG? (Requires MULTISCAN)*/
#define ENTROPY_OPT_SUPPORTED	    /* Optimization of entropy coding parms? */
#define INPUT_SMOOTHING_SUPPORTED   /* Input image smoothing option? */

#undef  D_ARITH_CODING_SUPPORTED    /* Arithmetic coding back end? */
#define D_MULTISCAN_FILES_SUPPORTED /* Multiple-scan JPEG files? */
#define D_PROGRESSIVE_SUPPORTED	    /* Progressive JPEG? (Requires MULTISCAN)*/
#define SAVE_MARKERS_SUPPORTED	    /* jpeg_save_markers() needed? */
#define BLOCK_SMOOTHING_SUPPORTED   /* Block smoothing? (Progressive only) */
#define IDCT_SCALING_SUPPORTED	    /* Output rescaling via IDCT? */
#undef  UPSAMPLE_SCALING_SUPPORTED  /* Output rescaling at upsample stage? */
#define UPSAMPLE_MERGING_SUPPORTED  /* Fast path for sloppy upsampling? */
#define QUANT_1PASS_SUPPORTED	    /* 1-pass color quantization? */
#define QUANT_2PASS_SUPPORTED	    /* 2-pass color quantization? */

#define RGB_RED		0	/* Offset of Red in an RGB scanline element */
#define RGB_GREEN	1	/* Offset of Green */
#define RGB_BLUE	2	/* Offset of Blue */
#define RGB_PIXELSIZE	3	/* JSAMPLEs per RGB scanline element */

#ifndef INLINE
#ifdef __GNUC__
#define INLINE __inline__
#endif
#ifndef INLINE
#define INLINE			/* default is to define it as empty */
#endif
#endif

#ifndef MULTIPLIER
#define MULTIPLIER  int		/* type for fastest integer multiply */
#endif

#ifndef FAST_FLOAT
#ifdef HAVE_PROTOTYPES
#define FAST_FLOAT  float
#else
#define FAST_FLOAT  double
#endif
#endif

#endif /* JPEG_INTERNAL_OPTIONS */
