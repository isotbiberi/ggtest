# $Id$

SUBDIRS = urvc2 apogee edtsao si8821 sxccd

bin_PROGRAMS = rts2-camd-miniccd rts2-camd-miniccd-il rts2-camd-dummy rts2-camd-sidecar rts2-camd-azcam rts2-camd-azcam3 rts2-camd-si8821 rts2-camd-zyla rts2-camd-pco rts2-camd-prosilica rts2-camd-allsky340 rts2-camd-prosilicagt4907 rts2-camd-fliKepler

noinst_HEADERS = ccd_msg.h reflex.h

LDADD = -L../../lib/sep -lsep -L../../lib/rts2 -lrts2 @LIB_NOVA@
AM_CXXFLAGS = @NOVA_CFLAGS@ -I../../include

rts2_camd_miniccd_SOURCES = miniccd.cpp

rts2_camd_miniccd_il_SOURCES = miniccd_il.cpp

rts2_camd_allsky340_SOURCES = allsky340.cpp
rts2_camd_allsky340_CXXFLAGS = -std=c++11 ${AM_CXXFLAGS} @CFITSIO_CFLAGS@ @MAGIC_CFLAGS@ -I../../include
rts2_camd_allsky340_LDADD = -L../../lib/rts2fits -lrts2image ${LDADD} @CFITSIO_LIBS@ @MAGIC_LIBS@


rts2_camd_dummy_SOURCES = dummy.cpp
rts2_camd_dummy_CXXFLAGS = ${AM_CXXFLAGS} @CFITSIO_CFLAGS@ @MAGIC_CFLAGS@ -I../../include
rts2_camd_dummy_LDADD = -L../../lib/rts2fits -lrts2image ${LDADD} @CFITSIO_LIBS@ @MAGIC_LIBS@


rts2_camd_zyla_SOURCES = zyla.cpp zyla/common.cpp zyla/CameraOperations.cpp zyla/CameraOperations.h zyla/FitsOperations.cpp zyla/FitsOperations.h zyla/fitsio.h
rts2_camd_zyla_CXXFLAGS = ${AM_CXXFLAGS} @CFITSIO_CFLAGS@ @MAGIC_CFLAGS@ -I../../include -Izyla
rts2_camd_zyla_LDADD = -L.libs/ -latcore -latutility -lpthread -L../../lib/rts2fits -lrts2image ${LDADD} @CFITSIO_LIBS@ @MAGIC_LIBS@


rts2_camd_pco_SOURCES = pco.cpp  
rts2_camd_pco_CXXFLAGS = ${AM_CXXFLAGS} @CFITSIO_CFLAGS@ @MAGIC_CFLAGS@ -I../../include -Ipco/pco_common/pco_include -Ipco/pco_common/pco_classes -Ipco/pco_usb/pco_classes
rts2_camd_pco_LDADD = -L.libs/ -lpthread -L../../lib/rts2fits -lrts2image ${LDADD} @CFITSIO_LIBS@ @MAGIC_LIBS@
rts2_camd_pco_LDFLAGS =  pco/pco_common/pco_lib/libpcocom_usb.a pco/pco_common/pco_lib/libpcodisp.a pco/pco_common/pco_lib/libpcolog.a pco/pco_common/pco_lib/libpcocnv.a pco/pco_common/pco_lib/libreorderfunc.a pco/pco_common/pco_lib/libpcocam_usb.a pco/pco_common/pco_lib/libpcofile.a

rts2_camd_prosilica_SOURCES = prosilica.cpp prosilica/VimbaCPP/Examples/SynchronousGrab/Console/Source/ApiController.cpp prosilica/VimbaCPP/Examples/SynchronousGrab/Console/Source/Bitmap.cpp
#rts2_camd_prosilica_LDFLAGS = prosilica/VimbaCPP/DynamicLib/x86_64bit/libVimbaCPP.so
rts2_camd_prosilica_CXXFLAGS = ${AM_CXXFLAGS} @CFITSIO_CFLAGS@ @MAGIC_CFLAGS@ -I../../include -Iprosilica/VimbaCPP/Examples/SynchronousGrab/Console/Source/ -Iprosilica/VimbaCPP/Include/ -Iprosilica -Iprosilica/VimbaCPP/Examples
rts2_camd_prosilica_LDADD = -L.libs/ -lVimbaCPP -lVimbaC -L../../lib/rts2fits -lrts2image ${LDADD} @CFITSIO_LIBS@ @MAGIC_LIBS@


