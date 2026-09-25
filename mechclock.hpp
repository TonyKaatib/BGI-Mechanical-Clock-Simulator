#pragma once

#ifndef MECHCLOCK_HPP
#define MECHCLOCK_HPP

#include <graphics.h>

#define PI 3.14159265358979323846
#define BPD 86400

using namespace std;

int black_colour = COLOR(0, 0, 0);
int background_colour = COLOR(13, 19, 30);
int gray_colour = COLOR(64, 64, 64);

int panel_colour = COLOR(22, 31, 45);
int border_colour = COLOR(61, 79, 98);
int text_colour = COLOR(234, 237, 230);
int muted_colour = COLOR(150, 167, 181);
int gold_colour = COLOR(230, 184, 94);
int cyan_colour = COLOR(100, 206, 223);
int red_colour = COLOR(244, 118, 101);
int green_colour = COLOR(134, 215, 152);

string fmt(const char* format, double x) {
    char buffer[80];
    snprintf(buffer, sizeof(buffer), format, x);
    return buffer;
}

struct Mechanical_Clock {
	int beats = 0;
	int spring_beats = BPD;
	double fraction = 0.0;
	bool running() const{return spring_beats > 0;};
	double seconds() const{return beats + (running() ? fraction : 0.0);};
	double reserveHours() const{ return spring_beats / 3600.0;};
	
	void advance(double seconds_to_advance){
		if (!running() || seconds_to_advance <= 0.0) return;
		fraction += seconds_to_advance;
		const int requested = static_cast<int>(fraction);
		const int delivered = min(requested, spring_beats);
		beats += delivered;
		spring_beats -= delivered;
		fraction -= delivered;
		if (!running()){
			fraction = 0.0;
		}
	};
	void wind() { spring_beats = BPD; }
	void reset() { *this = Mechanical_Clock{};}
};

void drawLabel(int x, int y, const string& value, int color, int size = 1){
	if (value.empty()) return;
	setcolor(color);
	settextstyle(SIMPLEX_FONT, USER_CHAR_SIZE, size);
	settextjustify(LEFT_TEXT, TOP_TEXT);
		
	string copy = value;
	outtextxy(x, y, &copy[0]);
};


void drawBox(int x1, int y1, int x2, int y2){
	setfillstyle(SOLID_FILL, panel_colour);
	bar(x1, y1, x2, y2);
	setcolor(border_colour);
	rectangle(x1, y1, x2, y2);
};
	
void drawArrow(int x1, int x2, int y, int color){
	setcolor(color);
	line(x1, y, x2, y);
	line(x2, y, x2 - 9, y - 5);
	line(x2, y, x2 - 9, y + 5);
};

void drawGear(int cx, int cy, int radius, int teeth, double angle, int color){
	setfillstyle(SOLID_FILL, panel_colour);
	setcolor(color);
	fillellipse(cx, cy, radius - 3, radius - 3);
	circle(cx, cy, radius - 4);
	circle(cx, cy, 10);
	for (int i = 0; i < teeth; ++i){
		const double a = angle + 2.0 * PI * i / teeth;
		line(cx + static_cast<int>((radius - 4) * cos(a)),
            cy + static_cast<int>((radius - 4) * sin(a)),
            cx + static_cast<int>((radius + 4) * cos(a)),
            cy + static_cast<int>((radius + 4) * sin(a)));
	}
	for (int i = 0; i < 4; ++i) {
    const double a = angle + 2.0 * PI * i / 4;
    line(cx + static_cast<int>(11 * cos(a)),
        cy + static_cast<int>(11 * sin(a)),
        cx + static_cast<int>((radius - 12) * cos(a)),
        cy + static_cast<int>((radius - 12) * sin(a)));
    }
    setfillstyle(SOLID_FILL, color);
    fillellipse(cx, cy, 4, 4);
}
	
void drawHand(int cx, int cy, double angle, int length, int color, int thick){
	const int x = cx + static_cast<int>(length * sin(angle));
    const int y = cy - static_cast<int>(length * cos(angle));
    setcolor(color);
    setlinestyle(SOLID_LINE, 0, thick);
    line(cx, cy, x, y);
    setlinestyle(SOLID_LINE, 0, NORM_WIDTH);
};

