#include <stdio.h>
#include <iostream>
#include <deque>
#include <algorithm>
#include <cstring>
#include <cassert>
#include <vector>

#define assertm(exp, msg) assert(((void)msg, exp))

using namespace std;

const unsigned int DATA_SIZE = 9;

class Range {
public:
    unsigned int left;
    unsigned int right;

    Range(unsigned int left, unsigned int right) {
        this->left = left;
        this->right = right;
    }

    bool operator&&(Range other) {
        return max(left, other.left) <= min(right, other.right);
    }
};

struct WriteLog {
    unsigned int start_index;
    unsigned int length;

    vector <Range> to_ranges() {
        vector <Range> res = {};
        unsigned int right = start_index + length - 1;

        if (right >= DATA_SIZE) {
            res.push_back({start_index, DATA_SIZE - 1});
            res.push_back({0, right % DATA_SIZE});
        } else {
            res.push_back({start_index, right});
        }
        return res;
    }
};

class CycleBuf {
private:
    char data[DATA_SIZE];
    deque <WriteLog> logs;
    unsigned int w;

public:
    CycleBuf() {
        memset(data, '?', DATA_SIZE);
        logs = {};
        w = 0;
    }

    void print_data() {
        cout << "data = `";
        for (int i = 0; i < DATA_SIZE; i++) cout << data[i];
        cout << "`" << endl;
    }

    void push(char inserted_data[]) {
        unsigned int inserted_length = strlen(inserted_data);
        assertm(inserted_length <= DATA_SIZE, "Нельзя вставить данные больше размера буфера");

        unsigned int left = w, right = w + inserted_length;
        vector <Range> current_ranges = {};

        if (right <= DATA_SIZE) {
            copy(inserted_data, inserted_data + inserted_length, data + w);
            w += inserted_length;
            current_ranges.push_back(Range(left, right - 1));
        } else {
            auto diff = DATA_SIZE - w; // сколько поместилось

            copy(inserted_data + 0, inserted_data + diff, data + w);
            copy(inserted_data + diff, inserted_data + inserted_length, data);

            current_ranges.push_back(Range(left, DATA_SIZE - 1));
            current_ranges.push_back(Range(0, (right % DATA_SIZE) - 1));
        }

        while (not logs.empty()) { // удалим больше не актуальные логи
            WriteLog log = logs.front();
            bool intersect = false;

            for (auto log_range: log.to_ranges())
                for (auto current: current_ranges)
                    intersect |= log_range && current;

            if (intersect) logs.pop_front();
            else break;
        }

        logs.push_back({left, inserted_length});
        w = right % DATA_SIZE;
        print_data();
    }

    void read() {
        assertm(not logs.empty(), "Нечего тут читать!");

        WriteLog log = logs.front();
        cout << "read = `";
        for (int i = 0; i < log.length; i++) cout << data[(log.start_index + i) % DATA_SIZE];
        cout << "`\n";
        logs.pop_front();
    }
};

int main() {
    CycleBuf my_buf;
    string s = "";

    do {
        if (s == "read") my_buf.read();
        else if (s != "") my_buf.push(&s[0]);
        cin >> s;
    } while (s != "q");
}
