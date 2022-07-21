#pragma once
#include "shape.h"

class CTriangle : public CShape
{
private:	
	Point points[4];
	int currentIndex = 1;
	bool closed = false;
	double leftInc1 = 0, leftInc2 = 0, rightInc1 = 0, rightInc2 = 0;

public:
	CTriangle(int x, int y, float r1, float g1, float b1, float r2, float g2, float b2, bool filled)
		: CShape(r1, g1, b1, r2, g2, b2, filled, "TRIANGLE ")
	{
		points[0] = Point(x, y);
		points[1] = Point(x, y);
	}

	CTriangle(int x0, int y0, int x1, int y1, int x2, int y2, float r1, float g1, float b1, float r2, float g2, float b2, bool filled)
		: CShape(r1, g1, b1, r2, g2, b2, filled, "TRIANGLE ")
	{
		points[0] = Point(x0, y0);
		points[1] = Point(x1, y1);
		points[2] = Point(x2, y2);

		currentIndex = 3;
		setRenderValues();
	}

	~CTriangle(){ cout << "Se destruyo un triangulo" << endl;}

	void newPoint(int x, int y) override
	{
		currentIndex++;
		if (currentIndex != 3)
		{
			points[currentIndex].x = points[currentIndex - 1].x;
			points[currentIndex].y = points[currentIndex - 1].y;
		}
		else
		{
			closed = true;
			setRenderValues();
		}
	}

	// Update current vertex position
	void update(int x1, int y1)
	{	
		points[currentIndex].x = x1; 
		points[currentIndex].y = y1;
	}

	void renderCtrlPoints()
	{
		// Render each vertex
		for (int i = 0; i < 3; i++)
			points[i].renderCtrlPoint();

		// Set color and style of the line
		glColor3f(1.0f, 1.0f, 1.0f);
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(1, 0xAAAA);
		glBegin(GL_LINE_LOOP);
			for (int i = 0; i < 4; i++)
				glVertex2i(boxPoints[i].x, boxPoints[i].y);
		glEnd();
		glDisable(GL_LINE_STIPPLE);
	}

