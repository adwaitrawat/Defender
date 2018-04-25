//
//
//	Project Aldnoah Valvrave
//	By - Adwait Rawat
//
//
#include <iostream>
#include <cmath>
#include <string>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

using namespace std;

unsigned int times, timesl, timehk, timehkl, timev, timevl, timep, timepl, timer, timerl, timel, timell;
unsigned short int speed = 2, enemy_speed = 2, rounds = 1, sp = 1, tackel_speed = 2, delayed = 100;
unsigned short int side = 20, ptr = 1, thickness = 5, life = 3, lifep = life, paused = 0, retreat = 0;
unsigned short int none_count = 0, loc = 0, which;
unsigned short int tackel = 0, tackel_limit = 80, bullet_count = 0, bullet_limit = 5;
unsigned short const int n = 36, sn = 2000, font_size = 84;
unsigned short int sx[sn], sy[sn];

enum State:uint8_t {
	Enemy = 0,
	Player = 1,
	Enemy_Hit = 2,
	Hit_Player = 3,
	None = 4
};

class Thing {
	public:
	float x, y, py;
	int hit, bullet;
	int bx, by;
	State state;
	void Give(float x1, float y1, State s, int h, int b) { 
		x = x1;
		y = y1;
		py = y;
		state = s;
		hit = h;
		bullet = b;
	}
}E[n], V;

ALLEGRO_BITMAP *image = NULL;

void set_initial() {
	V.state = State::Player;
	speed = enemy_speed = tackel_speed = 2; rounds = 1; sp = 1; delayed = 100;
	life = lifep = 3, paused = 0, retreat = 0;
	none_count = 0;
	tackel = 0, tackel_limit = 80, bullet_count = 0, bullet_limit = 5;
}

void background(char color = 'w') {
	for (int i = 0; i < sn; i++) {
		if (color == 'w') al_draw_pixel(sx[i], sy[i], al_map_rgb(255,255,255));
		else if (color == 'b') al_draw_pixel(sx[i], sy[i], al_map_rgb(0,0,0));
	}
}

void display_player(float cx, float cy, State s = State::Player, int ms = 0,int scale = 1) {
	if (s == State::Player) {
		al_draw_filled_triangle(cx, cy - scale*side, cx - scale*side/2, cy + scale*side, cx + scale*side/2, cy + scale*side,al_map_rgb(0,0,255));
		if (V.hit == 0 || ms == 1) al_draw_triangle(cx, cy - scale*side, cx + scale*side/2, cy + scale*side, cx - scale*side/2, cy + scale*side, al_map_rgb(0,0,127), scale*thickness);
	}
	else if (s == State::Hit_Player) {
		al_draw_filled_triangle(cx, cy - side, cx + side/2, cy + side, cx - side/2, cy + side, al_map_rgb(255,0,255));
		al_draw_triangle(cx, cy - side, cx + side/2, cy + side, cx - side/2, cy + side, al_map_rgb(255,0,127), thickness);
		for (int delay = 0; delay < delayed; delay++) {}
	}
	else if (s == State::None)
		al_draw_filled_triangle(cx, cy - side, cx + side/2, cy + side, cx - side/2, cy + side, al_map_rgb(255,255,255));
}

void star_generate() {
	for (int i = 0; i < sn; i++) {
		sx[i] = rand()%(640 - thickness) + thickness;
		sy[i] = rand()%480;
	}
}

void star_move() {
	for (int i = 0; i < sn; i++) {
		sy[i] += sp;
		if (sy[i] >= 480){
			int tempy = rand()%240;
			int tempx = rand()%(640 - thickness) + thickness;
			sx[i] = tempx; sy[i] = tempy;
		}
	}
}

void draw_bullet(float bx, float by, State s, int r = 0, int g = 0, int b = 0, int thick = 3) {
	if (s == State::Player || s == State::Hit_Player)
		al_draw_line(bx, by - side/2, bx, by + side/2, al_map_rgb(r,g,b), thick);
	else
		al_draw_line(bx, by, bx, by + side, al_map_rgb(r,g,b), thick);
}

