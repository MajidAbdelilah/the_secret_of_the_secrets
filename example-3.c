#include <gtk/gtk.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

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

typedef struct eb_data{
	char **text;
	gsize len;
	gsize escape;
	gsize block_size;
	gchar **output;
} t_eb_data;

void *escape_cequnce_block(void *data)
{
	t_eb_data d = *(t_eb_data*)data;
	gchar *text = *d.text;
	gsize i = 0;
	d.escape *= 2;
	// block_size += 1;
	d.block_size *= 2;
	gchar *result = calloc(d.len * (2 + (d.escape == 0)), 1);
	gsize res_len = 0;
	
	// #pragma omp parallel for
	for(i = 0; i < d.len; i += 2)
	{
		{
			if( !(i % d.block_size) && i)
			{
				// #pragma omp critical
				{
					//result[res_len++] = ' ';
					result[res_len++] = '\n';
				}
			}
			gsize j = 0;
			while(j < d.escape)
				j++;
			if((i + j) >= d.len)
				j = d.len - i;
			(text) += j;
			if(text[i] == 0)
				break;
			result[res_len++] = (text)[i];
			result[res_len++] = (text)[i + 1];
		}
	}
	result = realloc(result, res_len + 4);
	(*d.output) = result;
	return 0;
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

typedef struct fw_data
{
	char **text;
	gchar *word;
	bool *res;
	gsize text_len;
	gsize line_len;
	gsize *result_index_x;
	gsize *result_index_y;
}fw_data;

void *find_word_x(void *data)
{
	fw_data d = *(fw_data*)data;
	gsize i = 0;
	gsize word_len = strlen(d.word);
	gsize text_len = d.text_len;
	(*d.res) = 0;
	while((i + word_len - 1) < text_len && (*d.text)[i + word_len - 1])
	{
		// printf("%s, %s\n", (*d.text) + i, d.word);
		if(strncmp((*d.text) + i, d.word, word_len) == 0)
		{
			(*d.res) = 1;
			break ;
		}
		i++;
	}
	return 0;
}

void *find_word_y(void *data)
{
	fw_data d = *(fw_data*)data;

	gsize x = 0;
	gsize y = 0;
	gsize a = 0;
	gchar sub[100000] = {0};
	gsize word_len = strlen(d.word);
	while(((x * y) + word_len) < d.text_len)
	{
		y = 0;
		g_print("(d.text_len / d.line_len) = %ld\n", ((d.text_len) / d.line_len));
		while((y + word_len / 2) <= (d.text_len / d.line_len))
		{
			a = 0;
			while(a < word_len)
			{
				sub[a] = (*d.text)[(y + (a / 2)) * d.line_len + x];
				sub[a + 1] = (*d.text)[(y + (a / 2)) * d.line_len + x + 1];
				a += 2;
			}
			g_print("sub = %s, line_len = %ld\n", sub, d.line_len);
			if(strncmp(sub, d.word, word_len) == 0)
			{
				(*d.res) = 1;
				(*d.result_index_y) = y + 1;
				(*d.result_index_x) = x / 2 + 1;
				break;
			}
			y++;
		}
		if((*d.res) == 1)
			break;
		x += 2;
	}

	// exit(0);
	return 0;
}

gsize index_till_nl(gchar *str)
{
	gsize i = 0;

	while(str[i] && str[i] != '\n')
		i++;
	i++;
	return i;
}

void activate_callback(GtkApplication *app, gpointer data_ptr)
{
	(void)data_ptr;
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
	
	bool *ret1 = calloc(sizeof(bool), 1);
	bool *ret2 = calloc(sizeof(bool), 1);
	bool *ret3 = calloc(sizeof(bool), 1);
	bool *ret4 = calloc(sizeof(bool), 1);
	
	gsize block_size = 1;
	gsize escape = 1;
	gchar *result21 = NULL;
	gchar *result22 = NULL;
	gchar *result23 = NULL;
	gchar *result24 = NULL;
	
	gchar *result_final = NULL;
	t_eb_data data[10] = {0};
	fw_data fw_d[10];

	while(!(*ret1) && !(*ret2) && !(*ret3) && !(*ret4) && escape >= 0 && block_size >= 1)
	{
		pthread_t t1;
		pthread_t t2;
		pthread_t t3;

		data[0] = (t_eb_data){&result1, strlen(result1) + 1, escape, block_size, &result21};
		data[1] = (t_eb_data){&result1, strlen(result1) + 1, escape, block_size + 1, &result22};
		data[2] = (t_eb_data){&result1, strlen(result1) + 1, escape, block_size + 2, &result23};
		data[3] = (t_eb_data){&result1, strlen(result1) + 1, escape, block_size + 3, &result24};

		pthread_create(&t1, NULL, escape_cequnce_block, &data[0]);// escape_cequnce_block(&result1, strlen(result1) + 1, escape, block_size);
		pthread_create(&t2, NULL, escape_cequnce_block, &data[1]);// escape_cequnce_block(&result1, strlen(result1) + 1, escape, block_siz;
		pthread_create(&t3, NULL, escape_cequnce_block, &data[2]);// escape_cequnce_block(&result1, strlen(result1) + 1, escape, block_size);
		escape_cequnce_block(&data[3]);
		
		pthread_join(t1, NULL);
		pthread_join(t2, NULL);
		pthread_join(t3, NULL);
		
		
		printf("escape = %ld, block_size = %ld\n", escape, block_size);
		printf("escape = %ld, block_size = %ld\n", escape, block_size + 1);
		printf("escape = %ld, block_size = %ld\n", escape, block_size + 2);
		printf("escape = %ld, block_size = %ld\n", escape, block_size + 3);
		// printf("%s\n", *result21);
		// printf("%s\n", *result22);
		// printf("%s\n", *result23);
		// printf("%s\n", *result24);
		
		gchar *str = {0};
		quran_file = g_file_new_for_path("./test.txt");
		gsize str_len= 0;
		g_file_load_contents(quran_file, NULL, &str, &str_len, NULL, &file_error);;
		if (file_error != NULL)
    	{
    		g_printerr ("Unable to open “%s”: %s\n",
    	              g_file_peek_path (quran_file),
    	              file_error->message);
    		return;
    	}
	
	
		// gchar *test_str = "كدخجاتالقرانقمخذحكق";
		// gchar *str_test = "القران";
		// gchar **str_test_ptr = &str_test;
		// fw_data *fw_d2 =malloc(sizeof(fw_data) * 10);
		// fw_d2[0] =  (fw_data){str_test_ptr, str, ret1}; //{&result22,  str, &ret2}, {&result23,   str, &ret3}, {&result24,  str, &ret4}};
		// find_word(fw_d2);
		// if(ret1)
		// {
		// 	return ;
		// }
		// printf("%s\n", result21);
		gsize *first_index = malloc(sizeof(gsize));
		gsize *result_len = malloc(sizeof(gsize));
		fw_d[0] = (fw_data){&result21, str, ret1, strlen(result21), index_till_nl(result21), first_index, result_len};
		fw_d[1] = (fw_data){&result22,  str, ret2, strlen(result22), index_till_nl(result22), first_index, result_len};
		fw_d[2] = (fw_data){&result23,   str, ret3, strlen(result23), index_till_nl(result23), first_index, result_len};
		// fw_d[3] = (fw_data){&result24,  str, ret4, strlen(result24), index_till_nl(result24)};
		fw_d[3] = (fw_data){&str,  "محمد", ret4, str_len + 1, index_till_nl(str), first_index, result_len};
		pthread_create(&t1, NULL, find_word_x, &fw_d[0]);//ret1 = find_word(&result2,  "القذافي");
		pthread_create(&t2, NULL, find_word_x, &fw_d[1]);//ret1 = find_word(&result2,  "القذافي");
		pthread_create(&t3, NULL, find_word_x, &fw_d[2]);//ret1 = find_word(&result2,  "القذافي");
		find_word_y(&fw_d[3]);
		pthread_join(t1, NULL);
		pthread_join(t2, NULL);
		pthread_join(t3, NULL);


		// find_word((fw_data[10]){{&test_str, str, &te}});

		printf(" %d, %d, %d, %d\n", (*ret1), (*ret2), (*ret3), (*ret4));

		if( !(*ret1) && !(*ret2) && !(*ret3) && !(*ret4) )
		{
			if((block_size) >= 1000)
			{
				block_size = 1;
				if((escape) >= 10000)
					exit(0) ;
				escape++;
			}
			block_size += 4;
			free(result21);
			free(result22);
			free(result23);
			free(result24);
		}else {
			if((*ret1))
			{
				// free(result21);
				free(result22);
				free(result23);
				free(result24);
				result_final = result21;
			}else if((*ret2)){
				free(result21);
				// free(result22);
				free(result23);
				free(result24);
				result_final = result22;		
			}else if((*ret3)){
				free(result21);
				free(result22);
				// free(result23);
				free(result24);
				result_final = result23;		
			}else if((*ret4)){
				free(result21);
				free(result22);
				free(result23);
				// free(result24);
				result_final = result24;		
			}
			// printf("%s\n", *result24);
			break;
		}
	}

	g_print("position = x = %ld, y = %ld\n", *fw_d[3].result_index_x,  *fw_d[3].result_index_y);
	gtk_text_buffer_set_text(text_buffet, result_final, -1);
	
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