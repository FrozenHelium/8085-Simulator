#pragma once

#include "UI/Page.h"
#include "UI/pages/pages.h"


class DebugPage : public Page
{
public:
    void OnControlEvent(Control* control, int eventID)
    {
    }
    DebugPage(GtkWidget* parentWindow, GtkWidget* fixed)
    {
        this->m_ID = PAGE::DEBUGPAGE;
        this->Initialize(parentWindow, fixed);
        int x = 250, y = 100;
        int w = this->GetParentWidth(), h = this->GetParentHeight();

    }
private:
};