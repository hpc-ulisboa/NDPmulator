#define DATA_SIZE 0x1000

int main(int argc, char const *argv[])
{
	long int vec1[DATA_SIZE];

	for (int i = 0; i < DATA_SIZE; ++i)
	{
		vec1[i] = i;
	}

	for (int i = 0; i < DATA_SIZE; ++i)
	{
		for (int j = 0; j < DATA_SIZE; ++j)
		{
			vec1[j]++;
		}
	}

	return 0;
}
