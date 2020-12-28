#include <iostream>
#include <string>
#include <vector>
#include "json.hpp"
#include "httplib.h"
#include <random>

using namespace std;
using namespace httplib;
using namespace nlohmann;

// available actions and directions
const vector<string> ACTIONS = { "stay", "move", "eat", "take", "put" };
const vector<string> DIRECTIONS = { "up", "down", "right", "left" };

int main(int argc, char* argv[])
{
  // initialize random number generator
  random_device rd;
  mt19937 mt(rd());
  uniform_int_distribution<size_t> dist1(0, ACTIONS.size()-1); // pick random action from array on line 13
  uniform_int_distribution<size_t> dist2(0, DIRECTIONS.size()-1); // pick random direction from array on line 14

  // initialize the http server
  Server svr;
  // sim will make http post request to your bot
  svr.Post("/", [&](const Request& req, Response& res) {
    // your bot respons should be json object
    auto hive = json::parse(req.body);

    // loop through ants and give orders
    auto orders = std::vector<json>();
    for (auto& ant : hive["ants"].items()) {
      json order = {
        {"antId", ant.value()["id"] },
        {"act", ACTIONS[dist1(mt)] },
        {"dir", DIRECTIONS[dist2(mt)]}
      };
      // add order to your response object from line 32
      orders.push_back(order);
    }

    json response;
    response["orders"]=orders;

    cout << response.dump() << '\n';
    // finish your response and send back json to 
    res.set_content(response.dump(), "application/json");
    // json format sample:
    // {"orders": [
    //  {"antId":1,"act":"move","dir":"down"},
    //  {"antId":17,"act":"load","dir":"up"}
    // ]}

  });

  // starting listen for http calls on port :7070
  svr.listen("0.0.0.0", 7070);
  return 0;
}

// this code available at https://github.com/anthive/cpp
// to test it localy, submit post request with payload.json using postman or curl
// curl -X 'POST' -d @payload.json http://localhost:7070

// have fun!

