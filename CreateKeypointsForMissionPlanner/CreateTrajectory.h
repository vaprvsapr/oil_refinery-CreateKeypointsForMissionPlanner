#pragma once

#include "Structures.h"
#include "GetPixelCoordinate.h"
#include "Projection.h"

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

pair<vector<Coordinate3D>,
	vector<EulerAngles>> CreateTackTrajectory(
		Coordinate2D initial_point,
		Size2D delta,
		Size2D overlap, // {0.2, 0.3}
		double pitch,
		double height,
		double vertical_field_of_view,
		ImageSize image_size,
		double sea_level
	)
{
	MyCamera camera{
		{0, 0, height},
		{0, 3.141592 / 2 - pitch, 0},
		vertical_field_of_view,
		image_size
	};

	Coordinate3D
		top_left_3d = GetPixelCoordinate(camera,
			{ 0, 0 }),
		top_right_3d = GetPixelCoordinate(camera,
			{ 0, camera.image_size.width }),
		bottom_left_3d = GetPixelCoordinate(camera,
			{ camera.image_size.height, 0 }),
		bottom_right_3d = GetPixelCoordinate(camera,
			{ camera.image_size.height,
			camera.image_size.width });

	Coordinate2D
		top_left = Projection(camera.position, top_left_3d),
		top_right = Projection(camera.position, top_right_3d),
		bottom_left = Projection(camera.position, bottom_left_3d),
		bottom_right = Projection(camera.position, bottom_right_3d);

	Size2D rectangle_size = {
		abs(top_left.x - bottom_left.x),
		abs(bottom_left.y - bottom_right.y) / 2 + abs(top_left.y - top_right.y) / 2
	};
	cout << "a: " << rectangle_size.height << " b: " << rectangle_size.width << endl;

	Size2D shift = {
		rectangle_size.height * (1 - overlap.height),
		rectangle_size.width * (1 - overlap.width)
	};
	cout << "shift h: " << shift.height << ", w: " << shift.width << endl;

	size_t n_height = ceil(delta.height / shift.height);
	size_t n_width = ceil(delta.width / shift.width);

	Size2D alignment = {
		(rectangle_size.height + (n_height - 1) * shift.height - delta.height) / 2,
		(rectangle_size.width + (n_width - 1) * shift.width - delta.width) / 2
	};

	vector<Coordinate3D> positions;
	vector<EulerAngles> orientations;

	for (size_t j = 0; j < n_width; j++)
	{
		for (size_t i = 0; i < n_height; i++)
		{
			if ((j % 2) == 0)
			{
				positions.push_back({
					float(initial_point.x + i * shift.height +
					height / tan(3.141592 - pitch) - bottom_left.x - alignment.height - height / tan(3.141592 - pitch)),
					float(initial_point.y + j * shift.width +
					rectangle_size.width / 2 - alignment.width),
					float(height + sea_level)
					});
				orientations.push_back({ 0, pitch, -3.141592 / 2 });
			}
			if ((j % 2) == 1)
			{
				positions.push_back({
					float(initial_point.x + delta.height - i * shift.height -
					height / tan(3.141592 - pitch) + bottom_left.x + alignment.height + height / tan(3.141592 - pitch)),
					float(initial_point.y + j * shift.width +
					rectangle_size.width / 2 - alignment.width),
					float(height + sea_level)
					});
				orientations.push_back({ 0, pitch, 3.141592 / 2 });
			}
		}
	}
	return make_pair(positions, orientations);
}

void CreateWayPoints(
	pair<vector<Coordinate3D>,
	vector<EulerAngles>> trajectory,
	string dir,
	string file_name
)
{
	ofstream output;
	output.open(dir + file_name);
	cout << dir + file_name << endl;
	output << "QGC WPL 110\n";
	
	string line;

	double x0 = trajectory.first[0].x,
		y0 = trajectory.first[0].y;

	output << "0 1 0 16 0 0 0 0 0.0000001 0.0000001 5.000000 1\n";
	double k = 0.000009;
	double n = 0.0000001;
	for (int i = 0; i < trajectory.first.size(); i++)
	{
		if((abs(trajectory.first[i].y - y0) > 0.000001) && (abs(trajectory.first[i].x - x0) > 0.00001))
			output << setprecision(8) << i + 1 << " 0 3 16 0.00000000	0.00000000	0.00000000	0.00000000 " 
			<< (trajectory.first[i].y - y0) * k + n << " " 
			<< (trajectory.first[i].x - x0) * k + n << " "
			<< (trajectory.first[i].z - 130.0 + 0.000001)<< " 1\n";
		if ((abs(trajectory.first[i].y - y0) < 0.000001) && (abs(trajectory.first[i].x - x0) > 0.00001))
			output << setprecision(8) << i + 1 << " 0 3 16 0.00000000	0.00000000	0.00000000	0.00000000 "
			<< "0.00001" << " "
			<< (trajectory.first[i].x - x0) * k + n << " "
			<< (trajectory.first[i].z - 130.0 + 0.000001)<< " 1\n";
		if ((abs(trajectory.first[i].y - y0) > 0.000001) && (abs(trajectory.first[i].x - x0) < 0.00001))
			output << setprecision(8) << i + 1 << " 0 3 16 0.00000000	0.00000000	0.00000000	0.00000000 "
			<< (trajectory.first[i].y - y0) * k + n << " "
			<< "0.00001" << " "
			<< (trajectory.first[i].z - 130.0 + 0.000001) << " 1\n";
		if ((abs(trajectory.first[i].y - y0) < 0.000001) && (abs(trajectory.first[i].x - x0) < 0.00001))
			output << setprecision(8) << i + 1 << " 0 3 16 0.00000000	0.00000000	0.00000000	0.00000000 "
			<< "0.00001" << " "
			<< "0.00001" << " "
			<< (trajectory.first[i].z - 130.0 + 0.000001) << " 1\n";

		cout << i + 1 << " 0 3 16 0 0 0 0 " << (trajectory.first[i].y - y0 + n) * k << " " << (trajectory.first[i].x - x0 + n) * k << " " << trajectory.first[i].z - 130.0 + n << " 1\n";
		
	}
}