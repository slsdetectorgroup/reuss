#include "table.h"

void display_table(const std::vector<std::string> nodes) {
    ImGui::Text("Configuration");
    if (ImGui::BeginTable("receiver_table", 1)) {
        for (const auto & item : nodes) {
            ImGui::TableNextColumn();
            ImGui::Text(item.c_str());
        }
        ImGui::EndTable();
    }
}