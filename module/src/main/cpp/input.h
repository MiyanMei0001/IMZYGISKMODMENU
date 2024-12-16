/*
Created by Askan on 9/16/2022

Unity NativeInjectEvent hook for handling Dear ImGui Input

Telegram    : @AskanDX
GitHub      : @AKNOryx28
*/

#pragma once
#include <android/input.h>
#include <android/keycodes.h>
#include <imgui.h>
#include <imgui_impl_android.h>
#include <jni.h>
#include <cmath>

struct Vector2 {
    float x;
    float y;
};

namespace Unity {
    namespace Input {
        enum TouchPhase { Began = 0, Moved = 1, Stationary = 2, Ended = 3, Canceled = 4, };
        enum TouchType { Direct = 0, Indirect = 1, Stylus = 2, };
        struct Touch {
            int m_FingerId; // 0x0
            Vector2 m_Position; // 0x4
            Vector2 m_RawPosition; // 0xC
            Vector2 m_PositionDelta; // 0x14
            float m_TimeDelta; // 0x1C
            int m_TapCount; // 0x20
            TouchPhase m_Phase; // 0x24
            TouchType m_Type; // 0x28
            float m_Pressure; // 0x2C
            float m_maximumPossiblePressure; // 0x30
            float m_Radius; // 0x34
            float m_RadiusVariance; // 0x38
            float m_AltitudeAngle; // 0x3C
            float m_AzimuthAngle; // 0x40
        };

        static bool is_done = false;
        bool (*GetMouseButtonDown)(int index);

        Touch (*old_GetTouch)(int index);
        Touch GetTouch(int index) {
            Touch _touch = old_GetTouch(index);
            if (!g_IsSetup) return _touch;

            if (index == 0) {
                ImGuiIO &io = ImGui::GetIO();
                float x = _touch.m_Position.x;
                float y = std::round(g_GlHeight) - _touch.m_Position.y;

                switch (_touch.m_Phase) {
                case TouchPhase::Began:
                    io.AddMousePosEvent(x,y);
                    io.AddMouseButtonEvent(0, GetMouseButtonDown(0));
                    break;
                case TouchPhase::Moved:
                    io.AddMousePosEvent(x,y);
                    break;
                case TouchPhase::Ended:
                    io.AddMouseButtonEvent(0, GetMouseButtonDown(0));
                    io.AddMouseButtonEvent(1, GetMouseButtonDown(1));
                    io.AddMouseButtonEvent(2, GetMouseButtonDown(2));
                    break;
                default: break;
                }

                if (io.WantCaptureMouse) {
                    _touch.m_Position = {0,0};
                    _touch.m_RawPosition = {0,0};
                    _touch.m_TapCount = 0;
                    return _touch;
                }
            }

            return _touch;
        }

        void SetUp() {
            if (is_done) return;
           
            // ubah offset nya liat di dump.cs
            //UnityEngine.InputLegacyModule.dll
            auto ptr_GetTouch             = getAbsAddress(0x503377c); // Class Input - GetTouch
            auto ptr_GetMouseButtonDown   = getAbsAddress(0x50336fc); // Class Input - GetMouseButtonDown

            if (ptr_GetTouch != nullptr && ptr_GetMouseButtonDown != nullptr) {
                GetMouseButtonDown = reinterpret_cast<bool(*)(int)>((uintptr_t)ptr_GetMouseButtonDown);
                utils::hook(ptr_GetTouch, (func_t)GetTouch, (func_t*)&old_GetTouch);
                is_done = true;
            }
        }
    }
}
