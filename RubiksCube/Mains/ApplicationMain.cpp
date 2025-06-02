#include "GraphicsCortex.h"
#include "RubiksCube/RubiksCube.h"

int main() {

	glm::ivec2 window_resolution(512, 512);

	WindowDescription desc;
	desc.w_resolution = window_resolution;
	desc.w_scale_framebuffer_size = false;
	desc.w_scale_window_size = false;
	desc.f_multisample_count = 8;
	//desc.w_transparent = true;
	//desc.w_always_on_top = true;
	Window window(desc);

	Camera camera;
	camera.set_position(glm::vec3(0, 0, 6));
	camera.screen_width = window_resolution.x;
	camera.screen_height = window_resolution.y;
	camera.fov = 60;

	RubiksCube rubiks_cube;

	std::vector<RubiksCube::Movement> movements{
		RubiksCube::Movement(1, RubiksCube::axis::X),
		RubiksCube::Movement(0, RubiksCube::axis::Y),
		RubiksCube::Movement(2, RubiksCube::axis::Z),
	};
	int32_t movement_counter = 0;

	window.newsletters->on_key_events.subscribe([&](const Window::KeyPressResult& result) {
		if (result.key == Window::Key::K && result.action == Window::PressAction::PRESS) {
			if (movement_counter >= movements.size())
				movement_counter = 0;
			//rubiks_cube.move(movements[movement_counter].stack_index, movements[movement_counter].rotation_axis);
			rubiks_cube.move(rand()%3, RubiksCube::axis(rand()%3));
			movement_counter++;
		}
		});

	window.newsletters->on_window_resolution_events.subscribe([&](const glm::ivec2& resolution) {
		primitive_renderer::set_viewport_size(resolution);
		camera.screen_width = resolution.x;
		camera.screen_height = resolution.y;
		});

	window.newsletters->on_window_refresh_events.subscribe([&]() {
		primitive_renderer::clear(0, 0, 0, 0.0);
		rubiks_cube.render(camera);
		window.swap_buffers();
		});
	
	while (!window.should_close()) {
		double deltatime = window.handle_events();
		primitive_renderer::clear(0, 0, 0, 0.0);
		camera.handle_movements((GLFWwindow*)window.get_handle(), deltatime);
		rubiks_cube.render(camera);
		window.swap_buffers();
	}
}