	void setRenderValues()
	{
		// Order points by y-axis, p[0] is the smallest, p[2] is the biggest
		if (points[1].y <= points[0].y) 
		{ 
			swap(points[1], points[0]);
			if (pointSelected == &points[1])
				pointSelected = &points[0];
			else if (pointSelected == &points[0])
				pointSelected = &points[1];
		}
		if (points[2].y <= points[0].y) 
		{ 
			swap(points[2], points[0]); 
			if (pointSelected == &points[2])
				pointSelected = &points[0];
			else if (pointSelected == &points[0])
				pointSelected = &points[2];
		}
		if (points[2].y <= points[1].y) 
		{
			swap(points[2], points[1]); 
			if (pointSelected == &points[2])
				pointSelected = &points[1];
			else if (pointSelected == &points[1])
				pointSelected = &points[2];
		}

		if (points[1].y == points[2].y)
		{	// Bottom-flat triangle
			if (points[1].x > points[2].x)
			{
				rightInc1 = (double)(points[1].x - points[0].x) / (double)(points[1].y - points[0].y);
				leftInc1 = (double)(points[2].x - points[0].x) / (double)(points[2].y - points[0].y);
			}
			else
			{
				leftInc1 = (double)(points[1].x - points[0].x) / (double)(points[1].y - points[0].y);
				rightInc1 = (double)(points[2].x - points[0].x) / (double)(points[2].y - points[0].y);
			}
		}
		else if (points[0].y == points[1].y)
		{	// Top-flat triangle
			if (points[0].x > points[1].x)
			{
				leftInc2 = (double)(points[2].x - points[1].x) / (double)(points[2].y - points[1].y);
				rightInc2 = (double)(points[2].x - points[0].x) / (double)(points[2].y - points[0].y);
			}
			else
			{
				rightInc2 = (double)(points[2].x - points[1].x) / (double)(points[2].y - points[1].y);
				leftInc2 = (double)(points[2].x - points[0].x) / (double)(points[2].y - points[0].y);
			}
		}
		else
		{
			// General Case, split the triangle in a topflat and bottom-flat one
			points[3].x = (int)(((double)(points[2].x - points[0].x) / (double)(points[2].y - points[0].y) * (double)(points[1].y - points[2].y)) + (double)points[2].x);
			points[3].y = points[1].y;

			if (points[3].x > points[1].x)
			{	// The left side of the triangle has two slopes, the right side has one
				leftInc1 = (double)(points[1].x - points[0].x) / (double)(points[1].y - points[0].y);
				leftInc2 = (double)(points[2].x - points[1].x) / (double)(points[2].y - points[1].y);
				rightInc1 = (double)(points[2].x - points[0].x) / (double)(points[2].y - points[0].y);
				rightInc2 = rightInc1;
			}
			else if (points[3].x < points[1].x)
			{	// The right side of the triangle has two slopes, the left side has one
				rightInc1 = (double)(points[1].x - points[0].x) / (double)(points[1].y - points[0].y);
				rightInc2 = (double)(points[2].x - points[1].x) / (double)(points[2].y - points[1].y);
				leftInc1 = (double)(points[2].x - points[0].x) / (double)(points[2].y - points[0].y);
				leftInc2 = leftInc1;
			}
		}

		// Set bounding box values
		int minX = points[0].x, minY = points[0].y;
		int maxX = points[0].x, maxY = points[0].y;

		for (int i = 1; i < 3; i++)
		{
			if (points[i].x < minX)
				minX = points[i].x;
			else if (points[i].x > maxX)
				maxX = points[i].x;

			if (points[i].y < minY)
				minY = points[i].y;
			else if (points[i].y > maxY)
				maxY = points[i].y;
		}

		boxPoints[0].x = minX; boxPoints[0].y = minY;
		boxPoints[1].x = minX; boxPoints[1].y = maxY;
		boxPoints[2].x = maxX; boxPoints[2].y = maxY;
		boxPoints[3].x = maxX; boxPoints[3].y = minY;
	}

	void render(const bool modeHardware)
	{	
		if (modeHardware)
		{
			if (currentIndex == 3 && filled)
			{
				// Draw Content
				glColor3f(fillColor.r, fillColor.g, fillColor.b);
				glBegin(GL_TRIANGLES);
					for (int i = 0; i < 3; i++)
						glVertex2i(points[i].x, points[i].y);
				glEnd();
			}

			// Draw Current Border
			glColor3f(borderColor.r, borderColor.g, borderColor.b);
			glBegin(GL_LINES);
				for (int i = 0; i < currentIndex; i++)
				{
					glVertex2i(points[i % 3].x, points[i % 3].y);
					glVertex2i(points[(i+1) % 3].x, points[(i+1) % 3].y);
				}
			glEnd();
		}
		else
		{	
			// Draw Content
			if (currentIndex == 3 && filled)
			{
				double xLeft = (double)points[0].x, xRight = (double)points[0].x;
				// Draw upper semi-triangle filler
				for (int y = points[0].y; y <= points[1].y; y++)
				{
					horizontalLine(ceil(xLeft),(int)xRight, y, fillColor);
					xLeft += leftInc1;
					xRight += rightInc1;
				}
				
				// Draw lower semi-triangle filler
				xLeft = (double)points[2].x; xRight = (double)points[2].x;
				for (int y = points[2].y; y > points[1].y; y--)
				{
					horizontalLine(ceil(xLeft), (int)xRight, y, fillColor);
					xLeft -= leftInc2;
					xRight -= rightInc2;
				}
			}

			// Draw Current Border
			glColor3f(borderColor.r, borderColor.g, borderColor.b);
			glBegin(GL_LINES);
			for (int i = 0; i < currentIndex; i++)
			{
				glVertex2i(points[i % 3].x, points[i % 3].y);
				glVertex2i(points[(i + 1) % 3].x, points[(i + 1) % 3].y);
			}
			glEnd();
			
		}
	}

