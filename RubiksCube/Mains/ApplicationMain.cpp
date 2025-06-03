#include "GraphicsCortex.h"
#include "RubiksCube/RubiksCube.h"
#include "CameraController/CameraController.h"

int main() {

	glm::ivec2 window_resolution(1024, 1024);

	WindowDescription desc;
	desc.w_resolution = window_resolution;
	desc.w_scale_framebuffer_size = false;
	desc.w_scale_window_size = false;
	desc.f_multisample_count = 16;
	desc.w_transparent = true;
	Window window(desc);

	CameraController camera_controller;
	camera_controller.camera.screen_width = window_resolution.x;
	camera_controller.camera.screen_height = window_resolution.y;
	camera_controller.camera.fov = 45;

	RubiksCube rubiks_cube(3);

	bool movement_requested = false;
	bool movement_is_ready = false;	
	Window::KeyPressResult move_requesting_key;

	window.newsletters->on_key_events.subscribe([&](const Window::KeyPressResult& result) {
		
		glm::vec2 cursor_render_coord = glm::vec2(
			window.get_cursor_position().x,
			window.get_window_resolution().y - 1 - window.get_cursor_position().y) / glm::vec2(window.get_window_resolution());

		if (result.key == Window::Key::W && result.action == Window::PressAction::PRESS ||
			result.key == Window::Key::S && result.action == Window::PressAction::PRESS ||
			result.key == Window::Key::A && result.action == Window::PressAction::PRESS ||
			result.key == Window::Key::D && result.action == Window::PressAction::PRESS ||
			result.key == Window::Key::Q && result.action == Window::PressAction::PRESS ||
			result.key == Window::Key::E && result.action == Window::PressAction::PRESS)
		{
			if (!rubiks_cube.is_move_animation_happening()) {
				movement_is_ready = true;
			}
			else {
				movement_is_ready = false;
				movement_requested = true;
				move_requesting_key = result;
			}
		}

		if (
			(result.key == Window::Key::W || result.key == Window::Key::S) && 
			result.action == Window::PressAction::PRESS && 
			movement_is_ready
			) 
		{
			RubiksCube::piece_info info = rubiks_cube.get_cursor_piece(cursor_render_coord);
			if (info == RubiksCube::not_a_piece)
				return;

			glm::vec3 camera_forward = camera_controller.get_camera_forward();
			bool prefer_x = 
				glm::max(glm::dot(camera_forward, glm::vec3(1, 0, 0)), glm::dot(camera_forward, glm::vec3(-1, 0, 0))) 
				>
				glm::max(glm::dot(camera_forward, glm::vec3(0, 0, 1)), glm::dot(camera_forward, glm::vec3(0, 0, -1)));

			bool perefer_positive_x = glm::dot(camera_forward, glm::vec3(1, 0, 0)) > glm::dot(camera_forward, glm::vec3(-1, 0, 0));
			bool perefer_positive_z = glm::dot(camera_forward, glm::vec3(0, 0, 1)) > glm::dot(camera_forward, glm::vec3(1, 0, -1));

			RubiksCube::axis up_prefered_axis =
				prefer_x ?
				(perefer_positive_x ? RubiksCube::axis::NZ : RubiksCube::axis::Z) :
				(perefer_positive_z ? RubiksCube::axis::X : RubiksCube::axis::NX);

			RubiksCube::axis down_prefered_axis =
				prefer_x ?
				(perefer_positive_x ? RubiksCube::axis::Z : RubiksCube::axis::NZ) :
				(perefer_positive_z ? RubiksCube::axis::NX : RubiksCube::axis::X);

			RubiksCube::axis axis = 
				info.face == RubiksCube::face::left		? RubiksCube::axis::NZ : 
				info.face == RubiksCube::face::right	? RubiksCube::axis::Z : 
				info.face == RubiksCube::face::up		? up_prefered_axis :
				info.face == RubiksCube::face::down		? down_prefered_axis :
				info.face == RubiksCube::face::backward	? RubiksCube::axis::NX :
				info.face == RubiksCube::face::forward	? RubiksCube::axis::X : RubiksCube::axis::X;
			
			if (result.key == Window::Key::S)
				axis = RubiksCube::invert_axis(axis);

			int32_t stack_index = 
				axis == RubiksCube::axis::X		? info.coordinate.x :
				axis == RubiksCube::axis::NX	? info.coordinate.x :
				axis == RubiksCube::axis::Y		? info.coordinate.y :
				axis == RubiksCube::axis::NY	? info.coordinate.y :
				axis == RubiksCube::axis::Z		? info.coordinate.z :
				axis == RubiksCube::axis::NZ	? info.coordinate.z : info.coordinate.x;

			rubiks_cube.move(stack_index, axis);
			movement_is_ready = false;
		}
		if (
			(result.key == Window::Key::A || result.key == Window::Key::D) &&
			result.action == Window::PressAction::PRESS &&
			movement_is_ready
			) 
		{
			RubiksCube::piece_info info = rubiks_cube.get_cursor_piece(cursor_render_coord);
			if (info == RubiksCube::not_a_piece)
				return;

			glm::vec3 camera_forward = camera_controller.get_camera_forward();
			bool prefer_x =
				glm::max(glm::dot(camera_forward, glm::vec3(1, 0, 0)), glm::dot(camera_forward, glm::vec3(-1, 0, 0)))
				>
				glm::max(glm::dot(camera_forward, glm::vec3(0, 0, 1)), glm::dot(camera_forward, glm::vec3(0, 0, -1)));

			bool perefer_positive_x = glm::dot(camera_forward, glm::vec3(1, 0, 0)) > glm::dot(camera_forward, glm::vec3(-1, 0, 0));
			bool perefer_positive_z = glm::dot(camera_forward, glm::vec3(0, 0, 1)) > glm::dot(camera_forward, glm::vec3(1, 0, -1));

			RubiksCube::axis up_prefered_axis =
				prefer_x ?
				(perefer_positive_x ? RubiksCube::axis::NX : RubiksCube::axis::X) :
				(perefer_positive_z ? RubiksCube::axis::NZ : RubiksCube::axis::Z);

			RubiksCube::axis down_prefered_axis =
				prefer_x ?
				(perefer_positive_x ? RubiksCube::axis::X : RubiksCube::axis::NX) :
				(perefer_positive_z ? RubiksCube::axis::Z : RubiksCube::axis::NZ);

			RubiksCube::axis axis =
				info.face == RubiksCube::face::left		? RubiksCube::axis::NY :
				info.face == RubiksCube::face::right	? RubiksCube::axis::NY :
				info.face == RubiksCube::face::up		? up_prefered_axis :
				info.face == RubiksCube::face::down		? down_prefered_axis :
				info.face == RubiksCube::face::backward ? RubiksCube::axis::NY :
				info.face == RubiksCube::face::forward	? RubiksCube::axis::NY : RubiksCube::axis::Y;

			if (result.key == Window::Key::D)
				axis = RubiksCube::invert_axis(axis);

			int32_t stack_index =
				axis == RubiksCube::axis::X		? info.coordinate.x :
				axis == RubiksCube::axis::NX	? info.coordinate.x :
				axis == RubiksCube::axis::Y		? info.coordinate.y :
				axis == RubiksCube::axis::NY	? info.coordinate.y :
				axis == RubiksCube::axis::Z		? info.coordinate.z :
				axis == RubiksCube::axis::NZ	? info.coordinate.z : info.coordinate.x;

			rubiks_cube.move(stack_index, axis);
			movement_is_ready = false;
		}
		if (
			(result.key == Window::Key::Q || result.key == Window::Key::E) &&
			result.action == Window::PressAction::PRESS &&
			movement_is_ready
			) 
		{
			RubiksCube::piece_info info = rubiks_cube.get_cursor_piece(cursor_render_coord);
			if (info == RubiksCube::not_a_piece)
				return;

			RubiksCube::axis axis =
				info.face == RubiksCube::face::left		? RubiksCube::axis::NX :
				info.face == RubiksCube::face::right	? RubiksCube::axis::X :
				info.face == RubiksCube::face::up		? RubiksCube::axis::Y :
				info.face == RubiksCube::face::down		? RubiksCube::axis::NY:
				info.face == RubiksCube::face::backward ? RubiksCube::axis::Z :
				info.face == RubiksCube::face::forward	? RubiksCube::axis::NZ : RubiksCube::axis::Z;

			if (result.key == Window::Key::E)
				axis = RubiksCube::invert_axis(axis);

			int32_t stack_index =
				axis == RubiksCube::axis::X		? info.coordinate.x :
				axis == RubiksCube::axis::NX	? info.coordinate.x :
				axis == RubiksCube::axis::Y		? info.coordinate.y :
				axis == RubiksCube::axis::NY	? info.coordinate.y :
				axis == RubiksCube::axis::Z		? info.coordinate.z :
				axis == RubiksCube::axis::NZ	? info.coordinate.z : info.coordinate.x;

			rubiks_cube.move(stack_index, axis);
			movement_is_ready = false;

			movement_is_ready = false;
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
		primitive_renderer::clear(0, 0, 0, 0.0);
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
		primitive_renderer::clear(0, 0, 0, 0.0);
		camera_controller.handle_movements(window, deltatime);

		if (movement_requested)
			rubiks_cube.force_finish_move_animation();

		rubiks_cube.render(camera_controller.camera);
		
		if (movement_requested) {
			movement_is_ready = true;
			movement_requested = false;
			window.newsletters->on_key_events.publish(move_requesting_key);
		}

		window.swap_buffers();
	}
}