void drawDial(const Mechanical_Clock& clock){
	const int x = 999, y = 324, radius = 152;
	setcolor(gold_colour);
	circle(x, y, radius);
	circle(x, y, radius - 5);
	for (int i = 0; i < 60; ++i){
		const double a = 2 * PI * i / 60;
		const int inner = i % 5 == 0 ? radius - 22 : radius - 12;
		line(x + static_cast<int>(inner * sin(a)),
			 y - static_cast<int>(inner * cos(a)),
			 x + static_cast<int>((radius - 7) * sin(a)),
			 y - static_cast<int>((radius - 7) * cos(a)));
	}
	drawLabel(x - 7, y - radius + 30, "12", text_colour);
	drawLabel(x + radius - 38, y - 5, "3", text_colour);
    drawLabel(x - 4, y + radius - 49, "6", text_colour);
    drawLabel(x - radius + 25, y - 5, "9", text_colour);
    	
    const double time = clock.seconds();
    drawHand(x, y, 2 * PI * time / 43200.0, 78, text_colour, THICK_WIDTH);
    drawHand(x, y, 2 * PI * time / 3600.0, 112, cyan_colour, THICK_WIDTH);
    drawHand(x, y, 2 * PI * time / 60.0, 125, red_colour, NORM_WIDTH);
    setfillstyle(SOLID_FILL, gold_colour);
    fillellipse(x, y, 5, 5);
		
	const int total = clock.beats % (12 * 3600);
    const int hours = total / 3600;
    const int minutes = total / 60 % 60;
    const int seconds = total % 60;
    char display[64];
    snprintf(display, sizeof(display), "%02d:%02d:%02d", hours, minutes, seconds);
    drawLabel(950, 500, display, text_colour, 2);
}

void drawFrame(const Mechanical_Clock& Clock, int speed, bool paused){
	setbkcolor(background_colour);
	cleardevice();
	
	drawLabel(24, 19, "Mechanical Clock", gold_colour, 2);
	drawLabel(25, 56, "1 pulse per second| 24h of winding | 1 pulse = 1s in visualizer", gray_colour);
	
	drawBox(20, 88, 796, 570);
    drawBox(812, 88, 1180, 570);
    drawBox(20, 585, 1180, 770);
    drawLabel(39, 106, "Energy Flow and Transmission", text_colour);
    drawLabel(833, 106, "Visualizer", text_colour);
    
    const double t = Clock.seconds();
    const double tick_time = static_cast<double>(Clock.beats);
    
    drawGear(115, 266, 54, 20, -tick_time * 2 * PI / 3600.0, gold_colour);
    drawGear(308, 266, 62, 30, tick_time * 2 * PI / 60.0, cyan_colour);
    drawGear(503, 266, 56, 24, tick_time * 2 * PI / 3600.0, gold_colour);
    drawGear(698, 266, 50, 20, tick_time * 2 * PI / 43200.0, green_colour);
    drawArrow(174, 237, 266, muted_colour);
    drawArrow(375, 442, 266, muted_colour);
    drawArrow(566, 637, 266, muted_colour);

    drawLabel(82, 346, "Spring", gold_colour);
    drawLabel(263, 346, "Seconds", cyan_colour);
    drawLabel(465, 346, "Minutes", gold_colour);
    drawLabel(669, 346, "Hours", green_colour);
    drawLabel(227, 370, "1 turn / 60 s", muted_colour);
    drawLabel(426, 370, "1 turn / 3600 s", muted_colour);
    drawLabel(625, 370, "1 turn / 12 h", muted_colour);
    
    const double escape_angle = 2 * PI * (Clock.beats % 15) / 15.0;
	drawGear(294, 469, 31, 15, escape_angle, red_colour);
	drawLabel(345, 442, "Escapement", red_colour);
    //drawLabel(345, 460, "1 pulse per beat", text_colour);
    drawLabel(345, 460, "Pulses: " + to_string(Clock.beats), muted_colour);
    	
    const double balance_angle = Clock.running() ? 0.62 * sin(PI * t) : 0.0;
    	
    const int bx = 657, by = 465;
    setcolor(cyan_colour);
    circle(bx, by, 31);
    drawHand(bx, by, balance_angle, 26, cyan_colour, THICK_WIDTH);
    drawLabel(619, 515, "0,5 Hz balance", cyan_colour);

	drawDial(Clock);
	
	drawLabel(840, 532, "Hours", text_colour, 2);
    drawLabel(930, 532, "Minutes", cyan_colour, 2);
    drawLabel(1045, 532, "Seconds", red_colour, 2);
    
    const int remaining = static_cast<int>(615.0 * Clock.spring_beats / BPD);
    setcolor(border_colour);
    rectangle(39, 630, 657, 654);
    if (remaining > 0) {
        setfillstyle(SOLID_FILL, green_colour);
        bar(41, 632, 40 + remaining, 652);
    };
    
    drawLabel(675, 634, fmt("%.2f h / 24.00 h", Clock.reserveHours()), text_colour);
    drawLabel(39, 680, "s -> min: (8/60)(10/80) = 1/60", text_colour);
    drawLabel(39, 700, "min -> h: (10/40)(10/30) = 1/12", text_colour);
    drawLabel(39, 733, "Space: Pause | +/-: Speed | W: Wind-Up | R: Reset | ESC: Exit", cyan_colour);

    drawLabel(855, 603, paused ? "Status: PAUSA" :
          (Clock.running() ? "Status: Ongoing" : "Status: No wind-up"),
          Clock.running() ? green_colour : red_colour);
    drawLabel(855, 630, "Speed: x" + std::to_string(speed), text_colour);
    drawLabel(855, 658, "Time occured: " + fmt("%.1f s", t), muted_colour);
};

#endif // !MECHCLOCK_HPP