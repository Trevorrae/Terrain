#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
//#include <cstdlib>

#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>	
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif

bool shading = true;
float camPos[] = {50, 50, 50};

int meshsize = 300;
bool draw = true;
int wireframe = 1;
bool light = false;
bool terrain = false;

//initalize the windows globally
int window1;
int window2;

float heightmap[300][300]; //the height map for the terrain
float highestPoint;
float lowestPoint;
float normal_set[300][300][3];//creates the array to store the normals for the vectors

//materials
float m_amb[] = {0.1745f,0.01175f,0.01175f,0.55f}; //sets the ambiant value for the materials
float m_diff[] = {0.49f,0.99f,0.0f,0.55f}; //sets the diffuse colour of the materials
float m_spec[] = {0.727811f,0.626959f,0.626959f,0.55f}; //sets the specualar values for the materials
float shiny = 76.8;//sets the shiny value for the materials

//lights
float light_pos[] = {-150.0,150.0,100.0,1.0};//sets the start position of the first light
float light_pos2[] = {150.0,150,-100,1.0};//sets the start position of the second light
float amb0[4] = { 1.0f, 1.0f, 1.0f, 1.0f };//creates a clear ambiant light
float diff0[4] = { 1.0f, 1.0f, 1.0f, 1.0f };//creates a clear diffuse light
float spec0[4] = { 1.0f, 1.0f, 1.0f, 1.0f };//create a clear specualar light


void normalValueFunction(){
    float holderOne[3];
    float holderTwo[3];
    float holderThree[3];
   
    for(int x =0; x < meshsize; x++){
        for(int z=0; z < meshsize; z++){
        	//initialises the holder 1 with the vectors with the hight map at value x,z
            holderOne[0] = x+1;
            holderOne[1] = heightmap[x+1][z] - heightmap[x][z];
            holderOne[2] = z;
            
            //initialises the holder 2 with the vectors with the hight map at value x,z
            holderTwo[0] = x+1;
            holderTwo[1] = heightmap[x+1][z+1] - heightmap[x][z];
            holderTwo[2] = z+1;
            
            //finds the cross products between the 3 vectors
            holderThree[0] = holderOne[1]*holderTwo[2] - holderOne[2]*holderTwo[1];
            holderThree[1] = holderOne[2]*holderTwo[0] - holderOne[0]*holderTwo[2];
            holderThree[2] = holderOne[0]*holderTwo[1] - holderOne[1]*holderTwo[0];
            float l = sqrtf(holderThree[0]*holderThree[0] + holderThree[1]*holderThree[1] + holderThree[2]*holderThree[2]);
            
            //sets the normal vectors
            normal_set[x][z][0] = holderThree[0]/l;
            normal_set[x][z][1] = holderThree[1]/l;
            normal_set[x][z][2] = holderThree[2]/l;
        }
    }
}

