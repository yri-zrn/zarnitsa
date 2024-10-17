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

    ShowEntityContextMenu();

    ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
    ImGui::Begin("Property editor", show);

    if (ImGui::BeginChild("##tree", ImVec2(300, 0), ImGuiChildFlags_ResizeX | ImGuiChildFlags_Borders | ImGuiChildFlags_NavFlattened))
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
    ImGui::EndChild();

    // Right side: draw properties
    ImGui::SameLine();

    ImGui::BeginGroup(); // Lock X position
    if (auto entity = world->SelectedEntity)
    {
        const char* entity_name = entity.get<NameComponent>()->Name.c_str();
        ImGui::Text("%s", entity_name);
        const auto& translation = entity.get<TransformComponent, GlobalTag>()->Translation;
        ImGui::Text("(%0.3f, %0.3f, %0.3f)", translation.x, translation.y, translation.z);
        ImGui::TextDisabled("UID: 0x%08X (%i)", entity.id(), entity.id());
        ImGui::Separator();


        if (ImGui::BeginTable("##properties", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY))
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
    ImGui::EndGroup();

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
    
    ImGui::TableNextRow();
    ImGui::PushID(component_id);
    ImGui::TableNextColumn();

    ImGui::AlignTextToFramePadding();
    
    if (component_id.is_entity())
    {
        auto component = component_id.entity();
        if (component_id == registry->id<NameComponent>())
        {
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
            // ImGui::TextUnformatted("Albedo");
            
            // ImGui::TableNextColumn();

            // auto& rotation = entity.get_mut<MaterialComponent>()->Material;
            // ImGui::SetNextItemWidth(-FLT_MIN);
            // ImGui::DragFloat3("##Albedo", glm::value_ptr(rotation), 0.1f);
        }
    }
    else
    {
        auto relation = component_id.first();
        auto target = component_id.second();


        if (relation == registry->entity<TransformComponent>()
         && target == registry->entity<LocalTag>())
        {
            ImGui::TextUnformatted("Translation");
            ImGui::TextUnformatted("Rotation");
            ImGui::TextUnformatted("Scale");
            ImGui::TextUnformatted("Skew");
            
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
    //! FIXME: doesn't work properly
    // TODO: fix

    auto world    = &m_Layer->World;
    auto registry = &world->Registry;

    auto io = ImGui::GetIO();

    static char buffer[256];
    memset(buffer, 0, sizeof(buffer));
    strcpy_s(buffer, sizeof(buffer), "Unnamed");
    static std::string entity_name = "Unnamed";

    if (ImGui::BeginPopupVoidTitle("Entity"))
    {
        ImGui::TextDisabled("%s", "Name");
        ImGui::SameLine();
        if (ImGui::InputText("##Name", buffer, sizeof(buffer)))
        {
            entity_name = std::string(buffer);
        }

        if (ImGui::MenuItem("Create"))
        {
            world->CreateEntity(entity_name, m_Layer->Application->AssetManager.CubeMesh(&m_Layer->Application->Context));
        }
        ImGui::EndPopup();


    }
}

} // namespace zrn