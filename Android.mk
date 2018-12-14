LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := json
LOCAL_SRC_FILES := libjsoncpp.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := silk
LOCAL_SRC_FILES := libsilk.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := sgmcurl
LOCAL_SRC_FILES := libsgmcurl.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_CFLAGS += -DNO_ASM=1
LOCAL_CFLAGS += -DTIEG_REMARK=1
LOCAL_CFLAGS += -fvisibility=hidden
LOCAL_CPPFLAGS += -std=c++11
LOCAL_CPPFLAGS += -fvisibility=hidden

LOCAL_MODULE := VDsdk

MY_PATH := "C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\atlmfc\include" "C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\include"
LOCAL_SRC_FILES := VcyberVoiceSDK.cpp DNS.cpp TimeLog.cpp Configs.cpp SdkLib.cpp crypt.cpp Java_com_vcyber_shngi2_sdk.cpp tsilk.c Common.cpp VcyberSpeex.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/header/atlmfc/include $(LOCAL_PATH)/Include\shared $(LOCAL_PATH)/Include\um $(LOCAL_PATH)/C_include\include $(LOCAL_PATH)/json-1.6.5


LOCAL_LDLIBS := -llog -lz -pthread
LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib
LOCAL_LDLIBS += -fvisibility=hidden
#LOCAL_STATIC_LIBRARIES := jsoncpp silk
LOCAL_SHARED_LIBRARIES := sgmcurl json silk
APP_OPTIM:=release

include $(BUILD_SHARED_LIBRARY)