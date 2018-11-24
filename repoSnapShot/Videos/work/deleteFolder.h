#ifndef DELETEFOLDER_H
#define DELETEFOLDER_H

using namespace std;

void delete_folder_tree(const char *dirname)
{
	DIR *dir;
	int i=0;
	cout<<"\n";
	while(dirname[i]!=NULL){
		cout<<dirname[i];
		i++;
	}
	cout<<i<<"\n";
	struct dirent *entry;
	char path[PATH_MAX];

	if (path == NULL) {
		return;
	}
	dir = opendir(dirname);
	if (dir == NULL) {
		printf("Error opendir()");
		return;
	}

	while ((entry = readdir(dir)) != NULL) {
		if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
			snprintf(path, (size_t) PATH_MAX, "%s/%s", dirname, entry->d_name);
		if (entry->d_type == DT_DIR) {
			delete_folder_tree(path);
		} else {
			unlink(path);
		}
	}

	}
	closedir(dir);

	rmdir(dirname);

}

#endif