#rts2_camd_prosilicagt4907_SOURCES = prosilicagt4907.cpp Vimba_2_1/VimbaC/Examples/Common/PrintVimbaVersion.c Vimba_2_1/VimbaC/Examples/Common/ErrorCodeToMessage.c Vimba_2_1/VimbaC/Examples/Common/DiscoverGigECameras.c
#rts2_camd_prosilicagt4907_CXXFLAGS = ${AM_CXXFLAGS} @CFITSIO_CFLAGS@ @MAGIC_CFLAGS@ -I../../include -IVimba_2_1 -IVimba_2_1/VimbaC/Examples/Common -IVimba_2_1/VimbaC/Examples
#rts2_camd_prosilicagt4907_CFLAGS = ${AM_CXXFLAGS} @CFITSIO_CFLAGS@ @MAGIC_CFLAGS@ -I../../include -IVimba_2_1 -IVimba_2_1/VimbaC/Examples/Common -IVimba_2_1/VimbaC/Examples
#rts2_camd_prosilicagt4907_LDADD = -LVimba_2_1/VimbaC/DynamicLib/x86_64bit/ -L.libs/ -lVimbaCPP -lVimbaC -L../../lib/rts2fits -lrts2image ${LDADD} @CFITSIO_LIBS@ @MAGIC_LIBS@

rts2_camd_prosilicagt4907_SOURCES = prosilicagt4907.cpp Vimba_2_1/VimbaCPP/Examples/AsynchronousGrab/Console/Source/ApiController.cpp  Vimba_2_1/VimbaCPP/Examples/AsynchronousGrab/Console/Source/FrameObserver.cpp  zyla/FitsOperations.cpp
rts2_camd_prosilicagt4907_CXXFLAGS = ${AM_CXXFLAGS} @CFITSIO_CFLAGS@ @MAGIC_CFLAGS@ -I../../include -IVimba_2_1 -IVimba_2_1/VimbaCPP/Examples/Common -IVimba_2_1/VimbaCPP/Examples -IVimba_2_1/VimbaCPP/Examples/AsynchronousGrab/Console/Source -IVimba_2_1/VimbaImageTransform/Include
rts2_camd_prosilicagt4907_CFLAGS = ${AM_CXXFLAGS} @CFITSIO_CFLAGS@ @MAGIC_CFLAGS@ -I../../include -IVimba_2_1 -IVimba_2_1/VimbaCPP/Examples/Common -IVimba_2_1/VimbaCPP/Examples
rts2_camd_prosilicagt4907_LDADD = -LVimba_2_1/VimbaCPP/DynamicLib/x86_64bit/ -L.libs/ -LVimba_2_1/VimbaImageTransform/DynamicLib/x86_64bit -lVimbaCPP -lVimbaC -lVimbaImageTransform -L../../lib/rts2fits -lrts2image ${LDADD} @CFITSIO_LIBS@ @MAGIC_LIBS@

rts2_camd_fliKepler_SOURCES = fliKepler.cpp zyla/FitsOperations.cpp
rts2_camd_fliKepler_CXXFLAGS = ${AM_CXXFLAGS} @CFITSIO_CFLAGS@ @MAGIC_CFLAGS@ -I../../include -IfliKepler
rts2_camd_fliKepler_LDADD = -L.libs/ -L../../lib/rts2fits -lrts2image -lusb-1.0 -lpthread ${LDADD} @CFITSIO_LIBS@ @MAGIC_LIBS@
rts2_camd_fliKepler_LDFLAGS =  fliKepler/liblibflipro.a 




rts2_camd_sidecar_SOURCES = sidecar.cpp

rts2_camd_azcam_SOURCES = azcam.cpp
rts2_camd_azcam3_SOURCES = azcam3.cpp

rts2_camd_si8821_SOURCES = si8821.cpp

EXTRA_DIST = 

if SUNCYGMAC
EXTRA_DIST += urvc2.cpp apogee.cpp v4l.cpp
else
if ARMCPU
EXTRA_DIST += urvc2.cpp apogee.cpp

bin_PROGRAMS += rts2-camd-v4l

rts2_camd_v4l_SOURCES = v4l.cpp
else
bin_PROGRAMS += rts2-camd-urvc2 rts2-camd-apogee rts2-camd-v4l

rts2_camd_urvc2_SOURCES = urvc2.cpp
rts2_camd_urvc2_LDFLAGS = urvc2/liburvc2.a

rts2_camd_apogee_SOURCES = apogee.cpp
rts2_camd_apogee_LDFLAGS = apogee/libapogee.a

