#define PTI_IMPL
#include "pti.h"

// sokol
#include "sokol/sokol_app.h"
#include "sokol/sokol_log.h"

// libs
#include "dbgui/dbgui.h"
#include "imgui/imgui.h"

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

	return (sapp_desc) {
			.init_cb = init,
			.frame_cb = frame,
			.cleanup_cb = cleanup,
			.event_cb = event,
			.width = 800,
			.height = 600,
			.window_title = "pti (debug)",
			.logger = {
					.func = slog_func,
			},
#if defined(SOKOL_GLCORE)
#if defined(__APPLE__)
			.gl_major_version = 4,
			.gl_minor_version = 1,
#else
			.gl_major_version = 4,
			.gl_minor_version = 2,
#endif
#endif
	};
}

static struct {
	struct {
		unsigned int tick;
		int tick_accum;
	} timing;

	struct {
		GLuint vao;
		GLuint vbo;
		GLuint color0;
		GLuint program;
	} gl;
} state;

static unsigned int createShader(GLenum shaderType, const char *sourceCode) {
	//Create a new vertex shader object
	unsigned int shader = glCreateShader(shaderType);
	//Supply the shader object with source code
	glShaderSource(shader, 1, &sourceCode, NULL);
	//Compile the shader object
	glCompileShader(shader);
	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		//512 is an arbitrary length, but should be plenty of characters for our error message.
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		printf("Failed to compile shader: %s", infoLog);
	}
	return shader;
}


static void sokol_init_gfx(void) {
	const char *display_vs_src =
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

	const char *display_fs_src =
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

	;
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

	const auto width = _pti.vm.screen.width;
	const auto height = _pti.vm.screen.height;

	// create texture
	glGenTextures(1, &state.gl.color0);
	glBindTexture(GL_TEXTURE_2D, state.gl.color0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	// create shader
	auto createShader = [](GLenum type, const char *src) -> GLuint {
		auto shader = glCreateShader(type);
		glShaderSource(shader, 1, &src, NULL);
		glCompileShader(shader);
		int success;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			char infoLog[512];
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			printf("Failed to compile shader: %s", infoLog);
		}
		return shader;
	};

	auto vert_stage = createShader(GL_VERTEX_SHADER, display_vs_src);
	auto frag_stage = createShader(GL_FRAGMENT_SHADER, display_fs_src);

	state.gl.program = glCreateProgram();
	//Attach each stage
	glAttachShader(state.gl.program, vert_stage);
	glAttachShader(state.gl.program, frag_stage);
	//Link all the stages together
	glLinkProgram(state.gl.program);
	int success;
	glGetProgramiv(state.gl.program, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(state.gl.program, 512, NULL, infoLog);
		printf("Failed to link shader program: %s", infoLog);
	}

	glDeleteShader(vert_stage);
	glDeleteShader(frag_stage);
}

