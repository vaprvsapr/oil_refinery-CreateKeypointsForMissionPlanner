#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "CreateTrajectory.h"
#include "GetNoisy.h"

using namespace std;

int main()
{
	float FOV = 0.762;
	Size2D overlap = { 0.4, 0.1 };
	int height = 100;

	auto trajectory = CreateTackTrajectory(
		{ 469, 496 },
		{ 328, 218 },
		{ overlap.height, overlap.width },
		0,
		height,
		FOV,
		{ size_t(2000), size_t(3000) },
		130);

	int horizontal_noise = 7.5;
	int vertical_noise = 7.5;
	auto noisy_trajectory = GetNoisyCoordinates(trajectory.first, { 0, 0, 0 }, { double(horizontal_noise), double(horizontal_noise), double(vertical_noise) });

	string file_name = "ho=0." + to_string(int(overlap.width * 10)) +
		", vo=0." + to_string(int(overlap.height * 100)) + ".txt";

	CreateWayPoints(
		{noisy_trajectory, trajectory.second},
		"D:/important/logs/SONY 3000x2000/WayPoints3to2HFOV62/hn=7.5, vn=7.5/",
		file_name
	);

	//file_name = "ho=0.3, vo=0.4.txt";
	//CreateWayPoints(
	//	{ trajectory.first, trajectory.second },
	//	"D:/important/logs/SONY 6000x4000/WayPoints/",
	//	file_name
	//);
}