#include <stdio.h>
#include <cmath>
#include <stdlib.h>
#include <string.h>
#include <sstream>

#define PI 3.141592653589
#define DEG2RAD(deg) (deg * PI / 180)
#define epsilon 0.0001f
#define alpha 0.9999f

using namespace std;
std::string score1;

class Color
{
	public:
		float red, green, blue;
		Color() //constructor
		{
			red = 0.0f;
			green = 0.0f;
			blue = 0.0f;
		}

		void setColor(float r,float g,float b)
		{
			red = r;
			green = g;
			blue = b;
		}

};

class ScoreDisplay
{    
	public:
		char *s = strdup("Score");

		void output(float x, float y, const char *string) //to output text to screen
		{
			glColor3f( 1.0f, 1.0f, 1.0f );
			glRasterPos2f(x, y);
			int len, i;
			len = (int)strlen(string);
			for (i = 0; i < len; i++) 
			{
				glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, (int)string[i]);
			}

		}

		void printScore(int present_score)
		{  
			output(-0.4f,3.0f,s);
			ostringstream new_string; // integer number to string
			new_string<<present_score;
			score1 = new_string.str();
			output(-0.2f,2.7f,score1.data());
		}

};

class Box
{
	public:
		float side;   // Length of a box
		int fill;     // Just border = 0 or complete filling = 1
		Color col;

		void set_variables( float len,int t) // setting variables accordingly
		{
			side = len;
			fill = t;
		}

		float getSide()
		{
			return side;
		}

		void drawBox() 
		{

			glColor3f(col.red, col.green, col.blue);
			if(fill==0)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			else
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glBegin(GL_QUADS);
			glVertex2f(-side / 2, -side / 2);
			glVertex2f(side / 2, -side / 2);
			glVertex2f(side / 2, side / 2);
			glVertex2f(-side / 2, side / 2);
			glEnd();
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
};

class Line
{
	public:
		int state; // 0-> not visible 1-> visible
		float length;
		float angle;
		float x1, x2, y1, y2;

		Line()
		{
			length = 0.4f;
			angle = 0;
			state = 1;
		}

		void set_coordinates1(float a, float b)
		{
			x1 = a;
			y1 = b;
			x2 = a;
			y2 = b + length;
		}

		void setLength(float scale, float d)
		{
			length = d/scale*1.2f;
			if (length > 1.0f)
				length = 1.0f;
			if (length < 0.2f)
				length = 0.2f;
		}

		void updateLength(float a)
		{
			float temp = length + a;
			if ((temp <= 1.0f)&&(temp >= 0.2f))
			{
				length = temp;
				y2 += a;
			}
		}

		void updateAngle(float a)
		{
			angle += a;
		}

		void drawLine(float a, float b)
		{
			if (state == 0)
				return ;
			glColor3f(1.0, 0.0, 0.0);
			glPushMatrix();
			glTranslatef(a,b,0.0f);
			glRotatef(angle,  0,  0,  -1.0f);
			glTranslatef(-a,-b,0.0f);
			glBegin(GL_LINES);
			glVertex3f(x1, y1, 0.0);
			glVertex3f(x2, y2, 0.0);
			glEnd();
			glPopMatrix();
		}

		void resetLine(float a, float b)
		{
			angle = 0;
			length = 0.4f;
			state = 1;
			set_coordinates1(a,b);
		}
};

class Circle
{
	public:
		int fill; //0 ->no fill 1->fill
		float x, y, r;
		int visible; // 0->no 1-> yes
		Color col;
		float vx, vy, mass;

		Circle()
		{
			x = 0.0f;
			y = 0.0f;
			fill = 1;
			visible = 1;
			vx = 0.0f;
			vy = 0.0f;
		}

		void set_coordinates(float a, float b)
		{
			x = a;
			y = b;
		}

		void setFill(int a)
		{
			fill = a;
		}

		void setRad(float rad)
		{
			r = rad;
		}

		void drawCircle() 
		{ 
			glPushMatrix();
			glTranslatef(x, y, 0.0f);
			if (visible == 0)
				return ;
			glColor3f(col.red, col.green, col.blue);
			if (fill == 1)
				glBegin(GL_TRIANGLE_FAN);
			else
			{
				glLineWidth(1.8f);
				glBegin(GL_LINE_STRIP);
			}
			for(int i=0 ; i<360 ; i++) 
			{
				glVertex2f(r * cos(DEG2RAD(i)), r * sin(DEG2RAD(i)));
			}
			glEnd();
			glPopMatrix();
		}
		//From now motion parameters

		// update coordinates and velocities 
		void updateVelx(float a)
		{
			vx += a;
		}

		void updateVely(float b)
		{
			vy += b;
		}

		void updateCoodx()
		{
			x += vx;
		}

		void updateCoody()
		{
			y += vy;
		}

		void checkWall(float side)
		{
			if(x + r > side/2 || x - r < -side/2)
				vx *= -1 * alpha;
			if(y + r > side/2 || y - r < -side/2)
				vy *= -1 * alpha;
		}

