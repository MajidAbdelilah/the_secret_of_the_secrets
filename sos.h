#include <gtk/gtk.h>


int count_untill_pipe(char *str);
gsize adjust_quranic_text(char **text, gsize len);
gchar *remove_space_new_line(char **text, gsize len);
void *escape_cequnce_block(void *data);
int ft_escap_space_strcmp(gchar *str1, gchar *str2);
void *find_word_x(void *data);
void *find_word_y(void *data);
gsize index_till_nl(gchar *str);
void activate_callback(GtkApplication *app, gpointer data_ptr);