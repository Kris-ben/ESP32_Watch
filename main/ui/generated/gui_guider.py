# Copyright 2026 NXP
# NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
# accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
# activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
# comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
# terms, then you may not retain, install, activate or otherwise use the software.

import utime as time
import usys as sys
import lvgl as lv
import ustruct
import fs_driver

lv.init()

# Register display driver.
disp_drv = lv.sdl_window_create(240, 284)
lv.sdl_window_set_resizeable(disp_drv, False)
lv.sdl_window_set_title(disp_drv, "Simulator (MicroPython)")

# Regsiter input driver
mouse = lv.sdl_mouse_create()

# Add default theme for bottom layer
bottom_layer = lv.layer_bottom()
lv.theme_apply(bottom_layer)

fs_drv = lv.fs_drv_t()
fs_driver.fs_register(fs_drv, 'Z')

def anim_x_cb(obj, v):
    obj.set_x(v)

def anim_y_cb(obj, v):
    obj.set_y(v)

def anim_width_cb(obj, v):
    obj.set_width(v)

def anim_height_cb(obj, v):
    obj.set_height(v)

def anim_img_zoom_cb(obj, v):
    obj.set_scale(v)

def anim_img_rotate_cb(obj, v):
    obj.set_rotation(v)

global_font_cache = {}
def test_font(font_family, font_size):
    global global_font_cache
    if font_family + str(font_size) in global_font_cache:
        return global_font_cache[font_family + str(font_size)]
    if font_size % 2:
        candidates = [
            (font_family, font_size),
            (font_family, font_size-font_size%2),
            (font_family, font_size+font_size%2),
            ("montserrat", font_size-font_size%2),
            ("montserrat", font_size+font_size%2),
            ("montserrat", 16)
        ]
    else:
        candidates = [
            (font_family, font_size),
            ("montserrat", font_size),
            ("montserrat", 16)
        ]
    for (family, size) in candidates:
        try:
            if eval(f'lv.font_{family}_{size}'):
                global_font_cache[font_family + str(font_size)] = eval(f'lv.font_{family}_{size}')
                if family != font_family or size != font_size:
                    print(f'WARNING: lv.font_{family}_{size} is used!')
                return eval(f'lv.font_{family}_{size}')
        except AttributeError:
            try:
                load_font = lv.binfont_create(f"Z:MicroPython/lv_font_{family}_{size}.fnt")
                global_font_cache[font_family + str(font_size)] = load_font
                return load_font
            except:
                if family == font_family and size == font_size:
                    print(f'WARNING: lv.font_{family}_{size} is NOT supported!')

global_image_cache = {}
def load_image(file):
    global global_image_cache
    if file in global_image_cache:
        return global_image_cache[file]
    try:
        with open(file,'rb') as f:
            data = f.read()
    except:
        print(f'Could not open {file}')
        sys.exit()

    img = lv.image_dsc_t({
        'data_size': len(data),
        'data': data
    })
    global_image_cache[file] = img
    return img

def calendar_event_handler(e,obj):
    code = e.get_code()

    if code == lv.EVENT.VALUE_CHANGED:
        source = lv.calendar.__cast__(e.get_current_target())
        date = lv.calendar_date_t()
        if source.get_pressed_date(date) == lv.RESULT.OK:
            source.set_highlighted_dates([date], 1)

def spinbox_increment_event_cb(e, obj):
    code = e.get_code()
    if code == lv.EVENT.SHORT_CLICKED or code == lv.EVENT.LONG_PRESSED_REPEAT:
        obj.increment()
def spinbox_decrement_event_cb(e, obj):
    code = e.get_code()
    if code == lv.EVENT.SHORT_CLICKED or code == lv.EVENT.LONG_PRESSED_REPEAT:
        obj.decrement()

def digital_clock_cb(timer, obj, current_time, show_second, use_ampm):
    hour = int(current_time[0])
    minute = int(current_time[1])
    second = int(current_time[2])
    ampm = current_time[3]
    second = second + 1
    if second == 60:
        second = 0
        minute = minute + 1
        if minute == 60:
            minute = 0
            hour = hour + 1
            if use_ampm:
                if hour == 12:
                    if ampm == 'AM':
                        ampm = 'PM'
                    elif ampm == 'PM':
                        ampm = 'AM'
                if hour > 12:
                    hour = hour % 12
    hour = hour % 24
    if use_ampm:
        if show_second:
            obj.set_text("%d:%02d:%02d %s" %(hour, minute, second, ampm))
        else:
            obj.set_text("%d:%02d %s" %(hour, minute, ampm))
    else:
        if show_second:
            obj.set_text("%d:%02d:%02d" %(hour, minute, second))
        else:
            obj.set_text("%d:%02d" %(hour, minute))
    current_time[0] = hour
    current_time[1] = minute
    current_time[2] = second
    current_time[3] = ampm

def analog_clock_cb(timer, obj):
    datetime = time.localtime()
    hour = datetime[3]
    if hour >= 12: hour = hour - 12
    obj.set_time(hour, datetime[4], datetime[5])

def datetext_event_handler(e, obj):
    code = e.get_code()
    datetext = lv.label.__cast__(e.get_target())
    if code == lv.EVENT.FOCUSED:
        if obj is None:
            bg = lv.layer_top()
            bg.add_flag(lv.obj.FLAG.CLICKABLE)
            obj = lv.calendar(bg)
            scr = lv.screen_active()
            scr_height = scr.get_height()
            scr_width = scr.get_width()
            obj.set_size(int(scr_width * 0.8), int(scr_height * 0.8))
            datestring = datetext.get_text()
            year = int(datestring.split('/')[0])
            month = int(datestring.split('/')[1])
            day = int(datestring.split('/')[2])
            obj.set_showed_date(year, month)
            highlighted_days=[lv.calendar_date_t({'year':year, 'month':month, 'day':day})]
            obj.set_highlighted_dates(highlighted_days, 1)
            obj.align(lv.ALIGN.CENTER, 0, 0)
            lv.calendar_header_arrow(obj)
            obj.add_event_cb(lambda e: datetext_calendar_event_handler(e, datetext), lv.EVENT.ALL, None)
            scr.update_layout()

def datetext_calendar_event_handler(e, obj):
    code = e.get_code()
    calendar = lv.calendar.__cast__(e.get_current_target())
    if code == lv.EVENT.VALUE_CHANGED:
        date = lv.calendar_date_t()
        if calendar.get_pressed_date(date) == lv.RESULT.OK:
            obj.set_text(f"{date.year}/{date.month}/{date.day}")
            bg = lv.layer_top()
            bg.remove_flag(lv.obj.FLAG.CLICKABLE)
            bg.set_style_bg_opa(lv.OPA.TRANSP, 0)
            calendar.delete()

def ta_event_cb(e,kb):
    code = e.get_code()
    ta = lv.textarea.__cast__(e.get_target())
    if code == lv.EVENT.FOCUSED:
        kb.set_textarea(ta)
        kb.move_foreground()
        kb.remove_flag(lv.obj.FLAG.HIDDEN)

    if code == lv.EVENT.DEFOCUSED:
        kb.set_textarea(None)
        kb.move_background()
        kb.add_flag(lv.obj.FLAG.HIDDEN)

