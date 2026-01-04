#define PTI_IMPL
#include "pti.h"

// sokol
#include "sokol/sokol_app.h"
#include "sokol/sokol_audio.h"
#include "sokol/sokol_log.h"

#if defined(PTI_DEBUG)
#include "dbgui/dbgui.h"
#include "imgui/imgui.h"
#endif

#include <iostream>

// opengl
#if defined(SOKOL_GLCORE)
#if defined(__APPLE__)
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#elif defined(__linux__) || defined(__unix__)
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#else
#error ("opengl.h: Unknown platform")
#endif
#elif defined(SOKOL_GLES3)
#include <GLES3/gl3.h>
#else
#error ("opengl.h: unknown 3D API selected; must be SOKOL_GLCORE or SOKOL_GLES3")
#endif

// forward declarations
static void init(void);
static void frame(void);
static void cleanup(void);
static void event(const sapp_event *);

// main entry point
sapp_desc sokol_main(int argc, char *argv[]) {
	(void) argc;
	(void) argv;

	pti_desc desc = pti_main(argc, argv);
	pti_init(&desc);

	int width = desc.width * 3;
	int height = desc.height * 3;

	return (sapp_desc) {
			.init_cb = init,
			.frame_cb = frame,
			.cleanup_cb = cleanup,
			.event_cb = event,
			.width = width,
			.height = height,
			.window_title = "pti (debug)",
			.logger = {
					.func = slog_func,
			},
#if defined(SOKOL_GLCORE)
#if defined(__APPLE__)
			.gl = {
					.major_version = 4,
					.minor_version = 1,
			}
#else
			.gl = {
					.major_version = 4,
					.minor_version = 2,
			}
#endif
#endif
	};
}

static struct {
	struct {
		GLuint vao;
		GLuint vbo;
		GLuint color0;
		GLuint program;
		GLuint crt;
		GLuint tileset;
		GLuint font;
	} gl;

	bool crt = false;

	pti_sound_t tone;


#if defined(PTI_TRACE_HOOKS)
	pti_trace_hooks hooks;
#endif
} state;

static GLuint create_shader(GLenum stage, const char *sourceCode) {
	unsigned int shader = glCreateShader(stage);
	glShaderSource(shader, 1, &sourceCode, NULL);
	glCompileShader(shader);
	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		printf("Failed to compile shader: %s", infoLog);
	}
	return shader;
}

static GLuint create_program(GLuint vs, GLuint fs) {
	GLuint program = glCreateProgram();
	//Attach each stage
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	//Link all the stages together
	glLinkProgram(program);
	int success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		printf("Failed to link shader program: %s", infoLog);
	}
	return program;
}