void sokol_gfx_draw() {
	const auto width = _pti.vm.screen.width;
	const auto height = _pti.vm.screen.height;

	// clear default buffer
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// upload pixel data
	glBindTexture(GL_TEXTURE_2D, state.gl.color0);
	glActiveTexture(GL_TEXTURE0);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, _pti.screen);

	// bind shader
	glUseProgram(state.gl.program);
	glUniform1i(glGetUniformLocation(state.gl.program, "screen"), 0);

	// draw fullscreen quad
	glBindVertexArray(state.gl.vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, state.gl.color0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void imgui_debug_draw() {
	ImGui::Begin("PTI", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	if (ImGui::CollapsingHeader("Screen")) {
		ImGui::Text("Dimensions: (%d, %d)", _pti.vm.screen.width, _pti.vm.screen.height);
	}

	// virtual machine
	if (ImGui::CollapsingHeader("Draw")) {
		ImGui::Text("Camera: (%d, %d)", _pti.vm.draw.cam_x, _pti.vm.draw.cam_y);
		ImGui::Text("Clip: ((%d, %d), (%d, %d))", _pti.vm.draw.clip_x0, _pti.vm.draw.clip_y0, _pti.vm.draw.clip_x1, _pti.vm.draw.clip_y1);
		ImGui::Text("Color Key: %d", _pti.vm.draw.ckey);
		ImGui::Text("Dither: %d", _pti.vm.draw.dither);
		ImGui::Text("Font: %p", _pti.vm.draw.font);
		ImGui::Text("Tileset: %p", _pti.vm.draw.tileset);
		ImGui::Text("Tilemap: %p", _pti.vm.tilemap);
	}

	// buttons
	if (ImGui::CollapsingHeader("Buttons")) {
		ImGui::Text("Botton [Left]: %d", _pti.vm.hardware.btn_state[0]);
		ImGui::Text("Botton [Right]: %d", _pti.vm.hardware.btn_state[1]);
		ImGui::Text("Botton [Up]: %d", _pti.vm.hardware.btn_state[2]);
		ImGui::Text("Botton [Down]: %d", _pti.vm.hardware.btn_state[3]);
		ImGui::Text("Botton [D]: %d", _pti.vm.hardware.btn_state[4]);
		ImGui::Text("Botton [S]: %d", _pti.vm.hardware.btn_state[5]);
		ImGui::Text("Botton [W]: %d", _pti.vm.hardware.btn_state[6]);
		ImGui::Text("Botton [A]: %d", _pti.vm.hardware.btn_state[7]);
		ImGui::Text("Botton [8]: %d", _pti.vm.hardware.btn_state[8]);
	}

	// random
	if (ImGui::CollapsingHeader("Random")) {
		ImGui::Text("Random [0]: %d", _pti.vm.hardware.rnd_reg[0]);
		ImGui::Text("Random [1]: %d", _pti.vm.hardware.rnd_reg[1]);
		ImGui::Text("Random [2]: %d", _pti.vm.hardware.rnd_reg[2]);
		ImGui::Text("Random [3]: %d", _pti.vm.hardware.rnd_reg[3]);
	}

	ImGui::Text("Random [3]: %p", _pti.vm.tilemap);
	ImGui::Text("Random [3]: %d", _pti.vm.screen.width);
	ImGui::Text("Random [3]: %d", _pti.vm.screen.height);

	// cart
	if (ImGui::CollapsingHeader("Cart")) {
		ImGui::Text("begin: %p", _pti.cart.begin);
		ImGui::Text("cap: %p", _pti.cart.cap);
		ImGui::Text("end: %p", _pti.cart.end);
		ImGui::Text("it: %p", _pti.cart.it);
	}


	ImVec2 uv_min(0.0f, 1.0f);
	ImVec2 uv_max(1.0f, 0.0f);

	const auto width = _pti.vm.screen.width;
	const auto height = _pti.vm.screen.height;
	ImGui::Image((ImTextureID) (intptr_t) state.gl.color0, ImVec2(width, height), uv_min, uv_max);

	ImGui::End();
}

static void init(void) {
	/* initialize graphics */
	sokol_init_gfx();

	/* initialize debug ui */
	__dbgui_setup();

	/* initialize game */
	if (_pti.desc.init_cb != NULL) {
		_pti.desc.init_cb();
	}
}

static void cleanup(void) {
	__dbgui_shutdown();
}

#define PTI_FRAMERATE (30.0)
#define PTI_DELTA (1.0 / PTI_FRAMERATE)
#define TICK_DURATION_NS (PTI_DELTA * 1e9)
#define TICK_TOLERANCE_NS (1000000)

static void frame(void) {
	double frame_time_ns = sapp_frame_duration() * 1000000000.0;
	if (frame_time_ns > TICK_DURATION_NS) {
		frame_time_ns = TICK_DURATION_NS;
	}

	state.timing.tick_accum += frame_time_ns;
	while (state.timing.tick_accum + TICK_TOLERANCE_NS >= TICK_DURATION_NS) {
		state.timing.tick_accum -= TICK_DURATION_NS;
		state.timing.tick++;

		if (_pti.desc.frame_cb != NULL) {
			_pti.desc.frame_cb();
		}

		for (int i = 0; i < PTI_BUTTON_COUNT; i++) {
			_pti.vm.hardware.btn_state[i] &= ~(_PTI_KEY_PRESSED | _PTI_KEY_RELEASED);
		}
	}

	/* draw graphics */
	sokol_gfx_draw();

	__dbgui_begin();
	imgui_debug_draw();
	__dbgui_end();
}

static inline void btn_down(int pti_key, int sapp_key, int sapp_alt, const sapp_event *ev) {
	if (ev->key_code == sapp_key || ev->key_code == sapp_alt) {
		_pti.vm.hardware.btn_state[pti_key] |= (_PTI_KEY_STATE | _PTI_KEY_PRESSED);
		_pti.vm.hardware.btn_state[pti_key] &= ~_PTI_KEY_RELEASED;
	}
}

static inline void btn_up(int pti_key, int sapp_key, int sapp_alt, const sapp_event *ev) {
	if (ev->key_code == sapp_key || ev->key_code == sapp_alt) {
		_pti.vm.hardware.btn_state[pti_key] &= ~(_PTI_KEY_STATE | _PTI_KEY_PRESSED);
		_pti.vm.hardware.btn_state[pti_key] |= _PTI_KEY_RELEASED;
	}
}

static void event(const sapp_event *ev) {
	if (__dbgui_event(ev)) {
		return;
	}
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
			btn_up(PTI_DBG, SAPP_KEYCODE_C, SAPP_KEYCODE_C, ev);
			break;

		/* mouse: */
		case SAPP_EVENTTYPE_MOUSE_MOVE:
			break;

		default:
			break;
	}
}