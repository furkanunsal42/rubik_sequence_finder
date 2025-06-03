#include "RubiksCube.h"

#include "Default_Programs.h"
#include "PrimitiveRenderer.h"

#include "Application/ProgramSourcePaths.h"

RubiksCube::RubiksCube(int32_t cube_dimentions) :
	cube_dimentions(cube_dimentions)
{
	init();
}

void RubiksCube::render(Framebuffer& target_framebuffer, Camera& camera)
{
	target_framebuffer.bind_draw();
	render(camera);
}

void RubiksCube::render(Camera& camera)
{
	_update_move_animation();

	for (int x = 0; x < cube_dimentions; x++)
	for (int y = 0; y < cube_dimentions; y++)
	for (int z = 0; z < cube_dimentions; z++)
		_render_piece(camera, glm::ivec3(x, y, z), false);


	cursor_picking_framebuffer->bind_draw();
	primitive_renderer::set_viewport_size(cursor_picking_texture_resolution);
	primitive_renderer::clear(-1, -1, -1, -1);
	
	for (int x = 0; x < cube_dimentions; x++)
	for (int y = 0; y < cube_dimentions; y++)
	for (int z = 0; z < cube_dimentions; z++)
		_render_piece(camera, glm::ivec3(x, y, z), true);
}

int32_t RubiksCube::get_piece_index(face surface, int32_t x, int32_t y)
{
	if (surface == up || surface == down) {
		size_t width = cube_dimentions;
		size_t height = cube_dimentions;
		return y * width + x + 
			(surface == up        ? 0*width * height :
			 surface == down      ? 1*width * height : 0);
	}
	else {
		size_t width = cube_dimentions;
		size_t height = cube_dimentions;
		return y * width + x + 
			2 * cube_dimentions * cube_dimentions +
			(surface == left	  ? 0*width * height :
			surface == forward    ? 1*width * height : 
			surface == right	  ? 2*width * height :
			surface == backward   ? 3*width * height : 0);
	}
}

uint8_t RubiksCube::get_piece_id(face surface, int32_t x, int32_t y)
{
	int32_t piece_index = get_piece_index(surface, x, y);
	return pieces[piece_index];
}

glm::vec3 RubiksCube::get_piece_color(face surface, int32_t x, int32_t y)
{
	return colors[get_piece_id(surface, x, y)];
}

void RubiksCube::set_piece_id(face surface, int32_t x, int32_t y, uint8_t id)
{
	int32_t piece_index = get_piece_index(surface, x, y);
	pieces[piece_index] = id;
}

int32_t RubiksCube::get_piece_count()
{
	return  4 * cube_dimentions * cube_dimentions + 
			2 * cube_dimentions * cube_dimentions;
}

