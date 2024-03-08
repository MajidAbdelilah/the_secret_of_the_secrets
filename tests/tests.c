#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include "../sos.h"
#include <assert.h>

void test_adjust_quranic_text() {
    char input_text[] = "1|1|بسم الله الرحمن الرحيم\n1|2|الحمد لله رب العالمين\n1|3|الرحمن الرحيم\n1|4|مالك يوم الدين\n1|5|إياك نعبد وإياك نستعين\n1|6|اهدنا الصراط المستقيم\n1|7|صراط الذين أنعمت عليهم غير المغضوب عليهم ولا الضالين";
    gsize input_len = strlen(input_text);
    char *input_copy = (char*)malloc(input_len + 1);
    strcpy(input_copy, input_text);
    char expected_output[] = "بسم الله الرحمن الرحيم\nالحمد لله رب العالمين\nالرحمن الرحيم\nمالك يوم الدين\nإياك نعبد وإياك نستعين\nاهدنا الصراط المستقيم\nصراط الذين أنعمت عليهم غير المغضوب عليهم ولا الضالين";
    
    adjust_quranic_text(&input_copy, input_len + 1);
    
    printf("Input Copy: %s\n", input_copy);
    printf("Expected Output: %s\n", expected_output);
    
    // Check the modified input_copy against the expected output
    assert(strcmp(input_copy, expected_output) == 0);
    
    free(input_copy); // Free the allocated memory
}

int main(void) {
    test_adjust_quranic_text();
    printf("All tests passed!\n");
    return 0;
}