		int updateVelocities()
		{
			//checking with threshold
			if (((vx <= epsilon)&&(vx >= -epsilon))&&((vy <= epsilon)&&(vy >= -epsilon)))
			{
				return 0;
			}

			// action of friction
			float v = sqrt(vx*vx +vy*vy);
			if ((vx >= epsilon)||(vx <= -epsilon))
			{
				updateVelx((-0.0002f*vx/v)/mass);
			}
			if ((vy >= epsilon)||(vy <= -epsilon))
			{
				updateVely((-0.0002f*vy/v)/mass);
			}
			return 1;
		}

		int checkCollision(Circle a)
		{
			float x1 = this -> x;
			float y1 = this -> y;
			float x2 = a.x;
			float y2 = a.y;
			float vx1 = this -> vx;
			float vy1 = this -> vy;
			float vx2 = a.vx;
			float vy2 = a.vy;
			float d = get_distance(x1, y1, x2, y2);
			float d1 = get_distance(x1+vx1, y1+vy1, x2+vx2, y2+vy2);   
			if ((d <= (a.r + this->r))&&(d1<d))		//not to disturb if they are seperating
			{
				return 1;
			}
			return 0;
		}

		float get_distance(float x1,float y1, float x2, float y2)
		{
			return sqrt(pow((x1-x2),2) + pow((y1-y2),2));
		} 

		void collisions(Circle *a)
		{
			float x_dis, y_dis, dis, temp1,temp2;
			float q1, q2, q11, q22;
			Circle c1 = *a;
			Circle c2 = *this;
			x_dis = c2.x - c1.x;
			y_dis = c2.y - c1.y;
			dis = sqrt(x_dis * x_dis + y_dis * y_dis);
			q2 = (x_dis * c1.vx + y_dis * c1.vy)/dis;
			q1 = (x_dis * c2.vx + y_dis * c2.vy)/dis;
			if (c2.mass>1.5f)
			{
				temp1 = (4*q1 - q2)/3;
				temp2 = (q2 + 2*q1)/3;
				q1 = temp1;
				q2 = temp2;
			}
			q11 = (y_dis * c1.vx - x_dis * c1.vy)/dis;
			q22 = (y_dis * c2.vx - x_dis * c2.vy)/dis;

			a -> vx=((q1*x_dis+q11*y_dis)/dis);
			a -> vy=((q1*y_dis-q11*x_dis)/dis);
			this -> vx=((q2*x_dis+q22*y_dis)/dis);
			this -> vy=((q2*y_dis-q22*x_dis)/dis);
		}

		int checkHole(Circle hole)
		{
			float x1 = hole.x;
			float y1 = hole.y;
			float x2 = this->x;
			float y2 = this->y;
			float d = get_distance(x1, y1, x2, y2);

			if (d <= (this->r))
				return 1;
			return 0;
		}
};

class Striker: public Circle
{
	public:
		int state; // 0->on the line 1-> on the board
		Line line;

		Striker()
		{
			state = 0;
			mass = 2.0f;
		}

		void resetStriker(float side)
		{
			state = 0; 
			set_coordinates(0.0f, -side/2 + r*7/8);
			vx = 0.0f;
			vy = 0.0f;
			line.resetLine(x,y);
		}

		void startMove()
		{
			float len = line.length;
			float ang = line.angle;
			updateVelx(len*sin(DEG2RAD(ang))*0.05f);
			updateVely(len*cos(DEG2RAD(ang))*0.05f);
			state = 1;
			line.state = 0;
		}

		void updateProperties(Box border, Box inner,int c) // border ~ in_box
		{
			if(state == 1)
			{
				checkWall(border.getSide());

				updateCoodx();
				updateCoody();
				state = updateVelocities();
				if((state == 0))
				{
					resetStriker(inner.getSide());
				}
			}
		}
};

class Coin: public Circle
{
	public:
		float originalx, originaly;
		int type; // 0 -> black, 1-> white, 2-> red

		Coin()
		{
			r = 0.1f;
			visible = 1;
			mass = 1.0f;
		}

		void setOriginalCood(float a,float b)
		{
			originalx = a;
			originaly = b;
		}

		void resetCoin()
		{
			x = originalx;
			y = originaly;
		}

		void enterHole(int * score)
		{
			int coin_score;
			if (type == 2)
				coin_score = 50; // red
			if (type == 1)
				coin_score = 10; // white
			if (type == 0)
				coin_score = -5; // black
			(*score) += coin_score;
			r = 0;
			vx = 0;
			vy = 0;

		}

		void updateProperties(Box border, int * c)
		{
			if(visible == 1)
			{
				checkWall(border.getSide());

				// Update position acc to velocity
				int state = updateVelocities();
				if (state == 0)
				{
					*c = *c + 1;
					vx = 0.0f;
					vy = 0.0f;
					return ;
				}
				updateCoodx();
				updateCoody();
			}
		}
};
