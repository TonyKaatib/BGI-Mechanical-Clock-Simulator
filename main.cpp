#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include "mechclock.hpp"
#include <string>

int main(){
	initwindow(1200, 800, "Mechanical Clock - WinBGI", 0, 0, true);
	const int speeds[] = {1, 10, 60, 600, 3600};
	int speed_index;
	bool exit_requested = false;
	bool paused = false;
	Mechanical_Clock mechclock;	
    int page = 1;
    setactivepage(page);
    auto previous = std::chrono::steady_clock::now();
    
    while (!exit_requested) {
        while (kbhit()) {
        	const int key = getch();
            if (key == 27) exit_requested = true;
            else if (key == ' ') paused = !paused;
            else if (key == '+' || key == '=') speed_index = min(speed_index + 1, 4);
            else if (key == '-' || key == '_') speed_index = max(speed_index - 1, 0);
            else if (key == 'w' || key == 'W') mechclock.wind();
            else if (key == 'r' || key == 'R') mechclock.reset();
		};
		
		const auto now = std::chrono::steady_clock::now();
        const double real_seconds = std::chrono::duration<double>(now - previous).count();
        previous = now;
        if (!paused) {
            mechclock.advance(std::min(real_seconds, 0.10) * speeds[speed_index]);
        }
        
		drawFrame(mechclock, speeds[speed_index], paused);
		setvisualpage(page);
        page = 1 - page;
    	setactivepage(page);
        delay(16);
	};
	closegraph();
	return 0;
};