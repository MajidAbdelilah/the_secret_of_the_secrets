#include <gtk/gtk.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>


int count_untill_pipe(char *str)
{
	int i = 0;
	int res = 0;
	while(str[i] && str[i] != '\n')
	{
		if(str[i] == '|')
			res = i;
		i++;
	}
	return res + 1;
}



gsize adjust_quranic_text(char **text, gsize len)
{
	gsize i = 0;
	while((*text)[i])
	{
		int skip_count = count_untill_pipe((*text) + i);
		// printf("skip = %d, i = %ld, len = %ld\n", skip_count, i, len);
		memmove((*text) + i, (*text) + i + skip_count, len - (i + skip_count));
		while((*text)[i] && (*text)[i] != '\n')
		{
			i++;
		}
		if(!(*text)[i])
			break;
		i++;
	}
	(*text)[i] = 0;
	return i;
}

gchar *remove_space_new_line(char **text, gsize len)
{
	gsize i = 0;
	gchar *result = calloc(len + 4, 1);
	gsize res_len = 0;
	gsize start = 0;
	while((*text)[i])
	{
		memcpy(result + res_len, (*text) + start, i - start);
		res_len += i - start;
		int j = 0;
		while((*text)[i + j] == ' ')
			j++;
		if(j > 0)
		{
			// memmove((*text) + i, (*text) + i + j, len - (i + j));
			i += j;
		}	
		j = 0;
		while((*text)[i + j] == '\n')
			j++;
		if(j > 0)
		{
			// memmove((*text) + i, (*text) + i + j, len - (i + j));
			i += j;
		}
		// printf("i = %ld\n", i);
		start = i;
		i++;
	}
	result[res_len] = 0;
	result = realloc(result, res_len + 4);
	return result;
}

gchar *escape_cequnce_block(char **text, gsize len, gsize escape, gsize block_size)
{
	gsize i = 0;
	escape *= 2;
	// block_size += 1;
	block_size *= 2;
	gchar *result = calloc(len * (2 + (escape == 0)), 1);
	gsize res_len = 0;
	
	// #pragma omp parallel for
	for(i = 0; i < len; i += 2)
	{
		{
			if( !(i % block_size) && i)
			{
				// #pragma omp critical
				{
					result[res_len++] = ' ';
					result[res_len++] = '\n';
				}
			}
			gsize j = 0;
			while(j < escape)
				j++;
			if((i + j) >= len)
				j = len - i;
			// memmove((*text) + i, (*text) + i + j, len - (i + j));
			i += j;
			// #pragma omp critical
			{
				result[res_len++] = (*text)[i];
				result[res_len++] = (*text)[i + 1];
			}
			char c[3] = {0};
			c[0] = result[res_len - 2];
			c[1] = result[res_len - 1];
			result[res_len++] = ' ';
			// printf("i = %ld, char = %s\n", i, c);
		}
	}
	result = realloc(result, res_len + 4);
	return result;
}

int ft_escap_space_strcmp(gchar *str1, gchar *str2)
{
	gsize i = 0;
	gsize j = 0;

	while(str1[i] == ' ')
			i++;
	while(str2[j] == ' ')
			j++;
	while(str2[j + 1] && str1[i] == str2[j])
	{
		i++;
		j++;
		while(str1[i] == ' ')
			i++;
		while(str2[j] == ' ')
			j++;
	}
	return str1[i] - str2[j];
}

bool find_word(char **text, gchar *word)
{
	gsize i = 0;
	gsize word_len = strlen(word);
	while((*text)[i + word_len])
	{
		if(ft_escap_space_strcmp((*text) + i, word) == 0)
		{
			return 1;
		}
		i++;
	}
	return 0;
}

void activate_callback(GtkApplication *app, gpointer data)
{
	(void)data;
	GtkWidget *window = NULL;
	GtkWidget *text_view = NULL;
	GtkTextBuffer *text_buffet = NULL;
	GtkWidget *scrool_window;
	gchar *text = NULL;
	GFile *quran_file = NULL;
	GError *file_error = NULL;
	gchar *content = NULL;
	gsize len = 0;

	window = gtk_window_new();
	gtk_application_add_window(app, GTK_WINDOW(window));
	gtk_window_set_title(GTK_WINDOW(window), "example-3");
	gtk_window_set_default_size(GTK_WINDOW(window), 1920, 1080);

	text_view = gtk_text_view_new();
	text_buffet = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
	gtk_text_buffer_set_text(text_buffet, text, -1);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_NONE);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(text_view), FALSE);
	gtk_text_view_set_monospace(GTK_TEXT_VIEW(text_view), TRUE);
	
	
	quran_file = g_file_new_for_path("./quran-simple-clean.txt");
	
	g_file_load_contents(quran_file, NULL, &content, &len, NULL, &file_error);
	  // In case of error, print a warning to the standard error output
  if (file_error != NULL)
    {
      g_printerr ("Unable to open “%s”: %s\n",
                  g_file_peek_path (quran_file),
                  file_error->message);
      return;
    }
	len = adjust_quranic_text(&content, len + 1);
	gchar *result1 = remove_space_new_line(&content, len + 1);
	printf("result1 = %s\n", result1);
	
	int ret = 0;
	gsize block_size = 1;
	gsize escape = 1;
	gchar *result2 = NULL;
	while(ret != 1)
	{
		result2 = escape_cequnce_block(&result1, strlen(result1) + 1, escape, block_size);
		printf("%s\n", result2);
		ret = find_word(&result2,  "ايلول");
		if( !ret )
		{
			block_size++;
			if(block_size == 1000)
			{
				block_size = 1;
				escape++;
				if(escape == 1000)
					exit(0) ;
			}
			free(result2);
		}
	}

	gtk_text_buffer_set_text(text_buffet, result2, -1);
	
	scrool_window =  gtk_scrolled_window_new();
	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrool_window), text_view);

	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrool_window), GTK_POLICY_ALWAYS, GTK_POLICY_ALWAYS);
	gtk_scrolled_window_set_max_content_width(GTK_SCROLLED_WINDOW(scrool_window), INT_MAX);

	gtk_window_set_child(GTK_WINDOW(window), scrool_window);

	gtk_window_present(GTK_WINDOW(window));
}

int main(int arg_num, char **args)
{
	GtkApplication *app;
	int status;
	app = gtk_application_new("org.gtk4.exapmles", G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect(app, "activate", G_CALLBACK(activate_callback), NULL);
	status = g_application_run(G_APPLICATION(app), arg_num, args);
	g_object_unref(app);
	return status;
}