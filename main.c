#include <gtk/gtk.h>
#include <time.h>

GtkWidget *countdownLabel;
GtkWidget *showTimeCheckbox;

void updateCountdown() {
    // 获取当前时间
    time_t currentTime = time(NULL);
    struct tm *currentDate = localtime(&currentTime);

    // 自动设置目标年份
    int targetYear = currentDate->tm_year + 1900;
    if (currentDate->tm_mon > 5 || (currentDate->tm_mon == 5 && currentDate->tm_mday > 25)) {
        targetYear++;
    }
    int targetMonth = 6;
    int targetDay = 25;
    int targetHour = 0;
    int targetMinute = 0;
    int targetSecond = 0;

    // 设置目标时间
    struct tm targetDate = {0};
    targetDate.tm_year = targetYear - 1900;
    targetDate.tm_mon = targetMonth - 1;
    targetDate.tm_mday = targetDay;
    targetDate.tm_hour = targetHour;
    targetDate.tm_min = targetMinute;
    targetDate.tm_sec = targetSecond;
    targetDate.tm_isdst = -1;

    // 计算时间差
    time_t targetTimeInSeconds = mktime(&targetDate);
    time_t timeDifference = targetTimeInSeconds - currentTime;

    // 转换为天、小时、分钟和秒
    int days, hours, minutes, seconds;
    if (timeDifference >= 0) {
        days = timeDifference / (60 * 60 * 24);
        hours = (timeDifference / (60 * 60)) % 24;
        minutes = (timeDifference / 60) % 60;
        seconds = timeDifference % 60;
    } else {
        days = hours = minutes = seconds = 0;
    }

    // 格式化倒计时文本
    char countdownText[100];
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(showTimeCheckbox))) {
        sprintf(countdownText, "%d天 %d小时 %d分钟 %d秒", days, hours, minutes, seconds);
    } else {
        sprintf(countdownText, "%d天", days+1);
    }

    // 更新倒计时标签
    gtk_label_set_text(GTK_LABEL(countdownLabel), countdownText);
}

gboolean updateTime(gpointer data) {
    updateCountdown();
    return G_SOURCE_CONTINUE;
}

int main(int argc, char *argv[]) {
    // 初始化GTK库
    gtk_init(&argc, &argv);

    // 创建窗口
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "倒计日");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // 隐藏标题栏的关闭按钮
    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
    GtkWidget *text = gtk_label_new("距离中考还有");

    // 创建倒计时标签
    countdownLabel = gtk_label_new(NULL);
    PangoFontDescription *fontDesc = pango_font_description_from_string("SimHei 48");
    gtk_widget_modify_font(countdownLabel, fontDesc);
    gtk_widget_modify_font(text, fontDesc);
    pango_font_description_free(fontDesc);
    gtk_label_set_xalign(GTK_LABEL(countdownLabel), 0.5);
    gtk_label_set_yalign(GTK_LABEL(countdownLabel), 0.5);
    gtk_widget_set_size_request(countdownLabel, 280, 120);

    // 创建显示具体时间的复选框
    showTimeCheckbox = gtk_check_button_new_with_label("显示具体时间");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(showTimeCheckbox), TRUE);

    // 更新倒计时
    updateCountdown();

    // 将倒计时标签和复选框添加到窗口
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(text), vbox);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    gtk_box_pack_start(GTK_BOX(vbox), text, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), countdownLabel, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), showTimeCheckbox, TRUE, TRUE, 0);

    // 使用CSS样式设置窗口透明
    GtkCssProvider *cssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(cssProvider,
        "window {"
        "   background-color: rgba(255, 255, 255, 0.5);"
        "  background-image: none;"
        "   border-width: 0;"
        "   box-shadow: none;"
        "   margin: 0;"
        "   padding: 0;"

        "}"
        "checkbutton {"
        "   margin-left: 20px;"
          
        "}",
        -1, NULL);
    GtkStyleContext *styleContext = gtk_widget_get_style_context(window);
    gtk_style_context_add_provider(styleContext,
        GTK_STYLE_PROVIDER(cssProvider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    // 更新时间
    g_timeout_add_seconds(1, updateTime, NULL);

    // 显示窗口并运行主循环
    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
