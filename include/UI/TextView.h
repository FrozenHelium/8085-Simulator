#pragma once

#include "UI/Control.h"

class TextView;

class TextViewOnChangeCallbackData
{
public:
    TextView* viewControl;
    void* callbackData;
};

class TextView: public Control
{
public:
    TextView()
    {
        m_handle = gtk_text_view_new();
        m_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(m_handle));
        
        gtk_text_buffer_create_tag(m_buffer, "instruction", "foreground", "#5556FF", NULL);
        gtk_text_buffer_create_tag(m_buffer, "register", "foreground", "#55EE88", NULL);
        gtk_text_buffer_create_tag(m_buffer, "comment", "foreground", "#bfbfbf", NULL);
        gtk_text_buffer_create_tag(m_buffer, "normal", "foreground", "#222222", NULL);
        gtk_text_buffer_create_tag(m_buffer, "error", "foreground", "#ff0000", NULL);
    }
    void SetOnChangeCallback(GCallback onChangeHandler, void* callbackData)
    {
        m_onChangeHandler = onChangeHandler;
        m_callbackData.viewControl = this;
        m_callbackData.callbackData = callbackData;
        g_signal_connect(m_buffer, "changed", G_CALLBACK(m_onChangeHandler), &m_callbackData);
    }
    void RemoveTag(std::string tagName, int start, int end)
    {
        int y = 0;
        gtk_text_buffer_get_iter_at_mark(m_buffer, &m_iter, gtk_text_buffer_get_insert(m_buffer));
        y = gtk_text_iter_get_line(&m_iter);
        gtk_text_buffer_get_iter_at_line_offset(m_buffer, &m_start, y, start);
        gtk_text_buffer_get_iter_at_line_offset(m_buffer, &m_end, y, end);
        gtk_text_buffer_remove_tag_by_name(m_buffer, tagName.c_str(), &m_start, &m_end);
    }
    void ApplyTag(std::string tagName, int start, int end)
    {
        int y = 0;
        gtk_text_buffer_get_iter_at_mark(m_buffer, &m_iter, gtk_text_buffer_get_insert(m_buffer));
        y = gtk_text_iter_get_line(&m_iter);
        gtk_text_buffer_get_iter_at_line_offset(m_buffer, &m_start, y, start);
        gtk_text_buffer_get_iter_at_line_offset(m_buffer, &m_end, y, end);
        gtk_text_buffer_remove_all_tags(m_buffer, &m_start, &m_end);
        gtk_text_buffer_apply_tag_by_name(m_buffer, tagName.c_str(), &m_start, &m_end);
    }
    GtkTextBuffer* Buffer(){ return m_buffer; }
    std::string GetCurrentLine()
    {
        int x = 0, y = 0;
        gtk_text_buffer_get_iter_at_mark(m_buffer, &m_iter, gtk_text_buffer_get_insert(m_buffer));
        y = gtk_text_iter_get_line(&m_iter);
        gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(m_buffer), &m_start, y);
        gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(m_buffer), &m_end, y);
        gtk_text_iter_forward_to_line_end(&m_end);
        return gtk_text_buffer_get_text(m_buffer, &m_start, &m_end, FALSE);
    }
    void Set(GtkWidget* container, int id, int x, int y, int w, int h)
    {
        Control::Set(container, id, x, y, w, h);
        m_scroller = gtk_scrolled_window_new(NULL, NULL);
        gtk_scrolled_window_set_min_content_width(GTK_SCROLLED_WINDOW(m_scroller), m_w);
        gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(m_scroller), m_h);
        gtk_widget_reparent(m_handle, m_scroller);
        gtk_fixed_put(GTK_FIXED(m_container), m_scroller, m_x, m_y);
        gtk_widget_set_halign(m_handle, GTK_ALIGN_START);
        gtk_widget_set_valign(m_handle, GTK_ALIGN_END);
    }
    void Show()
    {
        gtk_widget_show(m_scroller);
        Control::Show();
    }
    void Hide()
    {   
        gtk_widget_hide(m_scroller);
        Control::Hide();
    }
    std::string GetText()
    {
        gtk_text_buffer_get_start_iter(m_buffer, &m_start);
        gtk_text_buffer_get_end_iter(m_buffer, &m_end);
        return gtk_text_buffer_get_text(m_buffer, &m_start, &m_end, FALSE);
    }
protected:
    GtkTextBuffer *m_buffer;
    GtkTextIter m_start, m_end;
    GtkTextIter m_iter;
    GCallback m_onChangeHandler;
    TextViewOnChangeCallbackData m_callbackData;
    GtkWidget* m_scroller;
};