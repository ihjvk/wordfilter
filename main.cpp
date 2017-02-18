#include "WordFilter.h"
#define  BAD_WORD_FILE_NAME "keywords.txt"
int main(int argc, char **argv)
{
	// load bad words;
	if (!WordFilter::Load(BAD_WORD_FILE_NAME))
	{
		return -1;
	}
	WordFilter::test();
	getchar();
	return 0;
}