#include "PropertyEditor.hpp"

#include "Application.hpp"
#include "Scene/Components.hpp"
#include "Sandbox/SandboxLayer.hpp"
#include "Scene/World.hpp"
#include "Common/Math.hpp"
#include "ImGuiLayer/ImGuiLayer.hpp"

namespace zrn
{

void PropertyEditor::Init(SandboxLayer* layer)
{
    m_Layer = layer;
}


void PropertyEditor::Show(bool* show)
{
    auto world    = &m_Layer->World;
    auto registry = &world->Registry;

    #ifdef TEST_ADOPT
    ImGui::Begin("Test adopt");
    {
        static int64_t ids[2] = { 0, 0 };

        ImGui::InputScalarN("UIDs", ImGuiDataType_S64, ids, 2, NULL, NULL, "%d", 0);

        if (ImGui::Button("Adopt"))
        {
            flecs::entity parent = registry->entity(ids[0]);
            flecs::entity child  = registry->entity(ids[1]);
            if (parent && child)
            {
                world->AddChild(parent, child);
            }
        }
    }
    ImGui::End();
    #endif
/*
    ImGui::Begin("Test mesh");
    {
        static int64_t id = 0;
        static std::string path;

        ImGui::InputScalar("UID", ImGuiDataType_S64, &id, NULL, NULL, "%d", 0);

        static char buffer[256];
        static bool flushed = false;
        if (!flushed)
        {
            memset(buffer, 0, sizeof(buffer));
            flushed = true;
        }

        ImGui::SetNextItemWidth(-FLT_MIN);
        if (ImGui::InputText("##Path", buffer, sizeof(buffer)))
        {
            path = std::string(buffer);
        }

        if (ImGui::Button("Load"))
        {
            // C:/Zarnitsa/Resources/monkey.glb
            // TEMP: consider better API

            auto mesh = m_Layer->Application->AssetManager.LoadMesh(&m_Layer->Application->Context, path);
            if (mesh.has_value())
            {
                auto entity = registry->entity(id);
                entity.get_mut<MeshComponent>()->Mesh = *mesh;
            }
        }
    }
    ImGui::End();
*/

    ShowEntityContextMenu();

    auto& io = ImGui::GetIO();
    // auto avail = ImGui::GetContentRegionAvail();
    // (float)ImGui::GetWindowWidth();
    ImGui::SetNextWindowSize(ImVec2(320, ImGui::GetWindowHeight()), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::Begin("Property editor", show, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
    // if (ImGui::BeginChild("##tree", ImVec2(300, 0), ImGuiChildFlags_ResizeX | ImGuiChildFlags_Borders | ImGuiChildFlags_NavFlattened))
    {
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_F, ImGuiInputFlags_Tooltip);
        ImGui::PushItemFlag(ImGuiItemFlags_NoNavDefaultFocus, true);
        if (ImGui::InputTextWithHint("##Filter", "incl,-excl", Filter.InputBuf, IM_ARRAYSIZE(Filter.InputBuf), ImGuiInputTextFlags_EscapeClearsAll))
            Filter.Build();
        ImGui::PopItemFlag();

        if (ImGui::BeginTable("##bg", 1, ImGuiTableFlags_RowBg))
        {
            // static auto q = m_World->Registry.query_builder<>();

            registry->query<const NameComponent, RootTag>()
                .each([this](flecs::entity entity, const NameComponent& name, RootTag){
                if (Filter.PassFilter(name.Name.c_str()))
                {
                    DrawNode(entity);
                }
            });
            ImGui::EndTable();
        }
    }
    ImGui::End();
    // ImGui::EndChild();

    // Right side: draw properties
    // ImGui::SameLine();

    // ImGui::BeginGroup(); // Lock X position
    auto avail = ImGui::GetContentRegionAvail();
    ImGui::SetNextWindowSize(ImVec2(430, ImGui::GetWindowHeight()), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowWidth(), 0), ImGuiCond_FirstUseEver, ImVec2(1, 0));
    ImGui::Begin("Property editor 2", show, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
    if (auto entity = world->SelectedEntity)
    {
        const char* entity_name = entity.get<NameComponent>()->Name.c_str();
        ImGui::Text("%s", entity_name);
        const auto& translation = entity.get<TransformComponent, GlobalTag>()->Translation;
        ImGui::Text("(%0.3f, %0.3f, %0.3f)", translation.x, translation.y, translation.z);
        ImGui::TextDisabled("UID: 0x%08X (%i)", entity.id(), entity.id());
        
        {
            auto& mesh_library = m_Layer->Application->AssetManager.GetMeshLibrary();
            std::vector<const char*> mesh_names;
            mesh_names.reserve(mesh_library.size());
            for (auto const& mesh : mesh_library)
                mesh_names.push_back(mesh.first.c_str());
            
            auto mesh = entity.get_mut<MeshComponent>();
            if (mesh)
            {
                static int current_mesh = 0;
                ImGui::SetNextItemWidth(-FLT_MIN);
                if (ImGui::Combo("##SelectedMesh", &current_mesh, mesh_names.data(), mesh_names.size()))
                {
                    mesh->Mesh = &mesh_library.at(mesh_names[current_mesh]);
                }
            }
        }
        
        ImGui::Separator();

        if (ImGui::BeginTable("##Properties", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY))
        {
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch, 2.0f); // Default twice larger

            entity.each([&](flecs::id component_id)
            {
                DrawComponent(entity, component_id);
            });
            ImGui::EndTable();
        }
    }
    // ImGui::EndGroup();

    ImGui::End();
}

void PropertyEditor::DrawNode(flecs::entity entity)
{
    auto world    = &m_Layer->World;
    auto registry = &world->Registry;

    ImGui::TableNextRow();
    ImGui::TableNextColumn();

    ImGui::PushID(entity.id());
    ImGuiTreeNodeFlags tree_flags = ImGuiTreeNodeFlags_None;
    tree_flags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;    // Standard opening mode as we are likely to want to add selection afterwards
    tree_flags |= ImGuiTreeNodeFlags_NavLeftJumpsBackHere;                                  // Left arrow support
    
    if (entity == world->SelectedEntity)
        tree_flags |= ImGuiTreeNodeFlags_Selected;

    if (entity.has<LeafTag>())
        tree_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet;
        
    bool node_open = ImGui::TreeNodeEx("", tree_flags, "%s", entity.get<NameComponent>()->Name.c_str());

    if (ImGui::IsItemFocused())
        world->SelectEntity(entity);

    if (ImGui::BeginDragDropSource())
    {
        ImGui::SetDragDropPayload("_ENTITY_CHILD", &entity, sizeof(entity));
        // ImGui::Text("Add child");
        ImGui::EndDragDropSource();
    }

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_ENTITY_CHILD"))
        {
            flecs::entity child = *((flecs::entity*)payload->Data);
            flecs::entity parent = entity;
            
            auto child_transform = child.get_mut<TransformComponent, LocalTag>();
            auto parent_transform = parent.get<TransformComponent, GlobalTag>();
            
            // parent.remove<LeafTag>();
            // child.remove<RootTag>()
            //     .add<LeafTag>()
            //     .child_of(parent);

            child_transform->Translation -= parent_transform->Translation;
            child_transform->Rotation    -= parent_transform->Rotation;
            child_transform->Scale       -= parent_transform->Scale;

            // flecs::def
            // Registry.defer_begin();
            
            
            // m_World->Registry.defer([&]()
            // {
                // m_World->AddChild(entity, child_entity);
            // });
        }
        ImGui::EndDragDropTarget();
    }

