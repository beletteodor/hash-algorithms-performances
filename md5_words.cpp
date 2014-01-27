#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>

#include <arpa/inet.h>
#include <openssl/md5.h>
#include <pthread.h>

#include "util.h"

using namespace std;

struct result {
	unsigned char res[MD5_DIGEST_LENGTH];
};

struct tdata {
	uint32_t begin;
	uint32_t end;
};

vector<string> words;
vector<result> results;
vector<string> results_hexa;

// Print the MD5 sum as hex-digits.
static string to_hex(unsigned char* md) 
{
	char line[MD5_DIGEST_LENGTH * 2 + 1];

	for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
		sprintf(&line[i * 2], "%02x", md[i]);
	}

	return string(line);
}

static void to_data(unsigned char *md, uint32_t &x, uint32_t &y)
{
	x = htonl(*((uint32_t *)md));
	y = htonl(*((uint32_t *)(&md[MD5_DIGEST_LENGTH / 2])));
}

static void *hash(void *arg)
{
	tdata *td = (tdata *)arg;

	for (uint32_t i = td->begin; i < td->end; i++) {
		string *s = &words[i];
		MD5((unsigned char*)s->c_str(), s->size(), results[i].res);
	}

	return NULL;
}

int main(int argc, char *argv[])
{
	ifstream input;
	ofstream output;
	vector<tdata> td;
	vector<pthread_t> threads;

	char line[128];
	int num_threads;
	int rc, step, idx;
	struct timeval t1, t2;

	KICK(argc < 5, "./md5 <input_file> <output_file> <data_file> <num_threads>");

	num_threads = atoi(argv[4]);
	input.open(argv[1]);
	DIE(input.fail(), "open");
	while (!input.eof()) {
		input.getline(line, 127);
		DIE(input.bad(), "getline");
		words.push_back(line);
	}
	input.close();
	words.pop_back();

	int size = words.size();
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
	cout << "md5: " << argv[1] << endl;
	cout << usec << " usec" << endl;

	for (int i = 0; i < size; i++) {
		results_hexa.push_back(to_hex(results[i].res));
	}

	map<string, string> m;
	int num_collisions = 0;
	cerr << "collisions: " << endl;
	for (int i = 0; i < size; i++) {
		string key = results_hexa[i];
		string value = words[i];

		if (m.count(key)) {
			cerr << "(" << m.find(key)->second << ", " << value << ")" << endl;
			num_collisions++;
		}

		m.insert(pair<string, string>(key, value));
	}
	cout << num_collisions << " collisions" << endl;

	output.open(argv[2]);
	DIE(output.fail(), "open");
	for (int i = 0; i < size; i++) {
		output << results_hexa[i] << " " << words[i] << endl;
	}
	output.close();

	output.open(argv[3]);
	DIE(output.fail(), "open");
	for (int i = 0; i < size; i++) {
		uint32_t x, y;
		to_data(results[i].res, x, y);
		output << x << " " << y << endl;
	}
	output.close();

	return 0;
}

