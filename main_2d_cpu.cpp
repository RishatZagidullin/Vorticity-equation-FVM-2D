#include <algorithm>
#include <memory>
#include <stdio.h>
#include <iostream>
#include <time.h>
#include <iomanip>
#include "fade2d/Fade_2D.h"
#include <cassert>
#include <utility>
#include "advection/example_meshes.h"
#include "advection/transport.h"
using namespace GEOM_FADE2D;
using namespace std;
using namespace example_meshes;
using namespace advection;
#include <ctime>    // For time()
#include <cstdlib>  // For srand() and rand()

void advection_init_smooth(double * u, vector<Triangle2*> const& triangles, int max_particle_size, double h, Point2 const& source_coord)
{
	for (int i = 0; i < max_particle_size; i++)
	{
		int k = 0;
		for (auto it(triangles.begin()); it != triangles.end(); it++)
		{	
			u[k+i*triangles.size()] = exp(-pow((*it)->getBarycenter().y()-source_coord.y(),2)*100.0-pow((*it)->getBarycenter().x()-source_coord.x(),2)*500.0-pow((i+1)*h,2));
			k++;
		}
	}
}

int main(int argc, char ** argv)
{
	double h, dr, dt;
	int TIME, max_particle_size, TOTAL_FRAMES;
	double length_x, length_y;
	double nominal_velocity_y, init_velocity_x;

	double volume_fraction, gravity, particle_density, fluid_density, fluid_viscosity;

	if(argc !=2) {cout << "need filename" << endl; return 2;}

	string filename{argv[1]};
	ifstream arguments;
	arguments.open(filename);
	vector<string> data;
	string line;
	int i = 0;
	while(getline(arguments, line))
	{
		data.push_back(line);
		i++;
	}
	if (data.size() != 14) {cout << "wrong data" << endl; return 2;}
	h = stod(data[0]);//0.05
	dr = stod(data[1]);//0.04
	dt = stod(data[2]);//0.001
	TIME = stoi(data[3]);//1
	max_particle_size = stoi(data[4]);//50
	TOTAL_FRAMES = stoi(data[5]);//100
	length_x = stod(data[6]);//2.0
	length_y = stod(data[7]);//0.5
	init_velocity_x = stod(data[8]);

	volume_fraction = stod(data[9]);
	gravity = stod(data[10]);
	particle_density = stod(data[11]);
	fluid_density = stod(data[12]);
	fluid_viscosity = stod(data[13]);
	
	nominal_velocity_y = 2.0*gravity/(9.0*fluid_viscosity)*(volume_fraction*particle_density - fluid_density)*pow((1.0/volume_fraction), 2.0/3.0)*0.01*0.01;
	cout << "nominal downward velocity is "<< nominal_velocity_y << endl;
	bool if_paint = (TOTAL_FRAMES == 0) ? 0 : 1;
	int periods = (TOTAL_FRAMES == 0) ? TIME : TIME/TOTAL_FRAMES;
	double max_face_length = dr+0.25*dr;
	double min_face_length = dr-0.25*dr;

	vector<Point2*> vertices;
	vector<Triangle2*> visual;
	Fade_2D GRID;
	//mesh_three_circles(GRID, visual, vertices, 0.01, 0.05);
	mesh_one_circle(GRID, visual, vertices, min_face_length, max_face_length);
	//setuha(GRID, visual, vertices, min_face_length, max_face_length, 30);
	//mesh_unstructured_square(GRID, visual, vertices, 0.01, 0.05);
	//mesh_unstructured_rect(GRID, visual, vertices, min_face_length, max_face_length);
	//mesh_structured_rect_var(GRID, visual, vertices, min_face_length, max_face_length, length_x, length_y);
	GRID.show("square.ps");

	//DATA FOR PYTHON VISUALIZER BEGIN
	ofstream velos;
	velos.open("velocities.txt");
	ofstream verts;
	verts.open("vertices.txt");
	ofstream faces;
	faces.open("faces.txt");
	ofstream colours;
	colours.open("colours.txt");
	ofstream vert_colours;
	vert_colours.open("vort_colors.txt");
	for (auto it(vertices.begin()); it != vertices.end(); it++)
	{
		verts << (*it)->x() << " " << (*it)->y() << " 0.0" << endl; 
	}
	verts.close();
	for (auto it(visual.begin()); it != visual.end(); it++)
	{
		for (int i = 0; i < 3; i++)
		{
			auto vert = find(vertices.begin(), vertices.end(),(*it)->getCorner(i));
			if (vert != vertices.end())  faces << distance(vertices.begin(), vert) << " ";
			else cout << "SOMETHING HAPPENED" << endl;
		}
		faces << endl;
	}
	faces.close();
	
	//DATA FOR PYTHON VISUALIZE END

	cout << "THE NUMBER OF triangles IS " << visual.size() << endl;
	unique_ptr<advection_2d> equation = make_unique<advection_2d>(visual);

	double *u = new double [visual.size()*max_particle_size];
    	double *initial_layer = new double [visual.size()*max_particle_size];
	double * vorticities = new double [visual.size()*max_particle_size];

	if (nominal_velocity_y > 0.0)
	{
		advection_init_smooth(u, visual, max_particle_size, h, Point2(0.0, 0.25));
	}
	else
	{
		advection_init_smooth(u, visual, max_particle_size, h, Point2(0.3, 0.0));
	}

	Vector2 * velocities = new Vector2 [visual.size()*max_particle_size];
	Vector2 * velocities_original = new Vector2 [visual.size()*max_particle_size];
	for (int i = 0; i < max_particle_size; i++)
	{
		int p = 0;
		for (auto it(visual.begin()); it != visual.end(); it++)
		{
			velocities_original[p + i * visual.size()] = Vector2(0.5, 0.0);
			velocities[p + i * visual.size()] = Vector2(0.5, 0.0);
			p++;
		}
	}
	//equation->update_velocities(velocities_original, max_particle_size);
	//equation->update_velocities(velocities, max_particle_size);
	clock_t start = clock();
	double duration;
	for (int t = 0; t < TIME; t++)
	{
		cout << t << endl;
		if (t%(periods) == 0)
		{
			equation->calc_vorticities(velocities_original, vorticities, max_particle_size);
			equation->calculate_velocities(velocities, velocities_original, vorticities, dt, max_particle_size);		
			//for (int i = 0; i < max_particle_size; i++)
			//{
			//	for (int p =0; p < visual.size(); p++)
			//	{
			//		velocities[p+i*visual.size()] = velocities_original[p+i*visual.size()] +velocities[p+i*visual.size()];
			//	}
			//}
			equation->update_velocities(velocities_original, max_particle_size);
			equation->update_velocities(velocities, max_particle_size);
		}
		if (t%periods == 0 && if_paint) 
		{
			double maximum = 0.0;
			double minimum = 0.0;
			double average = 0.0;
			for (int k = 0; k < visual.size(); k++)
			{
				if (vorticities[k] > maximum) maximum = vorticities[k];
				if (vorticities[k] < minimum) minimum = vorticities[k];
				average+=vorticities[k];
			}
			average = average/visual.size();
			cout << minimum << endl;
			cout << maximum << endl;
			cout << average << endl;
			double * vorticities_n = new double [visual.size()];
			

			for (int k = 0; k < visual.size(); k++)
			{
				vorticities_n[k] = (vorticities[k] - minimum)/(maximum-minimum);
			}
		
			for (int k = 0; k < visual.size(); k++)
			{
				int index = k;
				if (vorticities_n[index] <= 0.25)
				{
					vert_colours << 0.0 << " " << (vorticities_n[index]/0.25)  << " " << 1.0 << " " << 1.0 << " "; 
				}
				else if (vorticities_n[index] <= 0.5)
				{
				vert_colours << 0.0 << " " << 1.0 << " " << ((0.5-vorticities_n[index])/0.25) << " " << 1.0 << " "; 
				}
				else if (vorticities_n[index] <= 0.75)
				{
					vert_colours << ((vorticities_n[index]-0.5)/0.25) << " " << 1.0 << " " << 0.0 << " " << 1.0 << " "; 
				}
				else
				{
					vert_colours << 1.0 << " " << ((1.0-vorticities_n[index])/0.25) << " " << 0.0 << " " << 1.0 << " "; 
				}
				vert_colours << endl;
			}


			for (int k = 0; k < visual.size(); k++)
			{
				for (int i = 0; i < 1; i++)
				{
					int index = k+visual.size()*i*(max_particle_size/10);
					if (u[index] <= 0.25)
					{
						colours << 0.0 << " " << (u[index]/0.25)  << " " << 1.0 << " " << 1.0 << " "; 
					}
					else if (u[index] <= 0.5)
					{
						colours << 0.0 << " " << 1.0 << " " << ((0.5-u[index])/0.25) << " " << 1.0 << " "; 
					}
					else if (u[index] <= 0.75)
					{
						colours << ((u[index]-0.5)/0.25) << " " << 1.0 << " " << 0.0 << " " << 1.0 << " "; 
					}
					else
					{
						colours << 1.0 << " " << ((1.0-u[index])/0.25) << " " << 0.0 << " " << 1.0 << " "; 
					}
					velos << -(velocities[index].y()/30.0+visual[k]->getBarycenter().y()) << " " << (velocities[index].x()/30.0+visual[k]->getBarycenter().x())<< " " << 0.0 << " ";					
				}
				colours << endl;
				velos << endl;
			}
		}
		equation->solver(u, dt, velocities_original, max_particle_size);
		equation->solver(u, dt, velocities, max_particle_size);
		equation->solver(vorticities, dt, velocities_original, max_particle_size);
		equation->solver(vorticities, dt, velocities, max_particle_size);
	}
	duration = (clock() - start) / (double)CLOCKS_PER_SEC;
	cout << "duration " << duration << endl;
	colours.close();
	velos.close();
	return 0;
}

