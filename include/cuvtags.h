/* These are the tags used by the Carleton University 3D Viewer 3.0. */ 

#ifndef CUVTAGS_H
#define CUVTAGS_H

// typdefs for data
typedef unsigned char  cuv_tag;
typedef unsigned int   cuv_data_order_tag;

typedef unsigned char  cuv_ubyte;
typedef unsigned short cuv_ushort;
typedef unsigned int   cuv_uint;
typedef float          cuv_float;
typedef double         cuv_double;

// for bit/byte order
#define DATA_ORDER_CHECK  1351861536
#define DATA_NORMAL_ORDER 1351861536
#define DATA_MIRROR_ORDER 549819216

// for version checking
#define VERSION_STRING "Carleton University 3D Viewer 3.6"
#define SHORT_VERSION_STRING "Carleton University 3D Viewer"

// tags for the sections
#define BEGIN_DATA    0xC0
#define BEGIN_VERSION 0xC4
#define BEGIN_IMG_SET 0xC5
#define BEGIN_MOV_SET 0xC6
#define BEGIN_SCENE   0xC8
#define USER_INTERACT 0xC9
#define BEGIN_VIEW    0xCC
#define RESET_VIEW    0xCD
#define GRAB_IMAGE    0xCE
#define GRAB_FRAME    0xCF

#define END_DATA_STAY 0xA0
#define END_DATA_EXIT 0xA1
#define END_VERSION   0xA4
#define END_IMG_SET   0xA5
#define END_MOV_SET   0xA6
#define END_SCENE     0xA8
#define END_VIEW      0xAC

// tags while in the scene section
#define SPOINT        0x00
#define SLINE         0x01
#define STRIA         0x02
#define SQUADRI       0x03
#define SSPHERE       0x04
#define SSPHOID       0x05
#define STEXT         0x06

#define SENTITY_ID    0x80
#define SENTITY_CLEAR 0x81
#define SENTITY_KILL  0x82
#define SENTITY_NAME  0x83

// shape properties in the scene section
#define SFILL         0x01
#define SOUTLINE      0x02
#define SMULTICOLOR   0x04
#define STRANSPARENT  0x08
#define SNORMALS      0x10
#define SNAMED        0x20
#define SUNDEFINED    0x40
#define SRESERVED     0x80

// tags while in the view section
#define VCAM_TRANS    0x00
#define VCAM_ROTATE   0x01
#define VCAM_ORBIT    0x02
#define VOUTLINE_CLR  0x03
#define VPRESET_VIEW  0x04


#define VVIEW_MODE    0x10
#define VFOV          0x11
#define VCLIP_PLANE   0x12
#define VLINE_WIDTH   0x13
#define VSHADING      0x14
#define VBIN_PAL_SEL  0x15
#define VBIN_PAL_MAP  0x16
#define VSPHERE_TESS  0x17
#define VBIN_PAL_MIN_MAX  0x18
#define VBIN_PAL_TYPE 0x19

#define VLIGHTING     0x20
#define VLIGHT        0x21
#define VAMB_LIGHT    0x22
#define VDIFF_LIGHT   0x23
#define VBG_LIGHT     0x24
#define VGAMMA_LEVEL  0x25
#define VSPECULAR_LEVEL 0x26
#define VFOG_LEVEL    0x27

#define VWIREFRAME    0x30
#define VOUTLINES     0x31
#define VTWO_SIDED    0x32
#define VTRANSPARENT  0x33
#define VBIN_PAL      0x34
#define VANTIALIAS    0x35
#define VGAMMA        0x36
#define VOPAQUE_WIRE  0x37
#define VFOG          0x38

// arguments to the view tags
#define VRELATIVE     0x00
#define VABSOLUTE     0x01

#define VORHTO        0x00
#define VPERSPECTIVE  0x01

#define VFLAT         0x00
#define VSMOOTH       0x01

#define VOFF          0x00
#define VON           0x01


#endif
