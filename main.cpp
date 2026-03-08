#include "onyx/core/core.hpp"
#include "onyx/platform/window.hpp"
#include "onyx/asset/assets.hpp"
#include "onyx/execution/execution.hpp"
#include "onyx/rendering/context.hpp"

template <typename F> void RunWindow(Onyx::Window *window, F fun);

using Onyx::D2;
using Onyx::D3;
using namespace TKit::Alias;
namespace Math = Onyx::Math;

void Run2(Onyx::Window *window)
{
    Onyx::RenderContext<D2> *ctx = ONYX_CHECK_EXPRESSION(Onyx::Renderer::CreateContext<D2>());
    const Onyx::StatMeshData<D2> data = Onyx::Assets::CreateSquareMesh<D2>();
    const Onyx::Mesh square = Onyx::Assets::AddMesh(data);
    ONYX_CHECK_EXPRESSION(Onyx::Assets::Upload());

    window->CreateCamera<D2>();
    ctx->AddTarget(window);
    Onyx::PointLight<D2> *pl = ctx->AddPointLight();

    f32 time = 0.f;
    TKit::Clock clock{};
    RunWindow(window, [&] {
        ctx->Flush();
        const f32 x = cosf(time);

        ctx->FillColor(Onyx::Color::Orange);

        ctx->Push();
        ctx->Rotate(Math::Pi<f32>() * 0.25f * time);
        ctx->TranslateX(x);
        pl->SetPosition(f32v2{x, 0.f});
        ctx->StaticMesh(square);

        ctx->Pop();

        // ctx->Scale(2.f);
        // ctx->Circle();
        const TKit::Timespan elapsed = clock.Restart();
        time += elapsed.AsSeconds();
    });
}

void Run3(Onyx::Window *window)
{
    Onyx::RenderContext<D3> *ctx = ONYX_CHECK_EXPRESSION(Onyx::Renderer::CreateContext<D3>());
    const Onyx::StatMeshData<D3> cdata = Onyx::Assets::CreateCubeMesh();
    const Onyx::StatMeshData<D3> sdata = Onyx::Assets::CreateSphereMesh(32, 64);
    const Onyx::Mesh cube = Onyx::Assets::AddMesh(cdata);
    const Onyx::Mesh sphere = Onyx::Assets::AddMesh(sdata);
    const Onyx::MaterialData<D3> mdata{};
    const Onyx::Material material = Onyx::Assets::AddMaterial(mdata);
    ONYX_CHECK_EXPRESSION(Onyx::Assets::Upload());

    Onyx::Camera<D3> *cam = window->CreateCamera<D3>();
    ctx->AddTarget(window);

    // Onyx::Transform<D3> transform{};
    // transform.Scale = f32v3{1.f, 1.f, 10.f};
    // cam->SetView(transform);
    cam->SetPerspectiveProjection();

    ctx->AddDirectionalLight();
    const f32v3 lpos = f32v3{0.6f, 0.f, 0.3f};
    Onyx::PointLight<D3> *pl = ctx->AddPointLight(lpos, 1.f, 0.8f);
    pl->SetColor(Onyx::Color::Cyan);

    TKit::Clock clock{};
    RunWindow(window, [&] {
        ctx->Flush();
        ctx->Material(material);
        ctx->Push();
        ctx->Scale(0.03f);
        ctx->SetTranslation(lpos);
        ctx->StaticMesh(sphere);
        ctx->Pop();
        ctx->StaticMesh(cube);
        cam->ControlMovementWithUserInput(clock.Restart());
    });
}

int main()
{
    // --------- CPU
    // |
    // v
    // _________ GPU -> CPU VISIBLE
    // |
    // v
    // +++++++++ GPU -> DEVICE LOCAL

    ONYX_CHECK_EXPRESSION(Onyx::Initialize());
    Onyx::WindowSpecs spc{};
    spc.PresentMode = VK_PRESENT_MODE_FIFO_KHR;
    Onyx::Window *window = ONYX_CHECK_EXPRESSION(Onyx::Platform::CreateWindow(spc));

    Run2(window);

    Onyx::Terminate();
}

template <typename F> void RunWindow(Onyx::Window *window, const F fun)
{
    while (!window->ShouldClose())
        if (ONYX_CHECK_EXPRESSION(window->AcquireNextImage(Onyx::Block)))
        {
            fun();
            Onyx::Input::PollEvents();

            VKit::Queue *tqueue = Onyx::Execution::FindSuitableQueue(VKit::Queue_Transfer);
            VKit::Queue *gqueue = Onyx::Execution::FindSuitableQueue(VKit::Queue_Graphics);

            ONYX_CHECK_EXPRESSION(tqueue->UpdateCompletedTimeline());
            ONYX_CHECK_EXPRESSION(gqueue->UpdateCompletedTimeline());

            Onyx::CommandPool *tpool =
                ONYX_CHECK_EXPRESSION(Onyx::Execution::FindSuitableCommandPool(VKit::Queue_Transfer));
            Onyx::CommandPool *gpool =
                ONYX_CHECK_EXPRESSION(Onyx::Execution::FindSuitableCommandPool(VKit::Queue_Graphics));

            const VkCommandBuffer tcmd = ONYX_CHECK_EXPRESSION(Onyx::Execution::Allocate(tpool));

            ONYX_CHECK_EXPRESSION(Onyx::Execution::BeginCommandBuffer(tcmd));
            const Onyx::Renderer::TransferSubmitInfo tsinfo =
                ONYX_CHECK_EXPRESSION(Onyx::Renderer::Transfer(tqueue, tcmd));
            ONYX_CHECK_EXPRESSION(Onyx::Execution::EndCommandBuffer(tcmd));
            if (tsinfo)
                ONYX_CHECK_EXPRESSION(Onyx::Renderer::SubmitTransfer(tqueue, tpool, tsinfo));

            const VkCommandBuffer gcmd = ONYX_CHECK_EXPRESSION(Onyx::Execution::Allocate(gpool));
            ONYX_CHECK_EXPRESSION(Onyx::Execution::BeginCommandBuffer(gcmd));
            Onyx::Renderer::ApplyAcquireBarriers(gcmd);

            window->BeginRendering(gcmd);
            const Onyx::Renderer::RenderSubmitInfo rinfo =
                ONYX_CHECK_EXPRESSION(Onyx::Renderer::Render(gqueue, gcmd, window->CreateViewInfo()));
            window->MarkSubmission(gqueue->GetTimelineSempahore(), rinfo.InFlightValue);
            window->EndRendering(gcmd);

            ONYX_CHECK_EXPRESSION(Onyx::Execution::EndCommandBuffer(gcmd));
            ONYX_CHECK_EXPRESSION(Onyx::Renderer::SubmitRender(gqueue, gpool, rinfo));
            ONYX_CHECK_EXPRESSION(window->Present());
        }
}
