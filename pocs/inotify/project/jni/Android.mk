LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := inotify_test
LOCAL_SRC_FILES := inotify_test.cpp
LOCAL_CPPFLAGS := -std=gnu++0x -Wall         # whatever g++ flags you like
#LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -llog   # whatever ld flags you like

include $(BUILD_EXECUTABLE)    # <-- Use this to build an executable.
