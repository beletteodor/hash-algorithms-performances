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

struct tdata {
        uint32_t begin;
        uint32_t end;
};

vector<string> words;
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

static unsigned long lose(unsigned char *str, int count) {

        unsigned long h = 0;
        int c;
        
        for(int i = 0; i < count; i++) {
			c = *str++;
			h += c;
		}		

        return h;

}

static void *hash(void *arg)
{
        tdata *td = (tdata *)arg;

        for (uint32_t i = td->begin; i < td->end; i++) {
                string *s = &words[i];
                results[i] = lose((unsigned char *)s->c_str(), s->size());
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

        KICK(argc < 5, "./lose_words <input_file> <output_file> <data_file> <num_threads>");

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
        cout << "lose: " << argv[1] << endl;
        cout << usec << " usec" << endl;

        for (int i = 0; i < size; i++) {
                results_hexa.push_back(to_hex((unsigned char *)&results[i]));
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
                uint16_t x, y;
                to_data((unsigned char *)&results[i], x, y);
                output << x << " " << y << endl;
        }
        output.close();

        return 0;
}

