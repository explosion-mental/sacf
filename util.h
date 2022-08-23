/* See LICENSE file for copyright and license details. */

#define MAX(A, B)               ((A) > (B) ? (A) : (B))
#define MIN(A, B)               ((A) < (B) ? (A) : (B))
#define BETWEEN(X, A, B)        ((A) <= (X) && (X) <= (B))

void die(const char *fmt, ...);
void *ecalloc(size_t nmemb, size_t size);
void eglob(const char *path, glob_t *muhglob);
int pscanf(const char *path, const char *fmt, ...);
int pprintf(const char *path, const char *fmt, ...);
