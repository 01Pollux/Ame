#include <EcsModule/ViewportModule.hpp>

#include <EcsComponent/Viewport/Camera.hpp>

#include <Rhi/Device/RhiDevice.hpp>
#include <Window/Window.hpp>
#include <Window/WindowEventListener.hpp>

namespace Ame::Ecs
{
    static void OnWindowSizeChanged_UpdateCamera(
        Iterator&           iter,
        const Math::Size2I& newSize)
    {
        while (iter.next())
        {
            auto cameras = iter.field<CameraComponent>(0);

            for (auto i : iter)
            {
                auto& camera         = cameras[i];
                auto& cameraViewport = camera.GetViewport();

                if (!(cameraViewport.ClientHeight || cameraViewport.ClientWidth))
                {
                    continue;
                }

                if (cameraViewport.ClientHeight)
                {
                    cameraViewport.Height = newSize.Height() * cameraViewport.ClientHeight;
                }

                if (cameraViewport.ClientWidth)
                {
                    cameraViewport.Width = newSize.Width() * cameraViewport.ClientWidth;
                }

                camera.MarkChanged();
            }
        }
    }

    void ViewporEcsModule::RegisterSignals(
        WorldRef world)
    {
        auto& rhiDevice = world.GetWorld()->GetRhiDevice();
        if (!rhiDevice)
        {
            return;
        }

        auto window = rhiDevice->GetWindow();
        if (!window)
        {
            return;
        }

        auto allCameras = world->query<CameraComponent>();

        m_WindowSizeChangedConnection = window->GetEventListener().OnWindowSizeChanged(
            [allCameras](const Math::Size2I& newSize)
            {
                allCameras.run(std::bind_back(OnWindowSizeChanged_UpdateCamera, newSize));
            });
    }
} // namespace Ame::Ecs