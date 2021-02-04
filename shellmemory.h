/* List all the functions you want to be "public." */
void shell_memory_initialize();
void shell_memory_destroy();
const char *shell_memory_get(const char *key);
int shell_memory_set(const char *key, const char *value);
