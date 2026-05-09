#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <iomanip>

using namespace std;

// run command
vector<string> run_cmd(const string &cmd) {
    vector<string> out;
    FILE *fp = popen(cmd.c_str(), "r");
    if (!fp) return out;

    char buf[512];
    while (fgets(buf, sizeof(buf), fp)) {
        out.emplace_back(buf);
    }
    pclose(fp);
    return out;
}

// trim
string trim(const string &s) {
    size_t l = s.find_first_not_of(" \t\n\r");
    if (l == string::npos) return "";
    size_t r = s.find_last_not_of(" \t\n\r");
    return s.substr(l, r - l + 1);
}

int main() {

    while (true) {

        cout << "\033[H\033[J";
        cout << "=========== SSH MONITOR ===========\n\n";

        vector<string> lines = run_cmd("ps -eo user,tty --no-headers");

        map<string, set<string>> sessions;

        for (auto &l : lines) {
            istringstream iss(l);
            string user, tty;

            iss >> user >> tty;

            if (tty.find("pts/") != 0) continue;
            if (user == "root" || user == "admin") continue;

            sessions[user].insert(tty);
        }

        // ---- TABLE HEADER ----
        cout << left << setw(15) << "USER"
             << setw(10) << "SESSIONS"
             << "TTYs\n";
        cout << "-------------------------------------------\n";

        int total_users = 0;

        for (auto &entry : sessions) {

            string user = entry.first;
            auto &ttys = entry.second;

            if (ttys.empty()) continue;

            total_users++;

            // join ttys into single string
            string tty_list = "";
            int i = 0;
            for (auto &t : ttys) {
                tty_list += t;
                if (i < (int)ttys.size() - 1) tty_list += ", ";
                i++;
            }

            cout << left << setw(15) << user
                 << setw(10) << ttys.size()
                 << tty_list << "\n";

            // enforce single session
            int idx = 0;
            for (auto &t : ttys) {
                if (idx > 0) {
                    string cmd = "pkill -KILL -t " + t;
                    system(cmd.c_str());
                }
                idx++;
            }
        }

        cout << "-------------------------------------------\n";
        cout << "\nTotal Active Users: " << total_users << "\n";
        cout << "Refreshing every 2 seconds...\n";

        sleep(2);
    }

    return 0;
}