static void gl_init(void) {
	const char *default_vs_src =
#if defined(SOKOL_GLCORE)
			"#version 410\n"
#elif defined(SOKOL_GLES3)
			"#version 300 es\n"
			"precision mediump float;\n"
#endif
			"layout(location=0) in vec2 pos;\n"
			"out vec2 vs_texcoord;\n"
			"void main() {\n"
			"  gl_Position = vec4((pos.xy - 0.5) * 2.0, 0.0, 1.0);\n"
			"  vs_texcoord = vec2(pos.x, 1.0 - pos.y);\n"
			"}\n";

	const char *default_fs_src =
#if defined(SOKOL_GLCORE)
			"#version 410\n"
#elif defined(SOKOL_GLES3)
			"#version 300 es\n"
			"precision mediump float;\n"
#endif
			"uniform sampler2D screen;\n"
			"in vec2 vs_texcoord;\n"
			"out vec4 frag_color;\n"
			"void main() {\n"
			"  vec4 texel = texture(screen, vs_texcoord);\n"
			"  frag_color = texel;\n"
			"}\n";

	const char *crt_fs_src =
#if defined(SOKOL_GLCORE)
			"#version 410\n"
#elif defined(SOKOL_GLES3)
			"#version 300 es\n"
			"precision mediump float;\n"
			"precision mediump sampler2D;\n"
#endif
			"float PI = 3.14159;\n"
			"uniform sampler2D screen;\n"
			"in vec2 vs_texcoord;\n"
			"out vec4 frag_color;\n"
			"vec2 curvature = vec2(10.0);\n"
			"vec2 scanLineOpacity = vec2(0.5);\n"
			"float brightness = 2.0;\n"
			"float vignetteOpacity = 0.5;\n"
			"float vignetteRoundness = 1.0;\n"
			"vec2 curveRemapUV(vec2 uv) {\n"
			"  uv = uv * 2.0 - 1.0;\n"
			"  vec2 offset = abs(uv.yx) / vec2(curvature.x, curvature.y);\n"
			"  uv = uv + uv * offset * offset;\n"
			"  uv = uv * 0.5 + 0.5;\n"
			"  return uv;\n"
			"}\n"
			"vec3 scanLineIntensity(float uv, float resolution, float opacity) {\n"
			"  float intensity = sin(uv * resolution * PI * 2.0);\n"
			"  intensity = ((0.5 * intensity) + 0.5) * 0.9 + 0.1;\n"
			"  return vec3(pow(intensity, opacity));\n"
			"}\n"
			"vec3 vignetteIntensity(vec2 uv, vec2 resolution, float opacity, float roundness) {\n"
			"  float intensity = uv.x * uv.y * (1.0 - uv.x) * (1.0 - uv.y);\n"
			"  return vec3(clamp(pow((resolution.x / roundness) * intensity, opacity), 0.0, 1.0));\n"
			"}\n"
			"void main() {\n"
			"  vec2 uv = curveRemapUV(vs_texcoord);\n"
			"  if (uv.x < 0.0 || uv.y < 0.0 || uv.x > 1.0 || uv.y > 1.0) {\n"
			"    frag_color = vec4(0.0, 0.0, 0.0, 1.0);\n"
			"    return;\n"
			"  }\n"
			"  vec3 base_color = texture(screen, uv).rgb;\n"
			"  vec2 resolution = vec2(textureSize(screen, 0));\n"
			"  base_color *= vignetteIntensity(uv, resolution, vignetteOpacity, vignetteRoundness);\n"
			"  base_color *= scanLineIntensity(uv.x, resolution.x, scanLineOpacity.x);\n"
			"  base_color *= scanLineIntensity(uv.y, resolution.y, scanLineOpacity.y);\n"
			"  base_color *= brightness;\n"
			"  frag_color = vec4(base_color, 1.0);\n"
			"}\n";

	// clang-format off
	float vertices[] = {
		// pos (x, y) texcoord (u, v)
		-1.0f,  1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f,

		-1.0f,  1.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		1.0f,  1.0f, 1.0f, 1.0f,
	};
	// clang-format on

	// initialize fullscreen quad, buffer object
	glGenVertexArrays(1, &state.gl.vao);
	glGenBuffers(1, &state.gl.vbo);

	// bind vao, and vbo
	glBindVertexArray(state.gl.vao);
	glBindBuffer(GL_ARRAY_BUFFER, state.gl.vbo);

	// buffer data to vbo
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

	// positions and texcoords
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (sizeof(float) * 2));

	glBindVertexArray(0);

	const int width = _pti.vm.screen.width;
	const int height = _pti.vm.screen.height;

	// create texture
	glGenTextures(1, &state.gl.color0);
	glBindTexture(GL_TEXTURE_2D, state.gl.color0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	GLuint default_vs_stage = create_shader(GL_VERTEX_SHADER, default_vs_src);
	GLuint default_fs_stage = create_shader(GL_FRAGMENT_SHADER, default_fs_src);
	GLuint crt_fs_stage = create_shader(GL_FRAGMENT_SHADER, crt_fs_src);

	state.gl.program = create_program(default_vs_stage, default_fs_stage);
	state.gl.crt = create_program(default_vs_stage, crt_fs_stage);

	glDeleteShader(default_vs_stage);
	glDeleteShader(default_fs_stage);
}

