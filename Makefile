all:
	g++ button.cpp MCP23017.cpp -I -li2c -o button
