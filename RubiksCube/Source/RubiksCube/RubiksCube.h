#pragma once

#include "FrameBuffer.h"
#include "Mesh.h"
#include "Camera.h"
#include "ShaderCompiler.h"

#include <chrono>

class RubiksCube {
public:

	RubiksCube(int32_t cube_dimentions = 3);
	~RubiksCube() = default;

	void render(Framebuffer& target_framebuffer, Camera& camera);
	void render(Camera& camera);

	enum face {
		left	 = 0,
		right	 = 1,
		up		 = 2,
		down	 = 3,
		backward = 4,
		forward	 = 5,
	};

	enum axis {
		X,
		Y,
		Z,
		NX,
		NY,
		NZ
	};
	static axis invert_axis(axis axis);

	struct piece_info {
		piece_info() = default;
		bool operator==(const piece_info& other);
		face face = left;
		int32_t u = -1;
		int32_t v = -1;
		glm::ivec3 coordinate = glm::ivec3(-1);
	};

	struct Movement {
		Movement(size_t stack_index = 0, axis rotation_axis = X, float rotation_radian = 0) : 
			stack_index(stack_index), rotation_axis(rotation_axis), rotation_radian(rotation_radian) {}
		size_t stack_index = 0;
		axis rotation_axis = X;
		float rotation_radian = 0;
	};

	bool is_move_animation_happening();
	void force_finish_move_animation();
	void move(size_t stack_index, axis rotation_axis);

	void set_movement(size_t stack_index, axis rotation_axis, float rotation_radian);
	void clear_movement();

	constexpr static piece_info not_a_piece = piece_info();
	piece_info get_cursor_piece(glm::vec2 normalized_coordinates);

	const int32_t cube_dimentions;

	void highlight_face(face f, glm::ivec2 uv);
	void clear_highlight_face();
	void highlight_piece(glm::ivec3 coordinate);
	void clear_highlight_piece();

private:

	void init();
	
	int32_t get_piece_index(face surface, int32_t x, int32_t y);
	uint8_t get_piece_id(face surface, int32_t x, int32_t y);
	glm::vec3 get_piece_color(face surface, int32_t x, int32_t y);
	void set_piece_id(face surface, int32_t x, int32_t y, uint8_t id);
	int32_t get_piece_count();
	
	void _make_move(int32_t stack_index, axis rotation_axis);

	constexpr static float move_animation_duration_s = 1.8f;
	float _ease_out_quint(float t);
	float _ease_out_elastic(float t);
	float _ease_out_back(float t);

	glm::vec3 alpha_blending(glm::vec3 background, glm::vec3 foreground, float alpha);

	void _update_move_animation();
	void _render_piece(Camera& camera, glm::ivec3 coordinate, bool render_to_cursor_picking = false);

	std::chrono::time_point<std::chrono::system_clock> move_animation_begin;
	bool move_animation_playing = false;

	Movement active_movement;
	
	glm::vec3 highlight_color;
	float highlight_alpha;
	glm::ivec3 highlighted_piece_coordinate;
	bool piece_highlighted = false;
	face highlighted_face;
	glm::ivec2 highlighted_face_uv;
	bool face_highlighted = false;

	glm::vec3 colors[6];
	std::vector<uint8_t> pieces;

	std::shared_ptr<Program> cube_renderer;
	std::shared_ptr<Mesh> mesh_plane;

	glm::ivec2 cursor_picking_texture_resolution = glm::ivec2(1024, 1024);
	Texture2D::ColorTextureFormat cursor_picking_texture_format = Texture2D::ColorTextureFormat::RGBA32F;
	std::shared_ptr<Program> cursor_picking_renderer;
	std::shared_ptr<Texture2D> cursor_picking_texture;
	std::shared_ptr<Renderbuffer> cursor_picking_texture_depth;
	std::shared_ptr<Framebuffer> cursor_picking_framebuffer;
};
