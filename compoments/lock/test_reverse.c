#include <stdio.h>
#include <string.h>

int is_equal_reverse(char *s1, char *s2) {
	int n = strlen(s1);
	int m = strlen(s2);
	
	if (n != m) return -1; 
	
	for (int i = 0, j = n - 1; i < j; i++, j--) {
		if (s1[i] != s2[j]) return -1; 
	}

	return 0;
}

int main() {
	
	char s[] = "hello";
	char s1[] = "olleh";


	if (is_equal_reverse(s, s1) == 0) {
		printf("equal\n");
	}

	return 0;
}
