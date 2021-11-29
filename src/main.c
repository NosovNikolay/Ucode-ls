#include "uls.h"

int main(int argc, char const *argv[]) {
    const int COUNT_FLAGS = 16;
    const char FLAGS[16] = "ACGRST@acehlrtu1";

    cur_flags_t *cur_flags = mx_get_flags(COUNT_FLAGS, FLAGS, argc, argv);
    
    int arguments_count = count_dir_and_files(argc, argv);
    char **arguments = NULL;
    int arguments_before_vaidation = arguments_count;

    if (arguments_count == 0) {
        arguments = malloc(sizeof(char *));
        arguments[0] = mx_strdup(".");
        arguments_count++;
        arguments_before_vaidation = arguments_count;
    }
    else {
        arguments = parse_arguments(argc, argv, &arguments_count);
        mx_bubble_sort(arguments, arguments_count);
    }
    
    all_flags_t *usable_flags = malloc(sizeof(all_flags_t));

    usable_flags->is_C_print = true;
    usable_flags->is_list = false;
    usable_flags->is_long = false;
    usable_flags->is_A = false;
    usable_flags->is_a = false;
        
    usable_flags->is_reverse = false;
    usable_flags->is_common_sort = true;
    usable_flags->is_c_sort = false;
    usable_flags->is_S_sort = false;
    usable_flags->is_t_sort = false;
    usable_flags->is_u_sort = false;
        
    usable_flags->is_h_long = false;
    usable_flags->is_T_long = false;
    usable_flags->is_G_color = false;
    usable_flags->is_at = false;

    if (isatty(1) == 0) {
        usable_flags->is_list = true;
        usable_flags->is_C_print = false;
    }

    for (int i = 0; i < cur_flags->count; i++) {
        switch (cur_flags->flags[i]) {
            case 'l':
                usable_flags->is_long = true;
                usable_flags->is_list = false;
                usable_flags->is_C_print = false;
                break;
            case '1':
                usable_flags->is_list = true;
                usable_flags->is_long = false;
                usable_flags->is_C_print = false;
                break;
            case 'C':
                usable_flags->is_C_print = true;
                usable_flags->is_list = false;
                usable_flags->is_long = false;
                break;
            case 'a':
                usable_flags->is_a = true;
                usable_flags->is_A = false;   
                break;
            case 'A':
                usable_flags->is_A = true;
                usable_flags->is_a = false;
                break;
            case 'r':
                usable_flags->is_reverse = true;
                break;
            case 't':
                usable_flags->is_t_sort = true;
                break;
            case 'u':
                usable_flags->is_u_sort= true;
                usable_flags->is_c_sort = false;
                break;
            case 'c':
                usable_flags->is_c_sort = true;
                usable_flags->is_u_sort= false;
                break;
            case 'S':
                usable_flags->is_S_sort = true;
                break;
            case 'h':
                usable_flags->is_h_long = true;
                break;
            case 'T':
                usable_flags->is_T_long = true;
                break;
            case 'G':
                usable_flags->is_G_color = true;
                break;
            case '@':
                usable_flags->is_at = true;
                break;
            default:
                break;
            }
        }

    // Select a sort function 
    bool (*sort_func)(long_data_t *, long_data_t *)  = NULL;
    if (usable_flags->is_S_sort) {
        sort_func = mx_size_cmp;
    }
    else if (usable_flags->is_t_sort && !usable_flags->is_u_sort && !usable_flags->is_c_sort) {
        //time sort
        sort_func = mx_time_modif_cmp;
    }
    else if (usable_flags->is_u_sort && usable_flags->is_t_sort) {
        //vremya последнего доступа для сортировки 
        sort_func = mx_time_access_cmp;
    }
    else if (usable_flags->is_c_sort && usable_flags->is_t_sort) {
        //использовать время последней модификации описателя файла 
        sort_func = mx_time_status_cmp;
    } 
    else {
        sort_func = mx_default_cmp;
    }

    // цикл проходит через все аргументы
    int size_dirp;
    //struct dirent **dirp;
    long_data_t **all_long_data;
        
    // вытаскиваем файлы из arguments
    int count_files = 0;
    char **files_arguments_arr = mx_pop_files(arguments, &arguments_count, &count_files);
    bool is_files = true;
    if (count_files != 0) {
        all_long_data = mx_get_all_long_data(count_files, files_arguments_arr, ".");
        for (int i = 0; i < count_files; i++) {
            mx_get_redable_mode(all_long_data[i]);
            mx_get_redable_uid(all_long_data[i]);
            mx_get_redable_gid(all_long_data[i]); 
            all_long_data[i]->type_size = 'B';
            all_long_data[i]->size_remainder = 0;
            all_long_data[i]->at_link = NULL;
            all_long_data[i]->readlink = NULL;
            mx_islink(all_long_data[i]);
            mx_isplus(all_long_data[i]);
        }

        mx_insertion_sort(all_long_data, count_files, sort_func);
            
        if (usable_flags->is_reverse) {
            mx_reverse_array(all_long_data, count_files);
        }
        if (usable_flags->is_list) {
            mx_print_list(all_long_data, count_files, usable_flags);
        } 
        else if (usable_flags->is_long) {
            if (usable_flags->is_h_long) {
                mx_translate_size(all_long_data, count_files);
            }
            mx_print_long_data(all_long_data, count_files, usable_flags, is_files);
        }   
        else if (usable_flags->is_C_print) {
            char *temp_string = agruments_filter(all_long_data, count_files, usable_flags);
            if (temp_string) {
                mx_print_table_short(temp_string, all_long_data , count_files, usable_flags);
            }
        }
        if (arguments_count - count_files > 0) {
            mx_printchar('\n');
        }

    }
        
    is_files = false;
    // нужно каким-то образом их вывести 
        // чтобы вывести нужно удалять первый символ в файле (пооддставляет атоматом )
    char **names_arr;
    for (int i = 0; i < arguments_count; i++) {
        size_dirp = 0;
        if (arguments[i] != NULL) {
            names_arr = mx_get_inf_from_dir(arguments[i], &size_dirp, arguments_count);
            
            if (names_arr == NULL) {
                if (i != arguments_count - 1)
                    mx_printstr("\n");
                continue;
            }
        }
        else continue;
            
        if (size_dirp == 0) size_dirp++;
        all_long_data = mx_get_all_long_data(size_dirp, names_arr, arguments[i]);
            
        for (int j = 0; j < size_dirp; j++) {
            mx_get_redable_mode(all_long_data[j]);
            mx_get_redable_uid(all_long_data[j]);
            mx_get_redable_gid(all_long_data[j]); 
            all_long_data[j]->type_size = 'B';
            all_long_data[j]->size_remainder = 0;
            all_long_data[j]->at_link = NULL;
            all_long_data[j]->readlink = NULL;
        }
        // есть ли @ и +
        for (int j = 0; j < size_dirp; j++) {
            mx_islink(all_long_data[j]);
            mx_isplus(all_long_data[j]);
        }
        // сортировка 
        mx_insertion_sort(all_long_data, size_dirp, sort_func);
            
        // принт директори
        if ((size_dirp != 1 && arguments_count > 1) || arguments_count != arguments_before_vaidation) {
            mx_printstr(arguments[i]);
            mx_printstr(":\n");
        }
            
        if (usable_flags->is_reverse) {
            mx_reverse_array(all_long_data, size_dirp);
        }
        if (usable_flags->is_list) {
            mx_print_list(all_long_data, size_dirp, usable_flags);
        } 
        else if (usable_flags->is_long) {
            if (usable_flags->is_h_long) {
                    mx_translate_size(all_long_data, size_dirp);
                }
            mx_print_long_data(all_long_data, size_dirp, usable_flags, is_files);
        }
        else if (usable_flags->is_C_print) {
        char *temp_string = agruments_filter(all_long_data, size_dirp, usable_flags);   
            if (temp_string) {
                mx_print_table_short(temp_string, all_long_data , size_dirp, usable_flags);
            }
        }
        if (i + 1 != arguments_count && arguments_count != 1 ) {
            mx_printchar('\n');
        }
    }
    return 0;
}