void gl_draw() {
	const int width = _pti.vm.screen.width;
	const int height = _pti.vm.screen.height;

	// clear default buffer
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// upload pixel data
	glBindTexture(GL_TEXTURE_2D, state.gl.color0);
	glActiveTexture(GL_TEXTURE0);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, _pti.screen);

	// bind shader
	GLuint program = state.crt ? state.gl.crt : state.gl.program;
	glUseProgram(program);
	glUniform1i(glGetUniformLocation(state.gl.program, "screen"), 0);

	// draw fullscreen quad
	glBindVertexArray(state.gl.vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, state.gl.color0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

static void sokol_audio_cb(float *buffer, int num_frames, int num_channels) {
	// always clear buffer
	// memset(buffer, 0, num_frames * num_channels * sizeof(float));

	for (int i = 0; i < num_frames; i++) {
		int16_t mixed = _pti__audio_mix_sample();

		float f = mixed / 32768.0f;
		for (int c = 0; c < num_channels; c++) {
			buffer[i * num_channels + c] = f;
		}
	}
}

#if defined(PTI_DEBUG)
void imgui_debug_draw() {
	static const ImVec2 uv_min(0.0f, 0.0f);
	static const ImVec2 uv_max(1.0f, 1.0f);

	static bool show_display_window = false;
	static bool show_graphics_window = false;
	static bool show_hardware_window = false;
	static bool show_memory_window = false;
	static bool show_cart_window = false;
	static bool show_tileset_window = false;
	static bool show_font_window = false;
	static bool show_tilemap_window = false;

	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("pti")) {
			if (ImGui::MenuItem("Display")) {
				show_display_window = true;
			}
			if (ImGui::MenuItem("Graphics")) {
				show_graphics_window = true;
			}
			if (ImGui::MenuItem("Hardware")) {
				show_hardware_window = true;
			}
			if (ImGui::MenuItem("Memory")) {
				show_memory_window = true;
			}
			if (ImGui::MenuItem("Cart")) {
				show_cart_window = true;
			}
			if (ImGui::MenuItem("Tileset")) {
				show_tileset_window = true;
			}
			if (ImGui::MenuItem("Font")) {
				show_font_window = true;
			}
			if (ImGui::MenuItem("Tilemap")) {
				show_tilemap_window = true;
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	if (show_display_window) {
		ImGui::SetNextWindowSize(ImVec2(440, 400), ImGuiCond_Once);
		if (ImGui::Begin("Display", &show_display_window)) {
			ImGui::Text("Dimensions: (%d, %d)", _pti.vm.screen.width, _pti.vm.screen.height);
			ImGui::Text("Location: %p", _pti.screen);
			ImGui::Checkbox("CRT: ", &state.crt);

			const ImVec2 color0_vec2{(float) _pti.vm.screen.width, (float) _pti.vm.screen.height};
			ImGui::Image((ImTextureID) (intptr_t) state.gl.color0, color0_vec2, uv_min, uv_max);
		}
		ImGui::End();
	}

	if (show_graphics_window) {
		ImGui::SetNextWindowSize(ImVec2(440, 400), ImGuiCond_Once);
		if (ImGui::Begin("Graphics", &show_graphics_window)) {
			ImGui::Text("Camera: (%d, %d)", _pti.vm.draw.cam_x, _pti.vm.draw.cam_y);
			ImGui::Text("Clip: ((%d, %d), (%d, %d))", _pti.vm.draw.clip_x0, _pti.vm.draw.clip_y0, _pti.vm.draw.clip_x1, _pti.vm.draw.clip_y1);
			ImGui::Text("Color Key: %d", _pti.vm.draw.ckey);
			ImGui::Text("Dither: %d", _pti.vm.draw.dither);
		}
		ImGui::End();
	}

	if (show_hardware_window) {
		ImGui::SetNextWindowSize(ImVec2(440, 400), ImGuiCond_Once);
		if (ImGui::Begin("Hardware", &show_hardware_window)) {
			// buttons
			ImGui::Text("Botton [Left]: %d", _pti.vm.hardware.btn_state[0]);
			ImGui::Text("Botton [Right]: %d", _pti.vm.hardware.btn_state[1]);
			ImGui::Text("Botton [Up]: %d", _pti.vm.hardware.btn_state[2]);
			ImGui::Text("Botton [Down]: %d", _pti.vm.hardware.btn_state[3]);
			ImGui::Text("Botton [D]: %d", _pti.vm.hardware.btn_state[4]);
			ImGui::Text("Botton [S]: %d", _pti.vm.hardware.btn_state[5]);
			ImGui::Text("Botton [W]: %d", _pti.vm.hardware.btn_state[6]);
			ImGui::Text("Botton [A]: %d", _pti.vm.hardware.btn_state[7]);
			ImGui::Text("Botton [8]: %d", _pti.vm.hardware.btn_state[8]);

			// random
			ImGui::Text("Random [0]: %d", _pti.vm.hardware.rnd_reg[0]);
			ImGui::Text("Random [1]: %d", _pti.vm.hardware.rnd_reg[1]);
			ImGui::Text("Random [2]: %d", _pti.vm.hardware.rnd_reg[2]);
			ImGui::Text("Random [3]: %d", _pti.vm.hardware.rnd_reg[3]);
		}
		ImGui::End();
	}

	if (show_memory_window) {
		ImGui::SetNextWindowSize(ImVec2(440, 400), ImGuiCond_Once);
		if (ImGui::Begin("Memory", &show_memory_window)) {
			const size_t used_bytes = (size_t) (_pti.ram.it - _pti.ram.begin);
			const size_t capacity_bytes = (size_t) (_pti.ram.cap - _pti.ram.begin);

			const float used_kb = used_bytes / 1024.0f;
			const float capacity_kb = capacity_bytes / 1024.0f;

			ImGui::Text("Usage: %.2f KB / %.2f KB (%.2f%%)\n", used_kb, capacity_kb, (used_kb / capacity_kb) * 100.0);
			ImGui::Text("begin: %p", _pti.ram.begin);
			ImGui::Text("cap: %p", _pti.ram.cap);
			ImGui::Text("end: %p", _pti.ram.end);
			ImGui::Text("it: %p", _pti.ram.it);
		}
		ImGui::End();
	}

	if (show_cart_window) {
		ImGui::SetNextWindowSize(ImVec2(440, 400), ImGuiCond_Once);
		if (ImGui::Begin("Cart", &show_cart_window)) {
			const size_t used_bytes = (size_t) (_pti.cart.it - _pti.cart.begin);
			const size_t capacity_bytes = (size_t) (_pti.cart.cap - _pti.cart.begin);

			const float used_kb = used_bytes / 1024.0f;
			const float capacity_kb = capacity_bytes / 1024.0f;

			ImGui::Text("Usage: %.2f KB / %.2f KB (%.2f%%)\n", used_kb, capacity_kb, (used_kb / capacity_kb) * 100.0);
			ImGui::Text("begin: %p", _pti.cart.begin);
			ImGui::Text("cap: %p", _pti.cart.cap);
			ImGui::Text("end: %p", _pti.cart.end);
			ImGui::Text("it: %p", _pti.cart.it);
		}
		ImGui::End();
	}

	if (show_tileset_window) {
		ImGui::SetNextWindowSize(ImVec2(440, 400), ImGuiCond_Once);
		if (ImGui::Begin("Tileset", &show_tileset_window)) {
			ImGui::Text("Width: %d", _pti.vm.draw.tileset->width);
			ImGui::Text("Height: %d", _pti.vm.draw.tileset->height);
			ImGui::Text("Tile Width: %d", _pti.vm.draw.tileset->tile_w);
			ImGui::Text("Tile Height: %d", _pti.vm.draw.tileset->tile_h);
			ImGui::Text("Pixels: %p", _pti.vm.draw.tileset->pixels);

			const ImVec2 tileset_vec2{(float) _pti.vm.draw.tileset->width, (float) _pti.vm.draw.tileset->height};
			ImGui::Image((ImTextureID) (intptr_t) state.gl.tileset, tileset_vec2, uv_min, uv_max);
		}
		ImGui::End();
	}

	if (show_font_window) {
		ImGui::SetNextWindowSize(ImVec2(440, 400), ImGuiCond_Once);
		if (ImGui::Begin("Font", &show_font_window)) {
			ImGui::Text("Width: %d", _pti.vm.draw.font->width);
			ImGui::Text("Height: %d", _pti.vm.draw.font->height);
			ImGui::Text("Pixels: %p", _pti.vm.draw.font->pixels);

			const ImVec2 font_vec2{(float) _pti.vm.draw.font->width, (float) _pti.vm.draw.font->height};
			ImGui::Image((ImTextureID) (intptr_t) state.gl.font, font_vec2, uv_min, uv_max);
		}
		ImGui::End();
	}

	if (show_tilemap_window) {
		ImGui::SetNextWindowSize(ImVec2(440, 400), ImGuiCond_Once);
		if (ImGui::Begin("Tilemap", &show_tilemap_window)) {
			ImGui::Text("Width: %d", _pti.vm.tilemap->width);
			ImGui::Text("Height: %d", _pti.vm.tilemap->height);
			ImGui::Text("Tiles: %p", _pti.vm.tilemap->tiles);
		}
		ImGui::End();
	}
}
#endif

static void _pti_set_font(pti_bitmap_t *ptr) {
	glGenTextures(1, &state.gl.font);
	glBindTexture(GL_TEXTURE_2D, state.gl.font);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, ptr->width, ptr->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, ptr->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
}

static void _pti_set_tilemap(pti_tilemap_t *ptr) {
	// create texture (tileset)
}

static void _pti_set_tileset(pti_tileset_t *ptr) {
	const uint16_t width = ptr->width;
	const uint16_t height = ptr->height;
	glGenTextures(1, &state.gl.tileset);
	glBindTexture(GL_TEXTURE_2D, state.gl.tileset);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, ptr->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
}

static void init(void) {
	/* initialize graphics */
	gl_init();

	/* initialize audio */
	auto audio_desc = (saudio_desc) {
			.num_channels = 2,
			.stream_cb = sokol_audio_cb,
			.logger = {
					.func = slog_func,
			}};
	saudio_setup(audio_desc);

#if defined(PTI_DEBUG)
	/* initialize debug ui */
	__dbgui_setup();
#endif

#if defined(PTI_TRACE_HOOKS)
	/* install tracehooks */
	pti_trace_hooks hooks;
	hooks.set_font = _pti_set_font;
	hooks.set_tilemap = _pti_set_tilemap;
	hooks.set_tileset = _pti_set_tileset;
	pti_install_trace_hooks(&hooks);
#endif

	/* initialize game */
	if (_pti.desc.init_cb != NULL) {
		_pti.desc.init_cb();
	}
}

static void cleanup(void) {
	saudio_shutdown();
	glDeleteVertexArrays(1, &state.gl.vao);
	glDeleteBuffers(1, &state.gl.vbo);
	glDeleteTextures(1, &state.gl.color0);
	glDeleteProgram(state.gl.program);
	glDeleteProgram(state.gl.crt);
#if defined(PTI_DEBUG)
	__dbgui_shutdown();
#endif
}

static void frame(void) {
	if (_pti.vm.flags == PTI_REQUEST_SHUTDOWN) {
		sapp_request_quit();
		return;
	}

	double frame_time_ns = sapp_frame_duration() * 1000000000.0;
	pti_tick(frame_time_ns);

	/* draw graphics */
	gl_draw();

#if defined(PTI_DEBUG)
	/* debug ui */
	__dbgui_begin();
	imgui_debug_draw();
	if (_pti.desc.debug_cb != NULL) {
		_pti.desc.debug_cb();
	}
	__dbgui_end();
#endif
}

static inline void btn_down(int pti_key, int sapp_key, int sapp_alt, const sapp_event *ev) {
	if (ev->key_code == sapp_key || ev->key_code == sapp_alt) {
		pti_event(PTI_EVENTTYPE_KEY_DOWN, pti_key);
	}
}

static inline void btn_up(int pti_key, int sapp_key, int sapp_alt, const sapp_event *ev) {
	if (ev->key_code == sapp_key || ev->key_code == sapp_alt) {
		pti_event(PTI_EVENTTYPE_KEY_UP, pti_key);
	}
}

static void event(const sapp_event *ev) {
#if defined(PTI_DEBUG)
	if (__dbgui_event(ev)) {
		return;
	}
#endif
	switch (ev->type) {
		/* keyboard: */
		case SAPP_EVENTTYPE_KEY_DOWN:
			if (ev->key_repeat) {
				break;
			}
			btn_down(PTI_RIGHT, SAPP_KEYCODE_RIGHT, SAPP_KEYCODE_RIGHT, ev);
			btn_down(PTI_LEFT, SAPP_KEYCODE_LEFT, SAPP_KEYCODE_LEFT, ev);
			btn_down(PTI_UP, SAPP_KEYCODE_UP, SAPP_KEYCODE_UP, ev);
			btn_down(PTI_DOWN, SAPP_KEYCODE_DOWN, SAPP_KEYCODE_DOWN, ev);
			btn_down(PTI_A, SAPP_KEYCODE_D, SAPP_KEYCODE_D, ev);
			btn_down(PTI_B, SAPP_KEYCODE_S, SAPP_KEYCODE_S, ev);
			btn_down(PTI_X, SAPP_KEYCODE_W, SAPP_KEYCODE_W, ev);
			btn_down(PTI_Y, SAPP_KEYCODE_A, SAPP_KEYCODE_A, ev);
			btn_down(PTI_LSHOULDER, SAPP_KEYCODE_Q, SAPP_KEYCODE_Q, ev);
			btn_down(PTI_RSHOULDER, SAPP_KEYCODE_E, SAPP_KEYCODE_E, ev);
			btn_down(PTI_START, SAPP_KEYCODE_X, SAPP_KEYCODE_X, ev);
			btn_down(PTI_SELECT, SAPP_KEYCODE_Z, SAPP_KEYCODE_Z, ev);
			btn_down(PTI_DBG, SAPP_KEYCODE_C, SAPP_KEYCODE_C, ev);
			break;

		case SAPP_EVENTTYPE_KEY_UP:
			btn_up(PTI_RIGHT, SAPP_KEYCODE_RIGHT, SAPP_KEYCODE_RIGHT, ev);
			btn_up(PTI_LEFT, SAPP_KEYCODE_LEFT, SAPP_KEYCODE_LEFT, ev);
			btn_up(PTI_UP, SAPP_KEYCODE_UP, SAPP_KEYCODE_UP, ev);
			btn_up(PTI_DOWN, SAPP_KEYCODE_DOWN, SAPP_KEYCODE_DOWN, ev);
			btn_up(PTI_A, SAPP_KEYCODE_D, SAPP_KEYCODE_D, ev);
			btn_up(PTI_B, SAPP_KEYCODE_S, SAPP_KEYCODE_S, ev);
			btn_up(PTI_X, SAPP_KEYCODE_W, SAPP_KEYCODE_W, ev);
			btn_up(PTI_Y, SAPP_KEYCODE_A, SAPP_KEYCODE_A, ev);
			btn_up(PTI_LSHOULDER, SAPP_KEYCODE_Q, SAPP_KEYCODE_Q, ev);
			btn_up(PTI_RSHOULDER, SAPP_KEYCODE_E, SAPP_KEYCODE_E, ev);
			btn_up(PTI_START, SAPP_KEYCODE_X, SAPP_KEYCODE_X, ev);
			btn_up(PTI_SELECT, SAPP_KEYCODE_Z, SAPP_KEYCODE_Z, ev);
			btn_up(PTI_DBG, SAPP_KEYCODE_C, SAPP_KEYCODE_C, ev);
			break;

		/* mouse: */
		case SAPP_EVENTTYPE_MOUSE_MOVE:
			break;

		default:
			break;
	}
}