void createHeightCircle(){
    int random_x = rand()%meshsize;
    int random_z = rand()%meshsize;
    int random_radius = (rand()%20)+10;
    float disp = (rand()%12)+1;
    for(int x = 0; x<meshsize; x++){
        for(int z= 0; z<meshsize; z++){
            int dx = x - random_x;
            int dz = z - random_z;
            float distance = sqrtf((dx*dx) + (dz*dz));
            float pd = (distance*2)/random_radius;
            if (fabs(pd) <= 1.0){
                heightmap[x][z] += (disp/2.0) + (cos(pd*3.14)*(disp/2.0));
            }
        }
    }
    for(int i = 0; i < meshsize; i++){
    	for(int j = 0; j < meshsize; j++){
    		if(heightmap[i][j]>highestPoint){
    			highestPoint=heightmap[i][j];
    		}
    		if(heightmap[i][j]<lowestPoint){
    			lowestPoint=heightmap[i][j];
    		}
    	}
    }
  }	
  void topographicMap(int y, float greyscale){

	//for each different hight value for y changes the colour and shading of the topographicalMap
	if(y < 2){
		glColor3f(greyscale*0.49f,greyscale*0.99f,greyscale*0.0f);
	}
	else if(y < 4){
		glColor3f(greyscale*0.68f,greyscale*1.0f,greyscale*0.18f);
	}
	else if(y < 6){
		glColor3f(greyscale*0.20f,greyscale*0.80f,greyscale*0.20f);
	}
	else if(y < 8){
		glColor3f(greyscale*0.14f,greyscale*0.55f,greyscale*0.13f);
	}	
	else if(y < 10){
		glColor3f(0.0f,greyscale*0.5f,0.0f);
	}
	else if(y < 12){
		glColor3f(0.0f,greyscale*0.39f,0.0f);
	}
	else if(y < 14){
		glColor3f(greyscale*0.5f,0.0f,0.0f);
	}
	else if(y < 16){
		glColor3f(greyscale*0.67f,greyscale*0.13f,greyscale*0.13f);
	}
	else if(y < 18){
		glColor3f(greyscale*0.86f,greyscale*0.08f,greyscale*0.24f);
	}
	else if(y < 20){
		glColor3f(greyscale*1.0f,0.0f,0.0f);
	}
	else if(y < 22){
		glColor3f(greyscale*1.0f,greyscale*0.5f,greyscale*0.31f);
	}
	else if(y < 24){
		glColor3f(greyscale*1.0f,greyscale*0.27f,greyscale*0.0f);
	}
	else if(y < 26){
		glColor3f(greyscale*0.44f,greyscale*0.5f,greyscale*0.56f);
	}	

}
void drawMesh2D(){
	float i = 0;
	float j = 0;

	if(wireframe == 1 or wireframe == 3){//displays the lines if on line mode or both mode
		while(i < meshsize-1){
			j = 0;
			while(j < meshsize-1){
				float iI = meshsize/2;

				glColor3f(1.0f,1.0f,1.0f);//sets color to gray
				
				glBegin(GL_LINE_LOOP);//start drawing a line loop
	    			glVertex2f((i/iI)-1,(j/iI)-1);//left of window
	    	 		glVertex2f((i/iI)-1,((j+1)/iI)-1);//bottom of window
	    	  		glVertex2f(((i+1)/iI)-1,((j+1)/iI)-1);//right of window
	    			glVertex2f(((i+1)/iI)-1,(j/iI)-1);//top of window
	    		glEnd();//end drawing of line loop
			j++;
			}
		i++;	
		}
	}
	if(wireframe == 2 or wireframe == 3){//displays the polygons if in polygon mode or both mode
		for(int i=0; i<meshsize-1;i++){
			for(int j=0; j<meshsize-1; j++){
				float iI = meshsize/2;

				float greyscale = (((heightmap[i][j] - lowestPoint)/(highestPoint - lowestPoint))+0.08);
				glColor3f(greyscale,greyscale,greyscale);
			
				glBegin(GL_POLYGON);//start drawing a line loop
	    			glVertex2f((i/iI)-1,(j/iI)-1);//left of window
	    	 		glVertex2f((i/iI)-1,((j+1)/iI)-1);//bottom of window
	    	  		glVertex2f(((i+1)/iI)-1,((j+1)/iI)-1);//right of window
	    			glVertex2f(((i+1)/iI)-1,(j/iI)-1);//top of window
	    		glEnd();//end drawing of line loop
			}
		}
	}
}

void drawMesh(){

	if(wireframe == 1 or wireframe == 3){//displays the lines if on line mode or both mode
		for(int i=0; i<meshsize-1;i++){
			for(int j=0; j<meshsize-1; j++){
				
				glColor3f(0.0f,1.0f,0.0f);//sets color to green
				
				glBegin(GL_LINE_LOOP);//start drawing a line loop

	    			glVertex3f(i,heightmap[i][j],j);//left of window
	    	 		glVertex3f(i,heightmap[i][j+1],j+1);//bottom of window
	    	  		glVertex3f(i+1,heightmap[i+1][j+1],j+1);//right of window
	    			glVertex3f(i+1,heightmap[i+1][j],j);//top of window
	    		glEnd();//end drawing of line loop
			}
		}
	}
	if(wireframe == 2 or wireframe == 3){//displays the polygons if in polygon mode or both mode
		for(int i=0; i<meshsize-1;i++){
			for(int j=0; j<meshsize-1; j++){
				if(terrain==true){
					float greyscale = (((heightmap[i][j] - lowestPoint)/(highestPoint - lowestPoint))+0.08); //greyscale function to initalise the greyscale
					topographicMap(heightmap[i][j], greyscale);//calls the topographical map function passing in the current point and the greyscale value
				}
				else{
					float greyscale = (((heightmap[i][j] - lowestPoint)/(highestPoint - lowestPoint))+0.08);
					glColor3f(greyscale,greyscale,greyscale);
				}
				glBegin(GL_POLYGON);//start drawing a polygon loop
	    			glNormal3fv(normal_set[i][j]);
	    			glVertex3f(i,heightmap[i][j],j);//left of window
	    			glNormal3fv(normal_set[i][j+1]);
	    	 		glVertex3f(i,heightmap[i][j+1],j+1);//bottom of window
	    			glNormal3fv(normal_set[i+1][j+1]);
	    	  		glVertex3f(i+1,heightmap[i+1][j+1],j+1);//right of window
	    			glNormal3fv(normal_set[i+1][j]);
	    			glVertex3f(i+1,heightmap[i+1][j],j);//top of window
	    		glEnd();//end drawing of polygon loop
			}
		}
	}
}






