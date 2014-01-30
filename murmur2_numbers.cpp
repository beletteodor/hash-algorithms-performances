#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>

#include <arpa/inet.h>
#include <pthread.h>

#include "util.h"

using namespace std;

#define N ((1 << 18) - 1)

struct tdata {
	uint32_t begin;
	uint32_t end;
};

vector<unsigned long> results;
vector<string> results_hexa;

static string to_hex(unsigned char* md) 
{
	char line[9];

	for (int i = 0; i < 4; i++) {
		sprintf(&line[i * 2], "%02x", md[i]);
	}

	return string(line);
}

static void to_data(unsigned char *md, uint16_t &x, uint16_t &y)
{
	x = htons(*((uint16_t *)md));
	y = htons(*((uint16_t *)(&md[2])));
}

static unsigned long murmur2(unsigned char *key, int len)
{
  /* 'm' and 'r' are mixing constants generated offline.
     They're not really 'magic', they just happen to work well.  */

  const uint32_t m = 0x5bd1e995;
  const int r = 24;


  /* Initialize the hash to a 'random' value */
  unsigned long seed = 5381;
  unsigned long h = seed ^ len;

  /* Mix 4 bytes at a time into the hash */
  const unsigned char * data = (const unsigned char *)key;

  while(len >= 4)
  {
    uint32_t k = *(uint32_t*)data;

    k *= m;
    k ^= k >> r;
    k *= m;

    h *= m;
    h ^= k;

    data += 4;
    len -= 4;
  }

  /* Handle the last few bytes of the input array  */
  switch(len)
  {
      case 3: h ^= data[2] << 16;
      case 2: h ^= data[1] << 8;
      case 1: h ^= data[0];
          h *= m;
  };

  /* Do a few final mixes of the hash to ensure the last few
  // bytes are well-incorporated.  */

  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;

  return h;
}

static void *hash(void *arg)
{
	tdata *td = (tdata *)arg;

	for (uint32_t i = td->begin; i < td->end; i++) {
		results[i] = murmur2((unsigned char *)&i, 4);
	}

	return NULL;
}

int main(int argc, char *argv[])
{
	ofstream output;
	vector<tdata> td;
	vector<pthread_t> threads;

	int rc, step, idx;
	struct timeval t1, t2;

	KICK(argc < 4, "./md5 <output_file> <data_file> <num_threads>");

	int num_threads = atoi(argv[3]);
	int size = N + 1;
	results.resize(size);
	td.resize(num_threads);
	threads.resize(num_threads);
	step = size / num_threads;
	if (size % num_threads) {
		step++;
	}

	gettimeofday(&t1, NULL);

	idx = 0;
	for (int i = 0; i < num_threads; i++) {
		td[i].begin = idx;
		td[i].end = min(idx + step, size);
		idx = td[i].end;
		rc = pthread_create(&threads[i], NULL, hash, &td[i]);
		DIE(rc != 0, "pthread_create");
	}

	for (int i = 0; i < num_threads; i++) {
		rc = pthread_join(threads[i], NULL);
		DIE(rc != 0, "pthread_join");
	}

	gettimeofday(&t2, NULL);
	uint64_t usec = diff(&t2, &t1);
	cout << "murmur2: " << "0 - " << N << endl;
	cout << usec << " usec" << endl;

	for (int i = 0; i < size; i++) {
		results_hexa.push_back(to_hex((unsigned char *)&results[i]));
	}

	map<string, int> m;
	int num_collisions = 0;
	cerr << "collisions: " << endl;
	for (int i = 0; i < size; i++) {
		string key = results_hexa[i];

		if (m.count(key)) {
			cerr << "(" << m.find(key)->second << ", " << i << ")" << endl;
			num_collisions++;
		}

		m.insert(pair<string, int>(key, i));
	}
	cout << num_collisions << " collisions" << endl;

	output.open(argv[1]);
	DIE(output.fail(), "open");
	for (int i = 0; i < size; i++) {
		output << results_hexa[i] << " " << i << endl;
	}
	output.close();

	output.open(argv[2]);
	DIE(output.fail(), "open");
	for (int i = 0; i < size; i++) {
		uint16_t x, y;
		to_data((unsigned char *)&results[i], x, y);
		output << x << " " << y << endl;
	}
	output.close();

	return 0;
}

