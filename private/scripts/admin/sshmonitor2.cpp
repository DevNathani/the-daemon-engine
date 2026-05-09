#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <iomanip>
#include <algorithm>
#include <signal.h>

using namespace std;

struct Session {
    int pid;
    string ip;
};

// run shell command
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

// extract IP from sshd process string
string extract_ip(const string &cmd) {
    size_t lb = cmd.find('[');
    size_t rb = cmd.find(']');
    if (lb != string::npos && rb != string::npos && rb > lb) {
        string ip_port = cmd.substr(lb + 1, rb - lb - 1);
        size_t colon = ip_port.find(':');
        if (colon != string::npos)
            return ip_port.substr(0, colon);
        return ip_port;
    }
    return "unknown";
}

int main() {

    while (true) {

        cout << "\033[H\033[J";
        cout << "=========== SSH MONITOR (STABLE) ===========\n\n";

        // get processes
        vector<string> lines = run_cmd("ps -eo pid,user,args --no-headers");

        map<string, vector<Session>> sessions;

        for (auto &l : lines) {

            istringstream iss(l);

            int pid;
            string user;
            iss >> pid >> user;

            string cmd;
            getline(iss, cmd);
            cmd = trim(cmd);

            // only sshd processes
            if (cmd.find("sshd") == string::npos) continue;

            // skip main daemon
            if (cmd.find("-D") != string::npos) continue;

            // skip system users
            if (user == "root" || user == "admin") continue;

            // skip privilege separation process
            if (cmd.find("[priv]") != string::npos) continue;

            Session s;
            s.pid = pid;
            s.ip = extract_ip(cmd);

            sessions[user].push_back(s);
        }

        // ---- HEADER ----
        cout << left << setw(15) << "USER"
             << setw(10) << "COUNT"
             << "IPs\n";
        cout << "----------------------------------------------\n";

        int total_users = 0;

        for (auto &entry : sessions) {

            string user = entry.first;
            auto &vec = entry.second;

            if (vec.empty()) continue;

            total_users++;

            // sort by PID (oldest first)
            sort(vec.begin(), vec.end(), [](const Session &a, const Session &b) {
                return a.pid < b.pid;
            });

            cout << left << setw(15) << user
                 << setw(10) << vec.size();

            string ip_list = "";
            for (int i = 0; i < (int)vec.size(); i++) {
                ip_list += vec[i].ip;
                if (i != (int)vec.size() - 1) ip_list += ", ";
            }

            cout << ip_list << "\n";

            // enforce single session (keep oldest PID)
            for (int i = 1; i < (int)vec.size(); i++) {
                kill(vec[i].pid, SIGKILL);
            }
        }

        cout << "----------------------------------------------\n";
        cout << "\nTotal Active Users: " << total_users << "\n";
        cout << "Refresh: 2s\n";

        sleep(2);
    }

    return 0;
}