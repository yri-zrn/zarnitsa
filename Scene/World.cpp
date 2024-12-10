#include "World.hpp"

#include "Common/Math.hpp"

#include "Components.hpp"

//! TEMP: world must not know about graphics
#include "GraphicsDevice.hpp"
#include "AssetManager.hpp"

namespace zrn
{

void World::Init(GraphicsContext* context, AssetManager* manager)
{
    Context = context;
    m_Manager = manager;
}

void World::OnBegin()
{
    // m_World.component<TransformComponent>();
    // m_World.component<TagComponent>();

    // m_EntityQuery =
    //     m_World.query_builder<TransformComponent, TagComponent>()
    //            .cached()
    //            .build();
}

void World::OnUpdate(Timestep timestep)
{
    UpdateTransforms();
}

void World::OnEvent(Event event)
{

}

void World::OnEnd()
{
    
}

flecs::entity World::CreateEntity(const std::string& name)
{
    return 
        Registry.entity()
                .add<TransformComponent, GlobalTag>()
                .add<TransformComponent, LocalTag>()
                .add<RootTag>()
                .add<LeafTag>()
                .add<MeshComponent>()
                .add<MaterialComponent>()
                .set<MaterialComponent>({m_Manager->GetMaterial("Debug")})
                .set<NameComponent>({ name });
                // .add<MaterialComponent>();
}

flecs::entity World::CreateEntity(const std::string& name, Mesh& mesh)
{
    return
        Registry.entity()
                .add<TransformComponent, GlobalTag>()
                .add<TransformComponent, LocalTag>()
                .add<RootTag>()
                .add<LeafTag>()
                .add<MeshComponent>()
                .add<MaterialComponent>()
                .set<MaterialComponent>({m_Manager->GetMaterial("Debug")})
                .set<MeshComponent>({ &mesh })
                .set<NameComponent>({ name });
}

flecs::entity World::CreatePointLight(const std::string& name)
{
    return 
        Registry.entity()
                .add<TransformComponent, GlobalTag>()
                .add<TransformComponent, LocalTag>()
                .add<RootTag>()
                .add<LeafTag>()
                .add<PointLightComponent>()
                .set<NameComponent>({ name });
}

void World::SetTranslation(flecs::entity entity, glm::vec3 translation)
{
    auto* tranform = entity.get_mut<TransformComponent, LocalTag>();
    tranform->Translation = translation;
}

void World::AddChild(flecs::entity parent, flecs::entity child)
{
    auto child_transform = child.get_mut<TransformComponent, LocalTag>();
    auto parent_transform = parent.get<TransformComponent, GlobalTag>();
    
    child_transform->Translation -= parent_transform->Translation;
    child_transform->Rotation    -= parent_transform->Rotation;
    child_transform->Scale       /= parent_transform->Scale;

    // flecs::def
    // Registry.defer_begin();
    
    parent.remove<LeafTag>();
    child.remove<RootTag>()
        .add<LeafTag>()
        .child_of(parent);
    
    // Registry.defer([&]()
    // {
    // });

    // Registry.defer_end();
}

void World::UpdateTransforms()
{
    auto q =
        Registry.query_builder<const TransformComponent,
                               const TransformComponent*,
                               TransformComponent>()
               .term_at(0).second<LocalTag>()
               .term_at(1).second<GlobalTag>()
               .term_at(2).second<GlobalTag>()
               .term_at(1)
                   .parent().cascade()
               .build();
    
    q.each([](const TransformComponent& transform,
              const TransformComponent* parent_transform,
              TransformComponent& out_transform)
    {
        out_transform = transform;
        // out_transform.Translation = transform.Translation;
        // out_transform.Rotation    = transform.Rotation;
        // out_transform.Scale       = transform.Scale;

        if (parent_transform)
        {
            auto transform = parent_transform->Tranfsorm() * out_transform.Tranfsorm();
            glm::quat orientation;
            glm::vec3 skew;
            glm::vec4 perspective;
            // glm::decompose(transform, out_transform.Scale, orientation, out_transform.Translation, skew, perspective);
            glm::decompose(transform, out_transform.Scale, orientation, out_transform.Translation, out_transform.Skew, perspective);
            out_transform.Rotation = glm::eulerAngles(orientation);
            
            // Math::DecomposeTransform(transform, out_transform.Translation, out_transform.Rotation, out_transform.Scale);

            // out_transform.Transform *= parent_transform;
            // out_transform.Translation += parent_transform->Translation;
            // out_transform.Rotation    += parent_transform->Rotation;
            // out_transform.Scale       *= parent_transform->Scale;
        }
    });
}

} // namespace zrn