void keyboard(unsigned char key, int x, int y){
	int i = 0;

	/* key presses move the cube, if it isn't at the extents (hard-coded here) */
	switch (key)
	{
		case 'q':
		case 27:
			exit (0);
			break;
		case 'w':
		case 'W':
			if(wireframe ==3 ){
				wireframe = 1; //changes between wireframe, polygons and polygones with wireframe
			}
			else{
				wireframe++;
			}
			glutPostRedisplay();
			break;

		case 'r':
		case 'R':
			for(int x=0; x<300;x++){
				for(int y=0; y<300;y++){
					heightmap[x][y]=0; //resets and creates the mesh
				}
			}
			while(i<400){
				createHeightCircle();
				i++;
			}
			break;
		case 'k':
			if (shading==true){
				glShadeModel(GL_FLAT);//turns on shading
				shading=false;
			}
			else{
				glShadeModel(GL_SMOOTH);
				shading=true;
			}
			break;
		case 's':
		case 'S':
			light_pos[0]-=10;
			light_pos2[0] -=10;//sfedcv move the lights around
			break;

		case 'f':
		case 'F':
			light_pos[0] +=10;
			light_pos2[0]+=10;
			break;

		case 'e':
		case 'E':
			light_pos[2] -=10;
			light_pos2[2] -= 10;
			break;

		case 'd':
		case 'D':
			light_pos[2] +=10;
			light_pos2[2] += 10;
			break;
		
		case 'c':
		case 'C':
			light_pos[1] += 10;
			light_pos2[1] += 10;
			break;

		case 'v':
		case 'V':
			light_pos[1] -= 10;
			light_pos2[1] -= 10;
			break;
		case 'l':
		case 'L':
			if(light == true){
				light = false;//turns lights on or off
			}
			else{
				light = true;
			}
			break;
		case 't':
		case 'T':
			if(terrain == true){
				terrain = false; //turns on terrrain mode				
			}
			else{
				terrain = true;
				wireframe=2;				
			}
		default:

			break;

	}

	glutPostRedisplay();
}
void special(int key, int x, int y)
{
	// sets the keys to move around the camera 
	switch(key)
	{
		case GLUT_KEY_LEFT:
			camPos[0]-=1;
			break;

		case GLUT_KEY_RIGHT:
			camPos[0]+=1;
			break;

		case GLUT_KEY_UP:
			camPos[2] -= 1;
			break;

		case GLUT_KEY_DOWN:
			camPos[2] += 1;
			break;
		
		case GLUT_KEY_HOME:
			camPos[1] += 1;
			break;

		case GLUT_KEY_END:
			camPos[1] -= 1;
			break;


	}

	draw = false;
	glutPostRedisplay();
}



void menu(int value){
	int i =0;
	switch(value){
		case 1:
			meshsize = 50;	//changes the mesh size to 50
			break;
		case 2:
			meshsize = 150; //changes the mesh size to 150
			break;
		case 3:
			meshsize = 300; //changes the mesh size to 300
			break;
		case 4:
			while(i < 400){
				createHeightCircle(); //runs the create height circle algorithm to modify the height map value
				i++;
			}
			break;
		case 5:
			if(light == true){
				light = false; //turns lighting on or off
			}
			else{
				light = true;
			}
			break;
		case 6:
			meshsize = 100;
			break;
		case 7:
			meshsize = 200;
			break;
		case 8:
			meshsize = 250;
			break;
		default:
			break;


	}

}