void display() {
	if (paused == 0) {
		al_clear_to_color(al_map_rgb(0,0,0));
		background();
		al_draw_line(0, 0, 0, 480, al_map_rgb(255,255,255), thickness);
		al_draw_line(640, 0, 640, 480, al_map_rgb(255,255,255), thickness);
		al_draw_line(0, 480, 640, 480, al_map_rgb(255,255,255), thickness);
		if (V.bullet == 1) {
			draw_bullet(V.bx, V.by, V.state, 255);
			V.by -= speed;
			if (V.by <= 0) V.bullet = 0;
		}
		if (E[which].y + side >= 480 - thickness && E[which].state != State::None) retreat = 1;
		if (retreat == 1 && E[which].py != E[which].y) {
			E[which].y -= 4;
			if (E[which].state != State::None) {
				if (rounds == 2) {
					if (E[which].py >= E[which].y && tackel >= tackel_limit) {
						tackel = 0;
						retreat = 0;
						E[which].y = E[which].py;
						which = rand()%n;
					}
				}
				else if (rounds == 3) {
					if (E[which].py >= E[which].y && tackel >= tackel_limit/2) {
						tackel = 0;
						retreat = 0;
						E[which].y = E[which].py;
						which = rand()%n;
					}
				}
			}
			else if (E[which].state == State::None) {
				tackel = 0;
				retreat = 0;
				E[which].y = E[which].py;
				which = rand()%n;
			}	
		}
		else if (E[which].y + side <= 480 && E[which].state == State::None) {
			retreat = 0;
			tackel = 0;
			E[which].y = E[which].py;
			which = rand()%n;
		}
		for (int i = 0; i < n; i++) {
			if (E[i].bullet == 1) {
				draw_bullet(E[i].bx, E[i].by, E[i].state, 255, 255);
				E[i].by += enemy_speed;
				if (E[i].by >= 480) {E[i].bullet = 0; if (bullet_count >= bullet_limit) bullet_count = 0;}
			}
		}
		star_move();
		if (V.state == State::Player) {
			display_player(V.x, V.y);
		}
		else if (V.state == State::Hit_Player) {
			display_player(V.x, V.y, State::Hit_Player);
			V.state = State::Player;
		}
		else if (V.state == State::None) {
			display_player(V.x, V.y, State::None);
		}
	
		for (int i = 0; i < n; i++) {
			if (E[i].state == State::Enemy) {
				al_draw_filled_circle(E[i].x, E[i].y, side, al_map_rgb(0,255,0));
				al_draw_circle(E[i].x, E[i].y, side, al_map_rgb(0,127,0), thickness);
				al_draw_line(E[i].x - side/2, E[i].y - side/2, E[i].x, E[i].y,al_map_rgb(0,127,0),thickness);
				al_draw_line(E[i].x, E[i].y, E[i].x + side/2, E[i].y - side/2,al_map_rgb(0,127,0),thickness);
				if (E[i].hit == 0) al_draw_arc(E[i].x, E[i].y + side, side, 0, -3.14, al_map_rgb(0,127,0), thickness);
				E[i].x = E[i].x + pow(-1,loc)*0.5;
				if (E[0].x <= 40) loc = 0;
				else if (E[0].x >= 120) loc = 1;
			}
			else if (E[i].state == State::Enemy_Hit) {
				al_draw_filled_circle(E[i].x, E[i].y, side, al_map_rgb(255,255,0));
				al_draw_circle(E[i].x, E[i].y, side, al_map_rgb(255,127,0), thickness);
				al_draw_line(E[i].x - side/2, E[i].y - side/2, E[i].x, E[i].y,al_map_rgb(255,127,0),thickness);
				al_draw_line(E[i].x, E[i].y, E[i].x + side/2, E[i].y - side/2,al_map_rgb(255,127,0),thickness);
				E[i].x = E[i].x + pow(-1,loc)*0.5;
				if (E[0].x <= 40) loc = 0;
				else if (E[0].x >= 120) loc = 1;
				for (int delay = 0; delay < delayed; delay++) {}
				E[i].state = State::Enemy;
			}
			else if (E[i].state == State::None) {
				E[i].x = E[i].x + pow(-1,loc)*0.5;
				if (E[0].x <= 40) loc = 0;
				else if (E[0].x >= 120) loc = 1;
			}
		}
		al_flip_display();
	}
}

