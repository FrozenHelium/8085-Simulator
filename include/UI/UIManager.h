#pragma once

#include "Exception.h"
#include "UI/Page.h"
#include "UI/Menu.h"


#include "UI/Toolbar.h"

#include <stack>

enum EVENT{BUTTONCLICK, MENUITEMCLICK, TOOLITEMCLICK};

struct UIEventData
{
    int event;
    int controlID;
    UIEventData(int event, int controlID) :event(event), controlID(controlID){}
};

typedef void(*UIEventCallBack)(UIEventData*);


// UIManager: a custom class to handle pages and menu,
// only one object of the class should be created,
// addition and removal of pages and menus can be 
// done in UIManager::Initialize() function
class UIManager
{
public:
    // Creates and initializes the menus and pages used
    // in the application
    void Initialize(GtkWidget* parent, GtkWidget* fixed);
    void AddPage(Page* page);

    void AddMenu(std::string label, int id);
    void AddMenuItem(int menuID, std::string label, int itemID);
    void AddToolItemFromStock(const gchar* stockID, int itemID);

    // Makes transition between pages
    // 'page' should be one of the value in enum 'PAGE'
    void NavigateTo(int page);
    
    UIManager();

    // called when any menu item is pressed
    // data is a pointer of MenuBar::MenuEventData type
    static void MenuItemEventHandler(GtkWidget* widget, gpointer data);
    static void ToolItemEventHandler(GtkWidget* widget, gpointer data);
    static void PageEventHandler(ControlEventData* eventData);

    Page* GetPageByID(int id)
    {
        int pageIndex = -1;
        for (int i = 0, lim = m_pages.size(); i < lim; i++)
        {
            if (m_pages[i]->GetID() == id)
            {
                pageIndex = i;
                break;
            }
        }
        if (pageIndex == -1) throw Exception("Invalid page id");
        return m_pages[pageIndex];
    }

    int GetCurrentPageID()
    {
        if (m_currentPage)
        {
            return m_currentPage->GetID();
        }
        return -1;
    }

    static void SetEventHandler(UIEventCallBack eventHandler)
    {
        m_eventHandler = eventHandler;
    }

    void ShowMenu()
    {
        m_menubar.Show();
    }
    void ShowToolbar()
    {
        m_toolbar.Show();
    }

    void NavigateBack();
private:
    GtkWidget* m_parent;
    GtkWidget* m_fixed;
    std::vector<Page*> m_pages;
    MenuBar m_menubar;
    Toolbar m_toolbar;
    Page* m_currentPage;
    static UIManager* m_uiManager;
    std::stack<Page*> m_pageHistory;
    static UIEventCallBack m_eventHandler;
};