void initMenu(){
	//creates the menu
	glutCreateMenu(menu);
	glutAddMenuEntry("Start Circle", 4); //adds the start circle button which creates the hight map mesh
	glutAddMenuEntry("Turn On Lights", 5); //adds the turn on lights button which changes the light boolean to turn lights on
	glutAddMenuEntry("50x50 mesh",1);	//adds the 50x50 mesh button to change the size of the mesh to 50x50
	glutAddMenuEntry("100x100 mesh", 6); //adds the 100x100 mesh button to change the size of the mesh to 100x100
	glutAddMenuEntry("150x150 mesh",2); //adds the 150x150 mesh button to change the size of the mesh to 150x150
	glutAddMenuEntry("200x200 mesh", 7); //adds the 200x200 mesh button to change the size of the mesh to 200x200
	glutAddMenuEntry("250x250 mesh", 8); //adds the 250x250 mesh button to change the size of the mesh to 250x250
	glutAddMenuEntry("300x300 mesh",3); //adds the 300x300 mesh button to change the size of the mesh to 300x300



	glutAttachMenu(GLUT_RIGHT_BUTTON);

}
void init(void)
{
	glClearColor(0, 0, 0, 0);
	glColor3f(1, 1, 1);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0); //initalizes lighting and sets up the initalisation parameters
	glEnable(GL_LIGHT1); 

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1, 1, 1000);
}
void init2(void)
{
	glClearColor(0, 0, 0, 0);
	glColor3f(1, 1, 1);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(45, 1, 1, 1000);
}

void display()
{
	//float origin[3] = {0,0,0};
	glutSetWindow(window1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if(light == true){
		glEnable(GL_LIGHTING);
	}
	else{
		glDisable(GL_LIGHTING);
	}
	//glDisable(GL_LIGHTING)
	

	gluLookAt(camPos[0], camPos[1], camPos[2], 0,0,0, 0,1,0);
	glColor3f(1,1,1);
	
	//calls the light parameters set at top and runs them in display for every time display is run	
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos); 
    glLightfv(GL_LIGHT0, GL_AMBIENT, amb0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diff0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spec0); 
	//does the same for the 2nd light
	glLightfv(GL_LIGHT1, GL_POSITION, light_pos2); 
    glLightfv(GL_LIGHT1, GL_AMBIENT, amb0);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diff0);
	glLightfv(GL_LIGHT1, GL_SPECULAR, spec0); 
	//does the same but for the materials
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m_amb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m_diff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m_spec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny); 

	normalValueFunction();
	drawMesh();
	glutSwapBuffers();
}
void display2()
{
	//float origin[3] = {0,0,0};
	glutSetWindow(window2);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	//glColor3f(1,1,1);
		

	drawMesh2D();
	glutSwapBuffers();
}
void idle(){
	glutSetWindow(window1);
	glutPostRedisplay();
	glutSetWindow(window2);
	glutPostRedisplay();
}
void motion(int x, int y){

}
void passive(int x, int y){
	// /printf("passive: %i,%i\n", x, y);

}
void mouse(int btn, int state, int x, int y){
		if(btn==GLUT_LEFT_BUTTON && state==GLUT_DOWN){
			//for(int i = 0;i<=300; i++){
				//for(int j = 0; j<=300;j++){
				float current = heightmap[x][y];
				current = current +10;
				heightmap[x][y] = current;				
				glutPostRedisplay();

/*				drawMesh2D();
				drawMesh();*/
				//}
			//}
	}
}

void glutCallBacks(){
	glutDisplayFunc(display);	//registers "display" as the display callback function
	glutKeyboardFunc(keyboard); //registers "keyboard" as the keyboard callback function
	glutSpecialFunc(special);	//registers "special" as the special function callback
	initMenu();	//registers "initMenu" as the menu call back function
	glutIdleFunc(idle);
}



void glutCallBacks2(){
	glutDisplayFunc(display2);	//registers "display" as the display callback function
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutPassiveMotionFunc(passive);

	//glutKeyboardFunc(keyboard);
	//glutSpecialFunc(special);
	//initMenu();

}
int main(int argc, char** argv){
	
	//initalizes the glut program
	glutInit(&argc, argv);	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	
	//initalizes the window
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(100, 100);
	window1 = glutCreateWindow("Terrain 3D");	//creates the window
	init();
	glutCallBacks();


	glutInitWindowSize(500,500);
	glutInitWindowPosition(1000,300);
	window2 = glutCreateWindow("Terrain 2D");
	init2();
	glutCallBacks2();

	glEnable(GL_DEPTH_TEST);
	printf("To enter Topographical mode press t \n to alternate between wireframe, polygon and both use w \n to turn on shading use k \n to move camera around its axies use the arrow keys and num lock and end \n to turn on lights use l to move lights use s d f e c v around its axies \n to create and reset the mesh use r \n to raise the mesh click anywhere on the 2d terrain box");

	//glutCallBacks();
	//glutCallBacks2();
	glutMainLoop();				//starts the event loop


	return 0;

}