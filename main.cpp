#include "onyx/core/core.hpp"
#include "onyx/platform/window.hpp"
#include "onyx/execution/execution.hpp"

int main()
{
    ONYX_CHECK_EXPRESSION(Onyx::Initialize());

    Onyx::Window *window = ONYX_CHECK_EXPRESSION(Onyx::Platform::CreateWindow());
    while (!window->ShouldClose())
        if (ONYX_CHECK_EXPRESSION(window->AcquireNextImage(Onyx::Block)))
        {
            Onyx::Input::PollEvents();
            VKit::Queue *gqueue = Onyx::Execution::FindSuitableQueue(VKit::Queue_Graphics);
            ONYX_CHECK_EXPRESSION(gqueue->UpdateCompletedTimeline());
            Onyx::CommandPool *gpool =
                ONYX_CHECK_EXPRESSION(Onyx::Execution::FindSuitableCommandPool(VKit::Queue_Graphics));

            const VkCommandBuffer gcmd = ONYX_CHECK_EXPRESSION(Onyx::Execution::Allocate(gpool));
            ONYX_CHECK_EXPRESSION(Onyx::Execution::BeginCommandBuffer(gcmd));
            // Onyx::Renderer::ApplyAcquireBarriers(gcmd);

            window->BeginRendering(gcmd);
            const Onyx::Renderer::RenderSubmitInfo rinfo =
                ONYX_CHECK_EXPRESSION(Onyx::Renderer::Render(gqueue, gcmd, window->CreateViewInfo()));
            window->MarkSubmission(gqueue->GetTimelineSempahore(), rinfo.InFlightValue);
            window->EndRendering(gcmd);

            ONYX_CHECK_EXPRESSION(Onyx::Execution::EndCommandBuffer(gcmd));
            ONYX_CHECK_EXPRESSION(Onyx::Renderer::SubmitRender(gqueue, gpool, rinfo));
            ONYX_CHECK_EXPRESSION(window->Present());
        }

    Onyx::Terminate();
}