    if (node_open)
    {
        entity.children([this](flecs::entity child){
            DrawNode(child);
        });
        ImGui::TreePop();
    }
    ImGui::PopID();
}

void PropertyEditor::DrawComponent(flecs::entity entity, flecs::id component_id)
{
    auto world    = &m_Layer->World;
    auto registry = &world->Registry;

    if (component_id == registry->id<RootTag>() ||
        component_id == registry->id<LeafTag>())
    {
        return;
    }

    bool added_to_table = false;
    
    ImGui::TableNextRow();
    ImGui::PushID(component_id);
    ImGui::TableNextColumn();

    ImGui::AlignTextToFramePadding();
    
    if (component_id.is_entity())
    {
        // auto component = component_id.entity();
        if (component_id == registry->id<NameComponent>())
        {
            added_to_table = true;

            ImGui::TextUnformatted("Name");
            ImGui::TableNextColumn();

            auto& name = entity.get_mut<NameComponent>()->Name;
            static char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strcpy_s(buffer, sizeof(buffer), name.c_str());

            ImGui::SetNextItemWidth(-FLT_MIN);
            if (ImGui::InputText("##Name", buffer, sizeof(buffer)))
            {
                name = std::string(buffer);
            }
        }
        else if (component_id == registry->id<MaterialComponent>())
        {
            added_to_table = true;

            MaterialComponent* material = entity.get_mut<MaterialComponent>();

            ImGui::PushStyleVarY(ImGuiStyleVar_ItemSpacing, 9);
            ImGui::TextUnformatted("Material");
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::TextUnformatted("Albedo");
            ImGui::TextUnformatted("Emission");
            ImGui::TextUnformatted("Roughness");
            ImGui::TextUnformatted("Metallic");
            ImGui::TextUnformatted("Specular");
            ImGui::TextUnformatted("Normal");
            ImGui::PopStyleVar();
            
            ImGui::TableNextColumn();

            {
                // TODO: fix: bad solution
                auto& material_library = m_Layer->Application->AssetManager.GetMaterialLibrary();
                std::vector<const char*> material_names;
                material_names.reserve(material_library.size());
                for (auto const& material : material_library)
                    material_names.push_back(material.first.c_str());
                // !Does not work for some reason
                // std::transform(material_library.begin(),
                //                material_library.end(),
                //                std::back_inserter(material_names),
                //                [](std::pair<std::string, Material> m) -> const char* { return m.first.c_str(); });
                
                static int current_material = 0;
                ImGui::SetNextItemWidth(-FLT_MIN);
                if (ImGui::Combo("##SelectedMaterial", &current_material, material_names.data(), material_names.size()))
                {
                    material->Material = &material_library.at(material_names[current_material]);
                }
            }

            auto& albedo                      = material->Material->Albedo;
            auto& emission                    = material->Material->Emission;
            auto& roughness_metallic_specular = material->Material->RoughnessMetallicSpecular;

            auto& texture_library = m_Layer->Application->AssetManager.GetTextureLibrary();
            std::vector<const char*> texture_names;
            texture_names.reserve(texture_names.size()+1);
            texture_names.push_back("None");
            for (auto const& texture : texture_library)
                texture_names.push_back(texture.first.c_str());
            
            {
                ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::ColorEdit3("##Albedo", glm::value_ptr(albedo));

                static int current_albedo = 0;
                ImGui::SetNextItemWidth(-FLT_MIN);
                if (ImGui::Combo("##SelectedAlbedoTexture", &current_albedo, texture_names.data(), texture_names.size()))
                {
                    if (strcmp(texture_names[current_albedo], "None") == 0)
                        material->Material->Detach(TextureType::Albedo);
                    else
                        material->Material->Attach(texture_library.at(texture_names[current_albedo]), TextureType::Albedo);
                }
            }
            
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::ColorEdit3("##Emission", glm::value_ptr(emission));

            float* rms = glm::value_ptr(roughness_metallic_specular);

            {
                ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::DragFloat("##Roughness", rms, 0.01f, 0.0f, 1.0f);

                ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::DragFloat("##Metallic", rms + 1, 0.01f, 0.0f, 1.0f);

                ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::DragFloat("##Specular", rms + 2, 0.01f, 0.0f, 1.0f);

                static int current_roughness_metallic = 0;
                ImGui::SetNextItemWidth(-FLT_MIN);
                if (ImGui::Combo("##SelectedRoughnessMetallicTexture", &current_roughness_metallic, texture_names.data(), texture_names.size()))
                {
                    if (strcmp(texture_names[current_roughness_metallic], "None") == 0)
                        material->Material->Detach(TextureType::RoughnessMetallic);
                    else
                        material->Material->Attach(texture_library.at(texture_names[current_roughness_metallic]), TextureType::RoughnessMetallic);
                }

                static int current_normal = 0;
                ImGui::SetNextItemWidth(-FLT_MIN);
                if (ImGui::Combo("##SelectedNormal", &current_normal, texture_names.data(), texture_names.size()))
                {
                    if (strcmp(texture_names[current_normal], "None") == 0)
                        material->Material->Detach(TextureType::Normal);
                    else
                        material->Material->Attach(texture_library.at(texture_names[current_normal]), TextureType::Normal);
                }
            }
        }
        else if (component_id == registry->id<PointLightComponent>())
        {
            added_to_table = true;

            ImGui::TextUnformatted("Color");
            ImGui::TextUnformatted("Radiant Flux");
            
            ImGui::TableNextColumn();

            auto& flux  = entity.get_mut<PointLightComponent>()->RadiantFlux;
            auto& color = entity.get_mut<PointLightComponent>()->Color;
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::ColorEdit3("##PointLightColor", glm::value_ptr(color));
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::InputFloat("##PointLightFlux", &flux);
        }
    }
    else
    {
        auto relation = component_id.first();
        auto target = component_id.second();


        if (relation == registry->entity<TransformComponent>() && target == registry->entity<LocalTag>())
        {
            added_to_table = true;

            ImGui::PushStyleVarY(ImGuiStyleVar_ItemSpacing, 9);
            ImGui::TextUnformatted("Translation");
            ImGui::TextUnformatted("Rotation");
            ImGui::TextUnformatted("Scale");
            ImGui::TextUnformatted("Skew");
            ImGui::PopStyleVar();
            
            ImGui::TableNextColumn();
            
            auto& translation = entity.get_mut<TransformComponent, LocalTag>()->Translation;
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::DragFloat3("##Translation", glm::value_ptr(translation), 0.1f);

            auto& rotation = entity.get_mut<TransformComponent, LocalTag>()->Rotation;
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::DragFloat3("##Rotation", glm::value_ptr(rotation), 0.1f);

            auto& scale = entity.get_mut<TransformComponent, LocalTag>()->Scale;
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::DragFloat3("##Scale", glm::value_ptr(scale), 0.1f);

            auto& skew = entity.get_mut<TransformComponent, LocalTag>()->Skew;
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::DragFloat3("##Skew", glm::value_ptr(skew), 0.1f);
        }
    }

    ImGui::PopID();
}

