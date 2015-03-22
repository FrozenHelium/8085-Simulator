#include "UI/Menu.h"


////////////////////////////////////////////////////////////
// Definitions for member functions of MenuItem
////////////////////////////////////////////////////////////

void MenuItem::Create(const std::string& label)
{
    m_handle = gtk_menu_item_new_with_label(label.c_str());
}

int MenuItem::GetID()
{
    return m_id;
}

void MenuItem::SetID(int id)
{
    m_id = id;
}

GtkWidget* MenuItem::GetHandle()
{
    return m_handle;
}





////////////////////////////////////////////////////////////
// Definitions for member functions of Menu
////////////////////////////////////////////////////////////


void Menu::Create(const std::string& label, int id)
{
    m_handle = gtk_menu_new();
    m_menu = gtk_menu_item_new_with_label(label.c_str());
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(m_menu), m_handle);
    m_id = id;
}

int Menu::AddItem(const std::string& label, int id)
{
    m_items.resize(m_items.size() + 1);
    m_items[m_items.size() - 1].Create(label);
    gtk_menu_shell_append(GTK_MENU_SHELL(m_handle), m_items[m_items.size() - 1].GetHandle());
    m_items[m_items.size() - 1].SetID(id);
    return (m_items.size() - 1);
}


GtkWidget* Menu::GetItemHandle( int itemID)
{
    int itemIndex = -1;
    for (int i = 0, lim = m_items.size(); i < lim; i++)
    {
        if (m_items[i].GetID() == itemID)
        {
            itemIndex = i;
            break;
        }
    }
    if (itemIndex == -1)
    {
        throw Exception("invalid index for menu item");
    }
    return m_items[itemIndex].GetHandle();
}

GtkWidget* Menu::GetHandle()
{
    return m_handle;
}

GtkWidget* Menu::GetMenu()
{
    return m_menu;
}
int Menu::GetID()
{
    return m_id;
}

void Menu::Show()
{

    gtk_widget_show(m_handle);
    for (int i = 0, lim = m_items.size(); i < lim; i++)
    {
        gtk_widget_show(m_items[i].GetHandle());
    }
}

////////////////////////////////////////////////////////////
// Definitions for member functions of MenuBar
////////////////////////////////////////////////////////////

void MenuBar::FixedPut(GtkWidget* fixed)
{
    //gtk_fixed_put(GTK_FIXED(fixed), m_handle, 0, 0);
    gtk_container_add(GTK_CONTAINER(fixed), m_handle);
}

void MenuBar::Initialize(GtkWidget* parent)
{
    m_parent = parent;
    m_handle = gtk_menu_bar_new();
}

void MenuBar::AddMenu(std::string label, int id)
{
    m_menus.resize(m_menus.size() + 1);
    m_menus[m_menus.size() - 1].Create(label, id);
    gtk_menu_shell_append(GTK_MENU_SHELL(m_handle), m_menus[m_menus.size() - 1].GetMenu());
    
}

void MenuBar::AddMenuItem(unsigned int menuID, std::string itemLabel, int itemID)
{
    Menu* currentMenu = 0;
    for (int i = 0, lim = m_menus.size(); i < lim; i++)
    {
        if (m_menus[i].GetID() == menuID)
        {
            currentMenu = &m_menus[i];
            break;
        }
    }
    if (!currentMenu) throw Exception("invalid value for menuID");
    currentMenu->AddItem(itemLabel, itemID);
    g_signal_connect(G_OBJECT(currentMenu->GetItemHandle(itemID)),
        "activate", m_menuEventHandler, new MenuEventData(menuID, itemID));
}

void MenuBar::Show()
{
    gtk_widget_show_all(m_handle);
}
void MenuBar::SetEventHandler(GCallback eventHandler)
{
    m_menuEventHandler = eventHandler;
}
