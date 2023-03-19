LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_CPP_EXTENSION := .cpp .cc
LOCAL_MODULE    := AS4GTA
LOCAL_SRC_FILES := main.cpp forwards.cpp mod/logger.cpp mod/config.cpp
LOCAL_CFLAGS += -O2 -mfloat-abi=softfp -DNDEBUG -std=c++14
LOCAL_LDLIBS += -llog
include $(BUILD_SHARED_LIBRARY)