# Create screen_home
screen_home = lv.obj()
g_kb_screen_home = lv.keyboard(screen_home)
g_kb_screen_home.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_home), lv.EVENT.ALL, None)
g_kb_screen_home.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_home.set_style_text_font(test_font("SourceHanSerifSC_Regular", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home.set_size(240, 284)
screen_home.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_home, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_home.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home.set_style_bg_color(lv.color_hex(0x010101), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_home_img_wifi
screen_home_img_wifi = lv.image(screen_home)
screen_home_img_wifi.set_src(load_image(r"D:\NXP\GUider_Project\smart_watch\generated\MicroPython\wifi_close_18_18.png"))
screen_home_img_wifi.add_flag(lv.obj.FLAG.CLICKABLE)
screen_home_img_wifi.set_pivot(50,50)
screen_home_img_wifi.set_rotation(0)
screen_home_img_wifi.set_pos(3, 3)
screen_home_img_wifi.set_size(18, 18)
# Set style for screen_home_img_wifi, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_home_img_wifi.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_home_img_sun
screen_home_img_sun = lv.image(screen_home)
screen_home_img_sun.set_src(load_image(r"D:\NXP\GUider_Project\smart_watch\generated\MicroPython\qing_58_57.png"))
screen_home_img_sun.add_flag(lv.obj.FLAG.CLICKABLE)
screen_home_img_sun.set_pivot(50,50)
screen_home_img_sun.set_rotation(0)
screen_home_img_sun.set_pos(8, 158)
screen_home_img_sun.set_size(58, 57)
# Set style for screen_home_img_sun, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_home_img_sun.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_home_img_ruin
screen_home_img_ruin = lv.image(screen_home)
screen_home_img_ruin.set_src(load_image(r"D:\NXP\GUider_Project\smart_watch\generated\MicroPython\yu_60_59.png"))
screen_home_img_ruin.add_flag(lv.obj.FLAG.CLICKABLE)
screen_home_img_ruin.set_pivot(50,50)
screen_home_img_ruin.set_rotation(0)
screen_home_img_ruin.set_pos(86, 162)
screen_home_img_ruin.set_size(60, 59)
# Set style for screen_home_img_ruin, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_home_img_ruin.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_home_img_cloudy
screen_home_img_cloudy = lv.image(screen_home)
screen_home_img_cloudy.set_src(load_image(r"D:\NXP\GUider_Project\smart_watch\generated\MicroPython\yun_57_50.png"))
screen_home_img_cloudy.add_flag(lv.obj.FLAG.CLICKABLE)
screen_home_img_cloudy.set_pivot(50,50)
screen_home_img_cloudy.set_rotation(0)
screen_home_img_cloudy.set_pos(166, 162)
screen_home_img_cloudy.set_size(57, 50)
# Set style for screen_home_img_cloudy, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_home_img_cloudy.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_home_img_line
screen_home_img_line = lv.image(screen_home)
screen_home_img_line.set_src(load_image(r"D:\NXP\GUider_Project\smart_watch\generated\MicroPython\line_218_37.png"))
screen_home_img_line.add_flag(lv.obj.FLAG.CLICKABLE)
screen_home_img_line.set_pivot(50,50)
screen_home_img_line.set_rotation(0)
screen_home_img_line.set_pos(8, 89)
screen_home_img_line.set_size(218, 37)
# Set style for screen_home_img_line, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_home_img_line.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_home_label_city
screen_home_label_city = lv.label(screen_home)
screen_home_label_city.set_text("南宁")
screen_home_label_city.set_long_mode(lv.label.LONG.WRAP)
screen_home_label_city.set_width(lv.pct(100))
screen_home_label_city.set_pos(170, 10)
screen_home_label_city.set_size(71, 22)
# Set style for screen_home_label_city, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_home_label_city.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_city.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_city.set_style_text_color(lv.color_hex(0xf5f5f5), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_city.set_style_text_font(test_font("ZiTiQuanWeiJunHeiW22", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_city.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_city.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_city.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_city.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_city.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_city.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_city.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_city.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_city.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_city.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_home_digital_clock
screen_home_digital_clock_time = [int(11), int(25), int(50), ""]
screen_home_digital_clock = lv.label(screen_home)
screen_home_digital_clock.set_text("11:25:50")
screen_home_digital_clock_timer = lv.timer_create_basic()
screen_home_digital_clock_timer.set_period(1000)
screen_home_digital_clock_timer.set_cb(lambda src: digital_clock_cb(screen_home_digital_clock_timer, screen_home_digital_clock, screen_home_digital_clock_time, True, False ))
screen_home_digital_clock.set_pos(-30, 47)
screen_home_digital_clock.set_size(219, 59)
# Set style for screen_home_digital_clock, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_home_digital_clock.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_digital_clock.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_digital_clock.set_style_text_font(test_font("ZiTiQuanWeiJunHeiW22", 40), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_digital_clock.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_digital_clock.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_digital_clock.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_digital_clock.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_digital_clock.set_style_pad_top(7, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_digital_clock.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_digital_clock.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_digital_clock.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_digital_clock.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_home_label_data
screen_home_label_data = lv.label(screen_home)
screen_home_label_data.set_text("2026年1月16日\n")
screen_home_label_data.set_long_mode(lv.label.LONG.WRAP)
screen_home_label_data.set_width(lv.pct(100))
screen_home_label_data.set_pos(98, 35)
screen_home_label_data.set_size(152, 22)
# Set style for screen_home_label_data, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_home_label_data.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_data.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_data.set_style_text_color(lv.color_hex(0xf5f5f5), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_data.set_style_text_font(test_font("ZiTiQuanWeiJunHeiW22", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_data.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_data.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_data.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_data.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_data.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_data.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_data.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_data.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_data.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_data.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_home_label_week
screen_home_label_week = lv.label(screen_home)
screen_home_label_week.set_text("星期一")
screen_home_label_week.set_long_mode(lv.label.LONG.WRAP)
screen_home_label_week.set_width(lv.pct(100))
screen_home_label_week.set_pos(160, 64)
screen_home_label_week.set_size(71, 22)
# Set style for screen_home_label_week, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_home_label_week.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_week.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_week.set_style_text_color(lv.color_hex(0xf5f5f5), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_week.set_style_text_font(test_font("ZiTiQuanWeiJunHeiW22", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_week.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_week.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_week.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_week.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_week.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_week.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_week.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_week.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_week.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_week.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_home_label_today
screen_home_label_today = lv.label(screen_home)
screen_home_label_today.set_text("今日")
screen_home_label_today.set_long_mode(lv.label.LONG.WRAP)
screen_home_label_today.set_width(lv.pct(100))
screen_home_label_today.set_pos(3, 130)
screen_home_label_today.set_size(71, 22)
# Set style for screen_home_label_today, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_home_label_today.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_today.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_today.set_style_text_color(lv.color_hex(0xf5f5f5), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_today.set_style_text_font(test_font("ZiTiQuanWeiJunHeiW22", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_today.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_today.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_today.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_today.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_today.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_today.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_today.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_today.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_today.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_today.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_home_label_tomorow
screen_home_label_tomorow = lv.label(screen_home)
screen_home_label_tomorow.set_text("明日")
screen_home_label_tomorow.set_long_mode(lv.label.LONG.WRAP)
screen_home_label_tomorow.set_width(lv.pct(100))
screen_home_label_tomorow.set_pos(80, 130)
screen_home_label_tomorow.set_size(71, 22)
# Set style for screen_home_label_tomorow, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_home_label_tomorow.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_tomorow.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_tomorow.set_style_text_color(lv.color_hex(0xf5f5f5), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_tomorow.set_style_text_font(test_font("ZiTiQuanWeiJunHeiW22", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_tomorow.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_tomorow.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_tomorow.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_tomorow.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_tomorow.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_tomorow.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_tomorow.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_tomorow.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_tomorow.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_tomorow.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_home_label_later
screen_home_label_later = lv.label(screen_home)
screen_home_label_later.set_text("后日")
screen_home_label_later.set_long_mode(lv.label.LONG.WRAP)
screen_home_label_later.set_width(lv.pct(100))
screen_home_label_later.set_pos(166, 130)
screen_home_label_later.set_size(71, 22)
# Set style for screen_home_label_later, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_home_label_later.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_later.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_later.set_style_text_color(lv.color_hex(0xf5f5f5), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_later.set_style_text_font(test_font("ZiTiQuanWeiJunHeiW22", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_later.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_later.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_later.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_later.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_later.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_later.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_later.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_later.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_later.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_later.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_home_label_today_temp
screen_home_label_today_temp = lv.label(screen_home)
screen_home_label_today_temp.set_text("28-32℃")
screen_home_label_today_temp.set_long_mode(lv.label.LONG.WRAP)
screen_home_label_today_temp.set_width(lv.pct(100))
screen_home_label_today_temp.set_pos(8, 231)
screen_home_label_today_temp.set_size(71, 22)
# Set style for screen_home_label_today_temp, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_home_label_today_temp.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_today_temp.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_today_temp.set_style_text_color(lv.color_hex(0xf5f5f5), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_today_temp.set_style_text_font(test_font("ZiTiQuanWeiJunHeiW22", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_today_temp.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_today_temp.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_today_temp.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_today_temp.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_today_temp.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_today_temp.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_today_temp.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_today_temp.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_today_temp.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_today_temp.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_home_label_tomorow_temp
screen_home_label_tomorow_temp = lv.label(screen_home)
screen_home_label_tomorow_temp.set_text("27-31℃")
screen_home_label_tomorow_temp.set_long_mode(lv.label.LONG.WRAP)
screen_home_label_tomorow_temp.set_width(lv.pct(100))
screen_home_label_tomorow_temp.set_pos(86, 231)
screen_home_label_tomorow_temp.set_size(71, 22)
# Set style for screen_home_label_tomorow_temp, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_home_label_tomorow_temp.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_tomorow_temp.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_tomorow_temp.set_style_text_color(lv.color_hex(0xf5f5f5), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_tomorow_temp.set_style_text_font(test_font("ZiTiQuanWeiJunHeiW22", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_tomorow_temp.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_tomorow_temp.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_tomorow_temp.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_tomorow_temp.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_tomorow_temp.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_tomorow_temp.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_tomorow_temp.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_tomorow_temp.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_tomorow_temp.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_tomorow_temp.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_home_label_later_temp
screen_home_label_later_temp = lv.label(screen_home)
screen_home_label_later_temp.set_text("25-29℃")
screen_home_label_later_temp.set_long_mode(lv.label.LONG.WRAP)
screen_home_label_later_temp.set_width(lv.pct(100))
screen_home_label_later_temp.set_pos(166, 231)
screen_home_label_later_temp.set_size(71, 22)
# Set style for screen_home_label_later_temp, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_home_label_later_temp.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_later_temp.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_later_temp.set_style_text_color(lv.color_hex(0xf5f5f5), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_later_temp.set_style_text_font(test_font("ZiTiQuanWeiJunHeiW22", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_later_temp.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_later_temp.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_later_temp.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_later_temp.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_later_temp.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_later_temp.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_later_temp.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_later_temp.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_later_temp.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_home_label_later_temp.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_home.update_layout()
# Create screen_selete
screen_selete = lv.obj()
g_kb_screen_selete = lv.keyboard(screen_selete)
g_kb_screen_selete.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_selete), lv.EVENT.ALL, None)
g_kb_screen_selete.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_selete.set_style_text_font(test_font("SourceHanSerifSC_Regular", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_selete.set_size(240, 284)
screen_selete.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_selete, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_selete.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_selete.set_style_bg_color(lv.color_hex(0x0a0a0a), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_selete.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_selete_imgbtn_AP
screen_selete_imgbtn_AP = lv.imagebutton(screen_selete)
screen_selete_imgbtn_AP.add_flag(lv.obj.FLAG.CHECKABLE)
screen_selete_imgbtn_AP.set_src(lv.imagebutton.STATE.RELEASED, load_image(r"D:\NXP\GUider_Project\smart_watch\generated\MicroPython\WiFi2_77_59.png"), None, None)
screen_selete_imgbtn_AP.set_src(lv.imagebutton.STATE.PRESSED, load_image(r"D:\NXP\GUider_Project\smart_watch\generated\MicroPython\wifi_press_77_59.png"), None, None)
screen_selete_imgbtn_AP.set_src(lv.imagebutton.STATE.CHECKED_RELEASED, load_image(r"D:\NXP\GUider_Project\smart_watch\generated\MicroPython\WiFi2_77_59.png"), None, None)
screen_selete_imgbtn_AP.set_src(lv.imagebutton.STATE.CHECKED_PRESSED, load_image(r"D:\NXP\GUider_Project\smart_watch\generated\MicroPython\wifi_press_77_59.png"), None, None)
screen_selete_imgbtn_AP.add_flag(lv.obj.FLAG.CHECKABLE)
screen_selete_imgbtn_AP_label = lv.label(screen_selete_imgbtn_AP)
screen_selete_imgbtn_AP_label.set_text("")
screen_selete_imgbtn_AP_label.set_long_mode(lv.label.LONG.WRAP)
screen_selete_imgbtn_AP_label.set_width(lv.pct(100))
screen_selete_imgbtn_AP_label.align(lv.ALIGN.CENTER, 0, 0)
screen_selete_imgbtn_AP.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_selete_imgbtn_AP.set_pos(25, 27)
screen_selete_imgbtn_AP.set_size(77, 59)
# Set style for screen_selete_imgbtn_AP, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_selete_imgbtn_AP.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_selete_imgbtn_AP.set_style_text_font(test_font("ZiTiQuanWeiJunHeiW22", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_selete_imgbtn_AP.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_selete_imgbtn_AP.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_selete_imgbtn_AP.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_selete_imgbtn_AP, Part: lv.PART.MAIN, State: lv.STATE.PRESSED.
screen_selete_imgbtn_AP.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.PRESSED)
screen_selete_imgbtn_AP.set_style_text_color(lv.color_hex(0xFF33FF), lv.PART.MAIN|lv.STATE.PRESSED)
screen_selete_imgbtn_AP.set_style_text_font(test_font("ZiTiQuanWeiJunHeiW22", 12), lv.PART.MAIN|lv.STATE.PRESSED)
screen_selete_imgbtn_AP.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.PRESSED)
screen_selete_imgbtn_AP.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.PRESSED)
# Set style for screen_selete_imgbtn_AP, Part: lv.PART.MAIN, State: lv.STATE.CHECKED.
screen_selete_imgbtn_AP.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.CHECKED)
screen_selete_imgbtn_AP.set_style_text_color(lv.color_hex(0xFF33FF), lv.PART.MAIN|lv.STATE.CHECKED)
screen_selete_imgbtn_AP.set_style_text_font(test_font("ZiTiQuanWeiJunHeiW22", 12), lv.PART.MAIN|lv.STATE.CHECKED)
screen_selete_imgbtn_AP.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.CHECKED)
screen_selete_imgbtn_AP.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.CHECKED)
# Set style for screen_selete_imgbtn_AP, Part: lv.PART.MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
screen_selete_imgbtn_AP.set_style_image_opa(255, lv.PART.MAIN|lv.imagebutton.STATE.RELEASED)

