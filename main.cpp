#include "onyx/core/core.hpp"
#include "onyx/platform/window.hpp"
#include "onyx/asset/assets.hpp"
#include "onyx/execution/execution.hpp"
#include "onyx/rendering/context.hpp"
#include "tkit/container/stack_array.hpp"

template <typename F> void RunWindow(Onyx::Window *window, F fun);

using Onyx::D2;
using Onyx::D3;
using namespace TKit::Alias;
namespace Math = Onyx::Math;
inline f32 dot(const f32v2 &a, const f32v2 &b)
{
    return a[0] * b[0] + a[1] * b[1];
}
inline f32 dot(const f32v3 &a, const f32v3 &b)
{
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

struct Particle2D
{
    f32v2 pos;
    f32v2 vel;
    f32 mass;
    f32 radius;
};

struct Particle3D
{
    f32v3 pos;
    f32v3 vel;
    f32 mass;
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
    ctx->AddPointLight();

    u32 N;
    std::cout << "Numero de particulas: ";
    std::cin >> N;

    TKit::StackArray<Particle2D> particles;
    particles.Reserve(N);

    for (u32 i = 0; i < N; i++)
    {
        Particle2D p;
        p.pos = f32v2{0.f, 0.f};
        p.vel = f32v2{(rand() % 200 - 100) / 100.f, (rand() % 200 - 100) / 100.f};
        p.mass = (rand() % 100 + 50) / 100.f;
        p.radius = 0.05f + 0.01f * p.mass;
        particles.Append(p);
    }

    const f32 bw = 1.5f;
    const f32 bh = 1.5f;
    const f32 wallWidth = 0.1f;
    const f32v2 bounds = f32v2{0.5f * bw, 0.5f * bh};
    const f32 g = 1.f;

    TKit::Clock clock{};

    RunWindow(window, [&] {
        for (const Onyx::Event &event : window->GetNewEvents())
        {
            if (event.Type == Onyx::Event_Scrolled)
            {
                const f32 factor = Onyx::Input::IsKeyPressed(window, Onyx::Input::Key_LeftShift) ? 0.05f : 0.005f;
                cam->ControlScrollWithUserInput(factor * event.ScrollOffset[1]);
            }
        }
        window->FlushEvents();

        ctx->Flush();
        const TKit::Timespan elapsed = clock.Restart();
        cam->ControlMovementWithUserInput(elapsed);

        const f32 dt = elapsed.AsSeconds();

        for (Particle2D &p : particles)
        {
            p.vel[1] -= g * dt;
            p.pos += p.vel * dt;

            if (p.pos[1] <= -bounds[1] + p.radius && p.vel[1] < 0.f)
            {
                p.pos[1] = -bounds[1] + p.radius;
                p.vel[1] *= -1.f;
            }
            if (p.pos[1] >= bounds[1] - p.radius && p.vel[1] > 0.f)
            {
                p.pos[1] = bounds[1] - p.radius;
                p.vel[1] *= -1.f;
            }
            if (p.pos[0] <= -bounds[0] + p.radius && p.vel[0] < 0.f)
            {
                p.pos[0] = -bounds[0] + p.radius;
                p.vel[0] *= -1.f;
            }
            if (p.pos[0] >= bounds[0] - p.radius && p.vel[0] > 0.f)
            {
                p.pos[0] = bounds[0] - p.radius;
                p.vel[0] *= -1.f;
            }
        }

        const f32 e = 1.0f;

        for (u32 i = 0; i < particles.GetSize(); i++)
        {
            for (u32 j = i + 1; j < particles.GetSize(); j++)
            {
                Particle2D &a = particles[i];
                Particle2D &b = particles[j];

                f32v2 delta = b.pos - a.pos;
                f32 dist2 = dot(delta, delta);
                f32 minDist = a.radius + b.radius;

                if (dist2 <= minDist * minDist)
                {
                    const f32 dist = sqrt(dist2);
                    if (dist == 0.f)
                        continue;

                    f32v2 n = delta / dist;
                    f32v2 rv = b.vel - a.vel;
                    f32 velAlongNormal = dot(rv, n);

                    if (velAlongNormal > 0)
                        continue;

                    const f32 jImpulse = -(1 + e) * velAlongNormal / (1.f / a.mass + 1.f / b.mass);
                    const f32v2 impulse = jImpulse * n;

                    a.vel -= impulse / a.mass;
                    b.vel += impulse / b.mass;

                    const f32 penetration = minDist - dist;
                    const f32v2 correction = n * (penetration * 0.5f);

                    a.pos -= correction;
                    b.pos += correction;
                }
            }
        }

        for (auto &p : particles)
        {
            ctx->Push();
            ctx->FillColor(Onyx::Color::White);
            ctx->Scale(2.f * p.radius);
            ctx->TranslateX(p.pos[0]);
            ctx->TranslateY(p.pos[1]);
            ctx->Circle();
            ctx->Pop();
        }

        ctx->Push();
        ctx->ScaleY(wallWidth);
        ctx->ScaleX(bw);
        ctx->TranslateY(bounds[1]);
        ctx->StaticMesh(square);
        ctx->Pop();

        ctx->Push();
        ctx->ScaleX(wallWidth);
        ctx->ScaleY(bh + wallWidth);
        ctx->TranslateX(bounds[0]);
        ctx->StaticMesh(square);
        ctx->Pop();

        ctx->Push();
        ctx->ScaleX(wallWidth);
        ctx->ScaleY(bh + wallWidth);
        ctx->TranslateX(-bounds[0]);
        ctx->StaticMesh(square);
        ctx->Pop();

        ctx->Push();
        ctx->ScaleY(wallWidth);
        ctx->ScaleX(bw);
        ctx->TranslateY(-bounds[1]);
        ctx->StaticMesh(square);
        ctx->Pop();
    });
}

void Run3(Onyx::Window *window)
{
    Onyx::RenderContext<D3> *ctx = ONYX_CHECK_EXPRESSION(Onyx::Renderer::CreateContext<D3>());

    const Onyx::StatMeshData<D3> sdata = Onyx::Assets::CreateSphereMesh(16, 32);
    const Onyx::Mesh sphere = Onyx::Assets::AddMesh(sdata);

    ONYX_CHECK_EXPRESSION(Onyx::Assets::Upload());

    Onyx::Camera<D3> *cam = window->CreateCamera<D3>();
    ctx->AddTarget(window);
    cam->SetPerspectiveProjection();

    ctx->AddDirectionalLight();

    u32 N;
    std::cout << "Numero de particulas: ";
    std::cin >> N;

    TKit::StackArray<Particle3D> particles;
    particles.Reserve(N);

    for (u32 i = 0; i < N; i++)
    {
        Particle3D p;
        p.pos = f32v3{0.f, 0.f, 0.f};
        p.vel = f32v3{(rand() % 200 - 100) / 100.f, (rand() % 200 - 100) / 100.f, (rand() % 200 - 100) / 100.f};
        p.mass = (rand() % 100 + 50) / 100.f;
        p.radius = 0.05f + 0.01f * p.mass;

        particles.Append(p);
    }

    const f32v3 bounds = f32v3{0.75f, 0.75f, 0.75f};
    const f32 g = 1.f;

    TKit::Clock clock{};

    RunWindow(window, [&] {
        ctx->Flush();
        const TKit::Timespan elapsed = clock.Restart();
        cam->ControlMovementWithUserInput(elapsed);

        const f32 dt = elapsed.AsSeconds();

        // movimiento
        for (auto &p : particles)
        {
            p.vel[1] -= g * dt; // gravedad en Y
            p.pos += p.vel * dt;

            //colisiones con marcos
            for (int axis = 0; axis < 3; axis++)
            {
                if (p.pos[axis] <= -bounds[axis] + p.radius && p.vel[axis] < 0.f)
                {
                    p.pos[axis] = -bounds[axis] + p.radius;
                    p.vel[axis] *= -1.f;
                }
                if (p.pos[axis] >= bounds[axis] - p.radius && p.vel[axis] > 0.f)
                {
                    p.pos[axis] = bounds[axis] - p.radius;
                    p.vel[axis] *= -1.f;
                }
            }
        }

        // colisiones entre ellas
        const f32 e = 1.0f;

        for (u32 i = 0; i < particles.GetSize(); i++)
        {
            for (u32 j = i + 1; j < particles.GetSize(); j++)
            {
                auto &a = particles[i];
                auto &b = particles[j];

                f32v3 delta = b.pos - a.pos;
                f32 dist2 = dot(delta, delta);
                f32 minDist = a.radius + b.radius;

                if (dist2 <= minDist * minDist)
                {
                    f32 dist = sqrt(dist2);
                    if (dist == 0.f)
                        continue;

                    f32v3 n = delta / dist;
                    f32v3 rv = b.vel - a.vel;

                    f32 velAlongNormal = dot(rv, n);
                    if (velAlongNormal > 0)
                        continue;

                    f32 jImpulse = -(1 + e) * velAlongNormal / (1.f / a.mass + 1.f / b.mass);
                    f32v3 impulse = jImpulse * n;

                    a.vel -= impulse / a.mass;
                    b.vel += impulse / b.mass;

                    // corrección de penetración
                    f32 penetration = minDist - dist;
                    f32v3 correction = n * (penetration * 0.5f);

                    a.pos -= correction;
                    b.pos += correction;
                }
            }
        }

        for (auto &p : particles)
        {
            ctx->Push();
            ctx->Scale(p.radius * 2.f);
            ctx->SetTranslation(p.pos);
            ctx->StaticMesh(sphere);
            ctx->Pop();
        }
    });
}

int main()
{
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