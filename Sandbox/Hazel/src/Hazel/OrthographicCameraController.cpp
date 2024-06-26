#include "OrthographicCameraController.h"
#include "Input.h"
#include "KeyCodes.h"
#include "Core.h"
#include "Debugger/Instrumentor.h"

namespace Hazel {

    OrthographicCameraController::OrthographicCameraController(float aspecRato, bool rotation)
        : m_AspectRatio(aspecRato),
        m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel),
        m_Rotation(rotation)
    {}

    void OrthographicCameraController::OnUpdate(Timestep ts) {
        HZ_PROFILE_FUNCTION();

        if (Hazel::Input::IsKeyPressed(HZ_KEY_A))
            m_CameraPosition.x -= m_CameraMoveSpeed * ts;
        else if (Hazel::Input::IsKeyPressed(HZ_KEY_D))
            m_CameraPosition.x += m_CameraMoveSpeed * ts ;

        if (Hazel::Input::IsKeyPressed(HZ_KEY_W))
            m_CameraPosition.y += m_CameraMoveSpeed * ts ;
        else if (Hazel::Input::IsKeyPressed(HZ_KEY_S))
            m_CameraPosition.y -= m_CameraMoveSpeed * ts ;

        if (m_Rotation) {
            if (Hazel::Input::IsKeyPressed(HZ_KEY_Q))
                m_CameraRotation += m_CameraRotationSpeed * ts;
            if (Hazel::Input::IsKeyPressed(HZ_KEY_E))
                m_CameraRotation -= m_CameraRotationSpeed * ts;
        }

        m_Camera.SetPosition(m_CameraPosition);
        m_Camera.SetRotation(m_CameraRotation);
        m_CameraMoveSpeed = m_ZoomLevel;
    }

    // 在OnEvent中分发MouseScrolledEvent和WindowResizeEvent事件
    void OrthographicCameraController::OnEvent(Event &e) {
        HZ_PROFILE_FUNCTION();

        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<MouseScrolledEvent>(HZ_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
        dispatcher.Dispatch<WindowResizeEvent>(HZ_BIND_EVENT_FN(OrthographicCameraController::OnWindowsResized));
    }

    bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent &e) {
        HZ_PROFILE_FUNCTION();
        
        const auto y_offset = e.GetYOffset();
        m_ZoomLevel -= y_offset * 0.25;
        m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
        m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
        return false;
    }

    
    bool OrthographicCameraController::OnWindowsResized(WindowResizeEvent &e) {
        HZ_PROFILE_FUNCTION();

        m_AspectRatio = (float)e.GetWidth()/(float)e.GetHeight();
        m_Camera.SetProjection(-m_AspectRatio*m_ZoomLevel, m_AspectRatio*m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
        return false;
    }
}