# Create screen_selete_imgbtn_weather
screen_selete_imgbtn_weather = lv.imagebutton(screen_selete)
screen_selete_imgbtn_weather.add_flag(lv.obj.FLAG.CHECKABLE)
screen_selete_imgbtn_weather.set_src(lv.imagebutton.STATE.RELEASED, load_image(r"D:\NXP\GUider_Project\smart_watch\generated\MicroPython\weather_77_59.png"), None, None)
screen_selete_imgbtn_weather.set_src(lv.imagebutton.STATE.PRESSED, load_image(r"D:\NXP\GUider_Project\smart_watch\generated\MicroPython\wether_press_77_59.png"), None, None)
screen_selete_imgbtn_weather.set_src(lv.imagebutton.STATE.CHECKED_RELEASED, load_image(r"D:\NXP\GUider_Project\smart_watch\generated\MicroPython\weather_77_59.png"), None, None)
screen_selete_imgbtn_weather.set_src(lv.imagebutton.STATE.CHECKED_PRESSED, load_image(r"D:\NXP\GUider_Project\smart_watch\generated\MicroPython\wether_press_77_59.png"), None, None)
screen_selete_imgbtn_weather.add_flag(lv.obj.FLAG.CHECKABLE)
screen_selete_imgbtn_weather_label = lv.label(screen_selete_imgbtn_weather)
screen_selete_imgbtn_weather_label.set_text("")
screen_selete_imgbtn_weather_label.set_long_mode(lv.label.LONG.WRAP)
screen_selete_imgbtn_weather_label.set_width(lv.pct(100))
screen_selete_imgbtn_weather_label.align(lv.ALIGN.CENTER, 0, 0)
screen_selete_imgbtn_weather.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_selete_imgbtn_weather.set_pos(136, 27)
screen_selete_imgbtn_weather.set_size(77, 59)
# Set style for screen_selete_imgbtn_weather, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_selete_imgbtn_weather.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_selete_imgbtn_weather.set_style_text_font(test_font("ZiTiQuanWeiJunHeiW22", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_selete_imgbtn_weather.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_selete_imgbtn_weather.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_selete_imgbtn_weather.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_selete_imgbtn_weather, Part: lv.PART.MAIN, State: lv.STATE.PRESSED.
screen_selete_imgbtn_weather.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.PRESSED)
screen_selete_imgbtn_weather.set_style_text_color(lv.color_hex(0xFF33FF), lv.PART.MAIN|lv.STATE.PRESSED)
screen_selete_imgbtn_weather.set_style_text_font(test_font("ZiTiQuanWeiJunHeiW22", 12), lv.PART.MAIN|lv.STATE.PRESSED)
screen_selete_imgbtn_weather.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.PRESSED)
screen_selete_imgbtn_weather.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.PRESSED)
# Set style for screen_selete_imgbtn_weather, Part: lv.PART.MAIN, State: lv.STATE.CHECKED.
screen_selete_imgbtn_weather.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.CHECKED)
screen_selete_imgbtn_weather.set_style_text_color(lv.color_hex(0xFF33FF), lv.PART.MAIN|lv.STATE.CHECKED)
screen_selete_imgbtn_weather.set_style_text_font(test_font("ZiTiQuanWeiJunHeiW22", 12), lv.PART.MAIN|lv.STATE.CHECKED)
screen_selete_imgbtn_weather.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.CHECKED)
screen_selete_imgbtn_weather.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.CHECKED)
# Set style for screen_selete_imgbtn_weather, Part: lv.PART.MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
screen_selete_imgbtn_weather.set_style_image_opa(255, lv.PART.MAIN|lv.imagebutton.STATE.RELEASED)

# Create screen_selete_imgbtn_rli
screen_selete_imgbtn_rli = lv.imagebutton(screen_selete)
screen_selete_imgbtn_rli.add_flag(lv.obj.FLAG.CHECKABLE)
screen_selete_imgbtn_rli.set_src(lv.imagebutton.STATE.RELEASED, load_image(r"D:\NXP\GUider_Project\smart_watch\generated\MicroPython\rli_77_55.png"), None, None)
screen_selete_imgbtn_rli.set_src(lv.imagebutton.STATE.PRESSED, load_image(r"D:\NXP\GUider_Project\smart_watch\generated\MicroPython\rli_press_77_55.png"), None, None)
screen_selete_imgbtn_rli.set_src(lv.imagebutton.STATE.CHECKED_RELEASED, load_image(r"D:\NXP\GUider_Project\smart_watch\generated\MicroPython\rli_77_55.png"), None, None)
screen_selete_imgbtn_rli.set_src(lv.imagebutton.STATE.CHECKED_PRESSED, load_image(r"D:\NXP\GUider_Project\smart_watch\generated\MicroPython\rli_press_77_55.png"), None, None)
screen_selete_imgbtn_rli.add_flag(lv.obj.FLAG.CHECKABLE)
screen_selete_imgbtn_rli_label = lv.label(screen_selete_imgbtn_rli)
screen_selete_imgbtn_rli_label.set_text("")
screen_selete_imgbtn_rli_label.set_long_mode(lv.label.LONG.WRAP)
screen_selete_imgbtn_rli_label.set_width(lv.pct(100))
screen_selete_imgbtn_rli_label.align(lv.ALIGN.CENTER, 0, 0)
screen_selete_imgbtn_rli.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_selete_imgbtn_rli.set_pos(25, 112)
screen_selete_imgbtn_rli.set_size(77, 55)
# Set style for screen_selete_imgbtn_rli, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_selete_imgbtn_rli.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_selete_imgbtn_rli.set_style_text_font(test_font("ZiTiQuanWeiJunHeiW22", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_selete_imgbtn_rli.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_selete_imgbtn_rli.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_selete_imgbtn_rli.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_selete_imgbtn_rli, Part: lv.PART.MAIN, State: lv.STATE.PRESSED.
screen_selete_imgbtn_rli.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.PRESSED)
screen_selete_imgbtn_rli.set_style_text_color(lv.color_hex(0xFF33FF), lv.PART.MAIN|lv.STATE.PRESSED)
screen_selete_imgbtn_rli.set_style_text_font(test_font("ZiTiQuanWeiJunHeiW22", 12), lv.PART.MAIN|lv.STATE.PRESSED)
screen_selete_imgbtn_rli.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.PRESSED)
screen_selete_imgbtn_rli.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.PRESSED)
# Set style for screen_selete_imgbtn_rli, Part: lv.PART.MAIN, State: lv.STATE.CHECKED.
screen_selete_imgbtn_rli.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.CHECKED)
screen_selete_imgbtn_rli.set_style_text_color(lv.color_hex(0xFF33FF), lv.PART.MAIN|lv.STATE.CHECKED)
screen_selete_imgbtn_rli.set_style_text_font(test_font("ZiTiQuanWeiJunHeiW22", 12), lv.PART.MAIN|lv.STATE.CHECKED)
screen_selete_imgbtn_rli.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.CHECKED)
screen_selete_imgbtn_rli.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.CHECKED)
# Set style for screen_selete_imgbtn_rli, Part: lv.PART.MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
screen_selete_imgbtn_rli.set_style_image_opa(255, lv.PART.MAIN|lv.imagebutton.STATE.RELEASED)

