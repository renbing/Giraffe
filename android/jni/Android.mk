LOCAL_PATH:= $(call my-dir)
ORG_PATH := $(LOCAL_PATH)

include $(CLEAR_VARS)
include $(call all-makefiles-under,$(LOCAL_PATH))

include $(CLEAR_VARS)
LOCAL_PATH := $(ORG_PATH)

LOCAL_MODULE    := gljni
LOCAL_CFLAGS    := -Werror
LOCAL_CPPFLAGS  += -frtti
LOCAL_SRC_FILES := $(wildcard ../../src/*.cpp)
LOCAL_SRC_FILES += \
    Image.cpp \
    JSCContext.cpp \
	main.cpp


LOCAL_C_INCLUDES:= $(NDKROOT)/sources/cxx-stl/gnu-libstdc++/include $(NDKROOT)/sources/cxx-stl/gnu-libstdc++/libs/$(TARGET_ARCH_ABI)/include $(LOCAL_PATH)/ $(LOCAL_PATH)/include $(LOCAL_PATH)/include/png $(LOCAL_PATH)/include/js $(LOCAL_PATH)/../../src/shaders $(LOCAL_PATH)/../../src $(LOCAL_PATH)/../../include/SpiderMonkey $(LOCAL_PATH)/../../include/curl $(LOCAL_PATH)/../../include

LOCAL_LDLIBS    := $(NDKROOT)/sources/cxx-stl/gnu-libstdc++/libs/$(TARGET_ARCH_ABI)/libgnustl_static.a -L$(LOCAL_PATH)/lib/$(TARGET_ARCH_ABI) -L$(LOCAL_PATH)/../../lib -lGLESv2 -lGLESv1_CM -ldl -lz -llog -lpng -lcurl -lzip -ljs

APP_STL := gnustl_static -frtti

include $(BUILD_SHARED_LIBRARY)