	bool onClick(int x, int y)
	{
		if (x > boxPoints[0].x - 3 && x < boxPoints[2].x + 3 && y > boxPoints[0].y - 3 && y < boxPoints[2].y + 3)
		{
			clickedCtrlPoint(x, y);

			int y1 = 0, y2 = 0, y3 = 0;

			if (points[1].y == points[2].y)
			{
				y1 = (int)(((float)(points[1].x - points[0].x) / (float)(points[1].y - points[0].y) * (y - points[1].y)) + (points[1].x - x));
				y3 = (int)(((float)(points[0].x - points[2].x) / (float)(points[0].y - points[2].y) * (y - points[0].y)) + (points[0].x - x));
				return (y <= points[1].y && ( (y1 > 0 && y3 < 0) || (y1 < 0 && y3 > 0))) || pointSelected;
			}
			else if (points[0].y == points[1].y)
			{
				y2 = (int)(((float)(points[2].x - points[1].x) / (float)(points[2].y - points[1].y) * (y - points[2].y)) + (points[2].x - x));
				y3 = (int)(((float)(points[0].x - points[2].x) / (float)(points[0].y - points[2].y) * (y - points[0].y)) + (points[0].x - x));
				return (y >= points[1].y && ((y2 > 0 && y3 < 0) || (y2 < 0 && y3 > 0))) || pointSelected;
			}
			else 
			{
				y1 = (int)(((float)(points[1].x - points[0].x) / (float)(points[1].y - points[0].y) * (y - points[1].y)) + (points[1].x - x));
				y2 = (int)(((float)(points[2].x - points[1].x) / (float)(points[2].y - points[1].y) * (y - points[2].y)) + (points[2].x - x));
				y3 = (int)(((float)(points[0].x - points[2].x) / (float)(points[0].y - points[2].y) * (y - points[0].y)) + (points[0].x - x));
				return (y1 > 0 && y2 > 0 && y3 < 0) || (y1 < 0 && y2 < 0 && y3 > 0) || pointSelected;
			}
		}
		return false;
	}

	bool clickedCtrlPoint(int x, int y)
	{
		// We check if the click fell on a vertex
		int dx, dy;
		for (int i = 0; i < 3; i++)
		{
			dx = (x - points[i].x);
			dy = (y - points[i].y);
			// Check squared distance between vertex i and the click, threshold: 5 pixels
			if ((dx * dx + dy * dy) <= 25)
			{
				pointSelected = &points[i];
				return true;
			}
		}
		return false;
	}

	void onMove(int x, int y)
	{
		if (pointSelected)
		{	// Only move the vertex selected
			pointSelected->x = x;
			pointSelected->y = y;
			setRenderValues();
		}
		else
		{
			// We move the whole triangle
			int dx = x - anchorPoint.x;
			int dy = y - anchorPoint.y;

			anchorPoint.x = x; anchorPoint.y = y;

			for (int i = 0; i < 4; i++)
			{
				points[i].x += dx;
				points[i].y += dy;
			}

			moveBoundingBox(dx, dy);
		}
	}

	bool finished() override
	{
		return closed;
	}

	std::string getInfo() override
	{
		// Add position
		info += to_string(points[0].x) + " " + to_string(points[0].y) + " "
			 + to_string(points[1].x) + " " + to_string(points[1].y) + " "
			 + to_string(points[2].x) + " " + to_string(points[2].y) + " ";

		// Add border info
		info += to_string(borderColor.r) + " " + to_string(borderColor.g) + " "
			+ to_string(borderColor.b);

		// Add filler info
		if (filled)
		{
			info = "FILLED_" + info + " " + to_string(fillColor.r) + " "
				+ to_string(fillColor.g) + " " + to_string(fillColor.b);
		}

		return info + "\n";
	}
};