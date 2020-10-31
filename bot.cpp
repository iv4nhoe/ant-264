#include <iostream>
#include <string>
#include <vector>
#include "json.hpp"
#include "httplib.h"
#include <random>

using namespace std;
using namespace httplib;
using namespace nlohmann;

const vector<string> ACTIONS = { "stay", "move", "eat", "take", "put" };
const vector<string> DIRECTIONS = { "up", "down", "right", "left" };

int main(int argc, char* argv[])
{
  // Initialize random number generator
  random_device rd;
  mt19937 mt(rd());
  uniform_int_distribution<size_t> dist1(0, ACTIONS.size()-1);
  uniform_int_distribution<size_t> dist2(0, DIRECTIONS.size()-1);

  // Initialize the http server
  Server svr;
  svr.Post("/", [&](const Request& req, Response& res) {
    // Hive object from request payload
    auto hive = json::parse(req.body);

    // Loop through ants and give orders
    auto orders = std::vector<json>();
    for (auto& ant : hive["ants"].items()) {
      json order = {
        {"antId", ant.value()["id"] },
        {"act", ACTIONS[dist1(mt)] },
        {"dir", DIRECTIONS[dist2(mt)]}
      };
      orders.push_back(order);
    }

    json response;
    response["orders"]=orders;

    cout << response.dump() << '\n';
    res.set_content(response.dump(), "application/json");
    // json format sample:
    // {"orders": [
    //  {"antId":1,"act":"move","dir":"down"},
    //  {"antId":17,"act":"load","dir":"up"}
    // ]}

  });

  svr.listen("0.0.0.0", 7070);
  return 0;
}
