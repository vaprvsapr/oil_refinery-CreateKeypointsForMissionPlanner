#pragma once

#include <random>
#include <vector>
#include "Structures.h"
#include <iostream>

vector<Coordinate3D> GetNoisyCoordinates(
	const vector<Coordinate3D>& coordinates,
	const array<double, 3> mean,
	const array<double, 3> sigma)
{
	random_device rd{};
	mt19937 gen{ rd() };
	normal_distribution<float>
		dx(mean[0], sigma[0]),
		dy(mean[1], sigma[1]),
		dz(mean[2], sigma[2]);

	vector<Coordinate3D> noisy_coordinates;

	for (size_t i = 0; i < coordinates.size(); i++)
	{
		noisy_coordinates.push_back({
			coordinates[i].x + dx(gen),
			coordinates[i].y + dy(gen),
			coordinates[i].z + dz(gen)
			});
	}
	return noisy_coordinates;
}