void RubiksCube::_make_move(int32_t stack_index, axis rotation_axis)
{
	if (rotation_axis == X) {
		int32_t& x = stack_index;
		for (int32_t y = 0; y < cube_dimentions; y++) {
			auto temp = pieces[get_piece_index(up, x, y)];
			
			pieces[get_piece_index(up,		x, y)]						= pieces[get_piece_index(forward,  x, cube_dimentions-1-y)];
			pieces[get_piece_index(forward, x, cube_dimentions-1-y)]	= pieces[get_piece_index(down,	   x, cube_dimentions-1-y)];
			pieces[get_piece_index(down,	x, cube_dimentions-1-y)]	= pieces[get_piece_index(backward, x, y)];
			pieces[get_piece_index(backward,x, y)]						= temp;
		}

		if (stack_index == 0 || stack_index == cube_dimentions-1) {
			face f = (stack_index == 0) ? left : right;
			for (int32_t layer = 0; layer < (int32_t)cube_dimentions / 2; layer++) {
				for (int32_t i = layer; i < cube_dimentions-1-layer; i++) {
					std::swap(pieces[get_piece_index(f, i, layer)], pieces[get_piece_index(f, cube_dimentions-1-layer, i)]);
					std::swap(pieces[get_piece_index(f, i, layer)], pieces[get_piece_index(f, cube_dimentions-1-i, cube_dimentions-1-layer)]);
					std::swap(pieces[get_piece_index(f, i, layer)], pieces[get_piece_index(f, layer, cube_dimentions-1-i)]);
				}
			}
		}
	}
	else if (rotation_axis == NX) {
		int32_t& x = stack_index;
		for (int32_t y = 0; y < cube_dimentions; y++) {
			auto temp = pieces[get_piece_index(up, x, y)];
			
			pieces[get_piece_index(up,		x, y)]						= pieces[get_piece_index(backward, x, y)];
			pieces[get_piece_index(backward,x, y)]						= pieces[get_piece_index(down, x, cube_dimentions - 1 - y)];
			pieces[get_piece_index(down,	x, cube_dimentions-1-y)]	= pieces[get_piece_index(forward, x, cube_dimentions - 1 - y)];
			pieces[get_piece_index(forward, x, cube_dimentions-1-y)]	= temp;
		}

		if (stack_index == 0 || stack_index == cube_dimentions-1) {
			face f = (stack_index == 0) ? left : right;
			for (int32_t layer = 0; layer < (int32_t)cube_dimentions / 2; layer++) {
				for (int32_t i = layer; i < cube_dimentions-1-layer; i++) {
					std::swap(pieces[get_piece_index(f, i, layer)], pieces[get_piece_index(f, layer, cube_dimentions-1-i)]);
					std::swap(pieces[get_piece_index(f, i, layer)], pieces[get_piece_index(f, cube_dimentions-1-i, cube_dimentions-1-layer)]);
					std::swap(pieces[get_piece_index(f, i, layer)], pieces[get_piece_index(f, cube_dimentions-1-layer, i)]);
				}
			}
		}
	}
	else if (rotation_axis == Y) {
		int32_t& y = stack_index;
		for (int32_t x = 0; x < cube_dimentions; x++) {
			auto temp = pieces[get_piece_index(forward, x, y)];
			
			pieces[get_piece_index(forward,  x, y)]						= pieces[get_piece_index(right,		cube_dimentions-1-x, y)];
			pieces[get_piece_index(right,	 cube_dimentions-1-x, y)] = pieces[get_piece_index(backward,	cube_dimentions-1-x, y)];
			pieces[get_piece_index(backward, cube_dimentions-1-x, y)] = pieces[get_piece_index(left,		x, y)];
			pieces[get_piece_index(left,	 x, y)]						= temp;
		}

		if (stack_index == 0 || stack_index == cube_dimentions - 1) {
			face f = (stack_index == 0) ? down : up;
			for (int32_t layer = 0; layer < (int32_t)cube_dimentions / 2; layer++) {
				for (int32_t i = layer; i < cube_dimentions-1-layer; i++) {
					std::swap(pieces[get_piece_index(f, i, layer)], pieces[get_piece_index(f, cube_dimentions-1-layer, i)]);
					std::swap(pieces[get_piece_index(f, i, layer)], pieces[get_piece_index(f, cube_dimentions-1-i, cube_dimentions-1-layer)]);
					std::swap(pieces[get_piece_index(f, i, layer)], pieces[get_piece_index(f, layer, cube_dimentions-1-i)]);
				}
			}
		}
	}
	else if (rotation_axis == NY) {
		int32_t& y = stack_index;
		for (int32_t x = 0; x < cube_dimentions; x++) {
			auto temp = pieces[get_piece_index(forward, x, y)];
			
			pieces[get_piece_index(forward,  x, y)]						= pieces[get_piece_index(left, x, y)];
			pieces[get_piece_index(left,	 x, y)]						= pieces[get_piece_index(backward, cube_dimentions - 1 - x, y)];
			pieces[get_piece_index(backward, cube_dimentions-1-x, y)] = pieces[get_piece_index(right, cube_dimentions - 1 - x, y)];
			pieces[get_piece_index(right,	 cube_dimentions-1-x, y)] = temp;
		}

		if (stack_index == 0 || stack_index == cube_dimentions - 1) {
			face f = (stack_index == 0) ? down : up;
			for (int32_t layer = 0; layer < (int32_t)cube_dimentions / 2; layer++) {
				for (int32_t i = layer; i < cube_dimentions-1-layer; i++) {
					std::swap(pieces[get_piece_index(f, i, layer)], pieces[get_piece_index(f, layer, cube_dimentions-1-i)]);
					std::swap(pieces[get_piece_index(f, i, layer)], pieces[get_piece_index(f, cube_dimentions-1-i, cube_dimentions-1-layer)]);
					std::swap(pieces[get_piece_index(f, i, layer)], pieces[get_piece_index(f, cube_dimentions-1-layer, i)]);
				}
			}
		}
	}
	else if (rotation_axis == Z) {
		int32_t& x = stack_index;
		for (int32_t y = 0; y < cube_dimentions; y++) {
			auto temp = pieces[get_piece_index(up, y, x)];
			
			pieces[get_piece_index(up,    y, x)]					 = pieces[get_piece_index(right, x, cube_dimentions-1-y)];
			pieces[get_piece_index(right, x, cube_dimentions-1-y)] = pieces[get_piece_index(down,	 cube_dimentions-1-y, x)];
			pieces[get_piece_index(down,  cube_dimentions-1-y, x)] = pieces[get_piece_index(left,  x, y)];
			pieces[get_piece_index(left,  x, y)]					 = temp;
		}

		if (stack_index == 0 || stack_index == cube_dimentions - 1) {
			face f = (stack_index == 0) ? backward : forward;
			for (int32_t layer = 0; layer < (int32_t)cube_dimentions / 2; layer++) {
				for (int32_t i = layer; i < cube_dimentions-1-layer; i++) {
					std::swap(pieces[get_piece_index(f, i, layer)], pieces[get_piece_index(f, cube_dimentions-1-layer, i)]);
					std::swap(pieces[get_piece_index(f, i, layer)], pieces[get_piece_index(f, cube_dimentions-1-i, cube_dimentions-1-layer)]);
					std::swap(pieces[get_piece_index(f, i, layer)], pieces[get_piece_index(f, layer, cube_dimentions-1-i)]);
				}
			}
		}
	}
	else if (rotation_axis == NZ) {
		int32_t& x = stack_index;
		for (int32_t y = 0; y < cube_dimentions; y++) {
			auto temp = pieces[get_piece_index(up, y, x)];
			
			pieces[get_piece_index(up,    y, x)]					 = pieces[get_piece_index(left, x, y)];
			pieces[get_piece_index(left,  x, y)]					 = pieces[get_piece_index(down, cube_dimentions - 1 - y, x)];
			pieces[get_piece_index(down,  cube_dimentions-1-y, x)] = pieces[get_piece_index(right, x, cube_dimentions - 1 - y)];
			pieces[get_piece_index(right, x, cube_dimentions-1-y)] = temp;
		}

		if (stack_index == 0 || stack_index == cube_dimentions - 1) {
			face f = (stack_index == 0) ? backward : forward;
			for (int32_t layer = 0; layer < (int32_t)cube_dimentions / 2; layer++) {
				for (int32_t i = layer; i < cube_dimentions-1-layer; i++) {
					std::swap(pieces[get_piece_index(f, i, layer)], pieces[get_piece_index(f, layer, cube_dimentions-1-i)]);
					std::swap(pieces[get_piece_index(f, i, layer)], pieces[get_piece_index(f, cube_dimentions-1-i, cube_dimentions-1-layer)]);
					std::swap(pieces[get_piece_index(f, i, layer)], pieces[get_piece_index(f, cube_dimentions-1-layer, i)]);
				}
			}
		}
	}
}