void menu_display(ALLEGRO_FONT *fnt) {
	al_clear_to_color(al_map_rgb(255,255,255));
	background('b');
	al_draw_text(fnt, al_map_rgb(0,0,0), 320, 240 - 3*font_size/2, ALLEGRO_ALIGN_CENTRE, "START");
	al_draw_text(fnt, al_map_rgb(0,0,0), 320, 240 + font_size/2, ALLEGRO_ALIGN_CENTRE, "QUIT");

	if (ptr == 1) {
		al_draw_rectangle(320 - 2*font_size - thickness, 240 - 3*font_size/2, 320 + 2*font_size + thickness, 240 - font_size + 3*side/2, al_map_rgb(0,0,0), thickness);
	}
	else if (ptr == 0) {
		al_draw_rectangle(320 - 3*font_size/2 + thickness, 240 + font_size/2, 320 + 3*font_size/2 - thickness, 240 + 3*font_size/2 - side/2, al_map_rgb(0,0,0), thickness);
	}
	star_move();
	al_flip_display();
}

void dbndd(ALLEGRO_FONT *fnt) {
	al_clear_to_color(al_map_rgb(255,255,255));
	if (none_count == n && rounds == 3) { 
		image = al_load_bitmap("Pic/V (copy).jpg");
		al_draw_bitmap(image,thickness,0,0);
		al_rest(2.0);
	}
	else if (life == 0) {
		al_clear_to_color(al_map_rgb(0,0,0));
		image = al_load_bitmap("Pic/Defeat (copy).jpg");
		al_draw_bitmap(image,thickness,0,0);
		al_rest(2.0);
	}
	else {
		int scale = 2;

		display_player(320 - (2 + scale + 0.5)*side, 240, V.state, 1, scale);
		al_draw_line(320 - scale*side, 240 - scale*side, 320 + scale*side, 240 + scale*side, al_map_rgb(255,0,0), thickness);
		al_draw_line(320 + scale*side, 240 - scale*side, 320 - scale*side, 240 + scale*side, al_map_rgb(255,0,0), thickness);

		switch (life) {
			case 1: {
				al_draw_text(fnt, al_map_rgb(0,0,255), 320 + 5*side, 240 - font_size/2 - side/2,ALLEGRO_ALIGN_CENTRE, "1");
				break;
			}
			case 2: {
				al_draw_text(fnt, al_map_rgb(0,0,255), 320 + 5*side, 240 - font_size/2 - side/2,ALLEGRO_ALIGN_CENTRE, "2");
				break;
			}
			case 3: {
				al_draw_text(fnt, al_map_rgb(0,0,255), 320 + 5*side, 240 - font_size/2 - side/2,ALLEGRO_ALIGN_CENTRE, "3");
				break;
			}
		}
	}
	al_flip_display();
	al_rest(1.0);
}

