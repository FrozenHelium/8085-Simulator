#pragma once

#include <gtk/gtk.h>
#include <vector>

class ToolItem
{
public:
    ToolItem() :m_handle(0), m_id(-1){}
    void Set(const gchar* stockID, int itemID, GCallback callbackProcedure)
    {
        m_id = itemID;
        m_handle = gtk_tool_button_new_from_stock(stockID);
        g_signal_connect(G_OBJECT(m_handle), "clicked", G_CALLBACK(callbackProcedure), new int(m_id));
    }
    int GetID()
    {
        return m_id;
    }
    GtkWidget* GetHandle()
    {
        return GTK_WIDGET(m_handle);
    }
    GtkToolItem* GetItemHandle()
    {
        return m_handle;
    }
private:
    GtkToolItem* m_handle;
    int m_id;
};

class Toolbar 
{
public:
    Toolbar()
    {
        m_handle = gtk_toolbar_new();
        gtk_toolbar_set_style(GTK_TOOLBAR(m_handle), GTK_TOOLBAR_ICONS);
        gtk_container_set_border_width(GTK_CONTAINER(m_handle), 2);
    }
    void AddButtonFromStock(const gchar* stockID, int itemID)
    {
        m_items.resize(m_items.size() + 1);
        m_items[m_items.size() - 1].Set(stockID, itemID, m_toolItemEventHandler);
        gtk_toolbar_insert(GTK_TOOLBAR(m_handle), m_items[m_items.size() - 1].GetItemHandle(), -1);
    }
    void SetEventHandler(GCallback toolItemEventHandler)
    {
        m_toolItemEventHandler = toolItemEventHandler;
    }
    void Show()
    {
        gtk_widget_show(m_handle);
        for (int i = 0, lim = m_items.size(); i < lim; i++)
        {
            gtk_widget_show(m_items[i].GetHandle());
        }
    }
    void FixedPut(GtkWidget* fixed, int x, int y, int w, int h)
    {
        gtk_fixed_put(GTK_FIXED(fixed), m_handle, x, y);
        gtk_widget_set_size_request(m_handle, w, h);
    }
    ToolItem* GetItemByID(int id)
    {
        return &m_items[id];
}
private:
    GtkWidget* m_handle;
    GCallback m_toolItemEventHandler;
    std::vector<ToolItem> m_items;
};

