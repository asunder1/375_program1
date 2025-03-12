#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <set>
#include <chrono>

using namespace std;

set<string> purchased_items, initial_set;
unordered_map<string,int> sell_prices, buy_prices;
vector<string> current_problem_items;

struct result {
    set<string> set;
    int profit;
};

result compute_max_profit(set<string>&, int);
int compute_cost(set<string>&);
int compute_profit(set<string>&, int cost=-1);
set<string> generate_next_subset(set<string>&);


int main() {
    string current_line, name;
    int price, num_items, budget;
    result res;
    
    // populate sell_prices hash table
    ifstream market_file("market_price.txt");

    // skip first line
    getline(market_file, current_line);
    
    // populate hash table with all names and prices
    while (getline(market_file, current_line)) {
        istringstream iss(current_line);
        iss >> name >> price;
        cout << "Name: " << name << " Price: " << price << endl;
        sell_prices[name] = price;
    }
    market_file.close();

    cout << "Done with market prices" << endl;

    // solve problems in price_list
    ifstream cost_file("price_list.txt");

    // runs once for each problem in price_list.txt
    while (getline(cost_file, current_line)) {
        istringstream iss(current_line);
        iss >> num_items >> budget;
        cout << "num_items: " << num_items << " budget: " << budget << endl;
        buy_prices.clear();
        initial_set.clear();
        current_problem_items.clear();

        for (int i = 0; i < num_items; i++) {
            getline(cost_file, current_line);
            istringstream iss(current_line);
            iss >> name >> price;
            cout << "Name: " << name << " Price: " << price << endl;
            if (sell_prices.find(name) == sell_prices.end()) {
                cout << "Error: item in price_list.txt has no market price in market_price.txt" << endl;
            }
            buy_prices[name] = price;
            initial_set.insert(name);
            current_problem_items.push_back(name);
        }
        chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
        res = compute_max_profit(initial_set, budget);
        std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_elapsed = end - start;

        cout << "Possible Items: " << num_items << ". Profit: " << res.profit << ". Items purchased: " << res.set.size() 
            << ". Time: " << time_elapsed.count() << " seconds." << endl;
        for (string item : res.set) {
            cout << item << endl;
        }
        cout << endl;
    }
    cost_file.close();

}

result compute_max_profit(set<string>& i, int w) {
    result res;
    int max_profit = 0, current_cost, current_profit;
    set<string> current_set, best_set;

    // case where you can just buy every item
    if (compute_cost(i) < w) {
        res.set = i;
        res.profit = compute_profit(i);
        return res;
    }

    while (current_set != i) {
        current_cost = compute_cost(current_set);
        if (current_cost <= w) {
            current_profit = compute_profit(current_set, current_cost);
            if (current_profit > max_profit) {
                max_profit = current_profit;
                best_set = current_set;
            }
        }
        current_set = generate_next_subset(current_set);
    }
    res.set = best_set;
    res.profit = max_profit;
    return res;
}

int compute_cost(set<string>& set) {
    int total = 0;
    for (string item : set) {
        total += buy_prices.at(item);
    }
    return total;
}

int compute_profit(set<string>& set, int cost) {
    if (cost == -1) {
        cost = compute_cost(set);
    }
    int profit = cost * -1;
    for (string item : set) {
        profit += sell_prices.at(item);
    }
    return profit;
}

set<string> generate_next_subset(set<string>& subset) {
    int num_items = current_problem_items.size();
    vector<short> has_items;
    int i, j;
    set<string> next_subset;

    // get current subset items
    for (i = 0; i < num_items; i++) {
        if (subset.find(current_problem_items.at(i)) != subset.end()) {
            has_items.push_back(1);
        }
        else {
            has_items.push_back(0);
        }
    }

    // compute next subset items
    for (i = num_items - 1; i >= 0; i--) {
        if (has_items.at(i) == 1) {
            continue;
        }
        else {
            has_items.at(i) = 1;
            for (j = i + 1; j < num_items; j++) {
                has_items.at(j) = 0;
            }
            break;
        }
    }

    // generate actual subset of strings
    for (i = 0; i < num_items; i++) {
        if (has_items.at(i)) {
            next_subset.insert(current_problem_items.at(i));
        }
    }
    return next_subset;
}