#include "cutter.h"

void Cutter::start_new_planning(unsigned int furniture_id) {
    this->furniture_id = furniture_id;
    planned_successfully = false;
    leftovers.clear();
    elements.clear();
    components.clear();
    components_with_cuts.clear();
}

void Cutter::add_furniture_element(unsigned int id, std::string name, float width, float height, unsigned int type) {
    Element element;
    element.id = id;
    element.name = name;
    element.width = width;
    element.height = height;
    element.cut_planned = false;
    elements[type].push_back(element);
}

std::list<unsigned int> Cutter::get_used_material_type_ids() {
    std::list<unsigned int> ids;
    for (const auto& element : elements)
        ids.push_back(element.first);
    return ids;
}

void Cutter::add_stored_component(unsigned int id, float width, float height, unsigned int type) {
    Component component;
    component.id = id;
    component.width = width;
    component.height = height;
    components[type].push_back(component);
}

std::list<unsigned int> Cutter::get_used_stored_components_ids() {
    std::list<unsigned int> ids;
    for (const auto& component : components_with_cuts)
        ids.push_back(component.stored_component_id);
    return ids;
}

void Cutter::show_variables_contents() {
    std::cout << "Furniture ID: " << furniture_id << std::endl;
    std::cout << "Planned successfully: " << planned_successfully << std::endl;
    std::cout << "Leftovers: " << std::endl;
    for (const auto& leftover : leftovers)
        std::cout << "    - height: " << leftover.height << "; width: " << leftover.width << "; material type ID: " << leftover.id_material_type << std::endl;
    std::cout << "Elements: " << std::endl;
    for (const auto& list : elements) {
        std::cout << "    - material type ID: " << list.first << ":" << std::endl;
        for (const auto& element : list.second)
            std::cout << "        - ID: " << element.id << "; name: " << element.name << "; height: " << element.height << "; width: " << element.width << std::endl;
    }
    std::cout << "Components: " << std::endl;
    for (const auto& list : components) {
        std::cout << "    - material type ID: " << list.first << ":" << std::endl;
        for (const auto& component : list.second)
            std::cout << "        - ID: " << component.id << "; height: " << component.height << "; width: " << component.width << std::endl;
    }
    std::cout << "Components with cuts: " << std::endl;
    for (const auto& component : components_with_cuts) {
        std::cout << "    - component ID: " << component.stored_component_id << "; material type ID: " << component.material_type_id << std::endl;
        std::cout << "       outline - x: " << component.component_outline.pos_x << "; y: " << component.component_outline.pos_y << std::endl;
        std::cout << "       outline - w: " << component.component_outline.width << "; h: " << component.component_outline.height << std::endl;
        std::cout << "    - Leftovers: " << std::endl;
        for(const auto& leftover : component.leftovers) {
            std::cout << "          x: " << leftover.pos_x << "; y: " << leftover.pos_y << "; h: " << leftover.height << "; w: " << leftover.width << std::endl;
        }
        std::cout << "    - Elements: " << std::endl;
        for(const auto& leftover : component.elements) {
            std::cout << "          x: " << leftover.pos_x << "; y: " << leftover.pos_y << "; h: " << leftover.height << "; w: " << leftover.width << std::endl;
        }
    }
}

