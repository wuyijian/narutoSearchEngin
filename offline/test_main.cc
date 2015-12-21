#include "mysplit.h"

int main(void)
{
	elton::MySplit mysplit;

	mysplit.getVecForPage();

	mysplit.getVectorForSimhash();

	mysplit.writeWeiboToDB();

	mysplit.getVecForTfmap();



    mysplit.getMapForIndex();

	mysplit.writeIndexToDB();

	return 0;
}
