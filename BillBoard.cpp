#include "Billboard.h"
#include "PPMImage.h"


Billboard::Billboard(void)
{
}

Billboard::~Billboard(void)
{
}

void Billboard::ReadFile(string fileName)
{
	// Read the texture file, generate the texture object, and configure
	// the texturing parameters.

	this->textureImage.ReadFile(fileName);
			GLuint textureNum;
			glGenTextures(1, &textureNum);
			glBindTexture(GL_TEXTURE_2D, textureNum);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->textureImage.width, this->textureImage.height, 0,
				GL_RGB, GL_UNSIGNED_BYTE, this->textureImage.image);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
			this->textureNumber = textureNum;

}

void Billboard::SetSize(float width, float height)
{
	// Your code
	this->width = width; this->height = height;
}

void Billboard::SetLocation(Vector3 location)
{
	// Your code
	this->location = location;
}

void Billboard::SetOrientation(float orientation)
{
	// Your code
	this->orientation = orientation;
}

void Billboard::Draw()
{	
	// Draw the board and pillar.  Use texture mapping for the board only.
	// The pillar is drawn just using simple geometry, i.e., a rectangle.

	//Vector3 location={15.0,3.,15.0};
	//float orientation=180;
	//float width=7.0, height=4.5;
	GLUquadricObj *quadratic;
	quadratic = gluNewQuadric();
	glPushMatrix();
	glTranslatef(this->location.x, 0.0, this->location.z);


	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(quadratic, 0.1f, 0.1f, this->location.y, 32, 32);
	glPopMatrix();

	glPushMatrix();


	//glTranslatef(this->location.x,0.0,this->location.z);
	if (this->orientation == 90){


		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, this->textureNumber);
		glBegin(GL_QUADS);


		//front
		glTexCoord2f(1, 0);
		glVertex3f(this->location.x - .1, this->location.y, this->location.z + this->width / 2);

		glTexCoord2f(1, 1);
		glVertex3f(this->location.x - .1, this->location.y + this->height, this->location.z + this->width / 2);

		glTexCoord2f(0, 1);
		glVertex3f(this->location.x - .1, this->location.y + this->height, this->location.z - this->width / 2);

		glTexCoord2f(0, 0);
		glVertex3f(this->location.x - .1, this->location.y, this->location.z - this->width / 2);


		//back
		glTexCoord2f(1, 0);
		glVertex3f(this->location.x + .1, this->location.y, this->location.z + this->width / 2);
		glTexCoord2f(1, 1);
		glVertex3f(this->location.x + .1, this->location.y + this->height, this->location.z + this->width / 2);
		glTexCoord2f(0, 1);
		glVertex3f(this->location.x + .1, this->location.y + this->height, this->location.z - this->width / 2);
		glTexCoord2f(0, 0);
		glVertex3f(this->location.x + .1, this->location.y, this->location.z - this->width / 2);

		glEnd();
		glDisable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);

		
		//lower cover
		glVertex3f(this->location.x - .1, this->location.y, this->location.z + this->width / 2);
		glVertex3f(this->location.x + .1, this->location.y, this->location.z + this->width / 2);
		glVertex3f(this->location.x + .1, this->location.y, this->location.z - this->width / 2);
		glVertex3f(this->location.x - .1, this->location.y, this->location.z - this->width / 2);

		//upper cover
		glVertex3f(this->location.x - .1, this->location.y + this->height, this->location.z + this->width / 2);
		glVertex3f(this->location.x + .1, this->location.y + this->height, this->location.z + this->width / 2);
		glVertex3f(this->location.x + .1, this->location.y + this->height, this->location.z - this->width / 2);
		glVertex3f(this->location.x - .1, this->location.y + this->height, this->location.z - this->width / 2);

		//right cover
		glVertex3f(this->location.x - .1, this->location.y + this->height, this->location.z + this->width / 2);
		glVertex3f(this->location.x + .1, this->location.y + this->height, this->location.z + this->width / 2);
		glVertex3f(this->location.x + .1, this->location.y, this->location.z + this->width / 2);
		glVertex3f(this->location.x - .1, this->location.y, this->location.z + this->width / 2);

		//left cover
		glVertex3f(this->location.x - .1, this->location.y + this->height, this->location.z - this->width / 2);
		glVertex3f(this->location.x + .1, this->location.y + this->height, this->location.z - this->width / 2);
		glVertex3f(this->location.x + .1, this->location.y, this->location.z - this->width / 2);
		glVertex3f(this->location.x - .1, this->location.y, this->location.z - this->width / 2);

		glEnd();


	}
	else {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, this->textureNumber);
		glBegin(GL_QUADS);

		//front
		glTexCoord2f(0, 1);
		glVertex3f(this->location.x - this->width / 2, this->location.y + this->height, this->location.z + .1);
		glTexCoord2f(1, 1);
		glVertex3f(this->location.x + this->width / 2, this->location.y + this->height, this->location.z + .1);
		glTexCoord2f(1, 0);
		glVertex3f(this->location.x + this->width / 2, this->location.y, this->location.z + .1);
		glTexCoord2f(0, 0);
		glVertex3f(this->location.x - this->width / 2, this->location.y, this->location.z + .1);

		//back
		glTexCoord2f(0, 1);
		glVertex3f(this->location.x - this->width / 2, this->location.y + this->height, this->location.z - .1);
		glTexCoord2f(1, 1);
		glVertex3f(this->location.x + this->width / 2, this->location.y + this->height, this->location.z - .1);
		glTexCoord2f(1, 0);
		glVertex3f(this->location.x + this->width / 2, this->location.y, this->location.z - .1);
		glTexCoord2f(0, 0);
		glVertex3f(this->location.x - this->width / 2, this->location.y, this->location.z - .1);

		glEnd();
		glDisable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);

		//left
		//glTexCoord2f();
		glVertex3f(this->location.x - this->width / 2, this->location.y, this->location.z + .1);
		glVertex3f(this->location.x - this->width / 2, this->location.y + this->height, this->location.z + .1);
		glVertex3f(this->location.x - this->width / 2, this->location.y + this->height, this->location.z - .1);
		glVertex3f(this->location.x - this->width / 2, this->location.y, this->location.z - .1);


		//right	
		glVertex3f(this->location.x + this->width / 2, this->location.y, this->location.z + .1);
		glVertex3f(this->location.x + this->width / 2, this->location.y + this->height, this->location.z + .1);
		glVertex3f(this->location.x + this->width / 2, this->location.y + this->height, this->location.z - .1);
		glVertex3f(this->location.x + this->width / 2, this->location.y, this->location.z - .1);


		//lower cover
		glVertex3f(this->location.x - this->width / 2, this->location.y, this->location.z + .1);
		glVertex3f(this->location.x + this->width / 2, this->location.y, this->location.z + .1);
		glVertex3f(this->location.x + this->width / 2, this->location.y, this->location.z - .1);
		glVertex3f(this->location.x - this->width / 2, this->location.y, this->location.z - .1);

		//upper cover
		glVertex3f(this->location.x - this->width / 2, this->location.y + this->height, this->location.z + .1);
		glVertex3f(this->location.x + this->width / 2, this->location.y + this->height, this->location.z + .1);
		glVertex3f(this->location.x + this->width / 2, this->location.y + this->height, this->location.z - .1);
		glVertex3f(this->location.x - this->width / 2, this->location.y + this->height, this->location.z - .1);

		
		glEnd();
		glPopMatrix();

	}

	// Use glEnable() and glDisable() to change OpenGL states.
}
