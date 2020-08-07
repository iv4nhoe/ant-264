server: bot.cpp
	g++ -std=c++11 -pthread bot.cpp -o bot

clean:
	rm -rf bot