void hit_check() {	
	float TA1 = 1, TA2 = -1;
	
	if (E[which].x <= V.x)
		TA1 = (((V.x-side/2)*(E[which].y+side/(sqrt(2)))-(V.y+side)*(E[which].x+side/(sqrt(2))))-((V.x)*(E[which].y+side/(sqrt(2)))-(E[which].x+side/(sqrt(2)))*(V.y-side))+((V.x)*(V.y+side)-(V.y-side)*(V.x-side/2)))/2;
	else if (E[which].x >= V.x)
		TA2 = (((V.x+side/2)*(E[which].y+side/(sqrt(2)))-(V.y+side)*(E[which].x-side/(sqrt(2))))-((V.x)*(E[which].y+side/(sqrt(2)))-(E[which].x-side/(sqrt(2)))*(V.y-side))+((V.x)*(V.y+side)-(V.y-side)*(V.x+side/2)))/2;
	
	if (E[which].y + side >= V.y - side) {
		if (TA1 <= 0 || TA2 >= 0) {
			E[which].state = State::Enemy_Hit;
			E[which].hit++;
			if (E[which].hit >= 2){E[which].state = State::None; none_count++;}
			V.state = State::Hit_Player;
			V.hit++;
			if (V.hit >= 2) {V.state = State::None; if(life != 0) life--;}
			retreat = 1;
		}
	}
	
	for (int i = 0; i < n; i++) {
		if (E[i].state != State::None) {
			float db = sqrt((V.bx - E[i].x)*(V.bx - E[i].x)+(V.by + side/2 - E[i].y)*(V.by + side/2 - E[i].y));
			if ( db <= side + thickness) {
				V.bullet = 0;
				E[i].state = State::Enemy_Hit;
				E[i].hit++;
				V.bx = V.x; V.by = V.y - side;
				if (E[i].hit >= 2){E[i].state = State::None; none_count++;}
			}
		}
		
		float A1, A2;
		if (V.hit == 0) {		
			A1 = (((V.x - side/2 - thickness)*(E[i].by) - (V.y + side)*(E[i].bx)) - ((V.x)*(E[i].by) - (E[i].bx)*(V.y - side)) + ((V.x)*(V.y + side) - (V.y - side)*(V.x - side/2 + thickness)))/2;
			A2 = (((V.x + side/2 - thickness)*(E[i].by) - (V.y + side)*(E[i].bx)) - ((V.x)*(E[i].by) - (E[i].bx)*(V.y - side)) + ((V.x)*(V.y + side) - (V.y - side)*(V.x + side/2 + thickness)))/2;
		}
		
		else if (V.hit == 1) {
			A1 = (((V.x - side/2)*(E[i].by) - (V.y + side)*(E[i].bx)) - ((V.x)*(E[i].by) - (E[i].bx)*(V.y - side)) + ((V.x)*(V.y + side) - (V.y - side)*(V.x - side/2)))/2;
			A2 = (((V.x + side/2)*(E[i].by) - (V.y + side)*(E[i].bx)) - ((V.x)*(E[i].by) - (E[i].bx)*(V.y - side)) + ((V.x)*(V.y + side) - (V.y - side)*(V.x + side/2)))/2;
		}
		
		if (E[i].bx >= V.x - side/2 + thickness && E[i].bx <= V.x + side/2 + thickness) {
			if (A1 <= 0 || A2 >= 0) {
				 if (E[i].by + side/2 <= V.y + side && E[i].by + side/2 >= V.y - side) {
					E[i].bullet = 0;
					V.state = State::Hit_Player;
					E[i].bx = E[i].x; E[i].by = E[i].y + side;
					V.hit++;
					if (V.hit >= 2) {V.state = State::None; if(life != 0) life--;}
				}
			}
		}
	}
}

void enemy_bullet() {
	int you;
	you = rand()%n;
	
	for (int i = 0; i < n && E[i].bullet == 0; i++) {
		E[i].bx = E[i].x;
		E[i].by = E[i].y + side;
	}
	
	if (E[you].bullet == 0 && bullet_count == bullet_limit) {
		if (E[you].state != State::None) {
			E[you].bullet = 1;
			E[you].bx = E[you].x;
			E[you].by = E[you].y + side;
			bullet_count = 0;
		}
		else {enemy_bullet(); bullet_count = 0;}
	}
}

void round_display(int rnd, ALLEGRO_FONT *fnt) {
	al_clear_to_color(al_map_rgb(255,255,255));
	if (life != 0) {
		switch (rnd) {
			case 1:{
				al_draw_text(fnt, al_map_rgb(255,0,0), 320, 240 - font_size/2,ALLEGRO_ALIGN_CENTRE, "1st Wave");
				break;
			}
			case 2:{
				al_draw_text(fnt, al_map_rgb(255,0,0), 320, 240 - font_size/2,ALLEGRO_ALIGN_CENTRE, "2nd Wave");
				break;
			}
			case 3:{
				al_draw_text(fnt, al_map_rgb(255,0,0), 320, 240 - font_size/2,ALLEGRO_ALIGN_CENTRE, "3rd Wave");
				break;
			}
		}
		al_flip_display();
		al_rest(1.0);
	}
}

