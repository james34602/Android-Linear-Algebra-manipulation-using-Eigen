LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)   
LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_LDLIBS     += -ldl
LOCAL_CFLAGS += -ftemplate-backtrace-limit=0 -w
LOCAL_MODULE    := eigentest
LOCAL_SRC_FILES := eigentest.cpp
include $(BUILD_SHARED_LIBRARY)