float RubiksCube::_ease_out_quint(float t)
{
	return 1 - std::pow(1 - t, 5);
}

float RubiksCube::_ease_out_elastic(float t)
{
	const float c4 = (2 * glm::pi<float>()) / 3;

	return t == 0
		? 0
		: t == 1
		? 1
		: std::pow(3, -10 * t) * std::sin((t * 10 - 0.85) * c4) + 1;
}

float RubiksCube::_ease_out_back(float t)
{
	const float c1 = 1.70158;
	const float c2 = c1 * 1.525;
	 
	return t < 0.5
		? (std::pow(2 * t, 2) * ((c2 + 1) * 2 * t - c2)) / 2
		: (std::pow(2 * t - 2, 2) * ((c2 + 1) * (t * 2 - 2) + c2) + 2) / 2;
}

RubiksCube::axis RubiksCube::invert_axis(axis axis)
{
	switch (axis) {
	case X:	 return NX;
	case Y:	 return NY;
	case Z:	 return NZ;
	case NX: return X;
	case NY: return Y;
	case NZ: return Z;
	}

	return X;
}

bool RubiksCube::is_move_animation_happening()
{
	return move_animation_playing;
}

void RubiksCube::force_finish_move_animation()
{
	if (move_animation_playing)
		_make_move(active_movement.stack_index, active_movement.rotation_axis);

	move_animation_playing = false;
}