rts2_camd_v4l_SOURCES = v4l.cpp
endif
endif

if SBIG
bin_PROGRAMS += rts2-camd-sbig
rts2_camd_sbig_SOURCES = sbig.cpp
rts2_camd_sbig_LDFLAGS = @LIB_SBIG@ ${LDADD} @CFITSIO_LIBS@
rts2_camd_sbig_CXXFLAGS = @INC_SBIG@ -I../../include @CFITSIO_CFLAGS@
else
EXTRA_DIST += sbig.cpp
endif

if ANDOR 
bin_PROGRAMS += rts2-camd-andor
rts2_camd_andor_SOURCES = andor.cpp
rts2_camd_andor_LDFLAGS = @LIB_ANDOR@ @LIB_M@ @LIB_PTHREAD@
else
EXTRA_DIST += andor.cpp
endif

if ALTA
bin_PROGRAMS += rts2-camd-alta rts2-camd-altanet
rts2_camd_alta_SOURCES = alta.cpp
rts2_camd_alta_LDADD = @LIB_ALTA_USB@ @LIB_M@ ${LDADD}
rts2_camd_alta_CXXFLAGS = @INC_ALTA@ -I../../include
rts2_camd_altanet_SOURCES = alta.cpp
rts2_camd_altanet_LDADD = @LIB_ALTA_NET@ @LIB_M@ ${LDADD} -lcurl
rts2_camd_altanet_CXXFLAGS = @INC_ALTA@ -I../../include
else
EXTRA_DIST += alta.cpp
endif

if FLI
bin_PROGRAMS += rts2-camd-fli
rts2_camd_fli_SOURCES = fli.cpp
rts2_camd_fli_LDFLAGS = @LIB_FLI@ @LIB_M@
rts2_camd_fli_CXXFLAGS = @INC_FLI@ -I../../include
else
EXTRA_DIST += fli.cpp
endif

if MICCD
bin_PROGRAMS += rts2-camd-miccd
rts2_camd_miccd_SOURCES = miccd.cpp
rts2_camd_miccd_LDFLAGS = @LIB_MICCD@ @LIB_K8055@
rts2_camd_miccd_CXXFLAGS = @INC_MICCD@ -I../../include
else
EXTRA_DIST += miccd.cpp
endif

if GXCCD
bin_PROGRAMS += rts2-camd-gxccd
rts2_camd_gxccd_SOURCES = gxccd.cpp
rts2_camd_gxccd_LDFLAGS = @GXCCD_LIBS@ @USB1_LIBS@ -lpthread ${LDADD}
rts2_camd_gxccd_CXXFLAGS = @GXCCD_CFLAGS@ -I../../include
else
EXTRA_DIST += gxccd.cpp
endif

if EDTPDV
bin_PROGRAMS += rts2-camd-edtsao rts2-camd-reflex

rts2_camd_edtsao_SOURCES = edtsao.cpp
rts2_camd_edtsao_LDFLAGS = edtsao/libedtsao.a @LIB_EDTPDV@ @LIB_PTHREAD@
rts2_camd_edtsao_CXXFLAGS = @INC_EDTPDV@ -I../../include

rts2_camd_reflex_SOURCES = reflex.cpp
rts2_camd_reflex_LDFLAGS = @LIB_EDTPDV@ @LIB_PTHREAD@
rts2_camd_reflex_CXXFLAGS = @INC_EDTPDV@ -I../../include

else
EXTRA_DIST += edtsao.cpp reflex.cpp
endif

if ARC
bin_PROGRAMS += rts2-camd-arc
rts2_camd_arc_SOURCES = arc.cpp
rts2_camd_arc_LDFLAGS = @LIB_ARC@
rts2_camd_arc_CXXFLAGS = @INC_ARC@ -I../../include
else
EXTRA_DIST += arc.cpp
endif

if SI3097
bin_PROGRAMS += rts2-camd-si3097
rts2_camd_si3097_SOURCES = si3097.cpp
rts2_camd_si3097_CXXFLAGS = @INC_SI3097@ -I../../include
else
EXTRA_DIST += si3097.cpp
endif

if LIBUSB1
bin_PROGRAMS += rts2-camd-sx
rts2_camd_sx_SOURCES = sx.cpp
rts2_camd_sx_LDADD = -Lsxccd -lsxccdusb ${LDADD} @USB1_LIBS@
rts2_camd_sx_CXXFLAGS = @USB1_CFLAGS@ -I../../include
else
EXTRA_DIST += sx.cpp
endif