void reset(int dornot) {
	V.Give(320, 420, State::Player, 0, 0);
	paused = 0;
	bullet_count = 0;

	if (dornot == 1 || none_count == n){
		for (int i = 0; i < 9; i++)
			E[i].Give(i*60 + 40, 60, State::Enemy, 0, 0);
	
		for (int i = 9; i < 18; i++)
			E[i].Give(E[i - 9].x, 120, State::Enemy, 0, 0);

		for (int i = 18; i < 27; i++)
			E[i].Give(E[i - 18].x, 180, State::Enemy, 0, 0);
	
		for (int i = 27; i < 36; i++)
			E[i].Give(E[i - 27].x, 240, State::Enemy, 0, 0);
		tackel = 0;
	}

	V.bullet = 0;
	V.bx = V.x; V.by = V.y - side;
	
	for (int i = 0; i < n; i++) {
		E[i].bullet = 0;
		E[i].bx = E[i].x;
		E[i].by = E[i].y + side;
	}
	
	for (int i = 0; i < sn; i++) {
		int tempx = rand()%(640 - thickness) + thickness;
		int tempy = rand()%(480 - thickness);
		sx[i] = tempx; sy[i] = tempy;
	}
	speed = 2;
}

void enemy_tackel() {
	if (rounds == 2) {
		if (tackel == tackel_limit) {
			which = rand()%n;
		}
		if (retreat == 0 && tackel >= tackel_limit) {E[which].y += tackel_speed;}
	}
	else if (rounds == 3) {
		if (tackel == tackel_limit/2) {
			which = rand()%n;
		}
		if (retreat == 0 && tackel >= tackel_limit/2) {E[which].y += tackel_speed;}
	}
}

void retry_menu(ALLEGRO_FONT *fnt) {
	if (life != 0) { al_clear_to_color(al_map_rgb(255,255,255)); image = al_load_bitmap("Pic/V (copy).jpg"); background('b');}
	else { al_clear_to_color(al_map_rgb(0,0,0)); image = al_load_bitmap("Pic/Defeat (copy).jpg"); background();}
	al_draw_bitmap(image,thickness,-120,0);
	if (life != 0) {
		al_draw_text(fnt, al_map_rgb(0,0,0), 320, 360 - 3*font_size/2, ALLEGRO_ALIGN_CENTRE, "PLAY AGAIN");
		al_draw_text(fnt, al_map_rgb(0,0,0), 320, 360 + font_size/2, ALLEGRO_ALIGN_CENTRE, "QUIT");
	}
	else {
		al_draw_text(fnt, al_map_rgb(255,255,255), 320, 360 - 3*font_size/2, ALLEGRO_ALIGN_CENTRE, "RETRY");
		al_draw_text(fnt, al_map_rgb(255,255,255), 320, 360 + font_size/2, ALLEGRO_ALIGN_CENTRE, "QUIT");
	}

	if (ptr == 1) {
		if (life != 0) al_draw_rectangle(320 - 7*font_size/2 - thickness, 360 - 3*font_size/2, 320 + 7*font_size/2 + thickness, 360 - font_size + 3*side/2, al_map_rgb(0,0,0),  thickness);
		else al_draw_rectangle(320 - 2*font_size - thickness, 360 - 3*font_size/2, 320 + 2*font_size + thickness, 360 - font_size + 3*side/2, al_map_rgb(255,255,255),  thickness);
	}
	else if (ptr == 0) {
		if (life != 0) al_draw_rectangle(320 - 3*font_size/2 + thickness, 360 + font_size/2, 320 + 3*font_size/2 - thickness, 360 + 3*font_size/2 - side/2, al_map_rgb(0,0,0),  thickness);
		else al_draw_rectangle(320 - 3*font_size/2 + thickness, 360 + font_size/2, 320 + 3*font_size/2 - thickness, 360 + 3*font_size/2 - side/2, al_map_rgb(255,255,255),  thickness);
	}
	star_move();
	al_flip_display();
}

int menu_keyboard(ALLEGRO_KEYBOARD_STATE &key) {
	al_get_keyboard_state(&key);
	if (al_key_down(&key,ALLEGRO_KEY_UP) && ptr == 0) ptr += 1;
	if (al_key_down(&key,ALLEGRO_KEY_DOWN) && ptr == 1) ptr -= 1;
	if (al_key_down(&key,ALLEGRO_KEY_ENTER) || al_key_down(&key,ALLEGRO_KEY_PAD_ENTER)) if (ptr == 1) return 0; else if (ptr == 0) return 1;
	return -1;
}

