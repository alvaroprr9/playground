#include "onyx/core/core.hpp"
#include "onyx/platform/window.hpp"
#include "onyx/asset/assets.hpp"
#include "onyx/execution/execution.hpp"
#include "onyx/rendering/context.hpp"

template <typename F> void RunWindow(Onyx::Window *window, F fun);

using Onyx::D2;
using Onyx::D3;
using namespace TKit::Alias;
struct Particle
{
    f32 x, y;
    f32 vx, vy;
    f32 radius;
};

void Run2(Onyx::Window *window)
{
    Onyx::RenderContext<D2> *ctx = ONYX_CHECK_EXPRESSION(Onyx::Renderer::CreateContext<D2>());
    const Onyx::StatMeshData<D2> data = Onyx::Assets::CreateSquareMesh<D2>();
    const Onyx::Mesh square = Onyx::Assets::AddMesh(data);
    ONYX_CHECK_EXPRESSION(Onyx::Assets::Upload());

    Onyx::Camera<D2> *cam = window->CreateCamera<D2>();
    ctx->AddTarget(window);
    Onyx::PointLight<D2> *pl = ctx->AddPointLight();

    // cam->GetWorldMousePosition();
    // f32v2 vec;
    // to access x -> vec[0];
    // to access y -> vec[1];
    int N;
    std::cout << "Numero de particulas: ";
    std::cin >> N;
    
    std::vector<Particle> particles;

    for (int i = 0; i < N; i++)
    {
        Particle p;
        p.x = 0.f;
        p.y = 0.f;
        p.vx = (rand() % 200 - 100) / 100.f; // entre -1 y 1
        p.vy = (rand() % 200 - 100) / 100.f;
        p.radius = 0.08f;

        particles.push_back(p);
    }

    const f32 bw = 1.5f;
    const f32 bh = 1.5f;
    const f32 wallWidth = 0.1f;

    const f32 bx = 0.5f * bw;
    const f32 by = 0.5f * bh;

    const f32 g = 1.f;
    TKit::Clock clock{};
    RunWindow(window, [&] {
        for (const Onyx::Event &event : window->GetNewEvents())
            if (event.Type == Onyx::Event_Scrolled)
            {
                const f32 factor = Onyx::Input::IsKeyPressed(window, Onyx::Input::Key_LeftShift) ? 0.05f : 0.005f;
                cam->ControlScrollWithUserInput(factor * event.ScrollOffset[1]);
            }
        window->FlushEvents();

        ctx->Flush();
        const TKit::Timespan elapsed = clock.Restart();
        cam->ControlMovementWithUserInput(elapsed);

        const f32 dt = elapsed.AsSeconds();
        for (auto &p : particles)
        {
            p.vy -= g * dt;

            p.x += p.vx * dt;
            p.y += p.vy * dt;

            const f32 bY = by - p.radius - 0.5f * wallWidth;
            const f32 bX = bx - p.radius - 0.5f * wallWidth;

            if (p.y <= -bY && p.vy < 0.f)
            {
                p.y = -bY;
                p.vy = -p.vy;
            }
            if (p.y >= bY && p.vy > 0.f)
            {
                p.y = bY;
                p.vy = -p.vy;
            }
            if (p.x <= -bX && p.vx < 0.f)
            {
                p.x = -bX;
                p.vx = -p.vx;
            }
            if (p.x >= bX && p.vx > 0.f)
            {
                p.x = bX;
                p.vx = -p.vx;
            }
        }

        const f32 e = 1.0f; // elasticidad

        for (size_t i = 0; i < particles.size(); i++)
        {
            for (size_t j = i + 1; j < particles.size(); j++)
            {
                Particle &a = particles[i];
                Particle &b = particles[j];

                f32 dx = b.x - a.x;
                f32 dy = b.y - a.y;

                f32 dist2 = dx * dx + dy * dy;
                f32 minDist = a.radius + b.radius;

                if (dist2 <= minDist * minDist)
                {
                    f32 dist = sqrt(dist2);
                    if (dist == 0.f)
                        continue;

                    // normal
                    f32 nx = dx / dist;
                    f32 ny = dy / dist;

                    // velocidad relativa
                    f32 rvx = b.vx - a.vx;
                    f32 rvy = b.vy - a.vy;

                    // velocidad en la normal
                    f32 velAlongNormal = rvx * nx + rvy * ny;

                    if (velAlongNormal > 0)
                        continue;

                    // impulso
                    f32 jImpulse = -(1 + e) * velAlongNormal; // (-(1 + e) * velAlongNormal)/[1/ma +1/mb]
                    jImpulse /= 2.0f; // masas iguales

                    f32 impulseX = jImpulse * nx;
                    f32 impulseY = jImpulse * ny;

                    a.vx -= impulseX;
                    a.vy -= impulseY;

                    b.vx += impulseX;
                    b.vy += impulseY;

                    // corrección de penetración
                    f32 penetration = minDist - dist; 
                    f32 correction = penetration * 0.5f;

                    a.x -= correction * nx;
                    a.y -= correction * ny;

                    b.x += correction * nx;
                    b.y += correction * ny;
                }
            }
        }

        // ctx->FillColor(Onyx::Color::Orange);
        for (auto &p : particles)
        {
            ctx->Push();
            ctx->FillColor(Onyx::Color::White);
            ctx->Scale(2.f * p.radius);
            ctx->TranslateX(p.x);
            ctx->TranslateY(p.y);
            ctx->Circle();
            ctx->Pop();
        }


        ctx->Push();
        ctx->ScaleY(wallWidth);
        ctx->ScaleX(bw);
        ctx->TranslateY(by);
        ctx->StaticMesh(square);
        ctx->Pop();

        ctx->Push();
        ctx->ScaleX(wallWidth);
        ctx->ScaleY(bh + wallWidth);
        ctx->TranslateX(bx);
        ctx->StaticMesh(square);
        ctx->Pop();

        ctx->Push();
        ctx->ScaleX(wallWidth);
        ctx->ScaleY(bh + wallWidth);
        ctx->TranslateX(-bx);
        ctx->StaticMesh(square);
        ctx->Pop();

        ctx->Push();
        ctx->ScaleY(wallWidth);
        ctx->ScaleX(bw);
        ctx->TranslateY(-by);
        ctx->StaticMesh(square);
        ctx->Pop();

        // ctx->Scale(2.f);
        // ctx->Circle();
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
            Onyx::Input::PollEvents();
            fun();

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
