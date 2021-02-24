#include <iostream>
#include <string>
#include <vector>
#include "json.hpp"
#include "httplib.h"
#include <cassert>

using namespace std;
using namespace httplib;
using namespace nlohmann;

// const vector<string> ACTIONS = { "stay", "move", "eat", "take", "put" };
// const vector<string> DIRECTIONS = { "up", "down", "right", "left" };

class Map
{
public:
  Map(const json& pCells)
    :mCells(pCells)
  {
    assert(pCells.is_array());
  }

  std::string foodNearby(const json& pAnt)
  {
    assert(pAnt.is_object());
    const size_t y = pAnt["point"]["y"];
    const size_t x = pAnt["point"]["x"];
    assert(mCells.size() > y);
    assert(mCells[x].size() > x);

    if (y > 0 &&
        mCells[y-1][x].find("food") != mCells[y-1][x].end())
      return "up";

    if (x+1 < mCells[y].size() &&
        mCells[y][x+1].find("food") != mCells[y][x+1].end())
      return "right";

    if (y+1 < mCells.size() &&
        mCells[y+1][x].find("food") != mCells[y+1][x].end())
      return "down";

    if (x > 0 &&
        mCells[y][x-1].find("food") != mCells[y][x-1].end())
      return "left";

    return std::string();
  }

  bool canGoRight(const json& pAnt)
  {
    assert(pAnt.is_object());
    const size_t y = pAnt["point"]["y"];
    const size_t x = pAnt["point"]["x"];
    assert(mCells.size() > y);
    assert(mCells[y].size() > x);

    return x+1 < mCells[y].size() && mCells[y][x+1].empty();
  }

  bool canGoDown(const json& pAnt)
  {
    assert(pAnt.is_object());
    const size_t y = pAnt["point"]["y"];
    const size_t x = pAnt["point"]["x"];
    assert(mCells.size() > y);
    assert(mCells[y].size() > x);

    return y+1 < mCells.size() && mCells[y+1][x].empty();
  }

  bool canGoLeft(const json& pAnt)
  {
    assert(pAnt.is_object());
    const size_t y = pAnt["point"]["y"];
    const size_t x = pAnt["point"]["x"];
    assert(mCells.size() > y);
    assert(mCells[y].size() > x);

    return x > 0 && mCells[y][x-1].empty();
  }

  bool canGoUp(const json& pAnt)
  {
    assert(pAnt.is_object());
    const size_t y = pAnt["point"]["y"];
    const size_t x = pAnt["point"]["x"];
    assert(mCells.size() > y);
    assert(mCells[y].size() > x);

    return y > 0 && mCells[y-1][x].empty();
  }

private:
  json mCells;
};

int main(int argc, char* argv[])
{

  // Initialize the http server
  Server svr;
  svr.Post("/", [&](const Request& req, Response& res)
  {
    // Hive object from request payload
    auto hive = json::parse(req.body);
    Map tMap(hive["canvas"]["cells"]);
    // Loop through ants and give orders
    std::vector<json> orders;
    for (auto& ant : hive["ants"].items())
    {
      json order;
      order["antId"] = ant.value()["id"];

      std::string tDirection;
      if (!(tDirection = tMap.foodNearby(ant.value())).empty())
      {
        order["act"] = "eat";
        order["dir"] = tDirection;
      }
      else
      {
        order["act"] = "move";
        if (tMap.canGoRight(ant.value()))
          order["dir"] = "right";
        else if (tMap.canGoDown(ant.value()))
          order["dir"] = "down";
        else if (tMap.canGoLeft(ant.value()))
          order["dir"] = "left";
        else if (tMap.canGoUp(ant.value()))
          order["dir"] = "up";
        else
          order["act"] = "stay";
      }

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
