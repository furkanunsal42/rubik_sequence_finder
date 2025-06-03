#include "GraphicsCortex.h"
#include "RubiksCube/RubiksCube.h"
#include "CameraController/CameraController.h"

int main() {

	glm::ivec2 window_resolution(1024, 1024);

	WindowDescription desc;
	desc.w_resolution = window_resolution;
	desc.w_scale_framebuffer_size = false;
	desc.w_scale_window_size = false;
	desc.f_multisample_count = 0;
	//desc.w_transparent = true;
	//desc.w_always_on_top = true;
	Window window(desc);

	CameraController camera_controller;
	camera_controller.camera.screen_width = window_resolution.x;
	camera_controller.camera.screen_height = window_resolution.y;
	camera_controller.camera.fov = 60;

	RubiksCube rubiks_cube(3);

	std::vector<RubiksCube::Movement> movements{
		RubiksCube::Movement(1, RubiksCube::axis::NX),
		RubiksCube::Movement(0, RubiksCube::axis::NY),
		RubiksCube::Movement(2, RubiksCube::axis::NZ),
	};
	int32_t movement_counter = 0;

	window.newsletters->on_key_events.subscribe([&](const Window::KeyPressResult& result) {
		if (result.key == Window::Key::K && result.action == Window::PressAction::PRESS) {
			if (movement_counter >= movements.size())
				movement_counter = 0;
			//rubiks_cube.move(movements[movement_counter].stack_index, movements[movement_counter].rotation_axis);
			rubiks_cube.move(rand()%rubiks_cube.cube_dimentions, RubiksCube::axis(rand()%6));
			movement_counter++;
		}
		
		glm::ivec2 cursor_render_coord = glm::ivec2(
			window.get_cursor_position().x,
			window.get_window_resolution().y - 1 - window.get_cursor_position().y
		);

		if (result.key == Window::Key::W && result.action == Window::PressAction::PRESS) {
			RubiksCube::piece_info info = rubiks_cube.get_cursor_piece(cursor_render_coord);
			RubiksCube::axis axis = 
				info.face == RubiksCube::face::left		? RubiksCube::axis::NZ : 
				info.face == RubiksCube::face::right	? RubiksCube::axis::Z : 
				info.face == RubiksCube::face::up		? RubiksCube::axis::NY : 
				info.face == RubiksCube::face::down		? RubiksCube::axis::NY :
				info.face == RubiksCube::face::backward	? RubiksCube::axis::NX :
				info.face == RubiksCube::face::forward	? RubiksCube::axis::X : RubiksCube::axis::X;

			int32_t stack_index = 
				axis == RubiksCube::axis::X		? info.coordinate.x :
				axis == RubiksCube::axis::NX	? info.coordinate.x :
				axis == RubiksCube::axis::Y		? info.coordinate.y :
				axis == RubiksCube::axis::NY	? info.coordinate.y :
				axis == RubiksCube::axis::Z		? info.coordinate.z :
				axis == RubiksCube::axis::NZ	? info.coordinate.z : info.coordinate.x;

			std::cout << "coodinate is " << info.coordinate.x << " " << info.coordinate.y << " " << info.coordinate.z << std::endl;

			rubiks_cube.move(stack_index, axis);
		}
		if (result.key == Window::Key::S && result.action == Window::PressAction::PRESS) {
			
		}
		if (result.key == Window::Key::A && result.action == Window::PressAction::PRESS) {
			
		}
		if (result.key == Window::Key::D && result.action == Window::PressAction::PRESS) {
			
		}
		if (result.key == Window::Key::Q && result.action == Window::PressAction::PRESS) {
			
		}
		if (result.key == Window::Key::E && result.action == Window::PressAction::PRESS) {
			
		}

		});

	window.newsletters->on_window_resolution_events.subscribe([&](const glm::ivec2& resolution) {
		primitive_renderer::set_viewport_size(resolution);
		camera_controller.camera.screen_width = resolution.x;
		camera_controller.camera.screen_height = resolution.y;
		});

	window.newsletters->on_window_refresh_events.subscribe([&]() {
		Framebuffer::get_screen().bind_draw();
		primitive_renderer::set_viewport_size(window.get_window_resolution());
		primitive_renderer::clear(1, 0, 0, 1.0);
		rubiks_cube.render(camera_controller.camera);
		window.swap_buffers();
		});

	window.newsletters->on_mouse_key_events.subscribe([&](const Window::MousePressResult& result) {
		if (result.action == Window::PressAction::PRESS && result.button == Window::MouseButton::BUTTON1) {
			rubiks_cube.get_cursor_piece(glm::ivec2(window.get_cursor_position().x, window.get_window_resolution().y-1-window.get_cursor_position().y));
		}
		});
	
	while (!window.should_close()) {
		double deltatime = window.handle_events();
		Framebuffer::get_screen().bind_draw();
		primitive_renderer::set_viewport_size(window.get_window_resolution());
		primitive_renderer::clear(1, 0, 0, 1.0);
		camera_controller.handle_movements(window, deltatime);
		rubiks_cube.render(camera_controller.camera);
		//rubiks_cube.cursor_picking_framebuffer->blit_to_screen(glm::ivec2(1024), window.get_window_resolution(), Framebuffer::Channel::COLOR, Framebuffer::Filter::LINEAR);
		window.swap_buffers();
	}
}
