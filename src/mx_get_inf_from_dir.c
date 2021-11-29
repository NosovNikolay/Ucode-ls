#include "uls.h"

char **mx_get_inf_from_dir(const char *dir_name, int *size_dirp, int arguments_count) {
    DIR *dp;
    struct dirent *tmp;

    dp = opendir(dir_name);
        if (!dp) {
        char *name = mx_strnew(mx_strlen(dir_name));
        for (int i = mx_strlen(dir_name) - 1, k = 0; i >= 0; i--) {
        if (dir_name[i] != '/') {
            name[k] = dir_name[i];
            k++;
        }
        else break;
        }
        if (arguments_count > 1) {
                mx_printstr(dir_name);
                mx_printstr(":\n");
        }
            mx_printerr("uls: ");
            mx_str_reverse(name);
            perror(name);
            return NULL;
    }

    while ((tmp= readdir(dp)) != NULL) {
        *size_dirp = *size_dirp + 1;
    }
    closedir(dp);

    char **dirp = (char **)malloc(sizeof(char *) * (*size_dirp));
    tmp = NULL;
    dp = opendir(dir_name);

    for (int i = 0; i < *size_dirp; i++) {
        tmp = readdir(dp);
        dirp[i] = mx_strdup(tmp->d_name);
        if (dirp[i] == NULL) {
            mx_printerr("cant read\n");
            exit(1);
        }
    }
    // Vozmozhno mesto ute4ki ili oshibki
    // Elsi naebnetsa, smotret suda 
    dirp = mx_realloc(dirp, *size_dirp);

    closedir(dp);
    return dirp;
}

