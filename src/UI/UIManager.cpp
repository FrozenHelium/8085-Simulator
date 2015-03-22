
#include "UI/UIManager.h"

UIManager* UIManager::m_uiManager = 0;
UIEventCallBack UIManager::m_eventHandler = 0;



void UIManager::MenuItemEventHandler(GtkWidget* widget, gpointer data)
{
    MenuEventData *eventData = static_cast<MenuEventData*>(data);
    (m_eventHandler)(new UIEventData(EVENT::MENUITEMCLICK, eventData->menuItemID));
}


void UIManager::PageEventHandler(ControlEventData* eventData)
{
    (m_eventHandler)(new UIEventData(EVENT::BUTTONCLICK, eventData->control->GetID()));
}

void UIManager::ToolItemEventHandler(GtkWidget* widget, gpointer data)
{
    int itemID = *(static_cast<int*>(data));
    (m_eventHandler)(new UIEventData(EVENT::TOOLITEMCLICK, itemID));
}

void UIManager::AddPage(Page* page)
{
    m_pages.resize(m_pages.size() + 1);
    m_pages[m_pages.size() - 1] = page;
    page->SetEventHandler(PageEventHandler);
}


void UIManager::AddMenu(std::string label, int id)
{
    m_menubar.AddMenu(label, id);
}
void UIManager::AddMenuItem(int menuID, std::string label, int itemID)
{
    m_menubar.AddMenuItem(menuID, label, itemID);
}
void UIManager::AddToolItemFromStock(const gchar* stockID, int itemID)
{
    m_toolbar.AddButtonFromStock(stockID, itemID);
}


void UIManager::Initialize(GtkWidget* parent, GtkWidget* fixed)
{
    m_parent = parent;
    m_fixed = fixed;
    m_menubar.Initialize(parent);
    m_menubar.SetEventHandler(G_CALLBACK(MenuItemEventHandler));
    m_menubar.FixedPut(fixed);
    m_toolbar.FixedPut(fixed, 0, 32, 500, 30);
    m_toolbar.SetEventHandler(G_CALLBACK(ToolItemEventHandler));

}

void UIManager::NavigateBack()
{
    if (m_pageHistory.size() > 0)
    {
        Page* prevPage = m_pageHistory.top();
        if (m_currentPage)
        {
            m_currentPage->HideControls();
            m_pageHistory.pop();
        }
        m_currentPage = prevPage;
        m_currentPage->ShowControls();
    }
}

void UIManager::NavigateTo(int page)
{
    if (m_currentPage)
    {
        if (m_currentPage->GetID() == page)
        {
            return;
        }
        m_currentPage->HideControls();
        m_pageHistory.push(m_currentPage);
    }
    m_currentPage = this->GetPageByID(page);
    m_currentPage->ShowControls();
}

UIManager::UIManager()
{
    if (m_uiManager)
    {
        throw "can't create multiple objects of the UIManager class";
    }
    m_uiManager = this;
    m_parent = 0;
    m_fixed = 0;
    m_currentPage = 0;
}