bool keyboard(ALLEGRO_KEYBOARD_STATE &key) {
	al_get_keyboard_state(&key);
	if (al_key_down(&key,ALLEGRO_KEY_ESCAPE)) return true;
	if (paused == 0) {
		if (al_key_down(&key,ALLEGRO_KEY_LEFT) && V.x - side - thickness > 0) V.x -= 2;
		if (al_key_down(&key,ALLEGRO_KEY_RIGHT) && V.x + side + thickness < 640) V.x += 2;
		if (al_key_down(&key,ALLEGRO_KEY_SPACE) && V.bullet == 0) times += 1;
		if (!al_key_down(&key,ALLEGRO_KEY_SPACE) && V.bullet == 0) {timesl = times; times = 0;}
		if (timesl <= 15 && timesl > 0) {V.bullet = 1; V.bx = V.x; V.by = V.y - side; timesl = 0;}
		if (al_key_down(&key,ALLEGRO_KEY_D)) {for (int i = 0; i < n; i++){E[i].state = State::None;} none_count = n;}
		if (al_key_down(&key,ALLEGRO_KEY_I)) {speed++;}
		if (al_key_down(&key,ALLEGRO_KEY_H)) timehk += 1;
		if (!al_key_down(&key,ALLEGRO_KEY_H)) {timehkl = timehk; timehk = 0;}
		if (timehkl <= 15 && timehkl > 0) {if(life != 0)life--; timehkl = 0; V.state = State::None;}
		if (al_key_down(&key,ALLEGRO_KEY_V)) timev += 1;
		if (!al_key_down(&key,ALLEGRO_KEY_V)) {timevl = timev; timev = 0;}
		if (timevl <= 15 && timevl > 0) {rounds = 3; none_count = n; timevl = 0;}
		if (al_key_down(&key,ALLEGRO_KEY_L)) timel += 1;
		if (!al_key_down(&key,ALLEGRO_KEY_L)) {timell = timel; timel = 0;}
		if (timell <= 15 && timell > 0) {life = 0;  timell = 0; V.state = State::None;}
	}
	if (al_key_down(&key,ALLEGRO_KEY_R)) timer += 1;
	if (!al_key_down(&key,ALLEGRO_KEY_R)) {timerl = timer; timer = 0;}
	if (timerl <=15 && timerl > 0) {timerl = 0; life++; V.state = State::Player; reset(0);}
	if (al_key_down(&key,ALLEGRO_KEY_P)) {timep += 1;}
	if (!al_key_down(&key,ALLEGRO_KEY_P)) {timepl = timep; timep = 0;}
	if (timepl <= 99999 && timepl > 0) {if (paused == 0) paused = 1; else if (paused == 1) paused = 0; timepl = 0;}
	return false;
}

int retry_keyboard(ALLEGRO_KEYBOARD_STATE &key) {
	al_get_keyboard_state(&key);
	if (al_key_down(&key,ALLEGRO_KEY_UP) && ptr == 0) ptr += 1;
	if (al_key_down(&key,ALLEGRO_KEY_DOWN) && ptr == 1) ptr -= 1;
	if (al_key_down(&key,ALLEGRO_KEY_ENTER) || al_key_down(&key,ALLEGRO_KEY_PAD_ENTER)) if (ptr == 1) return 0; else if (ptr == 0) return 1;
	return -1;
}

