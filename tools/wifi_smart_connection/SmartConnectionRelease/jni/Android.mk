# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)

###########################################################
# Build the static library
###########################################################
include $(CLEAR_VARS)
LOCAL_MODULE:=stlportlib
LOCAL_CFLAGS:=-D_STLP_USE_NEWALLOC
LOCAL_SRC_FILES:=$(NDK_PATH)/sources/cxx-stl/stlport/libs/$(TARGET_ARCH_ABI)/libstlport_static.a
include $(PREBUILT_STATIC_LIBRARY)

# Include the static library
include $(CLEAR_VARS)
LOCAL_MODULE:=libsmart_connection
LOCAL_SRC_FILES:= $(LOCAL_PATH)/static_lib/$(TARGET_ARCH_ABI)/libsmart_connection.a 
include $(PREBUILT_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE:=smart_connection_jni

LOCAL_SRC_FILES:=$(LOCAL_PATH)/smart_connection_jni_loader.c
LOCAL_C_INCLUDES:=$(LOCAL_PATH)
LOCAL_LDLIBS += -llog 

LOCAL_STATIC_LIBRARIES:=libsmart_connection \
                        stlportlib

include $(BUILD_SHARED_LIBRARY)