# Create screen_selete_imgbtn_AI
screen_selete_imgbtn_AI = lv.imagebutton(screen_selete)
screen_selete_imgbtn_AI.add_flag(lv.obj.FLAG.CHECKABLE)
screen_selete_imgbtn_AI.set_src(lv.imagebutton.STATE.RELEASED, load_image(r"D:\NXP\GUider_Project\smart_watch\generated\MicroPython\AI_77_55.png"), None, None)
screen_selete_imgbtn_AI.set_src(lv.imagebutton.STATE.PRESSED, load_image(r"D:\NXP\GUider_Project\smart_watch\generated\MicroPython\AI_Press_77_55.png"), None, None)
screen_selete_imgbtn_AI.set_src(lv.imagebutton.STATE.CHECKED_RELEASED, load_image(r"D:\NXP\GUider_Project\smart_watch\generated\MicroPython\AI_77_55.png"), None, None)
screen_selete_imgbtn_AI.set_src(lv.imagebutton.STATE.CHECKED_PRESSED, load_image(r"D:\NXP\GUider_Project\smart_watch\generated\MicroPython\AI_Press_77_55.png"), None, None)
screen_selete_imgbtn_AI.add_flag(lv.obj.FLAG.CHECKABLE)
screen_selete_imgbtn_AI_label = lv.label(screen_selete_imgbtn_AI)
screen_selete_imgbtn_AI_label.set_text("")
screen_selete_imgbtn_AI_label.set_long_mode(lv.label.LONG.WRAP)
screen_selete_imgbtn_AI_label.set_width(lv.pct(100))
screen_selete_imgbtn_AI_label.align(lv.ALIGN.CENTER, 0, 0)
screen_selete_imgbtn_AI.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_selete_imgbtn_AI.set_pos(136, 112)
screen_selete_imgbtn_AI.set_size(77, 55)
# Set style for screen_selete_imgbtn_AI, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_selete_imgbtn_AI.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_selete_imgbtn_AI.set_style_text_font(test_font("ZiTiQuanWeiJunHeiW22", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_selete_imgbtn_AI.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_selete_imgbtn_AI.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_selete_imgbtn_AI.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_selete_imgbtn_AI, Part: lv.PART.MAIN, State: lv.STATE.PRESSED.
screen_selete_imgbtn_AI.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.PRESSED)
screen_selete_imgbtn_AI.set_style_text_color(lv.color_hex(0xFF33FF), lv.PART.MAIN|lv.STATE.PRESSED)
screen_selete_imgbtn_AI.set_style_text_font(test_font("ZiTiQuanWeiJunHeiW22", 12), lv.PART.MAIN|lv.STATE.PRESSED)
screen_selete_imgbtn_AI.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.PRESSED)
screen_selete_imgbtn_AI.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.PRESSED)
# Set style for screen_selete_imgbtn_AI, Part: lv.PART.MAIN, State: lv.STATE.CHECKED.
screen_selete_imgbtn_AI.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.CHECKED)
screen_selete_imgbtn_AI.set_style_text_color(lv.color_hex(0xFF33FF), lv.PART.MAIN|lv.STATE.CHECKED)
screen_selete_imgbtn_AI.set_style_text_font(test_font("ZiTiQuanWeiJunHeiW22", 12), lv.PART.MAIN|lv.STATE.CHECKED)
screen_selete_imgbtn_AI.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.CHECKED)
screen_selete_imgbtn_AI.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.CHECKED)
# Set style for screen_selete_imgbtn_AI, Part: lv.PART.MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
screen_selete_imgbtn_AI.set_style_image_opa(255, lv.PART.MAIN|lv.imagebutton.STATE.RELEASED)

