#include <engine.h>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>

#include "scenes/scenes.h"

using namespace std;

int main(int argc, char *argv[]) {

	// Scene scene = create_cornell_box_scene();
	// Scene scene = create_test_scene();
	// Scene scene = create_csg_test_scene();
	Scene scene = create_tea_scene();
	
	cerr << std::fixed << std::setprecision(0);
	cerr << "Rendering..." << endl;
	auto t0 = chrono::high_resolution_clock::now();

	scene.render();
	cerr << endl;
	cerr << std::setprecision(6);


	scene.camera->fb->export_rgb(CONFIG::RENDER_OUTPUT_PATH);
	scene.camera->fb->export_depth(CONFIG::DEPTH_OUTPUT_PATH);

	auto t1 = chrono::high_resolution_clock::now();
	int seconds, minutes, hours;
	UTIL::chrono_total_time_split(
		chrono::duration_cast<chrono::seconds>(t1 - t0).count(),
		seconds, minutes, hours
	);
	
	cerr << std::defaultfloat;
	cerr << "Total Render Time: " << hours << "h " << minutes << "m " << seconds << "s" << endl;
	cerr << "done" << endl;
	return 0;
	
}


