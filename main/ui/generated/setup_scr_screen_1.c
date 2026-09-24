/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "gui_guider.h"
#include "app_launcher.h"

void setup_scr_screen_1(lv_ui *ui)
{
    ui->screen_1_roller_1 = NULL;
    ui->screen_1_btn_set = NULL;
    ui->screen_1_btn_set_label = NULL;
    app_launcher_create(ui);
}