int main() {

	bool quit = false;

	ALLEGRO_DISPLAY *screen = NULL;
	ALLEGRO_KEYBOARD_STATE keystate;
	ALLEGRO_SAMPLE *menu = NULL;
	ALLEGRO_SAMPLE_ID menu_id;
	ALLEGRO_FONT *font = NULL, *retry_font = NULL, *menu_font = NULL, *round_font = NULL, *dbndd_font = NULL;

	if(!al_init()) {
		al_show_native_message_box(screen, "Allegro", "Error", "Failed to initialize Allegro", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return 0;
	}

	screen = al_create_display(640, 480);

	if(!screen) {
		al_show_native_message_box(screen, "Allegro", "Error", "Failed to Create Display", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return 0;
	}
	
	if(!al_init_primitives_addon()) {
		al_show_native_message_box(screen, "Allegro", "Error", "Failed to initialize Primitives", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return 0;
	}
	
	if(!al_install_keyboard()) {
		al_show_native_message_box(screen, "Allegro", "Error", "Failed to install kayboard", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return 0;
	}

	if(!al_install_audio()) {
		al_show_native_message_box(screen, "Allegro", "Error", "Failed to install audio", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return 0;
	}
	
	if(!al_init_acodec_addon()) {
		al_show_native_message_box(screen, "Allegro", "Error", "Failed to initialize audio codecs", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return 0;
	}

	if(!al_reserve_samples(1)) {
		al_show_native_message_box(screen, "Allegro", "Error", "Failed to reserve samples", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return 0;
	}
	
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_image_addon();

	menu = al_load_sample("Audio/19_AcyOrt_Mild.ogg");

	if(!menu) {
		al_show_native_message_box(screen, "Allegro", "Error", "Failed to Create load audio", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return 0;
	}

	font = al_load_ttf_font("Fonts/orbitron-light.ttf",3*font_size/2,0 );
	retry_font = al_load_ttf_font("Fonts/orbitron-light.ttf", font_size, 0);
	round_font = al_load_ttf_font("Fonts/orbitron-light.ttf", font_size, 0);
	dbndd_font = al_load_ttf_font("Fonts/orbitron-light.ttf",3*font_size/2, 0);
	menu_font = al_load_ttf_font("Fonts/orbitron-light.ttf", font_size, 0);
	
	if(!font) {
		al_show_native_message_box(screen, "Allegro", "Error", "Failed to Load Font", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return 0;
	}

	star_generate();
	
	al_play_sample(menu, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &menu_id);
	
	for (int i = 0; i < 150; i++) {
		al_clear_to_color(al_map_rgb(0,0,0));
		star_move();
		background();
		al_draw_text(font, al_map_rgb(255,255,255), 320 + thickness, 240 - 3*font_size/4,ALLEGRO_ALIGN_CENTRE, "Defender");
		al_flip_display();
	}

	while (!quit) {
		int in = menu_keyboard(keystate);
		if (in == 0) break;
		else if (in == 1) {quit = true; return 0;}
		
		menu_display(menu_font);
	}

Start:

	al_stop_sample(&menu_id);
	menu = al_load_sample("Audio/19_AcyOrt_Aggressive.ogg");
	
	if(!menu) {
		al_show_native_message_box(screen, "Allegro", "Error", "Failed to Create load audio", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return 0;
	}
	
	al_play_sample(menu, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &menu_id);

	reset(1);

	star_generate();
	
	while (rounds <= 3 && !quit && life != 0) {
		while (life != 0 && !quit && none_count != n) {
			cout<<"Round "<<rounds<<endl;
			cout<<"Life "<<life<<endl;

			if (!quit) {round_display(rounds, round_font); dbndd(dbndd_font); display(); al_rest(1.0);}

			while (!quit && none_count != n && V.state != State::None) {
				quit = keyboard(keystate);
				enemy_bullet();
				hit_check();
				if (rounds >= 2) enemy_tackel();

				display();
				bullet_count++;
				if (rounds >= 2 && paused != 1) tackel++;
			}
			if (rounds == 3 || life == 0) if (!quit) dbndd(dbndd_font);

			if (lifep != life) reset(0);
			else if (none_count == n)reset(1);
		}
		speed++;
		rounds++; none_count = 0; lifep = life; bullet_limit -= 2; sp++;
		if (rounds == 2) enemy_speed += 2;
		else if (rounds == 3) {enemy_speed += 1; tackel_speed += 2;}
	}

	al_stop_sample(&menu_id);
	menu = al_load_sample("Audio/19_AcyOrt_Mild.ogg");
	
	if(!menu) {
		al_show_native_message_box(screen, "Allegro", "Error", "Failed to Create load audio", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return 0;
	}
	
	al_play_sample(menu, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &menu_id);

	for (int location = 0; location >= -120 && !quit; location -= 2) {
		al_draw_bitmap(image, thickness, location, 0);
		al_flip_display();
	}

	if (life > 0 && !quit) cout<<"Victory!!!"<<endl;
	else if (V.state == State::None || life == 0) cout<<"Defeat!!!"<<endl;

	while(!quit) {
		short int retry = retry_keyboard(keystate);
		if (retry == 1) {quit = true; return 0;} 
		else if (retry == 0) {set_initial(); goto Start;}
		
		retry_menu(retry_font);
	}

	al_destroy_display(screen);
	al_destroy_sample(menu);
	al_destroy_font(font);
	return 0;
}