void RubiksCube::move(size_t stack_index, axis rotation_axis)
{
	force_finish_move_animation();

	move_animation_playing = true;
	move_animation_begin = std::chrono::system_clock::now();
	set_movement(stack_index, rotation_axis, 0.0f);
}

void RubiksCube::set_movement(size_t stack_index, axis rotation_axis, float rotation_radian)
{
	active_movement = Movement(stack_index, rotation_axis, rotation_radian);
}

void RubiksCube::clear_movement()
{
	active_movement = Movement();
}

RubiksCube::piece_info RubiksCube::get_cursor_piece(glm::ivec2 coordinates)
{
	if (cursor_picking_texture == nullptr)
		return piece_info();

	Image image = *cursor_picking_texture->get_image(Texture2D::ColorFormat::RGBA, Texture2D::Type::FLOAT, 0, coordinates.x, coordinates.y, 1, 1);
	glm::vec4 data = *(glm::vec4*)image.get_image_data();
	
	std::cout << data.x << " " << data.y << " " << data.z << " " << data.w << std::endl;

	if (glm::any(glm::equal(data, glm::vec4(-1))))
		return not_a_piece;

	piece_info info;
	info.face = face(data.x);
	info.u = data.y;
	info.v = data.z;
	info.coordinate = glm::ivec3(
		(int32_t)data.w % cube_dimentions,
		(int32_t)(data.w / cube_dimentions) % cube_dimentions, 
		(int32_t)data.w / (cube_dimentions*cube_dimentions)
	);
	
	return info;
}

void RubiksCube::init()
{
	cube_renderer = std::make_shared<Program>(Shader(rubik_cube_shader_directory / "vertex.vert", rubik_cube_shader_directory / "rubik.frag"));
	cursor_picking_renderer = std::make_shared<Program>(Shader(rubik_cube_shader_directory / "vertex.vert", rubik_cube_shader_directory / "cursor.frag"));
	
	cursor_picking_framebuffer = std::make_shared<Framebuffer>();
	cursor_picking_texture= std::make_shared<Texture2D>(
		cursor_picking_texture_resolution.x, 
		cursor_picking_texture_resolution.y, 
		cursor_picking_texture_format,
		1, 0, 0);
	cursor_picking_texture_depth = std::make_shared<Renderbuffer>(
		cursor_picking_texture_resolution.x,
		cursor_picking_texture_resolution.y,
		Texture2D::DepthStencilTextureFormat::DEPTH_COMPONENT24,
		0);
	cursor_picking_framebuffer->attach_color(0, cursor_picking_texture);
	cursor_picking_framebuffer->attach_depth(cursor_picking_texture_depth);
	cursor_picking_framebuffer->activate_draw_buffer(0);

	SingleModel unit_plane;
	unit_plane.verticies = {
		glm::vec3( 0.49,  0.49, 0),
		glm::vec3( 0.49, -0.49, 0),
		glm::vec3(-0.49,  0.49, 0),
		glm::vec3(-0.49, -0.49, 0)
	};
	unit_plane.indicies = {
		0, 1, 2,
		2, 1, 3
	};

	pieces.clear();
	pieces.resize(get_piece_count());

	colors[left		] = glm::vec3(232, 232, 233) / 255.0f;
	colors[right	] = glm::vec3(254, 240,  18) / 255.0f;
	colors[up		] = glm::vec3( 61, 217,  41) / 255.0f;
	colors[down		] = glm::vec3(  0, 111, 241) / 255.0f;
	colors[backward ] = glm::vec3(255,  30,  64) / 255.0f;
	colors[forward	] = glm::vec3(255, 145,  53) / 255.0f;

	for (int32_t f = 0; f < 6; f++)
	for (int32_t x = 0; x < cube_dimentions; x++)
	for (int32_t y = 0; y < cube_dimentions; y++)
		set_piece_id(face(f), x, y, f);

	mesh_plane = std::make_shared<Mesh>();
	mesh_plane->load_model(unit_plane);
}

