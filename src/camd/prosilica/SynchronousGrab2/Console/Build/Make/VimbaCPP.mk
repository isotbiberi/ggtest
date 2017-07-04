#include $(MAKE_INCLUDE_DIR)/Common.mk
#include $(MAKE_INCLUDE_DIR)/VimbaC.mk

#Compile options needed for VimbaCPP
VIMBACPP_CFLAGS = -I$(VIMBASDK_DIR)

#Linker options needed for VimbaCPP
VIMBACPP_LIBS   = -L$(BIN_DIR) -lVimbaCPP -Wl,-rpath-link,$(BIN_DIR)

#By default we copy libVimbaCPP.so next to the binary
$(BIN_DIR)/libVimbaCPP.so: $(BIN_DIR) VimbaC
	$(CP) $(VIMBASDK_DIR)/VimbaCPP/DynamicLib/$(ARCH)_$(WORDSIZE)bit/libVimbaCPP.so $(BIN_DIR)/

#Operations we have to do in order to prepare VimbaCPP
VimbaCPP: $(BIN_DIR)/libVimbaCPP.so
