#ifndef CUTTER_H
#define CUTTER_H

#include <map>
#include <list>
#include <string>
#include <iostream>

/**
 * \brief Represents rectangles to draw
 * 
 * pos_x - x (left to right) position of left top corner
 * pos_y - y (top to bottom) position of left top corner
 * height - height of rectangle
 * width - width of rectangle
*/
struct Rectangle {
    float pos_x;
    float pos_y;
    float height;
    float width;
};

/**
 * \brief Represents leftover rectangles created in cutting process
*/
struct Leftover {
    float height;
    float width;
    int id_material_type;
};

/**
 * \brief Represents element required for furniture assembly
 * 
 * id - element id
 * name - element name
 * width - element width
 * height - element height
 * cut_planned - flase if not planned yet, true if already planned
*/
struct Element {
    unsigned int id;
    std::string name;
    float width;
    float height;
    bool cut_planned;
};

/**
 * \brief Represents component (material in storage) which can be used to cut elements from
*/
struct Component {
    unsigned int id;
    float width;
    float height;
};

/**
 * \brief Represents component with cuts
 * 
 * component_outline - rectangle representing outline
 * elements - list of elements outlines
 * leftovers - list of leftover material
*/
struct Component_with_cuts {
    unsigned int stored_component_id;
    unsigned int material_type_id;
    struct Rectangle component_outline;
    std::list<struct Rectangle> elements;
    std::list<struct Rectangle> leftovers;
};

class Cutter {
    public:
        /**
         * \brief Sets id of furniture to cut and resets structure in preparation for new planning
         * 
         * \param id_furniture - id of furniture to cutting plan 
        */
        void start_new_planning(unsigned int furniture_id);

        /**
         * \brief Returns id of furniture currently set to cutting plan
         * 
         * \return id of furniture currently set to cutting plan
        */
        unsigned int get_furniture_id() { return furniture_id; }

        /**
         * \brief Returns information if planning ended with success
         * 
         * \return false - planning ended without success, true - planning ended with success
        */
        bool is_planning_valid() { return planned_successfully; }

        /**
         * \brief Returns details of all leftovers created during cutting
         * 
         * \return list of leftovers created during cutting
        */
        std::list<Leftover> get_created_leftovers() { return leftovers; }

        /**
         * \brief Adds element to list of elements needed for furniture
         * 
         * \param id - id of element
         * \param name - name of element
         * \param width - width of element
         * \param height - height of element
         * \param type - id of material type of given element
        */
        void add_furniture_element(unsigned int id, std::string name, float width, float height, unsigned int type);

        /**
         * \brief Returns of used material types ids
         * 
         * \return list of ids of used material types
        */
        std::list<unsigned int> get_used_material_type_ids();

        /**
         * \brief Adds component which will be used to cut furniture elements out of it
         * 
         * \param id - id of component
         * \param width - width of component
         * \param height - height of component
         * \param type - id of material type of given component
        */
        void add_stored_component(unsigned int id, float width, float height, unsigned int type);

        /**
         * \brief Returns list of ids of used components from storage
         * 
         * \return list of ids of used components from storage
        */
        std::list<unsigned int> get_used_stored_components_ids();

        /**
         * \brief Shows contents of module's all variables
        */
        void show_variables_contents();

        /**
         * \brief Returns list of stored components with cuts
         * 
         * \return list of stored components with cuts
        */
        std::list<Component_with_cuts> get_components_with_cuts() { return components_with_cuts; }

        /**
         * \brief Plan how to cut each component to get elements required for given furniture
        */
        void plan();

    private:
        /**
         * \brief ID of furniture which elements needs to be cut
        */
        unsigned int furniture_id;

        /**
         * \brief Did planning process ended successfully?
        */
        bool planned_successfully;

        /**
         * \brief List of leftovers (not needed materials) created during cutting process
        */
        std::list<Leftover> leftovers;

        /**
         * \brief Map of lists containing furniture elements
         * 
         * Key of map is ID of material type of elements in corresponding list
         * Lists contains objects of type Element
        */
        std::map<unsigned int, std::list<Element>> elements;

        /**
         * \brief Map of lists containing components (material in storage which can be used to cut elements from)
         * 
         * Key of map is ID of material type of elements in corresponding list
         * Lists contains objects of type Component
        */
        std::map<unsigned int, std::list<Component>> components;

        /**
         * \brief List of components with cuts required to create needed elements
        */
        std::list<Component_with_cuts> components_with_cuts;
};

#endif