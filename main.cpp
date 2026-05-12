#include "onyx/onyx.hpp"
#include "onyx/resources.hpp"

using Onyx::D2;
using Onyx::D3;

using namespace TKit::Alias;
namespace Math = Onyx::Math;

int main()
{
    Onyx::Initialize();

    Onyx::Window *win = Onyx::OpenWindow();
    Onyx::RenderContext<D2> *ctx = Onyx::CreateRenderContext<D2>();

    // TODO(Alvaro): Try to render a 2D capsule (called Stadium in onyx)
    const Onyx::StatMeshData<D2> quad = Onyx::CreateQuadMeshData<D2>();
    const Onyx::ResourcePool stPool = Onyx::Resources::CreateResourcePool<D2>(Onyx::Resource_StaticMesh);

    const Onyx::Resource qmesh = Onyx::Resources::RegisterMesh(stPool, quad);

    Onyx::Resources::Sync(Onyx::SyncFlag_StaticMeshes);

    Onyx::Camera<D2> cam{};
    cam.OrthoParameters.Size = 5.f;
    Onyx::RenderView<D2> *view = win->CreateRenderView(&cam, Onyx::RenderViewFlag_NormalizedCoordinates);
    view->ClearColor = Onyx::Color{0.2f};

    ctx->AddTarget(view);

    while (Onyx::Running())
    {
        ctx->Flush();
        ctx->StaticMesh(qmesh);

        win->ControlCamera(Onyx::GetDeltaTime(win), &cam);

        Onyx::Transfer();
        Onyx::Render();
    }

    Onyx::Terminate();
}