screen_selete.update_layout()
# Create screen_Rli
screen_Rli = lv.obj()
g_kb_screen_Rli = lv.keyboard(screen_Rli)
g_kb_screen_Rli.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_Rli), lv.EVENT.ALL, None)
g_kb_screen_Rli.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_Rli.set_style_text_font(test_font("SourceHanSerifSC_Regular", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_Rli.set_size(240, 284)
screen_Rli.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_Rli, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_Rli.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_Rli.set_style_bg_color(lv.color_hex(0x020202), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_Rli.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_Rli_win_1
screen_Rli_win_1 = lv.win(screen_Rli)
screen_Rli_win_1.add_title("title")
screen_Rli_win_1_header = screen_Rli_win_1.get_header()
screen_Rli_win_1_header.set_height(40)
screen_Rli_win_1_item0 = screen_Rli_win_1.add_button(lv.SYMBOL.CLOSE, 40)
screen_Rli_win_1_label = lv.label(screen_Rli_win_1.get_content())
screen_Rli_win_1.get_content().set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_Rli_win_1_label.set_text("this is a \nlong text \nto show \nscrollbar. \nif \nit \nis not \nlong enough, \nadd more content")
screen_Rli_win_1.set_pos(-685, 346)
screen_Rli_win_1.set_size(244, 285)
screen_Rli_win_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_Rli_win_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_Rli_win_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_Rli_win_1.set_style_bg_color(lv.color_hex(0xeeeef6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_Rli_win_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_Rli_win_1.set_style_outline_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_Rli_win_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_Rli_win_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
style_screen_Rli_win_1_extra_content_main_default = lv.style_t()
style_screen_Rli_win_1_extra_content_main_default.init()
style_screen_Rli_win_1_extra_content_main_default.set_bg_opa(255)
style_screen_Rli_win_1_extra_content_main_default.set_bg_color(lv.color_hex(0xeeeef6))
style_screen_Rli_win_1_extra_content_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_Rli_win_1_extra_content_main_default.set_text_color(lv.color_hex(0x393c41))
style_screen_Rli_win_1_extra_content_main_default.set_text_font(test_font("ZiTiQuanWeiJunHeiW22", 12))
style_screen_Rli_win_1_extra_content_main_default.set_text_opa(255)
style_screen_Rli_win_1_extra_content_main_default.set_text_letter_space(0)
style_screen_Rli_win_1_extra_content_main_default.set_text_line_space(2)
screen_Rli_win_1.get_content().add_style(style_screen_Rli_win_1_extra_content_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_Rli_win_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
style_screen_Rli_win_1_extra_header_main_default = lv.style_t()
style_screen_Rli_win_1_extra_header_main_default.init()
style_screen_Rli_win_1_extra_header_main_default.set_bg_opa(255)
style_screen_Rli_win_1_extra_header_main_default.set_bg_color(lv.color_hex(0xe6e6e6))
style_screen_Rli_win_1_extra_header_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_Rli_win_1_extra_header_main_default.set_text_color(lv.color_hex(0x393c41))
style_screen_Rli_win_1_extra_header_main_default.set_text_font(test_font("ZiTiQuanWeiJunHeiW22", 12))
style_screen_Rli_win_1_extra_header_main_default.set_text_opa(255)
style_screen_Rli_win_1_extra_header_main_default.set_text_letter_space(0)
style_screen_Rli_win_1_extra_header_main_default.set_text_line_space(2)
style_screen_Rli_win_1_extra_header_main_default.set_pad_top(5)
style_screen_Rli_win_1_extra_header_main_default.set_pad_right(5)
style_screen_Rli_win_1_extra_header_main_default.set_pad_bottom(5)
style_screen_Rli_win_1_extra_header_main_default.set_pad_left(5)
style_screen_Rli_win_1_extra_header_main_default.set_pad_column(5)
screen_Rli_win_1.get_header().add_style(style_screen_Rli_win_1_extra_header_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_Rli_win_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
style_screen_Rli_win_1_extra_btns_main_default = lv.style_t()
style_screen_Rli_win_1_extra_btns_main_default.init()
style_screen_Rli_win_1_extra_btns_main_default.set_radius(8)
style_screen_Rli_win_1_extra_btns_main_default.set_border_width(0)
style_screen_Rli_win_1_extra_btns_main_default.set_bg_opa(255)
style_screen_Rli_win_1_extra_btns_main_default.set_bg_color(lv.color_hex(0x2195f6))
style_screen_Rli_win_1_extra_btns_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_Rli_win_1_extra_btns_main_default.set_shadow_width(0)
screen_Rli_win_1_item0.add_style(style_screen_Rli_win_1_extra_btns_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_Rli_calendar_1
screen_Rli_calendar_1 = lv.calendar(screen_Rli)
screen_Rli_calendar_1.set_today_date(time.localtime()[0], time.localtime()[1], time.localtime()[2])
screen_Rli_calendar_1.set_showed_date(time.localtime()[0], time.localtime()[1])
screen_Rli_calendar_1_highlighted_days=[
lv.calendar_date_t({'year':2026, 'month':1, 'day':30})
]
screen_Rli_calendar_1.set_highlighted_dates(screen_Rli_calendar_1_highlighted_days, len(screen_Rli_calendar_1_highlighted_days))
screen_Rli_calendar_1_header = lv.calendar_header_arrow(screen_Rli_calendar_1)
screen_Rli_calendar_1.add_event_cb(lambda e: calendar_event_handler(e,screen_Rli_calendar_1), lv.EVENT.ALL, None)
screen_Rli_calendar_1.set_pos(0, 0)
screen_Rli_calendar_1.set_size(241, 237)
# Set style for screen_Rli_calendar_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_Rli_calendar_1.set_style_border_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_Rli_calendar_1.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_Rli_calendar_1.set_style_border_color(lv.color_hex(0xc0c0c0), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_Rli_calendar_1.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_Rli_calendar_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_Rli_calendar_1.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_Rli_calendar_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_Rli_calendar_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_Rli_calendar_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_Rli_calendar_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
style_screen_Rli_calendar_1_extra_header_main_default = lv.style_t()
style_screen_Rli_calendar_1_extra_header_main_default.init()
style_screen_Rli_calendar_1_extra_header_main_default.set_text_color(lv.color_hex(0xffffff))
style_screen_Rli_calendar_1_extra_header_main_default.set_text_font(test_font("ZiTiQuanWeiJunHeiW22", 12))
style_screen_Rli_calendar_1_extra_header_main_default.set_text_opa(255)
style_screen_Rli_calendar_1_extra_header_main_default.set_bg_opa(255)
style_screen_Rli_calendar_1_extra_header_main_default.set_bg_color(lv.color_hex(0x2195f6))
style_screen_Rli_calendar_1_extra_header_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
screen_Rli_calendar_1_header.add_style(style_screen_Rli_calendar_1_extra_header_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_Rli_calendar_1, Part: lv.PART.ITEMS, State: lv.STATE.DEFAULT.
screen_Rli_calendar_1.get_btnmatrix().set_style_bg_opa(255, lv.PART.ITEMS|lv.STATE.DEFAULT)
screen_Rli_calendar_1.get_btnmatrix().set_style_bg_color(lv.color_hex(0xffffff), lv.PART.ITEMS|lv.STATE.DEFAULT)
screen_Rli_calendar_1.get_btnmatrix().set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.ITEMS|lv.STATE.DEFAULT)
screen_Rli_calendar_1.get_btnmatrix().set_style_border_width(1, lv.PART.ITEMS|lv.STATE.DEFAULT)
screen_Rli_calendar_1.get_btnmatrix().set_style_border_opa(255, lv.PART.ITEMS|lv.STATE.DEFAULT)
screen_Rli_calendar_1.get_btnmatrix().set_style_border_color(lv.color_hex(0xc0c0c0), lv.PART.ITEMS|lv.STATE.DEFAULT)
screen_Rli_calendar_1.get_btnmatrix().set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.ITEMS|lv.STATE.DEFAULT)
screen_Rli_calendar_1.get_btnmatrix().set_style_text_color(lv.color_hex(0x0D3055), lv.PART.ITEMS|lv.STATE.DEFAULT)
screen_Rli_calendar_1.get_btnmatrix().set_style_text_font(test_font("ZiTiQuanWeiJunHeiW22", 12), lv.PART.ITEMS|lv.STATE.DEFAULT)
screen_Rli_calendar_1.get_btnmatrix().set_style_text_opa(255, lv.PART.ITEMS|lv.STATE.DEFAULT)

def screen_Rli_calendar_1_extra_ctrl_day_names_draw_event_cb(e):
    obj = lv.buttonmatrix.__cast__(e.get_target())
    dsc = lv.draw_task_t.__cast__(e.get_param())
    base_dsc = lv.draw_dsc_base_t.__cast__(dsc.draw_dsc)
    label_dsc = dsc.get_label_dsc()
    fill_dsc = dsc.get_fill_dsc()
    border_dsc = dsc.get_border_dsc()
    if base_dsc.id1 < 7:
        if label_dsc: label_dsc.color = lv.color_hex(0x0D3055)
        if label_dsc: label_dsc.font = test_font("ZiTiQuanWeiJunHeiW22", 12)

screen_Rli_calendar_1.get_btnmatrix().add_event_cb(screen_Rli_calendar_1_extra_ctrl_day_names_draw_event_cb, lv.EVENT.DRAW_TASK_ADDED, None)

def screen_Rli_calendar_1_extra_ctrl_highlight_draw_event_cb(e):
    obj = lv.buttonmatrix.__cast__(e.get_target())
    dsc = lv.draw_task_t.__cast__(e.get_param())
    base_dsc = lv.draw_dsc_base_t.__cast__(dsc.draw_dsc)
    label_dsc = dsc.get_label_dsc()
    fill_dsc = dsc.get_fill_dsc()
    border_dsc = dsc.get_border_dsc()
    if base_dsc.id1 >= 7 and obj.has_button_ctrl(base_dsc.id1, lv.buttonmatrix.CTRL.CUSTOM_2):
        if label_dsc: label_dsc.color = lv.color_hex(0x0D3055)
        if label_dsc: label_dsc.font = test_font("ZiTiQuanWeiJunHeiW22", 12)
        if fill_dsc: fill_dsc.opa = 255
        if fill_dsc: fill_dsc.color = lv.color_hex(0x2195f6)

screen_Rli_calendar_1.get_btnmatrix().add_event_cb(screen_Rli_calendar_1_extra_ctrl_highlight_draw_event_cb, lv.EVENT.DRAW_TASK_ADDED, None)

def screen_Rli_calendar_1_extra_ctrl_today_draw_event_cb(e):
    obj = lv.buttonmatrix.__cast__(e.get_target())
    dsc = lv.draw_task_t.__cast__(e.get_param())
    base_dsc = lv.draw_dsc_base_t.__cast__(dsc.draw_dsc)
    label_dsc = dsc.get_label_dsc()
    fill_dsc = dsc.get_fill_dsc()
    border_dsc = dsc.get_border_dsc()
    if base_dsc.id1 >= 7 and obj.has_button_ctrl(base_dsc.id1, lv.buttonmatrix.CTRL.CUSTOM_1):
        if label_dsc: label_dsc.color = lv.color_hex(0x0D3055)
        if label_dsc: label_dsc.font = test_font("ZiTiQuanWeiJunHeiW22", 12)
        if fill_dsc: fill_dsc.opa = 255
        if fill_dsc: fill_dsc.color = lv.color_hex(0x01a2b1)
        if border_dsc: border_dsc.width = 1
        if border_dsc: border_dsc.color = lv.color_hex(0xc0c0c0)
        if border_dsc: border_dsc.opa = 255

screen_Rli_calendar_1.get_btnmatrix().add_event_cb(screen_Rli_calendar_1_extra_ctrl_today_draw_event_cb, lv.EVENT.DRAW_TASK_ADDED, None)

def screen_Rli_calendar_1_extra_ctrl_other_month_draw_event_cb(e):
    obj = lv.buttonmatrix.__cast__(e.get_target())
    dsc = lv.draw_task_t.__cast__(e.get_param())
    base_dsc = lv.draw_dsc_base_t.__cast__(dsc.draw_dsc)
    label_dsc = dsc.get_label_dsc()
    fill_dsc = dsc.get_fill_dsc()
    border_dsc = dsc.get_border_dsc()
    if base_dsc.id1 >= 7 and obj.has_button_ctrl(base_dsc.id1, lv.buttonmatrix.CTRL.DISABLED):
        if label_dsc: label_dsc.color = lv.color_hex(0xA9A2A2)
        if label_dsc: label_dsc.font = test_font("ZiTiQuanWeiJunHeiW22", 12)
        if fill_dsc: fill_dsc.opa = 255
        if fill_dsc: fill_dsc.color = lv.color_hex(0xffffff)

screen_Rli_calendar_1.get_btnmatrix().add_event_cb(screen_Rli_calendar_1_extra_ctrl_other_month_draw_event_cb, lv.EVENT.DRAW_TASK_ADDED, None)

# Create screen_Rli_btn_return
screen_Rli_btn_return = lv.button(screen_Rli)
screen_Rli_btn_return_label = lv.label(screen_Rli_btn_return)
screen_Rli_btn_return_label.set_text("返回")
screen_Rli_btn_return_label.set_long_mode(lv.label.LONG.WRAP)
screen_Rli_btn_return_label.set_width(lv.pct(100))
screen_Rli_btn_return_label.align(lv.ALIGN.CENTER, 0, 0)
screen_Rli_btn_return.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_Rli_btn_return.set_pos(74, 248)
screen_Rli_btn_return.set_size(76, 29)
# Set style for screen_Rli_btn_return, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_Rli_btn_return.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_Rli_btn_return.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_Rli_btn_return.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_Rli_btn_return.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_Rli_btn_return.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_Rli_btn_return.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_Rli_btn_return.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_Rli_btn_return.set_style_text_font(test_font("ZiTiQuanWeiJunHeiW22", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_Rli_btn_return.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_Rli_btn_return.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_Rli.update_layout()
# Create screen_AI
screen_AI = lv.obj()
g_kb_screen_AI = lv.keyboard(screen_AI)
g_kb_screen_AI.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_AI), lv.EVENT.ALL, None)
g_kb_screen_AI.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_AI.set_style_text_font(test_font("SourceHanSerifSC_Regular", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI.set_size(240, 284)
screen_AI.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_AI, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_AI.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI.set_style_bg_color(lv.color_hex(0xebebeb), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_AI_btn_1
screen_AI_btn_1 = lv.button(screen_AI)
screen_AI_btn_1_label = lv.label(screen_AI_btn_1)
screen_AI_btn_1_label.set_text("按下说话")
screen_AI_btn_1_label.set_long_mode(lv.label.LONG.WRAP)
screen_AI_btn_1_label.set_width(lv.pct(100))
screen_AI_btn_1_label.align(lv.ALIGN.CENTER, 0, 0)
screen_AI_btn_1.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_AI_btn_1.set_pos(5, 221)
screen_AI_btn_1.set_size(231, 50)
# Set style for screen_AI_btn_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_AI_btn_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_btn_1.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_btn_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_btn_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_btn_1.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_btn_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_btn_1.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_btn_1.set_style_text_font(test_font("ZiTiQuanWeiJunHeiW22", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_btn_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_btn_1.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_AI_label_1
screen_AI_label_1 = lv.label(screen_AI)
screen_AI_label_1.set_text("")
screen_AI_label_1.set_long_mode(lv.label.LONG.WRAP)
screen_AI_label_1.set_width(lv.pct(100))
screen_AI_label_1.set_pos(66, 39)
screen_AI_label_1.set_size(155, 41)
# Set style for screen_AI_label_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_AI_label_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_1.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_1.set_style_text_font(test_font("ZiTiQuanWeiJunHeiW22", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_1.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_1.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_1.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_1.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_AI_label_2
screen_AI_label_2 = lv.label(screen_AI)
screen_AI_label_2.set_text("")
screen_AI_label_2.set_long_mode(lv.label.LONG.WRAP)
screen_AI_label_2.set_width(lv.pct(100))
screen_AI_label_2.set_pos(19, 167)
screen_AI_label_2.set_size(150, 38)
# Set style for screen_AI_label_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_AI_label_2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_2.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_2.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_2.set_style_text_font(test_font("ZiTiQuanWeiJunHeiW22", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_2.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_2.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_2.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_2.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_2.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_2.set_style_bg_color(lv.color_hex(0x1bd73b), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_2.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_2.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_2.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_2.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_2.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_AI_img_1
screen_AI_img_1 = lv.image(screen_AI)
screen_AI_img_1.set_src(load_image(r"D:\NXP\GUider_Project\smart_watch\generated\MicroPython\AI_54_46.png"))
screen_AI_img_1.add_flag(lv.obj.FLAG.CLICKABLE)
screen_AI_img_1.set_pivot(50,50)
screen_AI_img_1.set_rotation(0)
screen_AI_img_1.set_pos(5, 39)
screen_AI_img_1.set_size(54, 46)
# Set style for screen_AI_img_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_AI_img_1.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_AI_img_2
screen_AI_img_2 = lv.image(screen_AI)
screen_AI_img_2.set_src(load_image(r"D:\NXP\GUider_Project\smart_watch\generated\MicroPython\User_54_46.png"))
screen_AI_img_2.add_flag(lv.obj.FLAG.CLICKABLE)
screen_AI_img_2.set_pivot(50,50)
screen_AI_img_2.set_rotation(0)
screen_AI_img_2.set_pos(179, 163)
screen_AI_img_2.set_size(54, 46)
# Set style for screen_AI_img_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_AI_img_2.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_AI_label_3
screen_AI_label_3 = lv.label(screen_AI)
screen_AI_label_3.set_text("                AI聊天")
screen_AI_label_3.set_long_mode(lv.label.LONG.SCROLL_CIRCULAR)
screen_AI_label_3.set_width(lv.pct(100))
screen_AI_label_3.set_pos(27, 4)
screen_AI_label_3.set_size(211, 30)
# Set style for screen_AI_label_3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_AI_label_3.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_3.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_3.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_3.set_style_text_font(test_font("ZiTiQuanWeiJunHeiW22", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_3.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_3.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_3.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_3.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_3.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_3.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_3.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_3.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_3.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_3.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_3.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_label_3.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_AI_imgbtn_return
screen_AI_imgbtn_return = lv.imagebutton(screen_AI)
screen_AI_imgbtn_return.add_flag(lv.obj.FLAG.CHECKABLE)
screen_AI_imgbtn_return.set_src(lv.imagebutton.STATE.RELEASED, load_image(r"D:\NXP\GUider_Project\smart_watch\generated\MicroPython\return_27_30.png"), None, None)
screen_AI_imgbtn_return.set_src(lv.imagebutton.STATE.PRESSED, load_image(r"D:\NXP\GUider_Project\smart_watch\generated\MicroPython\return_press_27_30.png"), None, None)
screen_AI_imgbtn_return.set_src(lv.imagebutton.STATE.CHECKED_RELEASED, load_image(r"D:\NXP\GUider_Project\smart_watch\generated\MicroPython\return_27_30.png"), None, None)
screen_AI_imgbtn_return.set_src(lv.imagebutton.STATE.CHECKED_PRESSED, load_image(r"D:\NXP\GUider_Project\smart_watch\generated\MicroPython\return_press_27_30.png"), None, None)
screen_AI_imgbtn_return.add_flag(lv.obj.FLAG.CHECKABLE)
screen_AI_imgbtn_return_label = lv.label(screen_AI_imgbtn_return)
screen_AI_imgbtn_return_label.set_text("")
screen_AI_imgbtn_return_label.set_long_mode(lv.label.LONG.WRAP)
screen_AI_imgbtn_return_label.set_width(lv.pct(100))
screen_AI_imgbtn_return_label.align(lv.ALIGN.CENTER, 0, 0)
screen_AI_imgbtn_return.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_AI_imgbtn_return.set_pos(0, 4)
screen_AI_imgbtn_return.set_size(27, 30)
# Set style for screen_AI_imgbtn_return, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_AI_imgbtn_return.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_imgbtn_return.set_style_text_font(test_font("ZiTiQuanWeiJunHeiW22", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_imgbtn_return.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_imgbtn_return.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_AI_imgbtn_return.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_AI_imgbtn_return, Part: lv.PART.MAIN, State: lv.STATE.PRESSED.
screen_AI_imgbtn_return.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.PRESSED)
screen_AI_imgbtn_return.set_style_text_color(lv.color_hex(0xFF33FF), lv.PART.MAIN|lv.STATE.PRESSED)
screen_AI_imgbtn_return.set_style_text_font(test_font("ZiTiQuanWeiJunHeiW22", 12), lv.PART.MAIN|lv.STATE.PRESSED)
screen_AI_imgbtn_return.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.PRESSED)
screen_AI_imgbtn_return.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.PRESSED)
# Set style for screen_AI_imgbtn_return, Part: lv.PART.MAIN, State: lv.STATE.CHECKED.
screen_AI_imgbtn_return.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.CHECKED)
screen_AI_imgbtn_return.set_style_text_color(lv.color_hex(0xFF33FF), lv.PART.MAIN|lv.STATE.CHECKED)
screen_AI_imgbtn_return.set_style_text_font(test_font("ZiTiQuanWeiJunHeiW22", 12), lv.PART.MAIN|lv.STATE.CHECKED)
screen_AI_imgbtn_return.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.CHECKED)
screen_AI_imgbtn_return.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.CHECKED)
# Set style for screen_AI_imgbtn_return, Part: lv.PART.MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
screen_AI_imgbtn_return.set_style_image_opa(255, lv.PART.MAIN|lv.imagebutton.STATE.RELEASED)

screen_AI.update_layout()
# Create screen_wifi
screen_wifi = lv.obj()
g_kb_screen_wifi = lv.keyboard(screen_wifi)
g_kb_screen_wifi.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_wifi), lv.EVENT.ALL, None)
g_kb_screen_wifi.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_wifi.set_style_text_font(test_font("SourceHanSerifSC_Regular", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi.set_size(240, 284)
screen_wifi.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_wifi, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_wifi.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi.set_style_bg_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_wifi_list_1
screen_wifi_list_1 = lv.list(screen_wifi)
screen_wifi_list_1_item0 = screen_wifi_list_1.add_button(lv.SYMBOL.WIFI, "wifi")
screen_wifi_list_1_item1 = screen_wifi_list_1.add_button(lv.SYMBOL.WIFI, "wifi_1")
screen_wifi_list_1_item2 = screen_wifi_list_1.add_button(lv.SYMBOL.WIFI, "wifi_2")
screen_wifi_list_1_item3 = screen_wifi_list_1.add_button(lv.SYMBOL.WIFI, "wifi_3")
screen_wifi_list_1_item4 = screen_wifi_list_1.add_button(lv.SYMBOL.WIFI, "wifi_4")
screen_wifi_list_1.set_pos(2, 41)
screen_wifi_list_1.set_size(235, 133)
screen_wifi_list_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_wifi_list_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_wifi_list_1.set_style_pad_top(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_list_1.set_style_pad_left(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_list_1.set_style_pad_right(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_list_1.set_style_pad_bottom(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_list_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_list_1.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_list_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_list_1.set_style_border_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_list_1.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_list_1.set_style_border_color(lv.color_hex(0xe1e6ee), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_list_1.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_list_1.set_style_radius(3, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_list_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_wifi_list_1, Part: lv.PART.SCROLLBAR, State: lv.STATE.DEFAULT.
screen_wifi_list_1.set_style_radius(3, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
screen_wifi_list_1.set_style_bg_opa(255, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
screen_wifi_list_1.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
screen_wifi_list_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
# Set style for screen_wifi_list_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
style_screen_wifi_list_1_extra_btns_main_default = lv.style_t()
style_screen_wifi_list_1_extra_btns_main_default.init()
style_screen_wifi_list_1_extra_btns_main_default.set_pad_top(5)
style_screen_wifi_list_1_extra_btns_main_default.set_pad_left(5)
style_screen_wifi_list_1_extra_btns_main_default.set_pad_right(5)
style_screen_wifi_list_1_extra_btns_main_default.set_pad_bottom(5)
style_screen_wifi_list_1_extra_btns_main_default.set_border_width(0)
style_screen_wifi_list_1_extra_btns_main_default.set_text_color(lv.color_hex(0x0D3055))
style_screen_wifi_list_1_extra_btns_main_default.set_text_font(test_font("ZiTiQuanWeiJunHeiW22", 12))
style_screen_wifi_list_1_extra_btns_main_default.set_text_opa(255)
style_screen_wifi_list_1_extra_btns_main_default.set_radius(3)
style_screen_wifi_list_1_extra_btns_main_default.set_bg_opa(255)
style_screen_wifi_list_1_extra_btns_main_default.set_bg_color(lv.color_hex(0xffffff))
style_screen_wifi_list_1_extra_btns_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
screen_wifi_list_1_item4.add_style(style_screen_wifi_list_1_extra_btns_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_list_1_item3.add_style(style_screen_wifi_list_1_extra_btns_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_list_1_item2.add_style(style_screen_wifi_list_1_extra_btns_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_list_1_item1.add_style(style_screen_wifi_list_1_extra_btns_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_list_1_item0.add_style(style_screen_wifi_list_1_extra_btns_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_wifi_list_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
style_screen_wifi_list_1_extra_texts_main_default = lv.style_t()
style_screen_wifi_list_1_extra_texts_main_default.init()
style_screen_wifi_list_1_extra_texts_main_default.set_pad_top(5)
style_screen_wifi_list_1_extra_texts_main_default.set_pad_left(5)
style_screen_wifi_list_1_extra_texts_main_default.set_pad_right(5)
style_screen_wifi_list_1_extra_texts_main_default.set_pad_bottom(5)
style_screen_wifi_list_1_extra_texts_main_default.set_border_width(0)
style_screen_wifi_list_1_extra_texts_main_default.set_text_color(lv.color_hex(0x0D3055))
style_screen_wifi_list_1_extra_texts_main_default.set_text_font(test_font("ZiTiQuanWeiJunHeiW22", 12))
style_screen_wifi_list_1_extra_texts_main_default.set_text_opa(255)
style_screen_wifi_list_1_extra_texts_main_default.set_radius(3)
style_screen_wifi_list_1_extra_texts_main_default.set_transform_width(0)
style_screen_wifi_list_1_extra_texts_main_default.set_bg_opa(255)
style_screen_wifi_list_1_extra_texts_main_default.set_bg_color(lv.color_hex(0xffffff))
style_screen_wifi_list_1_extra_texts_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)

# Create screen_wifi_btn_1
screen_wifi_btn_1 = lv.button(screen_wifi)
screen_wifi_btn_1_label = lv.label(screen_wifi_btn_1)
screen_wifi_btn_1_label.set_text("扫描WiFi")
screen_wifi_btn_1_label.set_long_mode(lv.label.LONG.WRAP)
screen_wifi_btn_1_label.set_width(lv.pct(100))
screen_wifi_btn_1_label.align(lv.ALIGN.CENTER, 0, 0)
screen_wifi_btn_1.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_wifi_btn_1.set_pos(17, 219)
screen_wifi_btn_1.set_size(76, 29)
# Set style for screen_wifi_btn_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_wifi_btn_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_btn_1.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_btn_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_btn_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_btn_1.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_btn_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_btn_1.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_btn_1.set_style_text_font(test_font("ZiTiQuanWeiJunHeiW22", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_btn_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_btn_1.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_wifi_cont_1
screen_wifi_cont_1 = lv.obj(screen_wifi)
screen_wifi_cont_1.set_pos(152, 290)
screen_wifi_cont_1.set_size(236, 32)
screen_wifi_cont_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_wifi_cont_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_wifi_cont_1.set_style_border_width(2, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_cont_1.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_cont_1.set_style_border_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_cont_1.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_cont_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_cont_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_cont_1.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_cont_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_cont_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_cont_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_cont_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_cont_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_cont_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_wifi_digital_clock_1
screen_wifi_digital_clock_1_time = [int(11), int(25), int(50), ""]
screen_wifi_digital_clock_1 = lv.label(screen_wifi)
screen_wifi_digital_clock_1.set_text("11:25")
screen_wifi_digital_clock_1_timer = lv.timer_create_basic()
screen_wifi_digital_clock_1_timer.set_period(1000)
screen_wifi_digital_clock_1_timer.set_cb(lambda src: digital_clock_cb(screen_wifi_digital_clock_1_timer, screen_wifi_digital_clock_1, screen_wifi_digital_clock_1_time, False, False ))
screen_wifi_digital_clock_1.set_pos(152, 8)
screen_wifi_digital_clock_1.set_size(40, 29)
# Set style for screen_wifi_digital_clock_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_wifi_digital_clock_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_digital_clock_1.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_digital_clock_1.set_style_text_font(test_font("ZiTiQuanWeiJunHeiW22", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_digital_clock_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_digital_clock_1.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_digital_clock_1.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_digital_clock_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_digital_clock_1.set_style_bg_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_digital_clock_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_digital_clock_1.set_style_pad_top(7, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_digital_clock_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_digital_clock_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_digital_clock_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_digital_clock_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_wifi_img_wi_close
screen_wifi_img_wi_close = lv.image(screen_wifi)
screen_wifi_img_wi_close.set_src(load_image(r"D:\NXP\GUider_Project\smart_watch\generated\MicroPython\wifi_close_23_26.png"))
screen_wifi_img_wi_close.add_flag(lv.obj.FLAG.CLICKABLE)
screen_wifi_img_wi_close.set_pivot(50,50)
screen_wifi_img_wi_close.set_rotation(0)
screen_wifi_img_wi_close.set_pos(7, 8)
screen_wifi_img_wi_close.set_size(23, 26)
# Set style for screen_wifi_img_wi_close, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_wifi_img_wi_close.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_wifi_btn_return
screen_wifi_btn_return = lv.button(screen_wifi)
screen_wifi_btn_return_label = lv.label(screen_wifi_btn_return)
screen_wifi_btn_return_label.set_text("返回")
screen_wifi_btn_return_label.set_long_mode(lv.label.LONG.WRAP)
screen_wifi_btn_return_label.set_width(lv.pct(100))
screen_wifi_btn_return_label.align(lv.ALIGN.CENTER, 0, 0)
screen_wifi_btn_return.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_wifi_btn_return.set_pos(140, 219)
screen_wifi_btn_return.set_size(76, 29)
# Set style for screen_wifi_btn_return, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_wifi_btn_return.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_btn_return.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_btn_return.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_btn_return.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_btn_return.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_btn_return.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_btn_return.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_btn_return.set_style_text_font(test_font("ZiTiQuanWeiJunHeiW22", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_btn_return.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_btn_return.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_wifi.update_layout()
# Create screen_wifi_connect
screen_wifi_connect = lv.obj()
g_kb_screen_wifi_connect = lv.keyboard(screen_wifi_connect)
g_kb_screen_wifi_connect.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_wifi_connect), lv.EVENT.ALL, None)
g_kb_screen_wifi_connect.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_wifi_connect.set_style_text_font(test_font("SourceHanSerifSC_Regular", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect.set_size(240, 284)
screen_wifi_connect.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_wifi_connect, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_wifi_connect.set_style_bg_opa(248, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect.set_style_bg_color(lv.color_hex(0x030303), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_wifi_connect_ta_1
screen_wifi_connect_ta_1 = lv.textarea(screen_wifi_connect)
screen_wifi_connect_ta_1.set_text("Hello World")
screen_wifi_connect_ta_1.set_placeholder_text("")
screen_wifi_connect_ta_1.set_password_bullet("*")
screen_wifi_connect_ta_1.set_password_mode(True)
screen_wifi_connect_ta_1.set_one_line(False)
screen_wifi_connect_ta_1.set_accepted_chars("")
screen_wifi_connect_ta_1.set_max_length(32)
screen_wifi_connect_ta_1.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_wifi_connect), lv.EVENT.ALL, None)
screen_wifi_connect_ta_1.set_pos(19, 112)
screen_wifi_connect_ta_1.set_size(200, 60)
# Set style for screen_wifi_connect_ta_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_wifi_connect_ta_1.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect_ta_1.set_style_text_font(test_font("ZiTiQuanWeiJunHeiW22", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect_ta_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect_ta_1.set_style_text_letter_space(2, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect_ta_1.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect_ta_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect_ta_1.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect_ta_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect_ta_1.set_style_border_width(2, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect_ta_1.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect_ta_1.set_style_border_color(lv.color_hex(0xe6e6e6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect_ta_1.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect_ta_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect_ta_1.set_style_pad_top(4, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect_ta_1.set_style_pad_right(4, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect_ta_1.set_style_pad_left(4, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect_ta_1.set_style_radius(4, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_wifi_connect_ta_1, Part: lv.PART.SCROLLBAR, State: lv.STATE.DEFAULT.
screen_wifi_connect_ta_1.set_style_bg_opa(255, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
screen_wifi_connect_ta_1.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
screen_wifi_connect_ta_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
screen_wifi_connect_ta_1.set_style_radius(0, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)

# Create screen_wifi_connect_label_1
screen_wifi_connect_label_1 = lv.label(screen_wifi_connect)
screen_wifi_connect_label_1.set_text("连接到WiFi")
screen_wifi_connect_label_1.set_long_mode(lv.label.LONG.WRAP)
screen_wifi_connect_label_1.set_width(lv.pct(100))
screen_wifi_connect_label_1.set_pos(62, 69)
screen_wifi_connect_label_1.set_size(104, 19)
# Set style for screen_wifi_connect_label_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_wifi_connect_label_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect_label_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect_label_1.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect_label_1.set_style_text_font(test_font("ZiTiQuanWeiJunHeiW22", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect_label_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect_label_1.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect_label_1.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect_label_1.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect_label_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect_label_1.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect_label_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect_label_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect_label_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect_label_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect_label_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect_label_1.set_style_bg_image_src(load_image(r"D:\NXP\GUider_Project\smart_watch\generated\MicroPython\return_104_19.png"), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect_label_1.set_style_bg_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect_label_1.set_style_bg_image_recolor(lv.color_hex(0xfefefe), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect_label_1.set_style_bg_image_recolor_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect_label_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_wifi_connect_btn_set
screen_wifi_connect_btn_set = lv.button(screen_wifi_connect)
screen_wifi_connect_btn_set_label = lv.label(screen_wifi_connect_btn_set)
screen_wifi_connect_btn_set_label.set_text("确定")
screen_wifi_connect_btn_set_label.set_long_mode(lv.label.LONG.WRAP)
screen_wifi_connect_btn_set_label.set_width(lv.pct(100))
screen_wifi_connect_btn_set_label.align(lv.ALIGN.CENTER, 0, 0)
screen_wifi_connect_btn_set.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_wifi_connect_btn_set.set_pos(71, 191)
screen_wifi_connect_btn_set.set_size(77, 27)
# Set style for screen_wifi_connect_btn_set, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_wifi_connect_btn_set.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect_btn_set.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect_btn_set.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect_btn_set.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect_btn_set.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect_btn_set.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect_btn_set.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect_btn_set.set_style_text_font(test_font("ZiTiQuanWeiJunHeiW22", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect_btn_set.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wifi_connect_btn_set.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_wifi_connect.update_layout()
# Create screen_1
screen_1 = lv.obj()
g_kb_screen_1 = lv.keyboard(screen_1)
g_kb_screen_1.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_1), lv.EVENT.ALL, None)
g_kb_screen_1.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_1.set_style_text_font(test_font("SourceHanSerifSC_Regular", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_1.set_size(240, 284)
screen_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_1.update_layout()

def screen_home_event_handler(e):
    code = e.get_code()
    indev = lv.indev_active()
    gestureDir = lv.DIR.NONE
    if indev is not None: gestureDir = indev.get_gesture_dir()
    if (code == lv.EVENT.GESTURE and lv.DIR.LEFT == gestureDir):
        if indev is not None: indev.wait_release()
        pass
        lv.screen_load_anim(screen_selete, lv.SCR_LOAD_ANIM.MOVE_LEFT, 200, 200, False)
screen_home.add_event_cb(lambda e: screen_home_event_handler(e), lv.EVENT.ALL, None)

def screen_selete_event_handler(e):
    code = e.get_code()
    indev = lv.indev_active()
    gestureDir = lv.DIR.NONE
    if indev is not None: gestureDir = indev.get_gesture_dir()
    if (code == lv.EVENT.GESTURE and lv.DIR.RIGHT == gestureDir):
        if indev is not None: indev.wait_release()
        pass
        lv.screen_load_anim(screen_home, lv.SCR_LOAD_ANIM.MOVE_RIGHT, 400, 400, False)
screen_selete.add_event_cb(lambda e: screen_selete_event_handler(e), lv.EVENT.ALL, None)

def screen_selete_imgbtn_AP_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.screen_load_anim(screen_wifi, lv.SCR_LOAD_ANIM.NONE, 200, 200, False)
screen_selete_imgbtn_AP.add_event_cb(lambda e: screen_selete_imgbtn_AP_event_handler(e), lv.EVENT.ALL, None)

def screen_selete_imgbtn_rli_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.screen_load_anim(screen_Rli, lv.SCR_LOAD_ANIM.FADE_ON, 200, 200, False)
screen_selete_imgbtn_rli.add_event_cb(lambda e: screen_selete_imgbtn_rli_event_handler(e), lv.EVENT.ALL, None)

def screen_selete_imgbtn_AI_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.screen_load_anim(screen_AI, lv.SCR_LOAD_ANIM.FADE_ON, 200, 200, False)
screen_selete_imgbtn_AI.add_event_cb(lambda e: screen_selete_imgbtn_AI_event_handler(e), lv.EVENT.ALL, None)

def screen_Rli_btn_return_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.screen_load_anim(screen_selete, lv.SCR_LOAD_ANIM.FADE_ON, 200, 200, False)
screen_Rli_btn_return.add_event_cb(lambda e: screen_Rli_btn_return_event_handler(e), lv.EVENT.ALL, None)

def screen_AI_imgbtn_return_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.screen_load_anim(screen_selete, lv.SCR_LOAD_ANIM.FADE_ON, 200, 200, False)
screen_AI_imgbtn_return.add_event_cb(lambda e: screen_AI_imgbtn_return_event_handler(e), lv.EVENT.ALL, None)

def screen_wifi_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.screen_load_anim(screen_selete, lv.SCR_LOAD_ANIM.FADE_ON, 200, 200, False)
screen_wifi.add_event_cb(lambda e: screen_wifi_event_handler(e), lv.EVENT.ALL, None)

def screen_wifi_list_1_item1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.screen_load_anim(screen_wifi_connect, lv.SCR_LOAD_ANIM.FADE_ON, 200, 200, False)
screen_wifi_list_1_item1.add_event_cb(lambda e: screen_wifi_list_1_item1_event_handler(e), lv.EVENT.ALL, None)

def screen_wifi_list_1_item2_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.screen_load_anim(screen_wifi_connect, lv.SCR_LOAD_ANIM.FADE_ON, 200, 200, False)
screen_wifi_list_1_item2.add_event_cb(lambda e: screen_wifi_list_1_item2_event_handler(e), lv.EVENT.ALL, None)

def screen_wifi_list_1_item3_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.screen_load_anim(screen_wifi_connect, lv.SCR_LOAD_ANIM.FADE_ON, 200, 200, False)
screen_wifi_list_1_item3.add_event_cb(lambda e: screen_wifi_list_1_item3_event_handler(e), lv.EVENT.ALL, None)

def screen_wifi_list_1_item4_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.screen_load_anim(screen_wifi_connect, lv.SCR_LOAD_ANIM.FADE_ON, 200, 200, False)
screen_wifi_list_1_item4.add_event_cb(lambda e: screen_wifi_list_1_item4_event_handler(e), lv.EVENT.ALL, None)

def screen_wifi_list_1_item5_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.screen_load_anim(screen_wifi_connect, lv.SCR_LOAD_ANIM.FADE_ON, 200, 200, False)
screen_wifi_list_1_item5.add_event_cb(lambda e: screen_wifi_list_1_item5_event_handler(e), lv.EVENT.ALL, None)

def screen_wifi_connect_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.screen_load_anim(screen_wifi, lv.SCR_LOAD_ANIM.FADE_ON, 200, 200, False)
screen_wifi_connect.add_event_cb(lambda e: screen_wifi_connect_event_handler(e), lv.EVENT.ALL, None)

def screen_wifi_connect_btn_set_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.screen_load_anim(screen_wifi, lv.SCR_LOAD_ANIM.NONE, 200, 200, False)
screen_wifi_connect_btn_set.add_event_cb(lambda e: screen_wifi_connect_btn_set_event_handler(e), lv.EVENT.ALL, None)

# content from custom.py

# Load the default screen
lv.screen_load(screen_AI)

if __name__ == '__main__':
    while True:
        lv.task_handler()
        time.sleep_ms(5)
