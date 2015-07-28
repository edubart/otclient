LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := libotclient
LOCAL_SRC_FILES := libotclient.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libSDL2
LOCAL_SRC_FILES := libSDL2.so
include $(PREBUILT_SHARED_LIBRARY)