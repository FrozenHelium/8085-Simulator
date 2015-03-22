#pragma once

#include "Exception.h"

#include <vector>
#include <string>
#include <gtk/gtk.h>

// class to represent individual menu items
class MenuItem
{
public:
    MenuItem() :m_handle(0), m_id(-1){}

    // creates a menu item with given label
    void Create(const std::string& label);
    int GetID();
    void SetID(int id);

    // returns a widget handle representing the item
    GtkWidget* GetHandle();
private:
    GtkWidget* m_handle;
    int m_id;
};


// class to represent group of menu items
// i.e a single menu
class Menu
{
public:
    Menu() :m_handle(0), m_menu(0), m_id(0){}

    // creates a menu with specified label
    void Create(const std::string& label, int id);

    // adds  an item to the menu,
    // returns the id for the current item in the menu
    // id is the index of the item starting from zero
    int AddItem(const std::string& label, int id);

    // returns a widget handle representing a specfic item
    // specified by 'index' 
    // index and id are the same
    GtkWidget* GetItemHandle(int itemID);

    // return handle to the widget representing the menu list
    GtkWidget* GetHandle();

    // return handle to the widget representing the menu itself
    GtkWidget* GetMenu();

    // not used ( no need )
    // using gtk_widget_show_all(menubar) works for menu
    void Show();

    int GetID();
private:
    GtkWidget* m_handle;
    GtkWidget* m_menu;
    std::vector<MenuItem> m_items;
    int m_id;
};

struct MenuEventData
{
    int menuID;
    int menuItemID;
    MenuEventData(int menuID, int menuItemID) :menuID(menuID),
        menuItemID(menuItemID){}
};


// class to represent a menu bar
class MenuBar
{
public:
    
    MenuBar() :m_handle(0), m_parent(0), m_menuEventHandler(0){}

    void FixedPut(GtkWidget* fixed);
    void Initialize(GtkWidget* parent);

    // Adds a menu
    void AddMenu(std::string label, int id);

    // Adds an item to a specific menu
    void AddMenuItem(unsigned int menuID, std::string itemLabel, int id);

    // shows all the menu
    void Show();

    // sets the event handler function 
    void SetEventHandler(GCallback eventHandler);
private:
    GtkWidget* m_handle;
    GtkWidget* m_parent;
    std::vector<Menu> m_menus;
    GCallback m_menuEventHandler;
};