void RubiksCube::_update_move_animation()
{
	if (!move_animation_playing) {
		set_movement(0, X, 0);
		return;
	}

	auto animation_time = std::chrono::system_clock::now() - move_animation_begin;
	float animation_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(animation_time).count();
	float animation_duration_ms = move_animation_duration_s * 1000;

	if (animation_time_ms >= animation_duration_ms) {
		_make_move(active_movement.stack_index, active_movement.rotation_axis);
		set_movement(0, X, 0);
		move_animation_playing = false;
	}

	float animation_percentage = _ease_out_elastic(animation_time_ms / animation_duration_ms);
	float angle_radian = animation_percentage * glm::pi<float>() / 2.0f;
	
	bool inverted_movement = 
		active_movement.rotation_axis == NX || 
		active_movement.rotation_axis == NY || 
		active_movement.rotation_axis == NZ;

	active_movement.rotation_radian = inverted_movement ? -angle_radian : angle_radian;
}

void RubiksCube::_render_piece(Camera& camera, glm::ivec3 coordinate, bool render_to_cursor_picking)
{
	if (glm::any(glm::lessThan(coordinate, glm::ivec3(0, 0, 0))))
		return;
	if (glm::any(glm::greaterThanEqual(coordinate, glm::ivec3(cube_dimentions))))
		return;
	if (glm::all(glm::notEqual(coordinate, glm::ivec3(0))) && glm::all(glm::notEqual(coordinate, glm::ivec3(cube_dimentions-1))))
		return;

	camera.update_matrixes();

	if (!render_to_cursor_picking)
		camera.update_default_uniforms(*cube_renderer);
	else
		camera.update_default_uniforms(*cursor_picking_renderer);

	//glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	if (coordinate.x == 0) {
		constexpr face face = face::left;

		glm::mat4 piece_position = glm::identity<glm::mat4>();
		if ((active_movement.rotation_axis == X || active_movement.rotation_axis == NX) && active_movement.stack_index == 0)
			piece_position = glm::rotate(piece_position, active_movement.rotation_radian, glm::vec3(1, 0, 0));
		if ((active_movement.rotation_axis == Y || active_movement.rotation_axis == NY) && active_movement.stack_index == coordinate.y)
			piece_position = glm::rotate(piece_position, active_movement.rotation_radian, glm::vec3(0, 1, 0));
		if ((active_movement.rotation_axis == Z || active_movement.rotation_axis == NZ) && active_movement.stack_index == coordinate.z)
			piece_position = glm::rotate(piece_position, active_movement.rotation_radian, glm::vec3(0, 0, 1));

		piece_position = glm::translate(piece_position, glm::vec3(-cube_dimentions / 2.0f, -cube_dimentions / 2.0f + 0.5 + coordinate.y, -(-cube_dimentions / 2.0f + 0.5 + coordinate.z)));
		piece_position = glm::rotate(piece_position, glm::pi<float>() / 2, glm::vec3(0, 1, 0));

		if (!render_to_cursor_picking) {
			cube_renderer->update_uniform("color", glm::vec4(get_piece_color(face, coordinate.z, coordinate.y), 1));
			cube_renderer->update_uniform("model", piece_position);
			primitive_renderer::render(
				*cube_renderer,
				*mesh_plane->get_mesh(0),
				RenderParameters()
			);
		}
		else {
			int32_t piece_index =
				cube_dimentions * cube_dimentions * coordinate.z +
				cube_dimentions * coordinate.y +
				coordinate.x;
			cursor_picking_renderer->update_uniform("value", glm::vec4(face, coordinate.z, coordinate.y, piece_index));
			cursor_picking_renderer->update_uniform("model", piece_position);
			primitive_renderer::render(
				*cursor_picking_renderer,
				*mesh_plane->get_mesh(0),
				RenderParameters()
			);
		}
	}
	if (coordinate.x == cube_dimentions - 1) {
		constexpr face face = face::right;

		glm::mat4 piece_position = glm::identity<glm::mat4>();
		if ((active_movement.rotation_axis == X || active_movement.rotation_axis == NX) && active_movement.stack_index == cube_dimentions - 1)
			piece_position = glm::rotate(piece_position, active_movement.rotation_radian, glm::vec3(1, 0, 0));
		if ((active_movement.rotation_axis == Y || active_movement.rotation_axis == NY) && active_movement.stack_index == coordinate.y)
			piece_position = glm::rotate(piece_position, active_movement.rotation_radian, glm::vec3(0, 1, 0));
		if ((active_movement.rotation_axis == Z || active_movement.rotation_axis == NZ) && active_movement.stack_index == coordinate.z)
			piece_position = glm::rotate(piece_position, active_movement.rotation_radian, glm::vec3(0, 0, 1));
		piece_position = glm::translate(piece_position, glm::vec3(+cube_dimentions / 2.0f, -cube_dimentions / 2.0f + 0.5 + coordinate.y, -(-cube_dimentions / 2.0f + 0.5 + coordinate.z)));
		piece_position = glm::rotate(piece_position, -glm::pi<float>() / 2, glm::vec3(0, 1, 0));

		if (!render_to_cursor_picking) {
			cube_renderer->update_uniform("color", glm::vec4(get_piece_color(face, coordinate.z, coordinate.y), 1));
			cube_renderer->update_uniform("model", piece_position);
			primitive_renderer::render(
				*cube_renderer,
				*mesh_plane->get_mesh(0),
				RenderParameters()
			);
		}
		else {
			int32_t piece_index =
				cube_dimentions * cube_dimentions * coordinate.z +
				cube_dimentions * coordinate.y +
				coordinate.x;
			cursor_picking_renderer->update_uniform("value", glm::vec4(face, coordinate.z, coordinate.y, piece_index));
			cursor_picking_renderer->update_uniform("model", piece_position);
			primitive_renderer::render(
				*cursor_picking_renderer,
				*mesh_plane->get_mesh(0),
				RenderParameters()
			);
		}
	}
	if (coordinate.y == 0) {
		constexpr face face = face::down;

		glm::mat4 piece_position = glm::identity<glm::mat4>();
		if ((active_movement.rotation_axis == Y || active_movement.rotation_axis == NY) && active_movement.stack_index == 0)
			piece_position = glm::rotate(piece_position, active_movement.rotation_radian, glm::vec3(0, 1, 0));
		if ((active_movement.rotation_axis == X || active_movement.rotation_axis == NX) && active_movement.stack_index == coordinate.x)
			piece_position = glm::rotate(piece_position, active_movement.rotation_radian, glm::vec3(1, 0, 0));
		if ((active_movement.rotation_axis == Z || active_movement.rotation_axis == NZ) && active_movement.stack_index == coordinate.z)
			piece_position = glm::rotate(piece_position, active_movement.rotation_radian, glm::vec3(0, 0, 1));
		piece_position = glm::translate(piece_position, glm::vec3(-cube_dimentions / 2.0f + 0.5 + coordinate.x, -cube_dimentions / 2.0f, -(-cube_dimentions / 2.0f + 0.5 + coordinate.z)));
		piece_position = glm::rotate(piece_position, -glm::pi<float>() / 2, glm::vec3(1, 0, 0));

		if (!render_to_cursor_picking) {
			cube_renderer->update_uniform("color", glm::vec4(get_piece_color(face, coordinate.x, coordinate.z), 1));
			cube_renderer->update_uniform("model", piece_position);
			primitive_renderer::render(
				*cube_renderer,
				*mesh_plane->get_mesh(0),
				RenderParameters()
			);
		}
		else {
			int32_t piece_index =
				cube_dimentions * cube_dimentions * coordinate.z +
				cube_dimentions * coordinate.y +
				coordinate.x;
			cursor_picking_renderer->update_uniform("value", glm::vec4(face, coordinate.x, coordinate.z, piece_index));
			cursor_picking_renderer->update_uniform("model", piece_position);
			primitive_renderer::render(
				*cursor_picking_renderer,
				*mesh_plane->get_mesh(0),
				RenderParameters()
			);
		}
	}
	if (coordinate.y == cube_dimentions - 1) {
		constexpr face face = face::up;

		glm::mat4 piece_position = glm::identity<glm::mat4>();
		if ((active_movement.rotation_axis == Y || active_movement.rotation_axis == NY) && active_movement.stack_index == cube_dimentions - 1)
			piece_position = glm::rotate(piece_position, active_movement.rotation_radian, glm::vec3(0, 1, 0));
		if ((active_movement.rotation_axis == X || active_movement.rotation_axis == NX) && active_movement.stack_index == coordinate.x)
			piece_position = glm::rotate(piece_position, active_movement.rotation_radian, glm::vec3(1, 0, 0));
		if ((active_movement.rotation_axis == Z || active_movement.rotation_axis == NZ) && active_movement.stack_index == coordinate.z)
			piece_position = glm::rotate(piece_position, active_movement.rotation_radian, glm::vec3(0, 0, 1));
		piece_position = glm::translate(piece_position, glm::vec3(-cube_dimentions / 2.0f + 0.5 + coordinate.x, +cube_dimentions / 2.0f, -(-cube_dimentions / 2.0f + 0.5 + coordinate.z)));
		piece_position = glm::rotate(piece_position, glm::pi<float>() / 2, glm::vec3(1, 0, 0));

		if (!render_to_cursor_picking) {
			cube_renderer->update_uniform("color", glm::vec4(get_piece_color(face, coordinate.x, coordinate.z), 1));
			cube_renderer->update_uniform("model", piece_position);
			primitive_renderer::render(
				*cube_renderer,
				*mesh_plane->get_mesh(0),
				RenderParameters()
			);
		}
		else {
			int32_t piece_index =
				cube_dimentions * cube_dimentions * coordinate.z +
				cube_dimentions * coordinate.y +
				coordinate.x;
			cursor_picking_renderer->update_uniform("value", glm::vec4(face, coordinate.x, coordinate.z, piece_index));
			cursor_picking_renderer->update_uniform("model", piece_position);
			primitive_renderer::render(
				*cursor_picking_renderer,
				*mesh_plane->get_mesh(0),
				RenderParameters()
			);
		}
	}
	if (coordinate.z == 0) {
		constexpr face face = face::backward;

		glm::mat4 piece_position = glm::identity<glm::mat4>();
		if ((active_movement.rotation_axis == Z || active_movement.rotation_axis == NZ) && active_movement.stack_index == 0)
			piece_position = glm::rotate(piece_position, active_movement.rotation_radian, glm::vec3(0, 0, 1));
		if ((active_movement.rotation_axis == X || active_movement.rotation_axis == NX) && active_movement.stack_index == coordinate.x)
			piece_position = glm::rotate(piece_position, active_movement.rotation_radian, glm::vec3(1, 0, 0));
		if ((active_movement.rotation_axis == Y || active_movement.rotation_axis == NY) && active_movement.stack_index == coordinate.y)
			piece_position = glm::rotate(piece_position, active_movement.rotation_radian, glm::vec3(0, 1, 0));
		piece_position = glm::translate(piece_position, glm::vec3(-cube_dimentions / 2.0f + 0.5 + coordinate.x, -cube_dimentions / 2.0f + 0.5 + coordinate.y, +cube_dimentions / 2.0f));
		piece_position = glm::rotate(piece_position, glm::pi<float>(), glm::vec3(1, 0, 0));

		if (!render_to_cursor_picking) {
			cube_renderer->update_uniform("color", glm::vec4(get_piece_color(face, coordinate.x, coordinate.y), 1));
			cube_renderer->update_uniform("model", piece_position);
			primitive_renderer::render(
				*cube_renderer,
				*mesh_plane->get_mesh(0),
				RenderParameters()
			);
		}
		else {
			int32_t piece_index =
				cube_dimentions * cube_dimentions * coordinate.z +
				cube_dimentions * coordinate.y +
				coordinate.x;
			cursor_picking_renderer->update_uniform("value", glm::vec4(face, coordinate.x, coordinate.y, piece_index));
			cursor_picking_renderer->update_uniform("model", piece_position);
			primitive_renderer::render(
				*cursor_picking_renderer,
				*mesh_plane->get_mesh(0),
				RenderParameters()
			);
		}
	}
	if (coordinate.z == cube_dimentions - 1) {
		constexpr face face = face::forward;

		glm::mat4 piece_position = glm::identity<glm::mat4>();
		if ((active_movement.rotation_axis == Z || active_movement.rotation_axis == NZ) && active_movement.stack_index == cube_dimentions - 1)
			piece_position = glm::rotate(piece_position, active_movement.rotation_radian, glm::vec3(0, 0, 1));
		if ((active_movement.rotation_axis == X || active_movement.rotation_axis == NX) && active_movement.stack_index == coordinate.x)
			piece_position = glm::rotate(piece_position, active_movement.rotation_radian, glm::vec3(1, 0, 0));
		if ((active_movement.rotation_axis == Y || active_movement.rotation_axis == NY) && active_movement.stack_index == coordinate.y)
			piece_position = glm::rotate(piece_position, active_movement.rotation_radian, glm::vec3(0, 1, 0));
		piece_position = glm::translate(piece_position, glm::vec3(-cube_dimentions/2.0f + 0.5 + coordinate.x, -cube_dimentions/2.0f + 0.5 + coordinate.y, -cube_dimentions/2.0f));
		piece_position = glm::rotate(piece_position, 0.0f, glm::vec3(1, 0, 0));

		if (!render_to_cursor_picking) {
			cube_renderer->update_uniform("color", glm::vec4(get_piece_color(face, coordinate.x, coordinate.y), 1));
			cube_renderer->update_uniform("model", piece_position);
			primitive_renderer::render(
				*cube_renderer,
				*mesh_plane->get_mesh(0),
				RenderParameters()
			);
		}
		else {
			int32_t piece_index =
				cube_dimentions * cube_dimentions * coordinate.z +
				cube_dimentions * coordinate.y +
				coordinate.x;
			cursor_picking_renderer->update_uniform("value", glm::vec4(face, coordinate.x, coordinate.y, piece_index));
			cursor_picking_renderer->update_uniform("model", piece_position);
			primitive_renderer::render(
				*cursor_picking_renderer,
				*mesh_plane->get_mesh(0),
				RenderParameters()
			);
		}
	}

}

bool RubiksCube::piece_info::operator==(const piece_info& other)
{
	if(face != other.face) return false;
	if(u != other.u) return false;
	if(v != other.v) return false;
	if(coordinate != other.coordinate) return false;
	return true;
}
