/*
 * An indicator to display count of unread mails
 *
 * The command to find it:
 * $ mu find flag:attach | wc -l
 *
 */

#include <gtk/gtk.h>
#include <libappindicator/app-indicator.h>

gint
count_unread_mails ()
{
  gchar *stdout;
  gint status;

  if (g_spawn_command_line_sync ("mu find flag:unread", &stdout, NULL, &status, NULL) &&
      status == 0)
    {
      gint count = 0;
      char *p = stdout;
      for (; *p != '\0'; p++)
        if (*p == '\n') count++;
      g_free (stdout);
      return count;
    }
  else
    return 0;
}

gboolean
update_indicator_label (gpointer data)
{
  gint count;
  gchar *label;

  count = count_unread_mails ();
  label = g_strdup_printf ("%d", count);
  app_indicator_set_label (data, label, NULL);

  g_free (label);
  return TRUE;
}

void
activate (GtkApplication *app, gpointer data)
{
  AppIndicator *indicator;
  GtkWidget *menu, *item;

  indicator = app_indicator_new ("unread-mails-indicator",
                                 "mail-unread", /* FIXME: change */
                                 APP_INDICATOR_CATEGORY_APPLICATION_STATUS);
  menu = gtk_menu_new ();
  item = gtk_menu_item_new_with_label ("Quit");
  g_signal_connect_swapped (item, "activate", G_CALLBACK (g_application_quit), app);
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
  app_indicator_set_menu (indicator, GTK_MENU (menu));

  update_indicator_label (indicator);
  g_timeout_add_seconds (15, update_indicator_label, indicator);

  app_indicator_set_status (indicator, APP_INDICATOR_STATUS_ACTIVE);
  gtk_widget_show_all (menu);
  g_application_hold (G_APPLICATION (app));
}

int
main (int argc, char *argv[])
{
  GtkApplication *app;

  app = gtk_application_new ("com.github.xuchunyang.unread-mails-indicator",
                             G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  g_application_run (G_APPLICATION (app), argc, argv);

  g_object_unref (app);
  return 0;
}

/* Local Variables: */
/* compile-command: "gcc main.c `pkg-config --cflags --libs gtk+-3.0 appindicator3-0.1`" */
/* End: */