void PropertyEditor::ShowEntityContextMenu()
{
    auto world    = &m_Layer->World;
    auto registry = &world->Registry;

    auto io = ImGui::GetIO();

    static char buffer[256];
    // memset(buffer, 0, sizeof(buffer));
    // strcpy_s(buffer, sizeof(buffer), "Unnamed");
    static std::string entity_name = "Unnamed";
    // strcpy_s(buffer, sizeof(buffer), "Unnamed");

    ImGui::Begin("##CreateMenu", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoSavedSettings);

    static flecs::entity created_entity;

    if (ImGui::BeginPopupTitleVoidContext("Create"))
    {
        bool closed = false;
        if (ImGui::Button("Entity"))
        {
            ImGui::EndPopup();
            closed = true;
            ImGui::OpenPopup("Create Entity");
            created_entity = world->CreateEntity("Unnamed Entity", *m_Layer->Application->AssetManager.GetMesh("Cube"));
            
            Material material;
            material.Create(&m_Layer->Application->Context, m_Layer->Application->AssetManager.GetShader("GBuffer"));
            material.Albedo = { 0.95f, 0.95f, 0.95f };
            material.RoughnessMetallicSpecular = { 0.5f, 0.0f, 0.3f };
            m_Layer->Application->AssetManager.AddMaterial(material, "material_" + std::to_string(created_entity.id()));

            created_entity.get_mut<MaterialComponent>()->Material =
                m_Layer->Application->AssetManager.GetMaterial("material_" + std::to_string(created_entity.id()));

            world->SelectEntity(created_entity);
        }
        else if (ImGui::Button("Point Light"))
        {
            ImGui::EndPopup();
            closed = true;
            ImGui::OpenPopup("Create Point Light");
            created_entity = world->CreatePointLight("Unnamed Point Light");
            world->SelectEntity(created_entity);
        }
        if (!closed)
            ImGui::EndPopup();
    }

    if (ImGui::BeginPopupTitle("Create Entity"))
    {
        ImGui::SeparatorText("Create Entity");

            const char* entity_name = created_entity.get<NameComponent>()->Name.c_str();
            ImGui::Text("%s", entity_name);
            const auto& translation = created_entity.get<TransformComponent, GlobalTag>()->Translation;
            ImGui::Text("(%0.3f, %0.3f, %0.3f)", translation.x, translation.y, translation.z);
            ImGui::TextDisabled("UID: 0x%08X (%i)", created_entity.id(), created_entity.id());
            ImGui::Separator();

        // ImGui::TextDisabled("%s", "Name");
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopupTitle("Create Point Light"))
    {
        ImGui::SeparatorText("Create Point Light");

            const char* entity_name = created_entity.get<NameComponent>()->Name.c_str();
            ImGui::Text("%s", entity_name);
            const auto& translation = created_entity.get<TransformComponent, GlobalTag>()->Translation;
            ImGui::Text("(%0.3f, %0.3f, %0.3f)", translation.x, translation.y, translation.z);
            ImGui::TextDisabled("UID: 0x%08X (%i)", created_entity.id(), created_entity.id());
            ImGui::Separator();

        ImGui::EndPopup();
    }

    ImGui::End();
}

} // namespace zrn