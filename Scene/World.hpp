#pragma once

#include "Components.hpp"

#include "ViewportCamera.hpp"

#include "Scene/AssetManager.hpp"

#include "Common/Timestep.hpp"
#include "Platform/Events.hpp"

#include <flecs.h>
#include <string>

namespace zrn
{

class GraphicsContext;

class World
{
public:
    GraphicsContext* Context;

    flecs::world    Registry;
    ViewportCamera  Camera = {};
    flecs::entity   SelectedEntity = {};

public:
    //! TEMP:
    void Init(GraphicsContext* context);

    void OnBegin();
    void OnUpdate(Timestep timestep);
    void OnEvent(Event event);
    void OnEnd();
    
public:
    flecs::entity CreateEntity(const std::string& name);
    flecs::entity CreateEntity(const std::string& name, Mesh&& mesh);
    void AddChild(flecs::entity parent, flecs::entity child);
    void SetTranslation(flecs::entity entity, glm::vec3 position);
    void SelectEntity(flecs::entity entity) { SelectedEntity = entity; }

private:
    void UpdateTransforms();
};

} // namespace zrn