void Cutter::plan() {
    bool component_added_to_components_with_cuts;
    float pos_x;
    float pos_y;
    float largest_h_this_row;
    /* Sort elements descending by height in every list containing elements */
    for (unsigned int& type_id : get_used_material_type_ids())
        elements[type_id].sort([]( const Element& a, const Element& b) { return a.height > b.height; });
    /* Loop over material type IDs */
    for (unsigned int& type_id : get_used_material_type_ids()) {
        /* Loop over components */
        for (Component& component : components[type_id]) {
            Component_with_cuts component_with_cuts;
            Rectangle rect_component;
            component_added_to_components_with_cuts = false;
            pos_x = 0;
            pos_y = 0;
            largest_h_this_row = 0;
            /* Loop over elements */
            for (Element& element : elements[type_id]) {
                // Go to next element if cutting of this element was already planned
                if(element.cut_planned) continue;
                // Check if component is big enough to fit element
                if(component.height < element.height || component.width < element.width)
                    continue;
                // Check if element fits horizontally
                if ((pos_x + element.width) > component.width) {
                    // Save space left as leftover
                    Leftover leftover;
                    leftover.id_material_type = type_id;
                    leftover.width = component.width - pos_x;
                    leftover.height = largest_h_this_row;
                    if(leftover.width != 0 && leftover.height != 0)
                        leftovers.push_back(leftover);
                    Rectangle rect_leftover;
                    rect_leftover.pos_x = pos_x;
                    rect_leftover.pos_y = pos_y;
                    rect_leftover.width = component.width - pos_x;
                    rect_leftover.height = largest_h_this_row;
                    if(rect_leftover.width != 0 && rect_leftover.height != 0)
                        component_with_cuts.leftovers.push_back(rect_leftover);
                    // Go at the begining of next row
                    pos_x = 0;
                    pos_y += largest_h_this_row;
                    largest_h_this_row = 0;
                }
                // Check if element fits vertically
                if ((pos_y + element.height) > component.height) {
                    // Save space at the bottom of component as leftover
                    Leftover leftover;
                    leftover.id_material_type = type_id;
                    leftover.width = component.width;
                    leftover.height = component.height - pos_y;
                    if(leftover.width != 0 && leftover.height != 0)
                        leftovers.push_back(leftover);
                    Rectangle rect_leftover;
                    rect_leftover.pos_x = pos_x;
                    rect_leftover.pos_y = pos_y;
                    rect_leftover.width = component.width;
                    rect_leftover.height = component.height - pos_y;
                    if(rect_leftover.width != 0 && rect_leftover.height != 0)
                        component_with_cuts.leftovers.push_back(rect_leftover);
                    // Go to the next component
                    break;
                }   
                // Add component to components_with_cuts if not added yet
                if (component_added_to_components_with_cuts == false) {
                    rect_component.pos_x = 0;
                    rect_component.pos_y = 0;
                    rect_component.width = component.width;
                    rect_component.height = component.height;
                    component_with_cuts.stored_component_id = component.id;
                    component_with_cuts.material_type_id = type_id;
                    component_with_cuts.component_outline = rect_component;
                    component_added_to_components_with_cuts = true;
                }
                // Save height if element is heighest in row
                if (element.height > largest_h_this_row)
                    largest_h_this_row = element.height;
                // Add element to components_with_cuts
                Rectangle rect_element;
                rect_element.pos_x = pos_x;
                rect_element.pos_y = pos_y;
                rect_element.width = element.width;
                rect_element.height = element.height;
                component_with_cuts.elements.push_back(rect_element);
                // Add leftover to leftovers and components_with_cuts
                Leftover leftover;
                leftover.id_material_type = type_id;
                leftover.width = rect_element.width;
                leftover.height = largest_h_this_row - rect_element.height;
                if(leftover.width != 0 && leftover.height != 0)
                    leftovers.push_back(leftover);
                Rectangle rect_leftover;
                rect_leftover.pos_x = rect_element.pos_x;
                rect_leftover.pos_y = rect_element.pos_y + rect_element.height;
                rect_leftover.width = rect_element.width;
                rect_leftover.height = largest_h_this_row - rect_element.height;
                if(rect_leftover.width != 0 && rect_leftover.height != 0)
                    component_with_cuts.leftovers.push_back(rect_leftover);
                // Mark element as planned to cut
                element.cut_planned = true;
                // Move to the next free space in row
                pos_x += element.width;
            }
            // If component don't have any cuts skip adding to components with cuts
            if(component_with_cuts.elements.empty())
                continue;
            // Add leftover to the end of component horizontally
            Leftover leftover_hor;
            leftover_hor.id_material_type = type_id;
            leftover_hor.width = component.width - pos_x;
            leftover_hor.height = largest_h_this_row;
            if(leftover_hor.width != 0 && leftover_hor.height != 0)
                leftovers.push_back(leftover_hor);
            Rectangle rect_leftover_hor;
            rect_leftover_hor.pos_x = pos_x;
            rect_leftover_hor.pos_y = pos_y;
            rect_leftover_hor.width = component.width - pos_x;
            rect_leftover_hor.height = largest_h_this_row;
            if(rect_leftover_hor.width != 0 && rect_leftover_hor.height != 0)
                component_with_cuts.leftovers.push_back(rect_leftover_hor);
            // Add leftover to the end of component veritcally
            Leftover leftover_ver;
            leftover_ver.id_material_type = type_id;
            leftover_ver.width = component.width;
            leftover_ver.height = component.height - (pos_y + largest_h_this_row);
            if(leftover_ver.width != 0 && leftover_ver.height != 0)
                leftovers.push_back(leftover_ver);
            Rectangle rect_leftover_ver;
            rect_leftover_ver.pos_x = 0;
            rect_leftover_ver.pos_y = pos_y + largest_h_this_row;
            rect_leftover_ver.width = component.width;
            rect_leftover_ver.height = component.height - (pos_y + largest_h_this_row);
            if(rect_leftover_ver.width != 0 && rect_leftover_ver.height != 0)
                component_with_cuts.leftovers.push_back(rect_leftover_ver);
            // Add to components with cuts
            components_with_cuts.push_back(component_with_cuts);
        }
    }

    // Check if all components were planned to cut
    planned_successfully = true;
    for (unsigned int& type_id : get_used_material_type_ids()) {
        if(planned_successfully == false)
            break;
        for (Element& element : elements[type_id]) {
            if (element.cut_planned == false) {
                planned_successfully = false;
                break;
            }
        }
    }
}